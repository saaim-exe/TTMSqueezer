#pragma once
#include <datafeed/IDataFeed.h>
#include <core/Logger.h>
#include <core/ParseJSON.h>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <iostream>

//FINISH THIS CLASS

class YFinanceFeed : public IDataFeed {


public: 


	YFinanceFeed(const std::string symbol, const unsigned int& length = 0) {}

	~YFinanceFeed() { Logger::getInstance().log(LogLevel::INFO, "Finnhub Destroyed!"); }


	bool getNext(TradeData& t) override {

		if (!buffer.empty())
		{

		}


		return true; 
	}

	static size_t writeCallback(char* content, size_t size, size_t nmemb, void* userdata);
	// raw yfinance data 
	std::string fetch(const std::string& symbol, const unsigned int& length = 0);





private: 

	std::vector<TradeData> buffer; /// queue ? ??  
	
	std::string response_buffer;
	unsigned int response_length;

};