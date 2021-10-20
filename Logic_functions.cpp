#include "Logic_declarations.h"
#include "declarations.h"

std::mt19937 rng((unsigned int)time(0));

double random_gaussian_number(double mean, double std)
{
	std::normal_distribution<double> distribution(mean, std);
	return distribution(rng);
}


std::wstring string2wide( std::string& str)
{
	// deal with trivial case of empty string
	if (str.empty()) { return std::wstring(); }

	// determine required length of new string
	size_t reqLength = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0);

	// construct new string of required length
	std::wstring ret(reqLength, L'\0');

	// convert old string to new string
	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], (int)ret.length());

	// return new string ( compiler should optimize this away )
	return ret;
}

void load_company_data(std::vector<Company>* information, std::string file_name)
{
	std::fstream streamfile;
	streamfile.open(file_name);
	while (!streamfile.eof())
	{
		Company_info company;
		std::string line;
		std::stringstream ss;
		std::string word;
		double number{ 0 };

		std::getline(streamfile, line);

		ss.str(line);
		ss >> word;
		company.acronym = string2wide(word);;
		word.clear();
		ss >> word;
		company.company_name = string2wide(word);;
		word.clear();
		ss >> number;
		company.share_price = number;
		ss >> number;
		company.earnings_per_share = number;
		ss >> number;
		company.book_value_per_share = number;

		Company comp_class{ company };
		(*information).push_back(comp_class);
	}

	streamfile.close();

}

unsigned int get_share_index(std::vector<Company> * comp_info, std::wstring company_name)
{
	unsigned int index{0};

	for(index; index < (*comp_info).size(); ++index)
	{
		if ((*comp_info)[index].current_information.company_name == company_name)
		{
			return index;
		}
	}

	return 0;
}

double get_option_price(Company_info underlying_asset, std::wstring option_type)
{
	return underlying_asset.share_price*0.1;
}

//////////////////////////////////////////////////////////////////

/////////////////////////company_class////////////////////////////

//////////////////////////////////////////////////////////////////

Company::Company() {};
Company::~Company() {};

Company::Company(Company_info information)
{
	current_information = information;

	previous_round_information = information;
};

Company_info Company::get_company_info()
{
	return current_information;
}
//function to modify company values based on performance in the last round
void Company::end_round()
{
	Company_info temp;

	temp.acronym = current_information.acronym;
	temp.company_name = current_information.company_name;

	//ratio that estimates how well company has done compared to the "standard" 
	double earnings_performance = abs(random_gaussian_number(1, 0.1)); 

	//ratio that estimates how earnings change based on the fundamental company value
	double earnings_change =  abs(random_gaussian_number(1,0.1));

	//chang in fundamental value of the company that is random as business is fundamentaly random
	double book_value_change = abs(random_gaussian_number(1, 0.1));

	temp.book_value_per_share = current_information.book_value_per_share * earnings_performance;//some formula with degree of randomness

	temp.earnings_per_share = current_information.earnings_per_share * earnings_change; //some formula with degree of randomness

	temp.share_price = current_information.share_price * book_value_change * earnings_change * earnings_performance; //some formula that calculates 

	round_num++;

	previous_round_information = current_information;

	current_information = temp;
};

//////////////////////////////////////////////////////////////////

//////////////////////Securities_class////////////////////////////

//////////////////////////////////////////////////////////////////
Securities::Securities() {};
Securities::~Securities() {};

//////////////////////////////////////////////////////////////////

//////////////////////Stock_class////////////////////////////

//////////////////////////////////////////////////////////////////
Stocks::Stocks() {};
Stocks::~Stocks() {};

Stocks::Stocks(Company_info company_inf, unsigned int number_bought)
{
	company_information = company_inf;
	number_of_shares = number_bought;
	price_at_purchase = company_inf.share_price;
	current_price = price_at_purchase;
}

double Stocks::get_price()
{
	return company_information.share_price;
}

double Stocks::payout(unsigned int number_of_shares_sold)
{
	if (number_of_shares_sold> number_of_shares)
	{
		return 0; // if there is non zero number of shares and payout is 0 then something went wrong
	}
	number_of_shares = number_of_shares - number_of_shares_sold;
	return (number_of_shares_sold * current_price);
}

unsigned int Stocks::get_number_of_shares()
{
	return number_of_shares;
}

