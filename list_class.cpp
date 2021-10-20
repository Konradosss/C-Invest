#include "Logic_declarations.h"
#include "declarations.h"

//////////////////////////////////////////////////////////////////

/////////////////////////base_list_class//////////////////////////

//////////////////////////////////////////////////////////////////
List::List() {};
List::~List() {};

std::wstring List::to_str(unsigned int i)
{
	std::wstring temp{ std::to_wstring(i) };
	return temp;
}

std::wstring List::to_str(double num)
{
	std::wstring temp;
	std::wstring formatted;
	temp = std::to_wstring(num);
	for (unsigned int i{}; i < temp.size(); ++i)
	{
		if (temp[i] != '.') { formatted.push_back(temp[i]); }
		else
		{
			formatted.push_back(temp[i]);
			formatted.push_back(temp[i + 1]);
			formatted.push_back(temp[i + 2]);
			break;
		}
	}
	return formatted;
}

std::wstring List::to_percent(double num) //recieves a fraction like 0.21 = 21% etc.
{
	std::wstring formatted;
	formatted = to_str(num*100);
	formatted = formatted + L"%";
	return formatted;
}
//////////////////////////////////////////////////////////////////

////////////////////////stock_list_class//////////////////////////

//////////////////////////////////////////////////////////////////
Stock_market_list::Stock_market_list() {};
Stock_market_list::~Stock_market_list() {};

void Stock_market_list::create_window(HINSTANCE instance, Pointers * data, const wchar_t * window_name) 
{
	application_data = data;
	state = true;

	parent_window.hInstance = instance;
	parent_window.lpszClassName = window_name;
	parent_window.lpfnWndProc = mother_window_procedure;

	RegisterClass(&parent_window);

	parent_handle = CreateWindowEx(0, window_name, L"Stock Market List", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT,
		CW_USEDEFAULT, 750, 800, NULL, NULL, instance, this);

	RECT dimensions;

	GetClientRect(parent_handle, &dimensions);

	list_handle = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | LVS_REPORT |  WS_VISIBLE , 0, 0,
		std::abs(dimensions.left - dimensions.right), std::abs(dimensions.bottom - dimensions.top), parent_handle, NULL, instance, NULL);

	ListView_SetExtendedListViewStyleEx(list_handle, LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT);

	application_data->stock_exchange_list_window_handle = parent_handle;
};

LRESULT CALLBACK Stock_market_list::mother_window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar)
{
	Stock_market_list * ptr;
	if (message == WM_NCCREATE)
	{
		CREATESTRUCT* st = (CREATESTRUCT*)lpar;
		ptr = (Stock_market_list*)(*st).lpCreateParams;
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)ptr);
		(*ptr).parent_handle = window_handle;
		SetWindowTextW(window_handle, (*st).lpszName);
		return TRUE;
	}
	else 
	{ 
		ptr = (Stock_market_list*)GetWindowLongPtr(window_handle, GWLP_USERDATA); 
	}
	if (ptr)
	{
		return (*ptr).mother_window_messages(message, wpar, lpar);
	}
	return DefWindowProc(window_handle, message, wpar, lpar);
}

LRESULT Stock_market_list::mother_window_messages(UINT message, WPARAM wpar,LPARAM lpar)
{
	if (message == WM_CLOSE)
	{
		state = false;
		DestroyWindow(this->parent_handle);
		return 0;
	}
	if (message == WM_PAINT)
	{
		
		PAINTSTRUCT pt;
		BeginPaint(parent_handle, &pt);

		ListView_DeleteAllItems(list_handle);

		HWND hWndHdr = (HWND)::SendMessage(list_handle, LVM_GETHEADER, 0, 0);
		int count = (int)::SendMessage(hWndHdr, HDM_GETITEMCOUNT, 0, 0L);
		bool state;

		for (int k{count -1}; k >=0; --k)
		{
			state = SendMessage(list_handle, LVM_DELETECOLUMN, (WPARAM) k ,0);
		}
		

		lvcol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
		lvcol.fmt = LVCFMT_CENTER;

		for (unsigned int i{}; i < application_data->column_names.size(); ++i)
		{
			lvcol.pszText = (LPWSTR)application_data->column_names[i].c_str();
			if (i == 0 || i == 2) { lvcol.cx = 70; }
			else { lvcol.cx = 150; }
			SendMessage(list_handle, LVM_INSERTCOLUMN, (WPARAM)i, (LPARAM)&lvcol);

		}
	
		lvitem.mask = LVIF_TEXT;
		lvitem.cchTextMax = 255;
		for (unsigned int i{}; i < application_data->listed_companies.size(); ++i)
		{
			int item_index;
			for (unsigned int j{}; j < application_data->column_names.size(); ++j)
			{

				std::wstring temp;
				lvitem.iSubItem = j;
				if (j == 0)
				{
					lvitem.pszText = (LPWSTR)application_data->listed_companies[i].current_information.acronym.c_str();
					item_index = SendMessage(list_handle, LVM_INSERTITEM, 0,(LPARAM) &lvitem);
					lvitem.iItem = item_index;
				}
				if (j == 1)
				{
					lvitem.pszText = (LPWSTR)(application_data->listed_companies[i].current_information.company_name.c_str());
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 2)
				{
					temp = to_str(application_data->listed_companies[i].current_information.share_price);
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 3)
				{
					temp = to_percent((application_data->listed_companies[i].current_information.share_price -
						application_data->listed_companies[i].previous_round_information.share_price) / application_data->listed_companies[i].previous_round_information.share_price);
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 4)
				{
					temp = to_str(application_data->listed_companies[i].current_information.earnings_per_share);
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 5)
				{
					temp = to_str(application_data->listed_companies[i].current_information.book_value_per_share);
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}

			}
			
		}

		EndPaint(parent_handle, &pt);
	}

	

	return DefWindowProc(this->parent_handle, message, wpar, lpar);
}

