#include <core/Logger.h>

std::mutex Logger::log_mtx; 

void Logger::log(LogLevel level, const std::string& message)
{
	std::lock_guard<std::mutex> lock(log_mtx);
	
	auto f_message = formatMessage(level, message); 

	if (console_log)
	{
		writetoConsole(f_message); 
	}
}

std::string Logger::formatMessage(LogLevel level, const std::string& message) {

	// time 
	auto now = std::chrono::system_clock::now(); 
	std::time_t now_c = std::chrono::system_clock::to_time_t(now); 
	std::tm timeInfo{}; 
	localtime_s(&timeInfo, &now_c); 
	char timestamp[20]; 
	std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeInfo);

	// conversion 
	auto level_to_string = [](LogLevel level) -> std::string {

		switch (level)
		{
		case LogLevel::DEBUG:
			return "DEBUG";
		case LogLevel::INFO:
			return "INFO";
		case LogLevel::WARNING:
			return "WARNING";
		case LogLevel::ERR:
			return "ERROR";
		case LogLevel::CRITICAL:
			return "CRITICAL";
		default: 
			return "UNKNOWN";
		}
		};


	std::ostringstream oss; 
	oss.rdbuf()->pubsetbuf(nullptr, 0); 
	oss << "[" << timestamp << "]" << "[" << level_to_string(level) << "] " << message; 
	return oss.str(); 
}

void Logger::writetoConsole(const std::string& message_formatted) {
		
	std::cout << message_formatted << '\n'; 
	std::cout.flush();

}