cmake_minimum_required(VERSION 3.10)
project(CMAKE_CURRENT_LIST_DIR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find Paho pakken på systemet
# Dette erstatter både find_library og find_path
find_package(PahoMqttCpp REQUIRED)
find_package(spdlog REQUIRED)

# Find biblioteket manuelt
find_library(MARIADBCPP_LIB NAMES mariadbcpp)
# Find header-stien
find_path(MARIADBCPP_INCLUDE_DIR NAMES mariadb/conncpp.hpp)

if(NOT MARIADBCPP_LIB OR NOT MARIADBCPP_INCLUDE_DIR)
    message(FATAL_ERROR "Kunne ikke finde MariaDB Connector C++. Tjek installationen!")
endif()


# --- APP ---
add_executable(exec ./mariaDBinsert.cpp)
#target_include_directories(exec PUBLIC src ${MARIADBCPP_INCLUDE_DIR})
target_link_libraries(exec PRIVATE PahoMqttCpp::paho-mqttpp3 PahoMqttCpp::paho-mqttpp3 ${MARIADBCPP_LIB}  spdlog::spdlog)


# sudo systemctl stop mariadb
# mariadb_safe skip-safe-tables