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
#include <deque>
#include <numeric>
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

struct Candle {
	double open; 
	double close; 
	double high; 
	double low;
	double volume; 
	long long c_start_time;
	long long u_timestamp; 
};

struct Indicators {
	std::size_t TF; 
	double EMA; 
	double ATR; 
	BollingerResult b_result; 
	KeltnerResult k_result; 
};




namespace params {
	static constexpr std::size_t PERIOD = 20;
	static constexpr std::size_t MAX_BUFFER_SIZE = 500; // buffer size 
	static constexpr std::size_t THREAD_SLEEP = 10000;

	static constexpr std::size_t TF_60 = 60;
	static constexpr std::size_t TF_300 = 300;
	static constexpr std::size_t TF_900 = 900;
}



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

	FinnhubFeed f_access;
	// REMOVE THESE ONCE BELLOW ACCESSOR FUNCS COMPLETED!
	// void BollingerBands(std::size_t period = params::PERIOD); // removed 
	// void KeltnerChannels(std::size_t period = params::PERIOD); 


	// THESE FUNCTIONS NEED TO BE COMPELTED!!!
	void computeBollinger(const std::deque<Candle>& window); 
	void computeKeltner(const std::deque<Candle>& window); 
	double computeEMA(const std::deque<Candle>& window); 
	double computeATR(const std::deque<Candle>& window);
	void processStream(std::size_t period); 
	void signalDetector(); 


	template<typename Func> 
	void processFeed(Func f); 

	const Candle& getActiveCandle(std::size_t TF); 

private:
	

	std::unique_ptr<IDataFeed> feed_; 
	std::mutex ttm_mutex; 
	std::deque<BollingerResult> bollinger_buffer; 
	std::deque<KeltnerResult> keltner_buffer; 
	
	Candle m_candle_1min;
	Candle m_candle_5min;
	Candle m_candle_15min;

	std::deque<Candle> window_1m;
	std::deque<Candle> window_5m;
	std::deque<Candle> window_15m;




	std::unordered_map<std::string, double> first_trade_price_for_symbol; 
	std::unordered_map<std::string, std::unordered_map<int, std::unordered_map<long long, Candle>>> candles;
};