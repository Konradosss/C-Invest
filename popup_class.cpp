#include "Logic_declarations.h"
#include "declarations.h"
//////////////////////////////////////////////////////////////////

////////////////////base_popup_window_class///////////////////////

//////////////////////////////////////////////////////////////////
Popup_window::Popup_window(){}
Popup_window::~Popup_window(){}

unsigned int Popup_window::char_to_int(wchar_t *string)
{
	std::wstring temp{ string };
	return std::stoi(temp);
}

std::wstring Popup_window::char_to_string(wchar_t *string)
{
	std::wstring temp{ string };
	return temp;
}

unsigned int Popup_window::get_number_from_text_control(HWND text_box_handle)
{
	WORD length;
	WORD number_length;
	TCHAR number_of_shares[30];
	length = (WORD)SendMessage(text_box_handle, EM_LINELENGTH, 0, 0);

	if (length == 0) { return 0; }
	if (length > 9) 
	{ 
		return 0; 
	}

	*((LPWORD)number_of_shares) = length;
	number_length = (WORD)SendMessage(text_box_handle, EM_GETLINE, 0, (LPARAM)number_of_shares);
	number_of_shares[length] = NULL;
	return char_to_int(number_of_shares);
}

std::wstring Popup_window::get_selection_from_list(HWND drop_down_list)
{
	int index = SendMessage(drop_down_list, CB_GETCURSEL, 0, 0);
	TCHAR company_acronym[256];

	int length = SendMessage(drop_down_list, CB_GETLBTEXT, index, (LPARAM)company_acronym);

	std::wstring temp{ company_acronym };

	return temp;
}

std::wstring Popup_window::put_number_into_total_cost_expression(double number)
{
	std::string temp;
	std::string formatted;
	temp = std::to_string(number);
	for (unsigned int i{}; i < temp.size(); ++i)
	{
		if (temp[i] != '.') { formatted.push_back(temp[i]); }
		else 
		{
			formatted.push_back(temp[i]); 
			formatted.push_back(temp[i+1]);
			formatted.push_back(temp[i+2]);
			break;
		}
	}
	std::wstring expression{ L"Transaction value = " };
	return expression + string2wide(formatted)  ;
}
//////////////////////////////////////////////////////////////////

////////////////////stock_buying_class////////////////////////////

//////////////////////////////////////////////////////////////////
Popup_window_stock_buying::Popup_window_stock_buying() {};
Popup_window_stock_buying::~Popup_window_stock_buying() {};

void Popup_window_stock_buying::create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer)
{
	state = true;

	window.hInstance = instance_handle;
	window.lpfnWndProc = window_procedure;
	window.lpszClassName = name;

	portfolio_data = pointer;

	RegisterClass(&window);

	window_handle = CreateWindowEx(0, window.lpszClassName, L"Buy/Sell Stock", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 850, 200,
		NULL, NULL, instance_handle, this);

	portfolio_data->stocks_buy_window_handle = window_handle;

	text_box_handle = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT  | ES_NUMBER,
		50, 50, 120, 25, window_handle, NULL, instance_handle, this);
	dropdown_list_handle = CreateWindowEx(0, WC_COMBOBOX, L"", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | CBS_SORT,
		190, 50,190, 200, window_handle, NULL, instance_handle, this);
	ok_button_handle = CreateWindowEx(0, L"BUTTON", L"Execute", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		150, 90, 80, 50, window_handle, NULL, instance_handle, this);
	cancel_button_handle = CreateWindowEx(0, L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		300, 90, 80, 50, window_handle, NULL, instance_handle, this);
	buy_sell_selection_handle = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		400, 50, 120, 60, window_handle, NULL, instance_handle, this);
	static_name_handle = CreateWindowEx(0, L"STATIC", L"Transaction value = 0", WS_CHILD | WS_VISIBLE | SS_CENTER ,
		550, 50, 250, 25, window_handle, NULL, instance_handle, this);
}

