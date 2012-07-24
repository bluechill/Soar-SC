#include "Soar_Console.h"

using namespace std;

typedef basic_string<TCHAR> tstring;

Soar_Console* console = NULL;

static LONG edit_oldproc;

#define IDC_BUTTON1		101
#define IDC_TEXT_AREA	102
#define IDC_TEXT_EDIT	103

HWND editable_text_box;
HWND static_text_box;
HWND ghwnd;

HINSTANCE ghInst;
const char* lpClassName = "WinApp";

LRESULT CALLBACK edit_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CHAR:
		{
			if (wParam == VK_RETURN)
			{
				TCHAR buffer[1024];
				GetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), buffer, 1024);

				tstring msg(buffer);
				string message(msg.c_str());

				console->send_input(message);
			}
			else
				return CallWindowProc((WNDPROC)edit_oldproc, hwnd, message, wParam, lParam);

			break;
		}
		default:
		{
			return CallWindowProc((WNDPROC)edit_oldproc, hwnd, message, wParam, lParam);
			break;
		}
	}
	
	return 0;
}

// Main window callback function
LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
		{
			CreateWindowA("BUTTON", "Send", WS_CHILD | WS_VISIBLE,
				410, 425, 75, 25, hwnd, (HMENU)IDC_BUTTON1, ghInst, NULL);

			static_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 
				10, 10, 475, 400, hwnd, (HMENU)IDC_TEXT_AREA, ghInst, NULL);

			editable_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_HSCROLL,
				10, 425, 385, 25, hwnd, (HMENU)IDC_TEXT_EDIT, ghInst, NULL);

			edit_oldproc = GetWindowLong(editable_text_box, GWL_WNDPROC);
			SetWindowLong(editable_text_box, GWL_WNDPROC, (long)edit_proc);

			break;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wparam) == IDC_BUTTON1)
			{
				TCHAR buffer[1024];
				GetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), buffer, 1024);

				tstring msg(buffer);
				string message(msg.c_str());

				console->send_input(message);
			}

			break;
		}
	case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

int console_thread_function(void* data)
{
	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

Soar_Console::Soar_Console(std::vector<std::string> *console_queue, SDL_mutex* mu)
{
	if (console != NULL)
		return;

	this->console_queue = console_queue;
	this->mu = mu;

	console = this;

	ghInst = GetModuleHandle(NULL);

	WNDCLASSEX ex;

	ex.cbSize = sizeof(WNDCLASSEX);
	ex.cbClsExtra = 0;
	ex.cbWndExtra = 0;
	ex.hInstance = ghInst;
	ex.style = 0;
	ex.lpszMenuName = NULL;
	ex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	ex.hCursor = LoadCursor(NULL, IDC_ARROW);
	ex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	ex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	ex.lpszClassName = lpClassName;
	ex.style = NULL;
	ex.lpfnWndProc = mainWndProc;

	RegisterClassEx(&ex);

	ghwnd = CreateWindowEx(NULL, lpClassName, "Starcraft Console", 
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		200, 200, 500, 500, NULL, NULL, ghInst, NULL);

	ShowWindow(ghwnd, 1);

	console_thread = SDL_CreateThread(console_thread_function, NULL);
}

Soar_Console::~Soar_Console()
{
	SDL_WaitThread(console_thread, NULL);
	console = NULL;
}

void Soar_Console::send_input(std::string &input)
{
	SDL_mutexP(mu);
	console_queue->push_back(input);
	SDL_mutexV(mu);
}

void  Soar_Console::recieve_input(std::string &input)
{
	int iLength = GetWindowTextLength(static_text_box);
	SendMessage(static_text_box, EM_SETSEL, iLength, iLength);
	SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) input.c_str());
	SendMessage(static_text_box, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);
}