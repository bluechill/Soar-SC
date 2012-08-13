#include "Soar_Console.h" //Include the Soar Console header

extern void SetThreadName(const char *threadName, DWORD threadId);

using namespace std; //Use the standard namespace so we don't have to use std::string and can just type string, for example.

typedef basic_string<TCHAR> tstring; //Define a tstring as a string with a TCHAR as the character.  Used for conversions between TCHAR* and strings.  TCHAR* are returned from win32 functions sometimes.

Soar_Console* console = NULL; //Global this pointer of the console, initially set to NULL

static LONG edit_oldproc; //A pointer to the original function for handling input to the console command input area.

#define IDC_BUTTON1		101 //Define the button id to be 101, can be anything so I choose 101
#define IDC_TEXT_AREA	102 //Define the text area (console log) id to be 102
#define IDC_TEXT_EDIT	103 //Define the text edit area (console command input area) id to be 103

HWND editable_text_box; //Handle for the console command input area
HWND static_text_box; //Handle for the console log
HWND ghwnd; //Handle for the console window

HINSTANCE ghInst; //Process handle 
const char* lpClassName = "WinApp"; //Global variable for the class name type.  WinApp in this case.

LRESULT CALLBACK edit_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) //Function to replace the original function for handling input to the console input area
{
	switch (message) //Switch based on the type of message
	{
		case WM_CHAR: //If it's a key input
		{
			if (wParam == VK_RETURN) //Check if it's a return key
			{ //It is
				int iLength = GetWindowTextLength(static_text_box); //Get the length of the text in the console input area

				TCHAR buffer[1024]; //Create a buffer to hold the text
				GetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), buffer, 1024); //Get the text of the console input area and place it in the buffer

				tstring msg(buffer); //Construct a tstring around the buffer
				string message(msg.c_str()); //Get the char* representation of it so we can convert it to a string

				if (message.size() <= 0) //Check to see if the size is 0 or less than 0 (should be impossible since it is a size_t which is never less than 0)
					break; //It is so break out of the switch statement.

				console->send_input(message); //It isn't, so send the input to the send_input function which will put it in the event queue
				SetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), ""); //Set the text to be "", nothing.  Clear the text currently in there.
			}
			else //It isn't a return key
				return CallWindowProc((WNDPROC)edit_oldproc, hwnd, message, wParam, lParam); //So call the original function

			break; //Then break out of the switch statement
		}
		default: //Handle default messages by
		{
			return CallWindowProc((WNDPROC)edit_oldproc, hwnd, message, wParam, lParam); //Returning the result of calling the original function
		}
	}
	
	return 0;
}

// Main window callback function
LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) //Called for different window events
{
	switch (msg) //Handle the different messages
	{
	case WM_CREATE: //If it's a create message then the window has been created so create the different objects within it
		{
			RECT size; //Create a variable to hold the size of the window
			GetWindowRect(ghwnd, &size); //Put the size in the variable

			int width = size.right - size.left; //Calculate the width
			int height = size.top - size.bottom; //Calculate the height

			CreateWindowA("BUTTON", "Send", WS_CHILD | WS_VISIBLE, width - 90, height - 25, 75, 25, hwnd, (HMENU)IDC_BUTTON1, ghInst, NULL); //Create a send button to handle non-return send commands
			
			static_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 10, 10, width - 25, height - 50, hwnd, (HMENU)IDC_TEXT_AREA, ghInst, NULL); //Create the console log text box
			
			editable_text_box = CreateWindowA("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_HSCROLL, 10, height - 25, width - 115, 40, hwnd, (HMENU)IDC_TEXT_EDIT, ghInst, NULL); //Create the console input box.

			edit_oldproc = GetWindowLong(editable_text_box, GWL_WNDPROC); //Get the old command function of the console input box
			SetWindowLong(editable_text_box, GWL_WNDPROC, (long)edit_proc); //Set the current command function of the console input box to be our own

			break; //Then break out of the switch statement
		}
	case WM_COMMAND: //Handle a command
		{
			if (LOWORD(wparam) == IDC_BUTTON1) //If it's the send button
			{
				int iLength = GetWindowTextLength(static_text_box); //Get the length of the text in the console input area

				TCHAR buffer[1024]; //Create a buffer to hold the text
				GetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), buffer, 1024); //Get the text of the console input area and place it in the buffer

				tstring msg(buffer); //Construct a tstring around the buffer
				string message(msg.c_str()); //Get the char* representation of it so we can convert it to a string

				if (message.size() <= 0) //Check to see if the size is 0 or less than 0 (should be impossible since it is a size_t which is never less than 0)
					break; //It is so break out of the switch statement.

				console->send_input(message); //It isn't, so send the input to the send_input function which will put it in the event queue
				SetWindowText(GetDlgItem(ghwnd, IDC_TEXT_EDIT), ""); //Set the text to be "", nothing.  Clear the text currently in there.
			}

			break; //Then break out of the switch statement
		}
	case WM_CLOSE: //The window's close button was pressed
		{
			DestroyWindow(hwnd); //Destroy the window
			break; //Then break
		}
	case WM_DESTROY: //The window is being destroy
		{
			PostQuitMessage(0); //Post a quite message of "success"
			break; //Then break
		}
	case WM_SIZE: //The window's size was changed
		{
			int width = LOWORD(lparam); //Get the new width
			int height = HIWORD(lparam); //Get the new height

			HWND button = GetDlgItem(hwnd, IDC_BUTTON1); //Get the button handle 
			HWND area = GetDlgItem(hwnd, IDC_TEXT_AREA); //Get the console log handle
			HWND edit = GetDlgItem(hwnd, IDC_TEXT_EDIT); //Get the console input area handle
			
			MoveWindow(button, width - 90, height - 50, 75, 25, TRUE); //Update the button's position
			MoveWindow(area, 10, 10, width - 25, height - 75, TRUE); //Update the console log's position 
			MoveWindow(edit, 10, height - 50, width - 115, 40, TRUE); //Update the console input's area

			break; //Then break
		}
	default: //Handle all other commands by
		return DefWindowProc(hwnd, msg, wparam, lparam); //Return the default window function's output
	}

	return 0; //Return success
}

