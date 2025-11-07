#include "datafeed/YFinanceFeed.h"



size_t YFinanceFeed::writeCallback(char* content, size_t size, size_t nmemb, void* userdata)
{
	((std::string*)userdata)->append(content, size * nmemb);

	return size * nmemb;
}


std::string YFinanceFeed::fetch(const std::string& symbol, const unsigned int& length)
{
	CURL* curl = curl_easy_init();
	std::string response;

	if (curl)
	{
		std::string url = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol + "?interval=1d&range=1mo";

		// just warning -> this config is specific to your os	
		std::string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/141.0.0.0 Safari/537.36";
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			Logger::getInstance().log(LogLevel::ERR, curl_easy_strerror(res)); 

			//std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
		}

		curl_easy_cleanup(curl);
	}


	response_buffer = response;
	response_length = response.length();

	if (length > 0)
	{
		response_length = length;


		json response_json = JSON::stockDatatoJSON(response); 


		TradeData trade{
			.open = response_json["chart"]["result"][0]["indicators"]["quote"][0]["open"].get<double>(),
			.high = response_json["chart"]["result"][0]["indicators"]["quote"][0]["high"].get<double>(),
			.low = response_json["chart"]["result"][0]["indicators"]["quote"][0]["low"].get<double>(),
			.close = response_json["chart"]["result"][0]["indicators"]["quote"][0]["close"].get<double>(),
			.c_volume = response_json["chart"]["result"][0]["indicators"]["quote"][0]["volume"].get<double>()

		}; 

		buffer.push(trade); 
		response_buffer = response.substr(0, length);
		return response.substr(0, length);

	}

	return response;
}

