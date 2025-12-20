#include <indicators/TTMCalculator.h>

using namespace params; 

template<typename Func>
void TTM::processFeed(Func f)
{
	TradeData trade;

	while (feed_->size() == 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP)); // wait for finnhub to start sending data 

	}

	while (feed_->getNext(trade))
	{
		f(trade); 		
	}

}

const Candle& TTM::getActiveCandle(std::size_t TF)
{
	switch (TF) {
	case TF_60:	return m_candle_1min;
	case TF_300: return m_candle_5min; 
	case TF_900: return m_candle_15min; 
	}
}



void TTM::computeBollinger(const std::deque<Candle>& window)
{	

	std::vector<double> closes; 
	closes.reserve(window.size()); 

	for (const auto& p : window)
	{
		closes.push_back(p.close);
	}
	
	// SMA 
	double sum = std::accumulate(closes.begin(), closes.end(), 0.0);
	double sma = sum / closes.size(); 

	// Standard Deviation 

	double sum_sq_diff = 0.0;

	for (const auto& p : closes)
	{
		sum_sq_diff += std::pow(p - sma, 2);
	}

	double var = sum_sq_diff / window.size();
	double std_dev = std::sqrt(var);

	// Bollinger Bands

	double upper = sma + 2 * std_dev;
	double lower = sma - 2 * std_dev; 

	BollingerResult result{ sma, upper, lower, std_dev }; 

	{
		std::lock_guard<std::mutex> lock(ttm_mutex); 

		bollinger_buffer.push_back(result);		

		if (bollinger_buffer.size() > MAX_BUFFER_SIZE)
		{
			bollinger_buffer.pop_front(); 

		}
	}


}

double TTM::computeEMA(const std::deque<Candle>& window)
{
	int period = window.size(); 

	std::vector<double> closes; 
	closes.reserve(window.size()); 

	for (const auto& p : window)
	{
		closes.push_back(p.close); 
	}

	// SMA

	double sum = std::accumulate(closes.begin(), closes.end(), 0.0); 
	double sma = sum / period; 

	// EMA multiplier 

	double k = 2.0 / (period + 1); 

	double ema = sma; 

	for (const auto& c : closes)
	{
		ema = c * k + ema * (1 - k); 
	}

	return ema; 
}


double TTM::computeATR(const std::deque<Candle>& window)
{
	
	if (window.size() < 2)
	{
		return 0.0; 
	}

	double tr_sum = 0.0; 

	for (size_t i = 1; i < window.size(); ++i)
	{
		const Candle& curr = window[i]; 
		const Candle& prev = window[i - 1];

		double tr = std::max({
			curr.high - curr.low,
			std::abs(curr.high - prev.close),
			std::abs(curr.low - prev.close)
		}); 


		tr_sum += tr; 
	}



	return tr_sum / (window.size() - 1); 
}

void TTM::computeKeltner(const std::deque<Candle>& window)
{

	// Keltner Bands 

	// double k = 2.0 / (period + 1);	
	double multiplier = 2.0; 
	double atr = computeATR(window); 
	double ema = computeEMA(window); 
	double keltner_u_band = ema + (multiplier * atr); 
	double keltner_l_band = ema - (multiplier * atr); 

	

	KeltnerResult result{ ema, keltner_u_band, keltner_l_band, multiplier, atr }; 

	{
		std::lock_guard<std::mutex> lock(ttm_mutex);

		keltner_buffer.emplace_back(result); 

		if (keltner_buffer.size() > MAX_BUFFER_SIZE)
		{
			keltner_buffer.pop_front(); 
		}
	}

}


	/// FINISH KELTNER INDICATOR + prepare for GUI integration (Qt) 
	// JUST REALZIED, CANNOT IMPLEMENT KELTNER DUE TO NO OCHLV DATA , WILL NEED TO BUILD CANDLE STRUCT/CLASS and compute manually 
	
	// CALCULATE TIMEFRAME BOUNDARIES (DONE) 
	// 	NEXT IS TO COMPUTE KELTNER INDICATORS (EMA, ATR, U_BAND, L_BAND, ) 
