#include "Logic_declarations.h"
#include "declarations.h"

//instruction text to be displayed one line at a time
static const wchar_t *abc[] = {
	   TEXT("				How to use investment simulator (WIG20 Edition)				"),
	   TEXT(""),
	   TEXT("This programm is a simulation of investment porfolio. Player starts with 10k in cash and wants to make"),      
	   TEXT("the most money in shortest timeframe possible. investor can buy 3 types of securities: Bonds, Shares and"),
	   TEXT("Options. Every time player presses end round button market conditions change and so does the value of his assets."),   
	   TEXT("Each round represents certain unit of time that passed. Below, our consultant will describe securities available."),
	   TEXT(""), 
	   TEXT("Bonds: "),   
	   TEXT("Safest asset that will give player a guaranteed return in the form of interest added to the bond value."),
	   TEXT("Theny work in much the same way as saving accounts. Each bond costs 10 units and can be held for different"),  
	   TEXT("time periods. Highliy recomended investment!"),
	   TEXT(""),
	   TEXT("Shares:"),   
	   TEXT("These assets represent players part ownership of listed companies their value can increase or decrease just like"), 
	   TEXT("in real markets. There are 20 different companies listed on WIG20 and each can be a hidden gem. In the opinion of"),
	   TEXT("stock exchange mamagement buying these assets is considered gambling."),     
	   TEXT(""),  
	   TEXT("Options:"),
	   TEXT("These types of securities represent contracts that derive their value from other securities (eg. shares). There"),     
	   TEXT("are two types of options available: Call and Put options. Call option gives right to buy share at expiry time"),
	   TEXT("(3 rounds on WIG20) at a strike price (75% of share price at option purchase). This means that when you predict"),
	   TEXT("that share price will increase then you should buy this option as you will earn the difference between current"),
	   TEXT("share price and strike price. Put option gives right to sell share at expiry time (3 rounds) at a strike price"), 
	   TEXT("(125% of share price at option purchase). Both options cost 10% of current share price. Our consultant equates"),     
	   TEXT("buying these assets with gambling and is not recomending this risky investment."),
	   TEXT(""),
	   TEXT("I wish you the very best of luck and hope that you will find that game fun and interesting experience in"),
	   TEXT("futility and pointlessnes of modern stock market."),
	   TEXT(""),
	   TEXT(""),
	   TEXT(""),
	   TEXT(""),
	   TEXT(""),
	   TEXT(""),
	   TEXT(""),
	   TEXT(""),
	   TEXT("PS. If you really want to make money just buy bonds with highest interest rate. They feature compound interest"),
	   TEXT("and start ot grow exponentially once you reach certain portfolio size.")
};



