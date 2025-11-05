#pragma once

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#ifndef _WEBSOCKETPP_CPP11_STL_
#define _WEBSOCKETPP_CPP11_STL_
#endif

#ifndef _WEBSOCKETPP_NO_BOOST_RANDOM_
#define _WEBSOCKETPP_NO_BOOST_RANDOM_
#endif

#include "core/TradeData.h"
#include "datafeed/IDataFeed.h"
#include "core/ParseJSON.h"
#include "core/Logger.h"

#include <optional>
#include <cstdlib>
#include <string>
#include <functional>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <unordered_map>
#include <iostream>

#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_client.hpp> 
#include <websocketpp/client.hpp>
#include <asio/ssl/context.hpp>


namespace FinnhubWS {

	using websocketpp::lib::placeholders::_1;
	using websocketpp::lib::placeholders::_2;
	using websocketpp::lib::bind;
	using json = nlohmann::json;


	using client = websocketpp::client<websocketpp::config::asio_tls_client>;
	using context_ptr = websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>;

} 


class FinnhubFeed : public IDataFeed {
	
public:

	
	~FinnhubFeed() { Logger::getInstance().log(LogLevel::INFO, "Finnhub Destroyed!"); }
	
	bool getNext(TradeData& t) override {
		
		std::lock_guard<std::mutex> lock(buffer_mutex); 

		if (!buffer.empty())
		{
			t = buffer.front(); 
			buffer.pop(); 
			return true; 
		}


		return false; 
	}

	void Init(); 

	FinnhubWS::context_ptr Init_TLS(const char* hostname, websocketpp::connection_hdl hdl); 

	//callbacks
	void on_open(websocketpp::connection_hdl hdl, FinnhubWS::client* c);
	void on_message(websocketpp::connection_hdl hdl, FinnhubWS::client::message_ptr msg);
	void on_fail(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);

	// accessors 

	std::vector<std::string> getExchanges() const noexcept {
		return exchanges;
	}
	std::vector<std::string> getSymbols() const noexcept {
		return symbols;
	}
	std::vector<nlohmann::json> getMessagePayloads() const {
		return message_payloads;
	}


	std::shared_mutex& mutex() const { return finnhub_mutex; }


private:

	mutable std::optional<std::string> apiKey_;
	mutable std::shared_mutex finnhub_mutex;
	mutable std::mutex buffer_mutex; 


	std::vector<nlohmann::json> message_payloads;

	std::queue<TradeData> buffer; 

	std::vector<std::string> exchanges;
	std::vector<std::string> symbols;

	std::unordered_map <std::string, std::unordered_map<std::string, TradeData>> latest_trade;
	std::unordered_map <std::string, std::unordered_map<std::string, std::vector<TradeData>>> trade_data;

};
