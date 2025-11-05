#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <core/Logger.h>
#include <core/Config.h>


int main()
{
	//setup 
	curl_global_init(CURL_GLOBAL_DEFAULT);
	Logger::getInstance().enableConsole(true); 
	Logger::getInstance().log(LogLevel::INFO, "System Starting!"); 
	Config::printAPIKey(); 
	


		


	
	curl_global_cleanup(); 
	return 0;
}

/*Yfinance yf;

	std::string raw_data = yf.fetch("ATHR");

	json stock_data = yf.fetchasJSON();
	TTM ttm(stock_data);

	//std::cout << yf.getResponseBuffer() << '\n';


	ttm.BollingerBands(20);
	ttm.KeltnerChannels(20);

	if (ttm.isSqueezeActive())
	{
		std::cout << "TTM SQUEEZE IS ACTIVE \n";
	}
	else
	{
		std::cout << "No Squeeze Currently :( \n";
	}

	Finnhub f;

	auto api_key = f.getAPIKey();

	f.Init();


	if (api_key)
	{
		std::cout << "API Key: " << *api_key << '\n';
	}
	*/