void TTM::processStream(std::size_t period) {

	/*
		candle construction function / indicator updates for each timeframe (1m, 5m, 15m). 
		Finnhub does not natively support candle 
	
	*/



	TradeData trade;

	std::deque<double> window;
	std::deque<Candle> window_1m;
	this->window_1m = window_1m; 
	std::deque<Candle> window_5m;
	this->window_5m = window_5m; 
	std::deque<Candle> window_15m;
	this->window_15m = window_15m; 

	processFeed([&](const TradeData& trade)
		{
			double price = trade.getCloseorPrice();
			window.push_back(price);

			std::string key = trade.exchange + ":" + trade.symbol;


			if (!first_trade_price_for_symbol.count(key))
			{
				first_trade_price_for_symbol[key] = price;
			}

			if (!candles.count(key)) {
				candles[key] = {};
			}

			auto construct_candle_60 = [&]() {
				Candle candle{
				.open = price,
				.close = price,
				.high = price,
				.low = price,
				.volume = *trade.volume,
				.c_start_time = static_cast<int>(floor(trade.u_timestamp / TF_60)) * TF_60,
				.u_timestamp = trade.u_timestamp
				};

				m_candle_1min = candle;
				};

			auto construct_candle_300 = [&]() {
				Candle candle{
				.open = price,
				.close = price,
				.high = price,
				.low = price,
				.volume = *trade.volume,
				.c_start_time = static_cast<int>(floor(trade.u_timestamp / TF_300)) * TF_300,
				.u_timestamp = trade.u_timestamp
				};

				m_candle_5min = candle;
				};

			auto construct_candle_900 = [&]() {
				Candle candle{
				.open = price,
				.close = price,
				.high = price,
				.low = price,
				.volume = *trade.volume,
				.c_start_time = static_cast<int>(floor(trade.u_timestamp / TF_900)) * TF_900,
				.u_timestamp = trade.u_timestamp
				};

				m_candle_15min = candle;
				};


			if (!candles[key].count(TF_60))
			{
				construct_candle_60();
			}
			else
			{
				long long expected_start_60 = floor(trade.u_timestamp / TF_60) * TF_60;

				if (expected_start_60 == m_candle_1min.c_start_time)
				{
					m_candle_1min.close = price;
					m_candle_1min.high = std::max(m_candle_1min.high, price);
					m_candle_1min.low = std::min(m_candle_1min.low, price);
					m_candle_1min.volume += *trade.volume;
					m_candle_1min.u_timestamp = trade.u_timestamp;
				}
				else
				{
					candles[key][TF_60][m_candle_1min.c_start_time] = m_candle_1min;

					window_1m.push_back(m_candle_1min);

					construct_candle_60();


					if (window_1m.size() > period)
					{
						window_1m.pop_front();
					}


					if (window_1m.size() == period)
					{
						
						computeBollinger(window_1m);
						computeKeltner(window_1m); 
					}

				}


			}


			if (!candles[key].count(TF_300))
			{
				construct_candle_300();
			}
			else
			{
				long long expected_start_300 = floor(trade.u_timestamp / TF_300) * TF_300;

				if (expected_start_300 == m_candle_5min.c_start_time)
				{
					m_candle_5min.close = price;
					m_candle_5min.high = std::max(m_candle_5min.high, price);
					m_candle_5min.low = std::min(m_candle_5min.low, price);
					m_candle_5min.volume += *trade.volume;
					m_candle_5min.u_timestamp = trade.u_timestamp;
				}
				else
				{
					candles[key][TF_300][m_candle_5min.c_start_time] = m_candle_5min;

					window_5m.push_back(m_candle_5min);

					construct_candle_300();

					if (window_5m.size() > period)
					{
						window_5m.pop_front();
					}


					if (window_5m.size() == period)
					{

						computeBollinger(window_5m);
						computeKeltner(window_5m);

					}

				}
			}

			if (!candles[key].count(TF_900))
			{
				construct_candle_900();
			}
			else
			{
				long long expected_start_900 = floor(trade.u_timestamp / TF_900) * TF_900;

				if (expected_start_900 == m_candle_15min.c_start_time)
				{
					m_candle_15min.close = price;
					m_candle_15min.high = std::max(m_candle_15min.high, price);
					m_candle_15min.low = std::min(m_candle_15min.low, price);
					m_candle_15min.volume += *trade.volume;
					m_candle_15min.u_timestamp = trade.u_timestamp;

				}
				else
				{
					candles[key][TF_900][m_candle_15min.c_start_time] = m_candle_15min;

					window_15m.push_back(m_candle_15min);

					construct_candle_900();


					if (window_15m.size() > period)
					{
						window_15m.pop_front();
					}

					if (window_15m.size() == period)
					{

						computeBollinger(window_15m);
						computeKeltner(window_15m);

					}


				}

			}
		});
}