LRESULT CALLBACK Popup_window_stock_buying::window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar) {

	Popup_window_stock_buying *ptr;

	if (message == WM_NCCREATE)
	{
		CREATESTRUCT *st = (CREATESTRUCT*)lpar;
		ptr = (Popup_window_stock_buying*)(*st).lpCreateParams;
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)ptr);
		(*ptr).window_handle = window_handle;
		SetWindowTextW(window_handle, (*st).lpszName);
		
		return TRUE;
	}
	else
	{
		ptr = (Popup_window_stock_buying*)GetWindowLongPtr(window_handle, GWLP_USERDATA);
	}
	if (ptr) 
	{ 
		return (*ptr).mother_window_messages(message, wpar, lpar); 
	}

	return DefWindowProc(window_handle, message, wpar, lpar);
	
};

LRESULT Popup_window_stock_buying::mother_window_messages(UINT message, WPARAM wpar,LPARAM lpar)
{

	if (message == WM_PAINT)
	{
		SendMessage(this->dropdown_list_handle, CB_RESETCONTENT, 0, 0);

		SendMessage(this->text_box_handle, EM_SETLIMITTEXT, (WPARAM)8, 0);

		PAINTSTRUCT color;
		HDC scr = BeginPaint(window_handle, &color);
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));

		unsigned int size;
		size = portfolio_data->listed_companies.size();
		for (unsigned int i{ 0 }; i < size; ++i)
		{
			SendMessage(dropdown_list_handle,CB_ADDSTRING, 0, (LPARAM)portfolio_data->listed_companies[i].current_information.company_name.c_str());
		}
		SendMessage(dropdown_list_handle, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

		SendMessage(buy_sell_selection_handle, CB_ADDSTRING, 0, (LPARAM)L"Buy");

		SendMessage(buy_sell_selection_handle, CB_ADDSTRING, 0, (LPARAM)L"Sell");

		SendMessage(buy_sell_selection_handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		TextOutW(scr, 50, 25, L"Number of shares",lstrlenW(L"Number of shares"));

		TextOutW(scr, 230, 25, L"Company name", lstrlenW(L"Company name"));

		TextOutW(scr, 420, 25, L"Operation", lstrlenW(L"Operation"));

		EndPaint(window_handle, &color);

		number_of_shares_bought = get_number_from_text_control(text_box_handle);

		std::wstring share = get_selection_from_list(dropdown_list_handle);

		unsigned int index = get_share_index(&portfolio_data->listed_companies, share);

		double cost = portfolio_data->listed_companies[index].current_information.share_price * number_of_shares_bought;

		std::wstring expression = put_number_into_total_cost_expression(cost);

		SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
		UpdateWindow(static_name_handle);

		return 0;
	}
	if (message == WM_CLOSE)
	{
		state = false;
		DestroyWindow(window_handle);
		return 0;
	}
	if (message == WM_COMMAND)
	{
		HWND temp = (HWND)lpar;
		switch (LOWORD(wpar))
		{
		case BN_CLICKED:

			if (temp == ok_button_handle)
			{
				number_of_shares_bought = get_number_from_text_control(text_box_handle);
				
				std::wstring share = get_selection_from_list(dropdown_list_handle);

				std::wstring action = get_selection_from_list(buy_sell_selection_handle);

				portfolio_data->action = action;

				portfolio_data->share_acronym = share;
				
				bool transaction_state;

				//check if the action is legitimate with portfolio class
				if (action == L"Buy") 
				{
					transaction_state = portfolio_data->user_portfolio->buy_shares(number_of_shares_bought, share);
					InvalidateRect(portfolio_data->portfolio_list_window_handle, NULL, TRUE);
					UpdateWindow(portfolio_data->portfolio_list_window_handle);
				}
				if (action == L"Sell")
				{
					transaction_state = portfolio_data->user_portfolio->sell_shares(number_of_shares_bought, share);
					InvalidateRect(portfolio_data->portfolio_list_window_handle, NULL, TRUE);
					UpdateWindow(portfolio_data->portfolio_list_window_handle);
				}
				if (transaction_state)
				{
					MessageBox(text_box_handle, L"Operation completed", L"information", MB_OK | MB_ICONINFORMATION);
					SendMessage(portfolio_data->stock_exchange_list_window_handle, WM_PAINT, 0, 0);
				}
				else
				{
					MessageBox(text_box_handle, L"Error, not enougth funds or shares of a given type", L"Warning", MB_OK | MB_ICONWARNING);
				}
				
			}
			if (temp == cancel_button_handle)
			{
				state = false;
				DestroyWindow(window_handle);
				return 0;
			}
		case CBN_SELCHANGE:

			if (temp == dropdown_list_handle)
			{
				number_of_shares_bought = get_number_from_text_control(text_box_handle);


				std::wstring share = get_selection_from_list(dropdown_list_handle);

				//function that gets index of the chosen share 
				unsigned int index = get_share_index(&portfolio_data->listed_companies, share);

				double cost = portfolio_data->listed_companies[index].current_information.share_price * number_of_shares_bought;

				std::wstring expression = put_number_into_total_cost_expression(cost);

				SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
				UpdateWindow(static_name_handle);
			}
		case EN_CHANGE:

			if (temp == text_box_handle)
			{
				number_of_shares_bought = get_number_from_text_control(text_box_handle);

				std::wstring share = get_selection_from_list(dropdown_list_handle);

				//function that gets index of the chosen share 
				unsigned int index = get_share_index(&portfolio_data->listed_companies, share);
			
				double cost = portfolio_data->listed_companies[index].current_information.share_price * number_of_shares_bought;
			
				std::wstring expression = put_number_into_total_cost_expression(cost);

				SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
				UpdateWindow(static_name_handle);
			}
		//more cases here
	
		}
	
	}


	return DefWindowProc(window_handle, message, wpar, lpar);
}

