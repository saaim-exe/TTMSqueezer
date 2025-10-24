#include <nlohmann/json.hpp>
#include <finnhub_client.h>

#include <memory>
#include <string>
#include <iostream>







using json = nlohmann::json;

namespace JSON {

	// converting yfinance stock data to json (not real time/ historic data 'screenshot')
	json stockDatatoJSON(const std::string& raw_data); 

	// for finnhub (real time trade data for crpyto exchanges) 

	std::string tradeDatatoJSON(const std::vector<nlohmann::json>& message_payloads);


}







