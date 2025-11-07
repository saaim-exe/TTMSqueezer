#include "datafeed/FinnhubFeed.h"

void FinnhubFeed::Init() {


	try {

		// configurations 
		client_.set_access_channels(websocketpp::log::alevel::all);
		client_.clear_access_channels(websocketpp::log::alevel::frame_payload);
		client_.set_error_channels(websocketpp::log::elevel::all);

		client_.init_asio();

		//message / tls handlers 

		client_.set_message_handler(bind(&FinnhubFeed::on_message, this, FinnhubWS::_1, FinnhubWS::_2));

		std::string hostname = "ws.finnhub.io";

		
		if (!apiKey_)
		{
			Logger::getInstance().log(LogLevel::ERR, "MISSING API KEY!"); 
			return; 
		}
		
		std::string uri = "wss://" + hostname + "/?token=" + apiKey_.value();

		client_.set_tls_init_handler(bind(&FinnhubFeed::Init_TLS, this, hostname.c_str(), FinnhubWS::_1));
		client_.set_open_handler(bind(&FinnhubFeed::on_open, this, FinnhubWS::_1, &client_));
		client_.set_fail_handler(bind(&FinnhubFeed::on_fail, this, FinnhubWS::_1));

		client_.set_close_handler(bind(&FinnhubFeed::on_close, this, FinnhubWS::_1));
		client_.set_error_channels(websocketpp::log::elevel::all);

		websocketpp::lib::error_code ec;
		FinnhubWS::client::connection_ptr connection_ptr = client_.get_connection(uri, ec);
		if (ec) {

			std::cout << "Failed to Create Connection: " << ec.message() << std::endl;
			return;
		}

		client_.connect(connection_ptr);

		//webocket connection is configured in separate thread 
		ws_thread_ = std::thread([this]() {
			client_.run();
			}); 
		
	}
	catch (const websocketpp::exception& e)
	{
		std::cout << "Websocket Exception: " << e.what() << std::endl;

	}

}

FinnhubWS::context_ptr FinnhubFeed::Init_TLS(const char* hostname, websocketpp::connection_hdl hdl)
{
	FinnhubWS::context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

	try {

		ctx->set_options(
			asio::ssl::context::default_workarounds |
			asio::ssl::context::no_sslv2 |
			asio::ssl::context::no_sslv3 |
			asio::ssl::context::single_dh_use);
		ctx->set_verify_mode(asio::ssl::verify_none);
	}
	catch (const std::exception& e) {


		std::cout << "TLS Initialization Error: " << e.what() << std::endl;

	}

	return ctx;
}

void FinnhubFeed::on_open(websocketpp::connection_hdl hdl, FinnhubWS::client* c)
{
	std::cout << "FINNHUB::CONNECTION::OPEN \n";

	websocketpp::lib::error_code ec;
	FinnhubWS::client::connection_ptr con = c->get_con_from_hdl(hdl, ec);
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

void FinnhubFeed::on_message(websocketpp::connection_hdl hdl, FinnhubWS::client::message_ptr msg) {

	std::unique_lock<std::shared_mutex> lock(finnhub_mutex);


	//deserialize 
	auto payload = msg->get_payload();
	auto j = json::parse(payload);

//	std::cout << "Finnhub Message Received: " << payload << std::endl;

	if (j["type"] == "trade")
	{

		for (auto d : j["data"])
		{
			TradeData trade
			{
				.symbol = d["s"].get<std::string>(),
				.exchange = "",
				.u_timestamp = d["t"].get<long long>(),
				.type = DataType::TRADE, 
				.price = d["p"].get<double>(), 
				.volume = d["v"].get<double>()
			};
			std::string symbol_full = d["s"];
			auto pos = symbol_full.find(':');
			trade.exchange = symbol_full.substr(0, pos);
			trade.symbol = symbol_full.substr(pos + 1);

			trade_data[trade.exchange][trade.symbol].push_back(trade);
			latest_trade[trade.exchange][trade.symbol] = trade;

			{
				std::lock_guard<std::mutex> q_lock(buffer_mutex);

				// consider .emplace (std::move(trade))) for performance later ... 
				buffer.push(trade); 
			}

		}
	}

}

void FinnhubFeed::on_fail(websocketpp::connection_hdl hdl) {

	Logger::getInstance().log(LogLevel::ERR, "Websocket Connection Failed!"); 
}

void FinnhubFeed::on_close(websocketpp::connection_hdl hdl) {

	Logger::getInstance().log(LogLevel::INFO, "Websocket Connection Closed!"); 
}


