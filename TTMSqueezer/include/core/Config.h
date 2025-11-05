#pragma once 
#include <string>
#include <optional>
#include <vector>
#include <string>
#include <iostream>

namespace Config {

	std::optional<std::string> getAPIKey(); 
	void printAPIKey(); 

	extern std::vector<std::string> exchanges;
	extern std::vector<std::string> symbols;
	
}
