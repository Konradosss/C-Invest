#include "Logic_declarations.h"
#include "declarations.h"

Button::Button()
{
}

Button::~Button()
{
}

Stock_button::Stock_button()
{
}

Stock_button::~Stock_button()
{
}

Stock_button::Stock_button(HINSTANCE instance_handle, const wchar_t *name, HWND mother_window)
{
	window.lpfnWndProc = window_procedure;
	window.lpszClassName = name;
	window.hInstance = instance_handle;	
}

void Stock_button::Creating_Window(HINSTANCE instance_handle, HWND mother_window, unsigned int x_position, unsigned int y_position)
{
	window_handle = CreateWindowEx(NULL, L"BUTTON", window.lpszClassName,  WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x_position, y_position, 150, 50, mother_window, NULL, instance_handle, this);
}

LRESULT CALLBACK Stock_button::window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar)
{
	Stock_button *ptr;

	if (message == WM_NCCREATE)
	{
		CREATESTRUCT *cs = (CREATESTRUCT*)lpar;
		ptr = (Stock_button*)(*cs).lpCreateParams;
		SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)ptr);
		(*ptr).window_handle = window_handle;
	}
	else 
	{
		ptr = (Stock_button*)GetWindowLongPtr(window_handle, GWLP_USERDATA);
	}
	if (ptr)
	{
		return (*ptr).procedure(message, wpar, lpar);
	}
	return DefWindowProc(window_handle, message, wpar, lpar);
}

LRESULT Stock_button::procedure(UINT message, WPARAM wpar, LPARAM lpar)
{
		return  DefWindowProc((*this).window_handle, message, wpar, lpar);
}