//////////////////////////////////////////////////////////////////

////////////////////options_buying_class//////////////////////////

//////////////////////////////////////////////////////////////////
Popup_window_options_buying::Popup_window_options_buying() {};
Popup_window_options_buying::~Popup_window_options_buying() {};

void Popup_window_options_buying::create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer)
{
	state = true;

	window.hInstance = instance_handle;
	window.lpfnWndProc = window_procedure;
	window.lpszClassName = name;

	portfolio_data = pointer;

	RegisterClass(&window);

	window_handle = CreateWindowEx(0, window.lpszClassName, L"Buy Options", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 850, 200,
		NULL, NULL, instance_handle, this);

	portfolio_data->options_buy_window_handle = window_handle;

	text_box_handle = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_NUMBER,
		50, 50, 120, 25, window_handle, NULL, instance_handle, this);
	dropdown_list_handle = CreateWindowEx(0, WC_COMBOBOX, L"", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | CBS_SORT,
		190, 50, 190, 200, window_handle, NULL, instance_handle, this);
	ok_button_handle = CreateWindowEx(0, L"BUTTON", L"Execute", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		150, 90, 80, 50, window_handle, NULL, instance_handle, this);
	cancel_button_handle = CreateWindowEx(0, L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		300, 90, 80, 50, window_handle, NULL, instance_handle, this);
	put_call_selection_handle = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		400, 50, 120, 60, window_handle, NULL, instance_handle, this);
	static_name_handle = CreateWindowEx(0, L"STATIC", L"Transaction value = 0", WS_CHILD | WS_VISIBLE | SS_CENTER ,
		550, 50, 250, 25, window_handle, NULL, instance_handle, this);

}

LRESULT CALLBACK Popup_window_options_buying::window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar)
{
	Popup_window_options_buying *ptr;

	if (message == WM_NCCREATE)
	{
		CREATESTRUCT *st = (CREATESTRUCT*)lpar;
		ptr = (Popup_window_options_buying*)(*st).lpCreateParams;
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)ptr);
		(*ptr).window_handle = window_handle;
		SetWindowTextW(window_handle, (*st).lpszName);

		return TRUE;
	}
	else
	{
		ptr = (Popup_window_options_buying*)GetWindowLongPtr(window_handle, GWLP_USERDATA);
	}
	if (ptr)
	{
		return (*ptr).mother_window_messages(message, wpar, lpar);
	}

	return DefWindowProc(window_handle, message, wpar, lpar);

};

