#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "yfinance_client.h"
#include <ttm_calculator.h>
#include "finnhub_client.h"






int main()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	Yfinance yf;
	
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

	curl_global_cleanup(); 
	return 0;
}
