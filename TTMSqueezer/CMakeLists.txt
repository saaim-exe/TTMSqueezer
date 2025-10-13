# CMakeList.txt : CMake project for TTMSqueezer, include source and define
# project specific logic here.
#


add_executable(TTMSqueezer
src/main.cpp
src/ttm_calculator.cpp
data/yfinance_client.cpp "data/include/parse_json.h" "data/parse_json.cpp")

target_include_directories(TTMSqueezer
	PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/data/include
)

file(GLOB_RECURSE DATA_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/data/include/*.h")
source_group("Header Files\\Data" FILES ${DATA_HEADERS})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TTMSqueezer PROPERTY CXX_STANDARD 20)
endif()

find_package(CURL REQUIRED)

target_link_libraries(TTMSqueezer CURL::libcurl)