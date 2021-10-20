#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <string.h>
#include <Windows.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <wchar.h>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <random>

// two structures are that store usefull information for the programm

struct Company_info
{
	std::wstring acronym;
	std::wstring company_name;
	double share_price;
	double earnings_per_share;
	double book_value_per_share;
};
//////////////////////////////////////////////////////////////////

/////////////////////////company_class////////////////////////////

//////////////////////////////////////////////////////////////////
class Company
{
public:

	Company();
	~Company();

	Company(Company_info information);

	Company_info get_company_info();

	void end_round(); //  change price and other share specification

	Company_info current_information;

	Company_info previous_round_information;

private:

	unsigned int round_num{ 0 };

	double volatility{ 7.5 };

};
//////////////////////////////////////////////////////////////////

///////////////////few_usefull_functions//////////////////////////

//////////////////////////////////////////////////////////////////

void load_company_data(std::vector<Company>* information, std::string file_name);

std::wstring string2wide(std::string& str);

unsigned int get_share_index(std::vector<Company> * comp_info, std::wstring acronym);

double random_gaussian_number(double mean, double std);

double get_option_price(Company_info underlying_asset, std::wstring option_type); // option price function to display on popup window

//////////////////////////////////////////////////////////////////

//////////////////////securities_class////////////////////////////

//////////////////////////////////////////////////////////////////
class Securities
{
public:

	Securities();
	virtual ~Securities();

	unsigned int round{};

	virtual double get_price() = 0;

	virtual double payout(unsigned int number_of_shares_sold) = 0;

	virtual void end_round(Company_info new_round_info) = 0;

private:


};
//////////////////////////////////////////////////////////////////

/////////////////////////stock_class//////////////////////////////

//////////////////////////////////////////////////////////////////
class Stocks : virtual public Securities
{
public:

	Stocks();
	~Stocks();

	Stocks(Company_info company_information, unsigned int number_of_shares); 

	double get_price();

	unsigned int get_number_of_shares();

	double payout(unsigned int number_of_shares_sold);

	double get_price_at_purchase();

	void end_round(Company_info new_round_info);

	void add_stocks(unsigned int number_of_stocks_added); //stocks get added and price at purchase is averaged to reflect the return value on this asset

private:

	Company_info company_information;

	unsigned int number_of_shares{};

	double current_price{};

	double price_at_purchase{};

};
//////////////////////////////////////////////////////////////////

////////////////////////bonds_class///////////////////////////////

//////////////////////////////////////////////////////////////////
class Bonds : virtual public Securities
{
public:

	Bonds();
	~Bonds();

	Bonds(double interest_rate, unsigned int bond_num, unsigned int length);

	double get_price();

	double payout(unsigned int number_of_shares_sold); //number of shares sold is not used here 

	void end_round(Company_info new_round_info); // comapny information not used here but still pass for consistency

	unsigned int time_to_maturity{};

	unsigned int get_number_of_bonds();

	double face_value{}; //value of the bond that is incremented after each rund by interest

private:

	double interest_rate{};

	unsigned int number_of_bonds{};

	double bond_price{ 10 };

};
//////////////////////////////////////////////////////////////////

////////////////////////options_class/////////////////////////////

//////////////////////////////////////////////////////////////////
class Options : virtual public Securities
{
public:

	Options();
	~Options();

	Options(Company_info underlying_asset, std::wstring option_type, unsigned int number_of_options);

	double get_price();

	double payout(unsigned int number_of_shares_sold); //parameter not used here as options only resolve at the expiry date

	void end_round(Company_info new_round_info);

	unsigned int date_to_expiry{}; //all options last for 3 rounds

	unsigned int number_of_options{};

	std::wstring type_of_option{};

private:

	Company_info underlying_asset_info;

	double strike_price{}; //price at which share can be bought or sold

	double current_share_price{}; // curent share price

	double option_price{};


};
//////////////////////////////////////////////////////////////////

//////////////////////portfolio_class/////////////////////////////

//////////////////////////////////////////////////////////////////

//structure that helps me manage stocks and options of each company
struct Company_based_securities
{
	Company_info underlying_asset_information; //company info is stored here
	Stocks company_shares{ underlying_asset_information,0 }; //comapny has only one type of shares
	std::vector<Options> options; //you can have many different options for each share
};

unsigned int get_share_index(const std::vector<Company_based_securities> * comp_info, std::wstring company);

class Portfolio
{
public:

	Portfolio();
	~Portfolio();

	Portfolio(double funds, std::vector<Company>* info_ptr);

	double funds{};

	double total_portfolio_value{};
	
	unsigned int round{};
	
	void update_stocks_and_options(); //pass information from company list to stock and option lists

	void end_round();

	void calculate_total_portfolio_value();

	bool buy_shares(unsigned int number_of_shares, std::wstring company_name);

	bool buy_bonds(unsigned int number_of_bonds, double interest, unsigned int lenght);

	bool buy_options(unsigned int number_of_options, std::wstring company_name, std::wstring option_type);

	bool sell_shares(unsigned int number_of_shares, std::wstring company_name);

	std::vector<Company>* company_list;
	std::vector<Company_based_securities> stocks_and_options_for_each_company; //same size and index as company list vector
	std::vector<Bonds> bonds;

private:

	double bond_price{ 10 };

};
//////////////////////////////////////////////////////////////////

/////////////////////application_data/////////////////////////////

//////////////////////////////////////////////////////////////////

struct Pointers
{
public:
	//structure with HWNDs, portfolio data, market data etc.
	int number_of_shares;
	std::wstring share_acronym;
	std::wstring action;

	//bond data
	std::vector<std::wstring> bond_options{ L"1 round", L"2 rounds", L"5 rounds", L"10 rounds", L"15 rounds", L"20 rounds" };
	double bond_price{ 10 };
	std::vector<double> bond_interest{ 0.005,0.0055,0.006,0.0062,0.0064,0.0066 }; 
	std::vector<std::wstring> bond_interest_string{L"0.5%", L"0.55%", L"0.6%", L"0.62%", L"0.64%", L"0.66%"};
	std::vector<unsigned int> bond_number_of_rounds{ 1,2,5,10,15,20 };

	std::vector<std::wstring> column_names{ L"Acronym",L"Company Name", L"Price",L"Price Change", L"Earnings Per Share", L"Book Value Per Share" };

	std::vector<std::wstring> portfolio_column_names{ L"Type of Asset", L"Quantity", L"Current value/Share value", L"Company name", L"Total_value" , L"ROI", L"Option type", L"Rounds to expiry" };

	std::vector<Company> listed_companies;

	Portfolio *user_portfolio; //pointer to portfolio class so that "frontend" can communicate with the "backend"

	HWND stock_exchange_list_window_handle{ NULL }; // handle to stock list window to update when there is a change
	HWND portfolio_list_window_handle{ NULL }; // handle to portfolio window to send update messages
	HWND stocks_buy_window_handle{ NULL };
	HWND options_buy_window_handle{ NULL };
	HWND bonds_buy_window_handle{ NULL };
};