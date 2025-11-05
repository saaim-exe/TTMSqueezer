#include "core/ParseJSON.h"

json JSON::stockDatatoJSON(const std::string& raw_data)
{
	auto data = json::parse(raw_data);
	return data;
}

std::string JSON::tradeDatatoJSON(const std::vector<nlohmann::json>& message_payloads)
{
	const std::unique_ptr<FinnhubFeed> tradeData;

	std::unique_lock<std::shared_mutex> lock(tradeData->mutex());

	json message;

	message = tradeData->getMessagePayloads();

	std::string payload = message.dump(4);


	return payload;
}