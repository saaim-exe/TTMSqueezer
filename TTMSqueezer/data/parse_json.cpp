#include <parse_json.h>


json JSON::stockDatatoJSON(const std::string& raw_data)
{
	auto data = json::parse(raw_data); 
	return data; 
}

