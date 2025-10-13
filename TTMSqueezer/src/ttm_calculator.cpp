#include "ttm_calculator.h"
#include <yfinance_client.h>
#include <vector>
#include <algorithm>
#include <iomanip>

BollingerResult TTM::BollingerBands(size_t period) {

	// calculate SMA 
	//

	json close_prices = data["chart"]["result"][0]["indicators"]["quote"][0]["close"];
	std::vector<double> c_prices;

	size_t n = std::min(close_prices.size(), period);
	double sma{};

	for (size_t i = 0; i < n; ++i)
	{
		if (!close_prices[i].is_null())
		{
			// store in vec for later use potentially 
			double price = close_prices[i].get<double>();
			c_prices.push_back(price);
			sma += price;

		}
	}

	this->c_prices = c_prices; 

	if (!c_prices.empty())
	{
		sma /= c_prices.size();
		this->sma = sma;

		// std::cout << std::fixed << std::setprecision(6) << sma << '\n';
		// 
		// 
		// calculate s.d. 
		double sum_sq_diff = 0.0;
		double var = 0.0;
		double std_dev = 0.0;

		for (size_t i = 0; i < c_prices.size(); ++i)
		{
			sum_sq_diff += pow(c_prices[i] - sma, 2);
		}

		var = sum_sq_diff / c_prices.size();
		this->var = var;
		std_dev = sqrt(var);
		this->std_dev = std_dev;

		// calculate upper band 

		double upperband = sma + (2 * std_dev);
		bollinger_u_band = upperband;
		// calculate lower band 
		double lowerband = sma - (2 * std_dev);
		bollinger_l_band = lowerband;

		BollingerResult result{ sma, upperband, lowerband, std_dev };
		return result;
	}

	if (c_prices.empty())
	{
		std::cerr << "No Price Data! " << '\n';
		return BollingerResult{}; 
	}
}

KeltnerResult TTM::KeltnerChannels(size_t period)
{

	size_t n = std::min(c_prices.size(), period); 
	// calculate EMA 

	double k = 2.0 / (period + 1);
	double ema = sma;
	this->mutliplier = k;

	for (size_t i = 0; i < n; ++i)
	{
		ema = c_prices[i] * k + ema * (1 - k); 

	}

	// calculate ATR 

	double k_upper_band{}, k_lower_band{}, ATR{};



	json high = data["chart"]["result"][0]["indicators"]["quote"][0]["high"];
	json close = data["chart"]["result"][0]["indicators"]["quote"][0]["close"];
	json low = data["chart"]["result"][0]["indicators"]["quote"][0]["low"];

	if (!high.empty() && close.size() >= 2 && low.size() >= 2)
	{
		double todaysHigh = high.back().get<double>(); 
		double todaysClose = close.back().get<double>(); 
		double previousDayClose = close[close.size() - 2].get<double>(); 
		double todaysLow = low.back().get<double>(); 
		double previousDayLow = low[low.size() - 2].get<double>(); 

		ATR = std::max({ todaysHigh - todaysClose, todaysHigh - previousDayClose, todaysLow - previousDayLow });
		this->atr = ATR; 

		k_upper_band = ema + (k * ATR); 
		k_lower_band = ema - (k * ATR); 
		this->keltner_u_band = k_upper_band; 
		this->keltner_l_band = k_lower_band; 

	}
	else
	{
		std::cerr << "No high/close/low data available \n"; 
	}

	KeltnerResult result{ ema, k_upper_band, k_lower_band, k, ATR }; 

	return result; 
}


bool TTM::isSqueezeActive() const {

	if (bollinger_u_band == 0 || keltner_u_band == 0)
	{
		return false; 
	}

	bool squeeze = ((bollinger_u_band < keltner_u_band) && (bollinger_l_band > keltner_l_band));

	return squeeze; 
}
