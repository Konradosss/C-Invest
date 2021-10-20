#include "Logic_declarations.h"
//declarations of user interface parts of program


//window procedures for main windows
LRESULT CALLBACK WindowProc(HWND func, UINT unit_message, WPARAM size_flag, LPARAM lpar);

LRESULT CALLBACK WindowProc_transaction_history(HWND func_index, UINT message, WPARAM size_flag, LPARAM lpar);

//////////////////////////////////////////////////////////////////

///////////////virtual_base_class_for_buttons/////////////////////

//////////////////////////////////////////////////////////////////
class Button
{
public:

	Button();
	virtual ~Button();

};
//////////////////////////////////////////////////////////////////

/////////////////////button_class/////////////////////////////////

//////////////////////////////////////////////////////////////////
class Stock_button : virtual public Button
{
public:
	Stock_button();
	//window handle is to determine who is a mother window
	Stock_button(HINSTANCE instance_handle,const wchar_t *name, HWND mother_window); 
	~Stock_button();
	HWND window_handle;
	void Creating_Window(HINSTANCE instance_handle, HWND mother_window, unsigned int x_position, unsigned int y_position);

private:

	WNDCLASS window{};
	LRESULT procedure(UINT message, WPARAM wpar, LPARAM lpar);
	static LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar);

};
//////////////////////////////////////////////////////////////////

///////////////virtual_base_class_for_popups//////////////////////

//////////////////////////////////////////////////////////////////
class Popup_window
{
public:

	Popup_window();
	virtual ~Popup_window();
	bool state{ false };
	virtual void create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer)=0;
	HWND window_handle;

protected:

	//application data
	Pointers* portfolio_data;

	//conversions needed
	unsigned int char_to_int(wchar_t *string);
	std::wstring char_to_string(wchar_t *string);
	unsigned int get_number_from_text_control(HWND text_box_handle);
	std::wstring get_selection_from_list(HWND dorp_box_handle);
	std::wstring put_number_into_total_cost_expression(double number);

	virtual LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar)=0;

	WNDCLASS window{};

	//different handles
	HWND text_box_handle;
	HWND dropdown_list_handle;
	HWND ok_button_handle;
	HWND cancel_button_handle;
	HWND static_name_handle;
	HWND static_interest_rate_info;

	unsigned int transaction_cost{ 0 };

};
//////////////////////////////////////////////////////////////////

///////////////class_for_stock_operations_popup///////////////////

//////////////////////////////////////////////////////////////////
class Popup_window_stock_buying: public Popup_window
{
public:

	Popup_window_stock_buying();
	~Popup_window_stock_buying();
	void create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer);

private:

	//as above some usefull functions
	std::wstring selected_company_acronym{L""};
	unsigned int number_of_shares_bought{0};

	HWND buy_sell_selection_handle;
	WNDCLASS selection_dropdown{};


	LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar);

	static LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar);

};
//////////////////////////////////////////////////////////////////

///////////////class_for_options_operations_popup/////////////////

//////////////////////////////////////////////////////////////////
class Popup_window_options_buying: public Popup_window
{
public:
	Popup_window_options_buying();
	~Popup_window_options_buying();
	void create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer);

private:

	//variables and handles specific to options popup
	std::wstring selected_acronym;
	unsigned int number_of_options_bought;
	std::wstring type_of_option;

	HWND put_call_selection_handle;
	WNDCLASS put_call_selection{};

	LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar);

	static LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar);

};
//////////////////////////////////////////////////////////////////

/////////////////class_for_bonds_operations_popup/////////////////

//////////////////////////////////////////////////////////////////
class Popup_window_bonds_buying: public Popup_window
{
public:

	Popup_window_bonds_buying();
	~Popup_window_bonds_buying();
	void create_window(HINSTANCE instance_handle, const wchar_t *name, Pointers* pointer);

private:

	//as above
	std::wstring selected_acronym;
	unsigned int number_of_bonds_bought;
	std::wstring type_of_option;

	LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar);

	static LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar);

};
//////////////////////////////////////////////////////////////////

//////////////////////base_class_for_lists////////////////////////

//////////////////////////////////////////////////////////////////
class List
{
public:

	List();
	virtual ~List();

	bool state{ false };

	virtual void create_window(HINSTANCE instance, Pointers * data, const wchar_t * window_name)=0;

	//common handles needed to update the window
	HWND parent_handle;
	HWND list_handle;
	

protected:

	//pointer to application data
	Pointers *application_data;

	LVCOLUMN lvcol{};
	LVITEM lvitem{};

	WNDCLASS parent_window{};

	//some usefull functions
	virtual LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar)=0;

	std::wstring to_str(unsigned int i);
	std::wstring to_str(double num);
	std::wstring to_percent(double num);

};
//////////////////////////////////////////////////////////////////

/////////////////derived_class_for_stock_list/////////////////////

//////////////////////////////////////////////////////////////////
//specific stock list
class Stock_market_list : public List
{
public:

	Stock_market_list();
	~Stock_market_list();

	void create_window(HINSTANCE instance, Pointers * data, const wchar_t * window_name);

protected:

	LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar);
	static LRESULT CALLBACK mother_window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar);

};
//////////////////////////////////////////////////////////////////

////////////////////portfolio_list_class//////////////////////////

//////////////////////////////////////////////////////////////////
//list of portfolio positions
class Portfolio_list: public List
{
public:

	Portfolio_list();
	~Portfolio_list();

	void create_window(HINSTANCE instance, Pointers* data, const wchar_t *name);

protected:

	static LRESULT CALLBACK mother_window_procedure(HWND window_handle, UINT message, WPARAM wpar, LPARAM lpar);
	LRESULT mother_window_messages(UINT message, WPARAM wpar, LPARAM lpar);

	//static handles
	HWND static_funds_available_handle;
	HWND static_total_portfolio_value_handle;
	HWND static_round_number_handle;
};