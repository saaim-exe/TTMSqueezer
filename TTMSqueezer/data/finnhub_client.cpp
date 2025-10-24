#include "finnhub_client.h"
#include "parse_json.h"


#pragma message("ASIO_STANDALONE is defined")
#pragma message("_WEBSOCKETPP_NO_BOOST_RANDOM is defined")




std::optional<std::string> Finnhub::getAPIKey() const {

	if (apiKey_) return apiKey_; 

	const char* key = std::getenv("FINNHUB_API_KEY"); 
	
	if (!key)
	{
		std::cerr << "ERROR::FINNHUB_API_KEY::NOT_SET\n"; 
		return std::nullopt; 

	}

	apiKey_ = std::string(key);

	if (apiKey_->empty())
	{
		std::cerr << "ERROR:FINNHUB_API_KEY::EMPTY_VALUE\n"; 
		return std::nullopt; 
	}

	
	return apiKey_;  
}


void Finnhub::Init() {

	client c; 

	try {
		
		// configurations 
		c.set_access_channels(websocketpp::log::alevel::all); 
		c.clear_access_channels(websocketpp::log::alevel::frame_payload); 
		c.set_error_channels(websocketpp::log::elevel::all); 

		c.init_asio(); 

		//message / tls handlers 

		c.set_message_handler(bind(&Finnhub::on_message, this, _1, _2)); 

		std::string hostname = "ws.finnhub.io"; 
		std::string uri = "wss://" + hostname + "/?token=" + apiKey_.value();

		c.set_tls_init_handler(bind(&Finnhub::Init_TLS, this, hostname.c_str(), _1)); 
		c.set_open_handler(bind(&Finnhub::on_open, this, _1, &c));
		c.set_fail_handler(bind(&Finnhub::on_fail, this, _1));

		c.set_close_handler(bind(&Finnhub::on_close, this, _1));
		c.set_error_channels(websocketpp::log::elevel::all); 

		websocketpp::lib::error_code ec; 
		client::connection_ptr connection_ptr = c.get_connection(uri, ec);
		if (ec) {

			std::cout << "Failed to Create Connection: " << ec.message() << std::endl;
			return; 
		}

		c.connect(connection_ptr); 

		c.run(); 
	}
	catch (const websocketpp::exception& e)
	{
		std::cout << "Websocket Exception: " << e.what() << std::endl; 

	}


}

context_ptr Finnhub::Init_TLS(const char* hostname, websocketpp::connection_hdl hdl) {
	context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23); 

	try {

		ctx->set_options(
			asio::ssl::context::default_workarounds |
			asio::ssl::context::no_sslv2 |
			asio::ssl::context::no_sslv3 |
			asio::ssl::context::single_dh_use);
		ctx->set_verify_mode(asio::ssl::verify_none);
	}
	catch (const std::exception& e){

		std::cout << "TLS Initialization Error: " << e.what() << std::endl; 

	}

	return ctx; 
}

void Finnhub::on_open(websocketpp::connection_hdl hdl, client* c)
{
	std::cout << "FINNHUB::CONNECTION::OPEN \n"; 

	websocketpp::lib::error_code ec; 
	client::connection_ptr con = c->get_con_from_hdl(hdl, ec); 
	if (ec)
	{
		std::cout << "Failed to get connection pointer: " << ec.message() << std::endl; 
		return; 
	}
	else
	{
		std::cout << "Subscribed!" << std::endl; 
	}

	// FOR FUTURE ------->>>> ADD MORE EXCHANGE/SYMBOL PAIRS


	std::unique_lock<std::shared_mutex> lock(finnhub_mutex); 
	
	std::vector<std::pair<std::string, std::string>> pairs =
	{
		// exchange - symbol

		{"BINANCE", "BTCUSDT"},
		{"BINANCE", "SOLUSDT"},
		{"COINBASE", "BTC-USD"},
		{"COINBASE", "SOL-USD"},
	};

	for (const auto& [ex, sym] : pairs)
	{
		std::string subscribeMsg = R"({"type":"subscribe","symbol":")" + ex + ":" + sym + R"("})";
		std::cout << "Subscribing to: " << subscribeMsg << std::endl;
		c->send(hdl, subscribeMsg, websocketpp::frame::opcode::text);
	}

}

void Finnhub::on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)
{

	std::unique_lock<std::shared_mutex> lock(finnhub_mutex);


	//deserialize 
	auto payload = msg->get_payload(); 
	auto j = json::parse(payload); 

	std::cout << "Finnhub Message Received: " << payload << std::endl; 

	if (j["type"] == "trade")
	{

		for (auto d : j["data"])
		{
			TradeData trade
			{
				d["p"].get<double>(),
				d["s"].get<std::string>(),
				d["t"].get<long long>(),
				d["v"].get<double>()
			}; 

			std::string symbol_full = d["s"]; 
			auto pos = symbol_full.find(':'); 
			std::string exchange = symbol_full.substr(0, pos); 
			std::string symbol = symbol_full.substr(pos + 1); 

			trade_data[exchange][symbol].push_back(trade); 

			latest_trade[exchange][symbol] = trade; 
	

		}
	}
		
		
}



void Finnhub::on_close(websocketpp::connection_hdl hdl)
{
	std::cout << "Websocket Connection Failed!" << std::endl; 
}
void Finnhub::on_fail(websocketpp::connection_hdl hdl)
{
	std::cout << "Websocket Connection Closed!" << std::endl; 
}