LRESULT Popup_window_options_buying::mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar)
{
	if (message == WM_PAINT)
	{
		SendMessage(this->dropdown_list_handle, CB_RESETCONTENT, 0, 0);

		SendMessage(this->text_box_handle, EM_SETLIMITTEXT, (WPARAM)9, 0);

		PAINTSTRUCT color;
		HDC scr = BeginPaint(window_handle, &color);
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));

		unsigned int size;
		size = portfolio_data->listed_companies.size();
		for (unsigned int i{ 0 }; i < size; ++i)
		{
			SendMessage(dropdown_list_handle, CB_ADDSTRING, 0, (LPARAM)portfolio_data->listed_companies[i].current_information.company_name.c_str());
		}
		SendMessage(dropdown_list_handle, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

		SendMessage(put_call_selection_handle, CB_ADDSTRING, 0, (LPARAM)L"Call");

		SendMessage(put_call_selection_handle, CB_ADDSTRING, 0, (LPARAM)L"Put");

		SendMessage(put_call_selection_handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		FillRect(scr, &color.rcPaint, brush);

		TextOutW(scr, 50, 25, L"Number of options", lstrlenW(L"Number of options"));

		TextOutW(scr, 230, 25, L"Company name", lstrlenW(L"Company name"));

		TextOutW(scr, 420, 25, L"Option type", lstrlenW(L"Option type"));

		EndPaint(window_handle, &color);

		number_of_options_bought = get_number_from_text_control(text_box_handle);

		std::wstring share = get_selection_from_list(dropdown_list_handle);

		//function that gets index of the chosen share 
		unsigned int index = get_share_index(&portfolio_data->listed_companies, share);

		double cost = get_option_price(portfolio_data->listed_companies[index].current_information, L"Call") * number_of_options_bought;

		std::wstring expression = put_number_into_total_cost_expression(cost);

		SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
		UpdateWindow(static_name_handle);

		return 0;
	}
	if (message == WM_CLOSE)
	{
		state = false;
		DestroyWindow(window_handle);
		return 0;
	}
	if (message == WM_COMMAND)
	{
		HWND temp = (HWND)lpar;
		switch (LOWORD(wpar))
		{
		case BN_CLICKED:

			if (temp == ok_button_handle)
			{
				number_of_options_bought = get_number_from_text_control(text_box_handle);

				std::wstring share = get_selection_from_list(dropdown_list_handle);

				std::wstring option_type = get_selection_from_list(put_call_selection_handle);

				portfolio_data->action = option_type;

				portfolio_data->share_acronym = share;

				bool transaction_state;

				//check if the action is legitimate with portfolio class
				transaction_state = portfolio_data->user_portfolio->buy_options(number_of_options_bought, share,option_type);

				InvalidateRect(portfolio_data->portfolio_list_window_handle, NULL, TRUE);
				UpdateWindow(portfolio_data->portfolio_list_window_handle);
						
				if (transaction_state)
				{
					MessageBox(text_box_handle, L"Operation completed", L"information", MB_OK | MB_ICONINFORMATION);
					SendMessage(portfolio_data->stock_exchange_list_window_handle, WM_PAINT, 0, 0);
				}
				else
				{
					MessageBox(text_box_handle, L"Error, not enougth funds", L"Warning", MB_OK | MB_ICONWARNING);
				}

				//add confirmation if player has enought money 
			}
			if (temp == cancel_button_handle)
			{
				state = false;
				DestroyWindow(window_handle);
				return 0;
			}
			//more cases here
		case CBN_SELCHANGE:

			if (temp == dropdown_list_handle)
			{
				number_of_options_bought = get_number_from_text_control(text_box_handle);


				std::wstring share = get_selection_from_list(dropdown_list_handle);

				//function that gets index of the chosen share 
				unsigned int index = get_share_index(&portfolio_data->listed_companies, share);

				double cost = get_option_price( portfolio_data->listed_companies[index].current_information, L"Call") * number_of_options_bought; // to change to option price

				std::wstring expression = put_number_into_total_cost_expression(cost);

				SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
				UpdateWindow(static_name_handle);
			}
		case EN_CHANGE:

			if (temp == text_box_handle)
			{
				number_of_options_bought = get_number_from_text_control(text_box_handle);

				std::wstring share = get_selection_from_list(dropdown_list_handle);

				//function that gets index of the chosen share 
				unsigned int index = get_share_index(&portfolio_data->listed_companies, share);

				double cost = get_option_price(portfolio_data->listed_companies[index].current_information, L"Call") * number_of_options_bought;

				std::wstring expression = put_number_into_total_cost_expression(cost);

				SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
				UpdateWindow(static_name_handle);
			}
		}

	}


	return DefWindowProc(window_handle, message, wpar, lpar);
}

