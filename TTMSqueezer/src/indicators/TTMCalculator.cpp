#include <indicators/TTMCalculator.h>


BollingerResult TTM::BollingerBands(size_t period)
{
	// calculate SMA

	TradeData trade; 

	//	double sma; 
	// double upper_band;
	// double lower_band;
	// double std_dev;

	std::cout << "HI I RAN!"; 

	while (feed_->size() == 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // wait for finnnhub to start sending data 

	}

	while (feed_->getNext(trade))
	{
		
		size_t n = std::min(feed_->size(), period);

		double sma{}, upper_band{}, lower_band{}, std_dev{};

		if (feed_->size() != 0)
		{
			sma = trade.getCloseorPrice() / n;
			std::cout << "[SMA]: " << sma << std::endl; 

		}

	}

	BollingerResult b{};

	return b; 
}