double Stocks::get_price_at_purchase()
{
	return price_at_purchase;
}

void Stocks::end_round(Company_info company_state_after_round)
{
	company_information = company_state_after_round;
	current_price = company_state_after_round.share_price;
}

void Stocks::add_stocks(unsigned int number_of_added_shares)
{
	price_at_purchase = (number_of_shares*price_at_purchase + number_of_added_shares * current_price) / (number_of_shares + number_of_added_shares);
	number_of_shares = number_of_shares + number_of_added_shares;
}

//////////////////////////////////////////////////////////////////

////////////////////////bonds_class///////////////////////////////

//////////////////////////////////////////////////////////////////
Bonds::Bonds() {};
Bonds::~Bonds() {};

Bonds::Bonds(double rate, unsigned int bond_num, unsigned int length) 
{
	interest_rate = rate;
	number_of_bonds = bond_num;
	time_to_maturity = length;
	face_value = bond_price;
}

double Bonds::get_price() 
{
	return bond_price; 
}

double Bonds::payout(unsigned int number_of_shares_sold) //parameter not used you cant sell bonds in this game
{
	time_to_maturity = 0;
	return face_value*number_of_bonds;
}
//end round and add interest to the face value
void Bonds::end_round(Company_info new_round_info)
{
	--time_to_maturity;

	face_value = std::pow(face_value, 1 + interest_rate);
	
}

unsigned int Bonds::get_number_of_bonds()
{
	return number_of_bonds;
}

//////////////////////////////////////////////////////////////////

////////////////////////options_class/////////////////////////////

//////////////////////////////////////////////////////////////////
Options::Options() {};
Options::~Options() {};

Options::Options(Company_info underlying_asset, std::wstring option_type, unsigned int number_of_options_bought)
{
	type_of_option = option_type;
	underlying_asset_info = underlying_asset;
	number_of_options = number_of_options_bought;
	option_price = get_option_price(underlying_asset, option_type);
	date_to_expiry = 3; //3 rounds for each option
	current_share_price = underlying_asset.share_price;
	if (type_of_option == L"Call")
	{
		strike_price = 0.75*underlying_asset.share_price;
	}
	else
	{
		strike_price = 1.25*underlying_asset.share_price;
	}
}

double Options::get_price() 
{
	return option_price;
}

double Options::payout(unsigned int number_of_shares_sold)
{
	if (type_of_option == L"Call")
	{
		return max(0, current_share_price - strike_price) * number_of_options;
	}
	if (type_of_option == L"Put")
	{
		return max(0, strike_price - current_share_price) * number_of_options;
	}
	else { return 0; }
}

void Options::end_round(Company_info new_round_info)
{
	underlying_asset_info = new_round_info;
	current_share_price = underlying_asset_info.share_price;
	--date_to_expiry;
}

//////////////////////////////////////////////////////////////////

////////////////////////portfolio_class///////////////////////////

//////////////////////////////////////////////////////////////////

unsigned int get_share_index(const std::vector<Company_based_securities> * comp_info, std::wstring company_nm)
{
	for (unsigned int i{}; i < comp_info->size(); ++i)
	{
		if ((*comp_info)[i].underlying_asset_information.company_name == company_nm)
		{
			return i;
		}
	}
	return 0;
}

Portfolio::Portfolio() {};
Portfolio::~Portfolio() {};

Portfolio::Portfolio(double money, std::vector<Company>* info_ptr) 
{
	funds = money;
	company_list = info_ptr;
	for (unsigned int i{}; i < (*info_ptr).size(); i++)
	{
		Company_based_securities temp;
		temp.underlying_asset_information = (*info_ptr)[i].current_information;
		Stocks temp_stock{ temp.underlying_asset_information, 0 };
		temp.company_shares = temp_stock;
		stocks_and_options_for_each_company.push_back(temp);
	}

}

//update information inside company based securities vector
void Portfolio::update_stocks_and_options()
{
	for (unsigned int i{}; i < (*company_list).size(); ++i)
	{
		unsigned int index = get_share_index(&stocks_and_options_for_each_company, (*company_list)[i].current_information.company_name);
		stocks_and_options_for_each_company[index].underlying_asset_information = (*company_list)[i].current_information;
	}
}