void TTM::signalDetector() {

	/*
	 If the bollinger upper band is < keltner upper band and the bollinger lower band is greater than the keltner lower band
	 the current ticker is experiencing a 'squeeze'. The squeeze is stored in a boolean which returns true or false depending on its state. 
	 This is computed at the 1m, 5m and 15m timeframe. 
	*/



}

























//////////////////////////////////////








		/*				double sum = std::accumulate(window.begin(), window.end(), 0.0);
						double sma = sum / period;

						// ema
						double k = 2.0 / (period + 1);
						double ema = sma;


						for (const auto& p : window)
						{
							ema = p * k + ema * (1 - k);
							result.ema = ema;
						}

						// calculate ATR

						*/







/*	if (window.size() > period)
	{
		window.pop_front();
	}

	if (window.size() == period)
	{

		// calculate SMA

		double sum = std::accumulate(window.begin(), window.end(), 0.0);
		double sma = sum / period;

		//	{ "BINANCE", "BTCUSDT" },
		//	{ "BINANCE", "SOLUSDT" },
		//	{ "COINBASE", "BTC-USD" },
		//	{ "COINBASE", "SOL-USD" },
		//

			//std::cout << trade.exchange << " | " << trade.symbol << '\n';


		switch (f_access.hash_exchange(trade.exchange))
		{
		case exchange_hash::BINANCE:
			switch (f_access.hash_symbol(trade.symbol))
			{
			case symbol_hash::BTCUSDT:
				std::cout << "[BINANCE][BTCUSDT][SMA] " << sma << '\n' << std::flush;
				break;

			case symbol_hash::SOLUSDT:
				std::cout << "[BINANCE][SOLUSDT][SMA]" << sma << '\n' << std::flush;
				break;
			}
			break;
		case exchange_hash::COINBASE:
			switch (f_access.hash_symbol(trade.symbol))
			{
			case symbol_hash::BTC_USD:
				std::cout << "[COINBASE][BTC_USD][SMA] " << sma << '\n' << std::flush;
				break;
			case symbol_hash::SOL_USD:
				std::cout << "[COINBASE][SOL_USD][SMA] " << sma << '\n' << std::flush;
				break;
			}
			break;
		}

		// calculate s.d, variance
		double sum_sq_diff = 0.0;

		for (const auto& p : window)
		{
			sum_sq_diff += std::pow(p - sma, 2);
		}

		double var = sum_sq_diff / window.size();
		double std_dev = std::sqrt(var);


		// bollinger upper band, lower band

		double u_band = sma + (2 * std_dev);
		double l_band = sma - (2 * std_dev);

		result = { sma, u_band, l_band, std_dev };

		{
			std::lock_guard<std::mutex> lock(ttm_mutex);
			bollinger_buffer.push_back(result);

			// buffer size cap 500

			if (bollinger_buffer.size() > MAX_BUFFER_SIZE)
			{
				bollinger_buffer.pop_front();
			}
		}

	}

}); */