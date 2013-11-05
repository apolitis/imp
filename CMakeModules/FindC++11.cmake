if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE
		        GCC_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "GCC version: ${GCC_VERSION}")
if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
message(STATUS "Enabling g++ C++11 support")
add_definitions("--std=c++11")
elseif (GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
message(STATUS "Enabling g++ C++0x support")
add_definitions("--std=c++0x")
endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
message(STATUS "Enabling clang C++11 support")
add_definitions("--std=c++11")
endif()