//////////////////////////////////////////////////////////////////

////////////////////portfolio_list_class//////////////////////////

//////////////////////////////////////////////////////////////////
Portfolio_list::Portfolio_list() {};
Portfolio_list::~Portfolio_list() {};

void Portfolio_list::create_window(HINSTANCE instance, Pointers * data, const wchar_t * window_name)
{
	application_data = data;
	state = true;

	parent_window.hInstance = instance;
	parent_window.lpszClassName = window_name;
	parent_window.lpfnWndProc = mother_window_procedure;

	RegisterClass(&parent_window);

	parent_handle = CreateWindowEx(0, window_name, L"Portfolio", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE , CW_USEDEFAULT,
		CW_USEDEFAULT, 900, 600, NULL, NULL, instance, this);

	RECT dimensions;

	GetClientRect(parent_handle, &dimensions);

	list_handle = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | LVS_REPORT | WS_VISIBLE,0, 100,
		std::abs(dimensions.left - dimensions.right), std::abs(dimensions.bottom - dimensions.top), parent_handle, NULL, instance, NULL);

	static_funds_available_handle = CreateWindowEx(0, L"STATIC", L"Funds available = 10000", WS_CHILD | WS_VISIBLE | SS_CENTER,
		10, 10, 300, 25, parent_handle, NULL, instance, NULL);

	static_total_portfolio_value_handle = CreateWindowEx(0, L"STATIC", L"Total funds = 10000", WS_CHILD | WS_VISIBLE | SS_CENTER,
		10, 35, 300, 25, parent_handle, NULL, instance, NULL);

	static_round_number_handle = CreateWindowEx(0, L"STATIC", L"Round number = 0", WS_CHILD | WS_VISIBLE | SS_CENTER,
		10, 60, 300, 25, parent_handle, NULL, instance, NULL);

	ListView_SetExtendedListViewStyleEx(list_handle, LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT);

	application_data->portfolio_list_window_handle = parent_handle;
	
};

LRESULT CALLBACK Portfolio_list::mother_window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar)
{
	Portfolio_list * ptr;
	if (message == WM_NCCREATE)
	{
		CREATESTRUCT* st = (CREATESTRUCT*)lpar;
		ptr = (Portfolio_list*)(*st).lpCreateParams;
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)ptr);
		(*ptr).parent_handle = window_handle;
		SetWindowTextW(window_handle, (*st).lpszName);
		return TRUE;
	}
	else
	{
		ptr = (Portfolio_list*)GetWindowLongPtr(window_handle, GWLP_USERDATA);
	}
	if (ptr)
	{
		return (*ptr).mother_window_messages(message, wpar, lpar);
	}
	return DefWindowProc(window_handle, message, wpar, lpar);
	
}

