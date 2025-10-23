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

#include <optional>
#include <cstdlib>
#include <string>
#include <functional>
#include <iostream>

#include <websocketpp/config/asio_client.hpp> 
#include <websocketpp/client.hpp>
#include <asio/ssl/context.hpp>


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using context_ptr = websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>;


class Finnhub {

public:

	void Init(); 
	
	context_ptr Init_TLS(const char* hostname, websocketpp::connection_hdl hdl); 

	std::optional<std::string> getAPIKey() const; 



	
	//callbacks
	void on_open(websocketpp::connection_hdl hdl, client* c); 
	void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg); 
	void on_fail(websocketpp::connection_hdl hdl); 
	void on_close(websocketpp::connection_hdl hdl); 




private:

	mutable std::optional<std::string> apiKey_; 




};