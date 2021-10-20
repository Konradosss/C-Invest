#include "Logic_declarations.h"
#include "declarations.h"

#define Width 800
#define Height 575


//main function
int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev_inst, PWSTR cmd_line, int flag)
{
	
	const wchar_t Name[] = L"Control_Window";
	const wchar_t Name_index[] = L"Transaction_History_Window";

	//declare some variables
	RECT win_dimensions;
	Pointers application_state;

	//load company data and set application state
	load_company_data(&application_state.listed_companies, "Resources\\company_info.txt");

	Portfolio user_account{ 10000, &application_state.listed_companies };

	application_state.user_portfolio = &user_account;
	//create and set 2 windows
	WNDCLASS window1{};
	WNDCLASS window_transaction_history{};

	window1.lpfnWndProc = WindowProc;
	window1.hInstance = inst;
	window1.lpszClassName = Name;

	window_transaction_history.lpfnWndProc = WindowProc_transaction_history;
	window_transaction_history.hInstance = inst;
	window_transaction_history.lpszClassName = Name_index;

	RegisterClass(&window1);
	RegisterClass(&window_transaction_history);

	HWND func{ CreateWindowEx(0, Name, L"Investment Simulator WIG20 Edition", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX ,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
		NULL,NULL,inst,&application_state) };

	if (func == NULL) { return 0; }

	GetWindowRect(func, &win_dimensions);

	HWND func_index{ CreateWindowEx(0, Name_index, L"Instruction",WS_CAPTION | WS_SYSMENU |WS_MINIMIZEBOX | WS_VSCROLL,
		win_dimensions.right + 30, win_dimensions.bottom - Height, Width +100, Height,
		NULL,NULL,inst,NULL) };

	if (func_index == NULL) { return 0; }

	ShowWindow(func, flag); ShowWindow(func_index, flag);

	//create main message loop
	MSG message_queue{};

	while (GetMessage(&message_queue, NULL, 0, 0))
	{
		TranslateMessage(&message_queue);
		DispatchMessage(&message_queue);
	}
	
	return 0;
	
}