int console_thread_function(void* data) //Console thread function to distribute messages to and from the window
{
	SetThreadName("Console Message Loop", GetCurrentThreadId());

	MSG msg; //Create variable to hold the message

	while(GetMessage(&msg, NULL, 0, 0)) //While we have a message
	{
		TranslateMessage(&msg); //Translate it, parse it
		DispatchMessage(&msg); //Then send it
	}

	return msg.wParam; //Return the result
}

Soar_Console::Soar_Console(Events* event_queue) //Constructor for the soar console
{
	if (console != NULL) //If there is more than one console instance
		return; //Return

	console = this; //Set the global console instance variable to this one
	this->event_queue = event_queue; //Set the class event queue variable to the given one

	ghInst = GetModuleHandle(NULL); //Get the process handle
	WNDCLASSEX ex; //Create a variable for the window class attributes

	//Set some of the options of the window
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

	RegisterClassEx(&ex); //Register the set attirbutes

	ghwnd = CreateWindowEx(NULL, lpClassName, "Starcraft Console", WS_OVERLAPPEDWINDOW,	10, 10, 1000, 750, NULL, NULL, ghInst, NULL); //Create a new resizable window with the name "Starcraft Console"

	ShowWindow(ghwnd, 1); //Show the window to the user

	ShowCursor(true); //Make sure Starcraft doesn't hide the cursor

	lines = 0; //Set the number of lines output to 0

	console_thread = SDL_CreateThread(console_thread_function, NULL); //Create a new thread to handle the messages send from and to the window
}

Soar_Console::~Soar_Console() //Deconstructor
{
	SDL_WaitThread(console_thread, NULL); //Wait on the console thread, should be dead by now because the window was destroyed
	console = NULL; //Set the global console variable instance to NULL
}

void Soar_Console::send_input(std::string &input) //Send input to the event queue
{
	if (input.find("clear") == 0) //Unless it's a clear command
	{ //Since it is
		int iLength = GetWindowTextLength(static_text_box); //Get the length of  the text in the console log

		SendMessage(static_text_box, EM_SETSEL, 0, iLength); //"Select" all of the text in the window

		string line = ""; //Create a variable of the replacement text, in this case a blank text

		SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) line.c_str()); //Set the "selected" text to the replacement text

		lines = 0; //Set the number of lines output to 0
	}
	else //Otherwise
		event_queue->add_event(Soar_Event(input, false)); //Add command to the event queue
}

void  Soar_Console::recieve_input(std::string &input) //Recieve input to put in the console log
{
	string result; //Create a variable for the result to output to the log

	for (size_t last = 0, prev = 0;prev = last, (last = input.find("\n", last+1)) != string::npos;) //While we have a \n
	{
		result += string(input.begin()+prev, input.begin()+last) + "\r\n"; //Split along the \n and then add the string upto the \n to the result with a \r\n
		lines++; //Increase the number of lines output
	}

	if (result.empty()) //If result is empty
	{
		result += input + "\r\n"; //Set the result to be the input plus a \r\n
		lines++; //Then increase the number of lines output
	}

	int iLength = GetWindowTextLength(static_text_box); //Get the amount of text ouput in the console log
	SendMessage(static_text_box, EM_SETSEL, iLength, iLength); //Set the selection to be an append
	SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) result.c_str()); //Append the text

	if (lines >= 1000) //If the number of lines 
	{
		while (lines > 1000) //While the output line count is greater than 1000
		{
			int line_length = SendMessage(static_text_box, EM_LINELENGTH, 0, NULL); //Get the length of the first line

			SendMessage(static_text_box, EM_SETSEL, 0, line_length+1); //Select the first line

			string line = ""; //Create a variable with the replacement text, "" nothing, in this case to clear the first line, remove the first line.

			SendMessage(static_text_box, EM_REPLACESEL, 0, (LPARAM) line.c_str()); //Replace the selected text with the replacement text
			lines--; //Decrement the number of lines output.
		}
		SendMessage(static_text_box, WM_VSCROLL, SB_BOTTOM, NULL); //Scroll to the bottom of the console log
	}
}
