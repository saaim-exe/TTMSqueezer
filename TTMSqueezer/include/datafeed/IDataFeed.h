#pragma once
#include "core/TradeData.h"

#include <string>
#include <vector>
#include <iostream>


class IDataFeed {
public:


	virtual void Init() {}; 
	virtual ~IDataFeed() = default; 
	virtual size_t size() const = 0; 
	virtual bool getNext(TradeData& t) = 0;

};


