#include <curl/curl.h>
#include <core/Logger.h>
#include <core/Config.h>
#include <core/ParseJSON.h>
#include <core/TradeData.h>
#include <datafeed/IDataFeed.h>
#include <datafeed/FinnhubFeed.h>
#include <datafeed/YFinanceFeed.h>
#include <indicators/TTMCalculator.h>

#include <memory>
#include <string>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>


int main()
{
	//setup 
	curl_global_init(CURL_GLOBAL_DEFAULT);
	Logger::getInstance().enableConsole(true); 
	Logger::getInstance().log(LogLevel::INFO, "System Starting!"); 

	TTM t(std::make_unique<FinnhubFeed>()); 

	Logger::getInstance().log(LogLevel::INFO, "TTM constructed!");

	std::thread ttm_thread(&TTM::BollingerBands, &t, 20);
	
	ttm_thread.join(); 




		


	
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