void Portfolio::calculate_total_portfolio_value()
{
	total_portfolio_value = funds;

	for (unsigned int i{}; i < stocks_and_options_for_each_company.size(); ++i)
	{
		total_portfolio_value = (total_portfolio_value + (stocks_and_options_for_each_company[i].company_shares.get_number_of_shares() 
			* stocks_and_options_for_each_company[i].company_shares.get_price()));
	}
	for (unsigned int j{}; j < bonds.size(); ++j)
	{
		total_portfolio_value = total_portfolio_value + bonds[j].face_value * bonds[j].get_number_of_bonds();
	}
	//exclude options as they are a gamble, essentially
}

void Portfolio::end_round()
{
	Company_info temp; //dummy variable to pass to end turn bond function

	for (unsigned int i{}; i < (*company_list).size(); ++i)
	{
		(*company_list)[i].end_round();
	}
	(*this).update_stocks_and_options();
	if (bonds.size() != 0) 
	{
		int j;
		for (j = (static_cast<int>(bonds.size()) - 1); j >= 0 ; --j)
		{
			bonds[j].end_round(temp);
			if (bonds[j].time_to_maturity == 0)
			{
				funds = funds + bonds[j].payout(0); //bonds reach maturity
				bonds.erase(bonds.begin() + j);
			}
		}
	}

	//now deal with stocks and options 
	for (unsigned int k{}; k < stocks_and_options_for_each_company.size(); ++k)
	{
		//pass updated info to stocks and options structure
		stocks_and_options_for_each_company[k].company_shares.end_round(stocks_and_options_for_each_company[k].underlying_asset_information);

		if (stocks_and_options_for_each_company[k].options.size() != 0) 
		{
			int l;
			for ( l = ( static_cast<int>(stocks_and_options_for_each_company[k].options.size()) - 1 ); l >= 0; --l)
			{
				stocks_and_options_for_each_company[k].options[l].end_round(stocks_and_options_for_each_company[k].underlying_asset_information);
				if (stocks_and_options_for_each_company[k].options[l].date_to_expiry == 0)
				{
					funds = funds + stocks_and_options_for_each_company[k].options[l].payout(0);
					stocks_and_options_for_each_company[k].options.erase(stocks_and_options_for_each_company[k].options.begin() + l);
				}
			}
		}
	}
	round++;
}

bool Portfolio::sell_shares(unsigned int number_of_shares_sold, std::wstring comp_name)
{
	unsigned int index = get_share_index(&stocks_and_options_for_each_company, comp_name);
	if (number_of_shares_sold > stocks_and_options_for_each_company[index].company_shares.get_number_of_shares()) //you cant sell what you do not own
	{
		return false;
	}
	double revenue = stocks_and_options_for_each_company[index].company_shares.payout(number_of_shares_sold); //payout function already substracts number of shares sold

	funds = funds + revenue;

	return true;

}

bool Portfolio::buy_shares(unsigned int number_of_shares, std::wstring comp_name)
{
	unsigned int index = get_share_index(&stocks_and_options_for_each_company, comp_name);
	double cost = number_of_shares * stocks_and_options_for_each_company[index].underlying_asset_information.share_price;

	if (cost > funds) //you cant buy more than you have money for
	{
		return false;
	}
	funds = funds - cost;
	
	stocks_and_options_for_each_company[index].company_shares.add_stocks(number_of_shares);
	return true;
}

bool Portfolio::buy_bonds(unsigned int number_of_bonds, double interest, unsigned int lenght)
{
	double cost = number_of_bonds * bond_price;

	if (cost > funds) //check if we have money
	{
		return false;
	}
	funds = funds - cost;
	Bonds new_bonds{ interest,number_of_bonds, lenght };
	bonds.push_back(new_bonds);
	return true;
}

bool Portfolio::buy_options(unsigned int number_of_options, std::wstring comp_name, std::wstring option_type)
{

	unsigned int index = get_share_index(&stocks_and_options_for_each_company, comp_name);
	double cost = number_of_options * get_option_price(stocks_and_options_for_each_company[index].underlying_asset_information, option_type);

	if (cost > funds)
	{
		return false; //check if user has money
	}
	funds = funds - cost; 
	Options new_option{ stocks_and_options_for_each_company[index].underlying_asset_information,option_type,number_of_options };
	stocks_and_options_for_each_company[index].options.push_back(new_option);
	return true;
}

