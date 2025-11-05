# CMakeList.txt : CMake project for TTMSqueezer, include source and define
# project specific logic here.
#

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(TTMSqueezer
src/main.cpp
src/ttm_calculator.cpp
 "include/indicators/TTMCalculator.h"
 "include/strategy/"
 "include/core/TradeData.h" "include/datafeed/IDataFeed.h"
 "include/datafeed/FinnhubFeed.h" "src/datafeed/FinnhubFeed.cpp" "include/core/Config.h" "src/core/Config.cpp" "include/core/Logger.h" "src/core/Logger.cpp" "include/core/ParseJSON.h" "src/core/ParseJSON.cpp" "src/core/TradeData.cpp" "src/datafeed/YFinanceFeed.cpp" "include/datafeed/YFinanceFeed.h" "src/indicators/TTMCalculator.cpp")

target_include_directories(TTMSqueezer
	PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/data/include
    C:/Users/1942/vcpkg/installed/x64-windows/include
)

target_compile_definitions(TTMSqueezer PRIVATE
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL
    _WEBSOCKETPP_NO_BOOST_RANDOM
)


file(GLOB_RECURSE DATA_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/data/include/*.h")
source_group("Header Files\\Data" FILES ${DATA_HEADERS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TTMSqueezer PROPERTY CXX_STANDARD 20)
endif()

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(asio CONFIG REQUIRED)


target_link_libraries(TTMSqueezer
    PRIVATE
       CURL::libcurl
       OpenSSL::SSL
       OpenSSL::Crypto
       asio
)