//////////////////////////////////////////////////////////////////

//////////////////////bonds_buying_class//////////////////////////

//////////////////////////////////////////////////////////////////
Popup_window_bonds_buying::Popup_window_bonds_buying() {};
Popup_window_bonds_buying::~Popup_window_bonds_buying() {};

void Popup_window_bonds_buying::create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer)
{
	state = true;

	window.hInstance = instance_handle;
	window.lpfnWndProc = window_procedure;
	window.lpszClassName = name;

	portfolio_data = pointer;

	RegisterClass(&window);

	window_handle = CreateWindowEx(0, window.lpszClassName, L"Buy bonds", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 750, 200,
		NULL, NULL, instance_handle, this);

	portfolio_data->bonds_buy_window_handle = window_handle;

	text_box_handle = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_NUMBER,
		50, 50, 120, 25, window_handle, NULL, instance_handle, this);
	dropdown_list_handle = CreateWindowEx(0, WC_COMBOBOX, L"", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
		200, 50, 120, 200, window_handle, NULL, instance_handle, this);
	ok_button_handle = CreateWindowEx(0, L"BUTTON", L"Execute", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		150, 90, 80, 50, window_handle, NULL, instance_handle, this);
	cancel_button_handle = CreateWindowEx(0, L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		300, 90, 80, 50, window_handle, NULL, instance_handle, this);
	static_name_handle = CreateWindowEx(0, L"STATIC", L"Transaction value = 0", WS_CHILD | WS_VISIBLE | SS_CENTER,
		350, 50, 250, 25, window_handle, NULL, instance_handle, this);
	static_interest_rate_info = CreateWindowEx(0, L"STATIC", L"Interest rate on bonds = 4%", WS_CHILD | WS_VISIBLE | SS_CENTER ,
		350, 25, 250, 25, window_handle, NULL, instance_handle, this);
	
}

LRESULT CALLBACK Popup_window_bonds_buying::window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar)
{
	Popup_window_bonds_buying *ptr;

	if (message == WM_NCCREATE)
	{
		CREATESTRUCT *st = (CREATESTRUCT*)lpar;
		ptr = (Popup_window_bonds_buying*)(*st).lpCreateParams;
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)ptr);
		(*ptr).window_handle = window_handle;
		SetWindowTextW(window_handle, (*st).lpszName);

		return TRUE;
	}
	else
	{
		ptr = (Popup_window_bonds_buying*)GetWindowLongPtr(window_handle, GWLP_USERDATA);
	}
	if (ptr)
	{
		return (*ptr).mother_window_messages(message, wpar, lpar);
	}

	return DefWindowProc(window_handle, message, wpar, lpar);
}

