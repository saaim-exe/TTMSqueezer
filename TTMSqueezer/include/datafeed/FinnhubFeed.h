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

#include <optional>
#include <cstdlib>
#include <string>
#include <functional>
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

	
	~FinnhubFeed() { std::cout << "[FINNHUB]: " << "Destroyed!" << std::endl; }
	bool getNext(TradeData& t) override {
		return true; 
	}


	void Init(); 






	

private:






};
