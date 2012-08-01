#include "stdafx.h"

#include <windows.h>

#include <string>

using namespace std;

HINSTANCE ghInst;
const wchar_t* lpClassName = L"WinApp";

static LONG edit_oldproc;

#define IDC_BUTTON1		101
#define IDC_TEXT_AREA	102
#define IDC_TEXT_EDIT	103

HWND editable_text_box;
HWND static_text_box;
HWND ghwnd;

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

				string message = "Message: ";
				wstring msg(buffer);
				message += string(msg.begin(), msg.end());

				MessageBoxA(ghwnd, message.c_str(), "", MB_OK);
				SetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), L"");
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

			editable_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
				10, 425, 385, 25, hwnd, (HMENU)IDC_TEXT_EDIT, ghInst, NULL);

			edit_oldproc = GetWindowLong(editable_text_box, GWL_WNDPROC);
			SetWindowLong(editable_text_box, GWL_WNDPROC, (long)edit_proc);

			SetFocus(editable_text_box);
			break;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wparam) == IDC_BUTTON1)
			{
				MessageBoxA(ghwnd, "You clicked button1!", "button1", MB_OK);
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

// Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	ghInst = hInstance;

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

	ghwnd = CreateWindowEx(NULL, lpClassName, L"Starcraft Console", 
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		200, 200, 500, 500, NULL, NULL, ghInst, NULL);

	ShowWindow(ghwnd, nShowCmd);

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}