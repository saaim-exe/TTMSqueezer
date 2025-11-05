#pragma once
#include <nlohmann/json.hpp>
#include <datafeed/FinnhubFeed.h>

#include <memory>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <iostream>


using json = nlohmann::json; 

namespace JSON {

	json stockDatatoJSON(const std::string& raw_data);
	std::string tradeDatatoJSON(const std::vector<nlohmann::json>& message_payloads);
}