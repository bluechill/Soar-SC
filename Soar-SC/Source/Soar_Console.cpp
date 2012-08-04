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

TRACKMOUSEEVENT tme;

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
				std::cout << "VK_RETURN" << std::endl;

				int iLength = GetWindowTextLength(static_text_box);

				TCHAR buffer[1024];
				GetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), buffer, 1024);

				tstring msg(buffer);
				string message(msg.c_str());

				if (message.size() <= 0)
					break;

				console->send_input(message);
				SetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), "");
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
			RECT size;
			GetWindowRect(ghwnd, &size);

			int width = size.right - size.left;
			int height = size.top - size.bottom;

			CreateWindowA("BUTTON", "Send", WS_CHILD | WS_VISIBLE,
				width - 90, height - 25, 75, 25, hwnd, (HMENU)IDC_BUTTON1, ghInst, NULL);

			static_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 
				10, 10, width - 25, height - 50, hwnd, (HMENU)IDC_TEXT_AREA, ghInst, NULL);

			editable_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_HSCROLL,
				10, height - 25, width - 115, 40, hwnd, (HMENU)IDC_TEXT_EDIT, ghInst, NULL);

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
	case WM_SIZE:
		{
			int width = LOWORD(lparam);
			int height = HIWORD(lparam);

			HWND button = GetDlgItem(hwnd, IDC_BUTTON1);
			HWND area = GetDlgItem(hwnd, IDC_TEXT_AREA);
			HWND edit = GetDlgItem(hwnd, IDC_TEXT_EDIT);
			
			MoveWindow(button, width - 90, height - 50, 75, 25, TRUE);
			MoveWindow(area, 10, 10, width - 25, height - 75, TRUE);
			MoveWindow(edit, 10, height - 50, width - 115, 40, TRUE);

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

Soar_Console::Soar_Console(Events* event_queue)
{
	if (console != NULL)
		return;

	console = this;
	this->event_queue = event_queue;

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
		WS_OVERLAPPEDWINDOW,
		10, 10, 1000, 750, NULL, NULL, ghInst, NULL);

	ShowWindow(ghwnd, 1);

	ShowCursor(true);

	lines = 0;

	console_thread = SDL_CreateThread(console_thread_function, NULL);
}

Soar_Console::~Soar_Console()
{
	SDL_WaitThread(console_thread, NULL);
	console = NULL;
}

void Soar_Console::send_input(std::string &input)
{
	if (input.find("clear") == 0)
	{
		int iLength = GetWindowTextLength(static_text_box);

		SendMessage(static_text_box, EM_SETSEL, 0, iLength);

		string line = "";

		SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) line.c_str());

		lines = 0;
	}
	else
	{
		std::cout << "SEND_INPUT" << std::endl;
		event_queue->add_event(Soar_Event(input, false));
	}
}

void  Soar_Console::recieve_input(std::string &input)
{
	string result;

	for (size_t last = 0, prev = 0;prev = last, (last = input.find("\n", last+1)) != string::npos;)
	{
		result += string(input.begin()+prev, input.begin()+last) + "\r\n";
		lines++;
	}

	if (result.empty())
	{
		result += input + "\r\n";
		lines++;
	}

	int iLength = GetWindowTextLength(static_text_box);
	SendMessage(static_text_box, EM_SETSEL, iLength, iLength);
	SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) result.c_str());

	if (lines >= 1000)
	{
		int line_length = SendMessage(static_text_box, EM_LINELENGTH, 0, NULL);

		SendMessage(static_text_box, EM_SETSEL, 0, line_length+1);

		string line = "";

		SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) line.c_str());
		SendMessage(static_text_box, WM_VSCROLL, SB_BOTTOM, NULL);
	}
}