LRESULT Popup_window_bonds_buying::mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar)
{
	if (message == WM_PAINT)
	{
		SendMessage(this->dropdown_list_handle, CB_RESETCONTENT, 0, 0);

		PAINTSTRUCT color;
		HDC scr = BeginPaint(window_handle, &color);
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));

		//bonds with different expiry dartes
		unsigned int size;
		size = portfolio_data->bond_options.size();
		for (unsigned int i{ 0 }; i < size; ++i)
		{
			SendMessage(dropdown_list_handle, CB_ADDSTRING, 0, (LPARAM)portfolio_data->bond_options[i].c_str());
		}
		SendMessage(dropdown_list_handle, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

		FillRect(scr, &color.rcPaint, brush);

		TextOutW(scr, 50, 25, L"Number of bonds", lstrlenW(L"Number of bonds"));

		TextOutW(scr, 200, 25, L"Length of bonds", lstrlenW(L"Length of bonds"));

		EndPaint(window_handle, &color);
		return 0;
	}
	if (message == WM_CLOSE)
	{
		state = false;
		DestroyWindow(window_handle);
		return 0;
	}
	if (message == WM_COMMAND)
	{
		HWND temp = (HWND)lpar;
		switch (LOWORD(wpar))
		{
		case BN_CLICKED:

			if (temp == ok_button_handle)
			{
				number_of_bonds_bought = get_number_from_text_control(text_box_handle);

				std::wstring number_of_rounds = get_selection_from_list(dropdown_list_handle);

				double interest_rate;

				unsigned int i;

				for (i = 0; i < portfolio_data->bond_options.size(); ++i)
				{
					if (portfolio_data->bond_options[i] == number_of_rounds)
					{
						interest_rate = portfolio_data->bond_interest[i];
						break;
					}
				}

				double cost = portfolio_data->bond_price * number_of_bonds_bought; 

				bool transaction_state;

				//check if the action is legitimate with portfolio class
				transaction_state = portfolio_data->user_portfolio->buy_bonds(number_of_bonds_bought,interest_rate,portfolio_data->bond_number_of_rounds[i]);

				InvalidateRect(portfolio_data->portfolio_list_window_handle, NULL, TRUE);
				UpdateWindow(portfolio_data->portfolio_list_window_handle);

				if (transaction_state)
				{
					MessageBox(text_box_handle, L"Operation completed", L"information", MB_OK | MB_ICONINFORMATION);
					SendMessage(portfolio_data->stock_exchange_list_window_handle, WM_PAINT, 0, 0);
				}
				else
				{
					MessageBox(text_box_handle, L"Error, not enougth funds", L"Warning", MB_OK | MB_ICONWARNING);
				}
				//add confirmation if player has enought money 
			}
			if (temp == cancel_button_handle)
			{
				state = false;
				DestroyWindow(window_handle);
				return 0;
			}
			//more cases here
		case CBN_SELCHANGE:

			if (temp == dropdown_list_handle)
			{
				number_of_bonds_bought = get_number_from_text_control(text_box_handle);

				std::wstring number_of_rounds = get_selection_from_list(dropdown_list_handle);

				double interest_rate;

				unsigned int i;

				for (i = 0; i < portfolio_data->bond_options.size(); ++i)
				{
					if (portfolio_data->bond_options[i] == number_of_rounds)
					{
						interest_rate = portfolio_data->bond_interest[i];
						break;
					}
				}

				double cost = portfolio_data->bond_price * number_of_bonds_bought;

				std::wstring expression = put_number_into_total_cost_expression(cost);

				std::wstring interest_expression = L"Interest rate: " + portfolio_data->bond_interest_string[i];

				SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
				UpdateWindow(static_name_handle);

				SendMessage(static_interest_rate_info, WM_SETTEXT, 0, (LPARAM)interest_expression.c_str());
				UpdateWindow(static_interest_rate_info);
			}
		case EN_CHANGE:

			if (temp == text_box_handle)
			{
				number_of_bonds_bought = get_number_from_text_control(text_box_handle);

				std::wstring number_of_rounds = get_selection_from_list(dropdown_list_handle);

				double interest_rate;

				unsigned int i;

				for (i = 0; i < portfolio_data->bond_options.size(); ++i)
				{
					if (portfolio_data->bond_options[i] == number_of_rounds)
					{
						interest_rate = portfolio_data->bond_interest[i];
						break;
					}
				}

				double cost = portfolio_data->bond_price * number_of_bonds_bought;

				std::wstring expression = put_number_into_total_cost_expression(cost);

				std::wstring interest_expression = L"Interest rate: " + portfolio_data->bond_interest_string[i];

				SendMessage(static_name_handle, WM_SETTEXT, 0, (LPARAM)expression.c_str());
				UpdateWindow(static_name_handle);

				SendMessage(static_interest_rate_info, WM_SETTEXT, 0, (LPARAM)interest_expression.c_str());
				UpdateWindow(static_interest_rate_info);
			}
		}

	}


	return DefWindowProc(window_handle, message, wpar, lpar);
}