LRESULT CALLBACK WindowProc(HWND window_handle, UINT unit_message, WPARAM wpar, LPARAM lpar)
{
	static HDC window_screen;
	static HDC memory_window;
	HBITMAP image;
	static HBITMAP hOldBmp;
	static HINSTANCE inst;
	
	static HWND temp;
	static HWND stock_buy_button;
	static HWND bonds_buy_button;
	static HWND options_buy_button;
	static HWND open_transaction_log;
	static HWND open_portfolio_overview;
	static HWND open_stock_listing;
	static HWND end_round;
	static HWND information_handle;

	static Pointers * application_data_pointer;

	static Popup_window_stock_buying stock_window;
	static Popup_window_options_buying option_window;
	static Popup_window_bonds_buying bond_window;
	static Stock_market_list stock_market_list_window;
	static Portfolio_list portfolio_list_window;

	static HBRUSH brush;
	static PAINTSTRUCT color;

	switch (unit_message)
	{
	
	case WM_COMMAND:

		switch (LOWORD(wpar)) {

			case BN_CLICKED:
				temp = (HWND)lpar;
				if (temp == stock_buy_button && stock_window.state == false) 
				{
					//stock popup function here
					stock_window.create_window(inst, L"Buy or Sell stock", application_data_pointer);
					
					return 0;
				}
				if (temp == bonds_buy_button && bond_window.state == false)
				{
					bond_window.create_window(inst, L"Buy bonds", application_data_pointer);
					return 0;

					//bonds popup window here
				}
				if (temp == options_buy_button && option_window.state == false)
				{
					option_window.create_window(inst, L"Buy options", application_data_pointer);
					return 0;
					//options buy here
				}
				if (temp == open_portfolio_overview && portfolio_list_window.state == false)
				{
					portfolio_list_window.create_window(inst, application_data_pointer, L"Portfolio window");
					return 0;
				}
				if (temp == open_stock_listing && stock_market_list_window.state == false)
				{
					stock_market_list_window.create_window(inst, application_data_pointer, L"Stock List");
					return 0;
				}
				if(temp == end_round)
				{
					application_data_pointer->user_portfolio->end_round();

					InvalidateRect(stock_window.window_handle, NULL, TRUE);
					UpdateWindow(stock_window.window_handle);

					InvalidateRect(portfolio_list_window.parent_handle, NULL, TRUE);
					UpdateWindow(portfolio_list_window.parent_handle);

					InvalidateRect(stock_market_list_window.parent_handle, NULL, TRUE);
					UpdateWindow(stock_market_list_window.parent_handle);

					InvalidateRect(option_window.window_handle, NULL, TRUE);
					UpdateWindow(option_window.window_handle);

					InvalidateRect(bond_window.window_handle, NULL, TRUE);
					UpdateWindow(bond_window.window_handle);

					if (application_data_pointer->user_portfolio->round == 400)
					{
						MessageBox(window_handle, L"Congratulations! You reached the end of the universe", L"Information", MB_OK | MB_ICONINFORMATION);
						DestroyWindow(window_handle);
						SendMessage(window_handle, WM_DESTROY, 0, 0);
					}

					return 0;
				}
			default:
				return 0;

		}

	case WM_CLOSE:

		if (MessageBox(window_handle, L"Are you sure?", L"Warning", MB_YESNO | MB_ICONWARNING) == IDYES) {
			DestroyWindow(window_handle);
			return 0;
		}
		else { return 0; }
		
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;

	case WM_PAINT:

		image = (HBITMAP)LoadImage(0, L"Resources\\Image_Control_Window.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		brush =  CreateSolidBrush(RGB(200,255,255)) ;
		window_screen = BeginPaint(window_handle, &color);
		
		// Create a memory device compatible with the above DC variable
		memory_window = CreateCompatibleDC(window_screen);
		// Select the new bitmap
		hOldBmp = (HBITMAP)SelectObject(memory_window, image);

		// Copy the bits from the memory DC into the current dc
		BitBlt(window_screen, 0, 0, 800, 575, memory_window, 0, 0, SRCCOPY);

		DeleteDC(memory_window);

		EndPaint(window_handle, &color);
		return 0;

	case WM_CREATE: {

		inst = GetModuleHandle(NULL);

		Stock_button button1(inst, L"Buy/Sell stock", window_handle);
		Stock_button button2(inst, L"Buy bonds", window_handle);
		Stock_button button3(inst, L"Buy options", window_handle);
		Stock_button button4(inst, L"Show Portfolio", window_handle);
		Stock_button button5(inst, L"Show stock listing", window_handle);
		Stock_button button7(inst, L"End Turn", window_handle);

		button1.Creating_Window(inst, window_handle, 620, 30);
		button2.Creating_Window(inst, window_handle, 620, 130);
		button3.Creating_Window(inst, window_handle, 620, 230);

		button4.Creating_Window(inst, window_handle, 620, 330);
		button5.Creating_Window(inst, window_handle, 620, 430);

		button7.Creating_Window(inst, window_handle, 30, 30);

		options_buy_button = button3.window_handle;
		bonds_buy_button = button2.window_handle;
		stock_buy_button = button1.window_handle;
		open_portfolio_overview = button4.window_handle;
		open_stock_listing = button5.window_handle;
		end_round = button7.window_handle;
		
		return 0;
	}
	case WM_NCCREATE:

		CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT*>( lpar);
		SetWindowTextW(window_handle, (*cs).lpszName);
		application_data_pointer = reinterpret_cast<Pointers*>((*cs).lpCreateParams);
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)application_data_pointer);
		return TRUE;

	}
	



	return DefWindowProc(window_handle, unit_message, wpar, lpar);
}


