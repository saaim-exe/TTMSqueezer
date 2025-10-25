#pragma once
#include "core/TradeData.h"

#include <string>
#include <vector>
#include <iostream>


class IDataFeed {
public:

	virtual ~IDataFeed() = default; 
	virtual bool getNext(TradeData& t) = 0;

};


