#pragma once

#include <datafeed/IDataFeed.h>
#include <datafeed/FinnhubFeed.h> 
#include <datafeed/YFinanceFeed.h>
#include <core/TradeData.h>
#include <core/ParseJSON.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <chrono>
#include <memory>
#include <iostream>


struct BollingerResult {
	double sma;
	double upper_band;
	double lower_band;
	double std_dev;
};

struct KeltnerResult {
	double ema;
	double upper_band;
	double lower_band;
	double multiplier;
	double atr;
};


class TTM {

public:
	// constructor that optionally takes finnhub or yfinance feed 
	explicit TTM(std::unique_ptr<IDataFeed> feed) 
		: feed_(std::move(feed)) {
		
		try {
			feed_->Init();
		}
		catch (const std::exception& e)
		{
			std::cout << "TTM Construct Fail! " << e.what() << std::endl; 
		}
		
	}

	BollingerResult BollingerBands(size_t period = 20); 




private:
	
	std::unique_ptr<IDataFeed> feed_; 
	std::mutex ttm_mutex; 


};