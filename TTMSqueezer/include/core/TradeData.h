#pragma once
#include <string>
#include <optional>
#include <iostream>


enum class DataType {
	TRADE,
	CANDLE
};


struct TradeData {


	// shared 
	std::string symbol; 
	std::string exchange; 
	long long u_timestamp = 0; 
	DataType type = DataType::TRADE; 

	// live data (trade specific)

	std::optional<double> price;
	std::optional<double> volume; 
	
	// historical data (candle specific) 

	std::optional<double> open; 
	std::optional<double> high; 
	std::optional<double> low; 
	std::optional<double> close; 
	std::optional<double> c_volume; 

	// helpers 

	double getCloseorPrice() const {

		if (close) return *close; 
		if (price) return *price; 

		return 0.0; 
	}

};