LRESULT Portfolio_list::mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar)
{
	if (message == WM_CLOSE)
	{
		state = false;
		DestroyWindow(this->parent_handle);
		return 0;
	}
	if (message == WM_PAINT)
	{

		PAINTSTRUCT pt;
		BeginPaint(parent_handle, &pt);

		ListView_DeleteAllItems(list_handle);

		HWND hWndHdr = (HWND)::SendMessage(list_handle, LVM_GETHEADER, 0, 0);
		int count = (int)::SendMessage(hWndHdr, HDM_GETITEMCOUNT, 0, 0L);
		bool state;

		for (int k{ count - 1 }; k >= 0; --k)
		{
			state = SendMessage(list_handle, LVM_DELETECOLUMN, (WPARAM)k, 0);
		}


		lvcol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
		lvcol.fmt = LVCFMT_CENTER;

		for (unsigned int i{}; i < application_data->portfolio_column_names.size(); ++i)
		{
			lvcol.pszText = (LPWSTR)application_data->portfolio_column_names[i].c_str();
			if (i == 2 ) { lvcol.cx = 184; }
			else { lvcol.cx = 94; }
			SendMessage(list_handle, LVM_INSERTCOLUMN, (WPARAM)i, (LPARAM)&lvcol);

		}

		lvitem.mask = LVIF_TEXT;
		lvitem.cchTextMax = 255;
		//first display bonds that investor has
		for (unsigned int i{}; i < application_data->user_portfolio->bonds.size(); ++i)
		{
			int item_index;
			std::wstring temp;
			for (unsigned int j{}; j < application_data->portfolio_column_names.size(); ++j)
			{
				lvitem.iSubItem = j;
				if (j == 0)
				{
					lvitem.pszText = (LPWSTR)L"Bond";
					item_index = SendMessage(list_handle, LVM_INSERTITEM, 0, (LPARAM)&lvitem);
					lvitem.iItem = item_index;
				}
				if (j == 1)
				{
					temp = to_str((application_data->user_portfolio->bonds[i].get_number_of_bonds()));
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 2)
				{
					temp = to_str(application_data->user_portfolio->bonds[i].get_price());
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 3)
				{
					temp = L"None";
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 4)
				{
					temp = to_str(application_data->user_portfolio->bonds[i].face_value * application_data->user_portfolio->bonds[i].get_number_of_bonds());
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 5)
				{
					temp = to_str(application_data->user_portfolio->bonds[i].get_number_of_bonds() * (application_data->user_portfolio->bonds[i].face_value - 10));
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
				if (j == 7)
				{
					temp = to_str(application_data->user_portfolio->bonds[i].time_to_maturity);
					lvitem.pszText = (LPWSTR)temp.c_str();
					SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
				}
			}

		}
		//then display stocks and options of each company
		for (unsigned int l{}; l < application_data->user_portfolio->stocks_and_options_for_each_company.size(); ++ l)
		{
			if (application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_number_of_shares() != 0) 
			{
				int item_index;
				std::wstring temp;
				for (unsigned int j{}; j < application_data->portfolio_column_names.size(); ++j)
				{
					lvitem.iSubItem = j;
					if (j == 0)
					{
						lvitem.pszText = (LPWSTR)L"Stock";
						item_index = SendMessage(list_handle, LVM_INSERTITEM, 0, (LPARAM)&lvitem);
						lvitem.iItem = item_index;
					}
					if (j == 1)
					{
						temp = to_str((application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_number_of_shares()));
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 2)
					{
						temp = to_str(application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_price());
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 3)
					{
						temp = application_data->user_portfolio->stocks_and_options_for_each_company[l].underlying_asset_information.company_name;
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 4)
					{
						temp = to_str(application_data->user_portfolio->stocks_and_options_for_each_company[l].underlying_asset_information.share_price *
							application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_number_of_shares());
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 5)
					{
						temp = to_str((application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_price() -
							application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_price_at_purchase())
							* application_data->user_portfolio->stocks_and_options_for_each_company[l].company_shares.get_number_of_shares());
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}

				}
			}
		}
		//at last display options that investor bought
		for (unsigned int l{ 0 }; l < application_data->user_portfolio->stocks_and_options_for_each_company.size(); ++l)
		{
			for (unsigned int k{0}; k <application_data->user_portfolio->stocks_and_options_for_each_company[l].options.size(); ++k)
			{
				int item_index;
				std::wstring temp;
				for (unsigned int j{}; j < application_data->portfolio_column_names.size(); ++j)
				{
					lvitem.iSubItem = j;
					if (j == 0)
					{
						lvitem.pszText = (LPWSTR)L"Option";
						item_index = SendMessage(list_handle, LVM_INSERTITEM, 0, (LPARAM)&lvitem);
						lvitem.iItem = item_index;
					}
					if (j == 1)
					{
						temp = to_str((application_data->user_portfolio->stocks_and_options_for_each_company[l].options[k].number_of_options));
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 2)
					{
						temp = to_str(application_data->user_portfolio->stocks_and_options_for_each_company[l].underlying_asset_information.share_price);
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 3)
					{
						temp = application_data->user_portfolio->stocks_and_options_for_each_company[l].underlying_asset_information.company_name;
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 4)
					{
						temp = to_str((double)0);
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 5)
					{
						temp = to_str((double)0);
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 6)
					{
						temp = application_data->user_portfolio->stocks_and_options_for_each_company[l].options[k].type_of_option;
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}
					if (j == 7)
					{
						temp = to_str(application_data->user_portfolio->stocks_and_options_for_each_company[l].options[k].date_to_expiry);
						lvitem.pszText = (LPWSTR)temp.c_str();
						SendMessage(list_handle, LVM_SETITEM, 0, (LPARAM)&lvitem);
					}

				}
			}
		}



		std::wstring temp = L"Funds available = " + to_str(application_data->user_portfolio->funds);
		SendMessage(static_funds_available_handle, WM_SETTEXT, 0, (LPARAM)temp.c_str());
		UpdateWindow(static_funds_available_handle);

		application_data->user_portfolio->calculate_total_portfolio_value();

		temp = L"Total portfolio value = " + to_str(application_data->user_portfolio->total_portfolio_value);
		SendMessage(static_total_portfolio_value_handle, WM_SETTEXT, 0, (LPARAM)temp.c_str());
		UpdateWindow(static_total_portfolio_value_handle);

		temp = L"Number of rounds = " + to_str(application_data->user_portfolio->round);
		SendMessage(static_round_number_handle, WM_SETTEXT, 0, (LPARAM)temp.c_str());
		UpdateWindow(static_round_number_handle);

		EndPaint(parent_handle, &pt);
	}



	return DefWindowProc(this->parent_handle, message, wpar, lpar);
};
