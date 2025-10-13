#pragma once
#include <nlohmann/json.hpp>
#include "yfinance_client.h"
#include "parse_json.h"

	
using json = nlohmann::json; 

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

	TTM(const json& stockData) : data(stockData) {}
	
	BollingerResult BollingerBands(size_t period = 20); 
	KeltnerResult KeltnerChannels(size_t period = 20); 
	bool isSqueezeActive() const; 


	// metric accessors 

	double getSMA() const noexcept { return sma; }	
	double getEMA() const noexcept { return ema;  }
	double getVar() const noexcept { return var; }
	double getStd_Dev() const noexcept { return std_dev;}
	

	

private: 

	json data;

	std::vector<float> prices; // will use for real time updates later on 
	std::vector<double> c_prices; 
	double sma; 
	double var; 
	double std_dev; 
	double bollinger_u_band;
	double bollinger_l_band; 

	double ema; 
	double keltner_u_band; 
	double keltner_l_band; 
	double mutliplier; 
	double atr;


};