LRESULT CALLBACK WindowProc_transaction_history(HWND window_handle, UINT message, WPARAM size_flag, LPARAM lpar)
{
	static HBRUSH brush;
	static PAINTSTRUCT color;
	static HDC paint;
	static SCROLLINFO si;
	static RECT dimensions;
	static HFONT hFont;

	static size_t length;
	static int height{ 20 };
	static int number_of_lines;
	static int window_height;
	static int window_width;
	static int scroll_position;
	static int last_visible_line;
	static int first_visible_line;

	switch (message)
	{
	case WM_CLOSE:
			DestroyWindow(window_handle);
			DeleteObject(hFont);
			return 0;
	case WM_CREATE:

		hFont = CreateFont(height, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
		SendMessage(window_handle, WM_SETFONT, (WPARAM) hFont, TRUE);
		paint = GetDC(window_handle);
	
		ReleaseDC(window_handle,paint);

		return 0;

	case WM_SIZE:

		window_height = HIWORD(lpar);
		window_width = LOWORD(lpar);

		number_of_lines = sizeof(abc) / sizeof(*abc);

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = number_of_lines - 1;
		si.nPage = (window_height / height);
		SetScrollInfo(window_handle, SB_VERT, &si, TRUE);
		return 0;

	case WM_VSCROLL:
		
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(window_handle, SB_VERT, &si);
		scroll_position = si.nPos;

		switch (LOWORD(size_flag)) //loword of WM_VSRCOLL
		{
			// User clicked the HOME keyboard key.
		case SB_TOP:
			si.nPos = si.nMin;
			break;

			// User clicked the END keyboard key.
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			si.nPos -= 1;
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			si.nPos += 1;
			break;

			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

			// User dragged the scroll box.
		case SB_THUMBTRACK:
			si.nPos = HIWORD(size_flag);
			break;


		default:
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(window_handle, SB_VERT, &si, TRUE);
		GetScrollInfo(window_handle, SB_VERT, &si);

		if (si.nPos != scroll_position)
		{
			ScrollWindow(window_handle,0, height*(scroll_position - si.nPos), NULL, NULL);
			UpdateWindow(window_handle);
		}

		return 0;

	case WM_PAINT:

		//some code from wm_size message in case new entry gets added to the list
		GetClientRect(window_handle, &dimensions);
		number_of_lines = sizeof(abc) / sizeof(*abc);
		window_height = dimensions.bottom - dimensions.top;

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = number_of_lines - 1;
		si.nPage = (window_height / height);
		SetScrollInfo(window_handle, SB_VERT, &si, TRUE);

		// CHANGE FONT SELECT OBJECT
		paint = BeginPaint(window_handle, &color);
		SelectObject(paint, hFont);

		brush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(paint, &color.rcPaint, brush);
		

		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(window_handle, SB_VERT, &si);
		scroll_position = si.nPos;

		first_visible_line = max(0,scroll_position + dimensions.top/height);
		last_visible_line = min(number_of_lines - 1,scroll_position + dimensions.bottom/height) ; //wrong

		for (int i{ first_visible_line }; i <= last_visible_line; i++)
		{
			length = lstrlenW(abc[i]);
			TextOut(paint, 10, height*(i - scroll_position), abc[i], length);
			
		}
		
		EndPaint(window_handle, &color);

		return 0;
	 
	}
	return DefWindowProc(window_handle, message, size_flag, lpar);
}

