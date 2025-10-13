#pragma once
#include <iostream>
#include <string>
#include <parse_json.h>

using json = nlohmann::json;


class Yfinance {


public:

	static size_t writeCallback(char* content, size_t size, size_t nmemb, void* userdata); 
	std::string fetch(const std::string& symbol, const unsigned int& length = 0);
	
	// accessors 

	std::string getResponseBuffer() const
	{
		if (response_buffer.empty())
		{
			return {}; 
		}
		return response_buffer;

	}
	unsigned int getBufferLength() const {

		if (response_length != 0)
		{
			return response_length; 
		}
		else
		{
			std::cerr << "No data is being fetched!" << std::endl; 
		}

	}

	json fetchasJSON() const
	{
		if (response_buffer.empty())
		{
			std::cerr << "Error: No response data available to convert to JSON.\n"; 
			return {}; 
		}

		return JSON::stockDatatoJSON(response_buffer); 
	}


private:
	
	std::string response_buffer; 
	unsigned int response_length;
};

