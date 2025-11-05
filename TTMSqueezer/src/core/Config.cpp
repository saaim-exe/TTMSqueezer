#include <core/Config.h>


	std::vector<std::string> Config::exchanges;
	std::vector<std::string> Config::symbols;



std::optional<std::string> Config::getAPIKey() {


	std::optional<std::string> apiKey_;

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

void Config::printAPIKey() {

	if (auto api_key = getAPIKey())
	{
		std::cout << "API Key: " << *api_key << std::endl;
	}
	else
	{
		std::cout << "No valid API key found.\n"; 
	} 
}



