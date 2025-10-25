
#include <ctime>
#include <sstream>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <iostream>

//SINGLETON CLASS FOR LOGGING 


enum class LogLevel {
	DEBUG,
	INFO, 
	WARNING,
	ERR,
	CRITICAL,
	UKNOWN
};

class Logger {

public: 
	
	static Logger& getInstance() {
		static Logger instance; 
		return instance; 
	}

	void log(LogLevel level, const std::string& message); 	
	void enableConsole(bool enable) { console_log = enable; }
	bool isConsoleEnabled() const { return console_log; }


private:

	Logger() = default; 
	~Logger() = default;  

	Logger(const Logger&) = delete; 
	Logger& operator=(const Logger&) = delete; 

	static std::mutex log_mtx;
	bool console_log = true;

	std::string formatMessage(LogLevel level, const std::string& message); 
	void writetoConsole(const std::string& message_formatted);

};

