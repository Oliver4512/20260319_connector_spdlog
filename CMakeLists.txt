cmake_minimum_required(VERSION 3.10)
project(MQTT_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find Paho pakken på systemet
# Dette erstatter både find_library og find_path
find_package(PahoMqttCpp REQUIRED)

# Find biblioteket manuelt
find_library(MARIADBCPP_LIB NAMES mariadbcpp)
# Find header-stien
find_path(MARIADBCPP_INCLUDE_DIR NAMES mariadb/conncpp.hpp)

if(NOT MARIADBCPP_LIB OR NOT MARIADBCPP_INCLUDE_DIR)
    message(FATAL_ERROR "Kunne ikke finde MariaDB Connector C++. Tjek installationen!")
endif()


# --- APP ---
add_executable(mqtt_sub ./mariaDBinsert.cpp)
target_include_directories(mqtt_sub PUBLIC src ${MARIADBCPP_INCLUDE_DIR})
target_link_libraries(mqtt_sub PRIVATE PahoMqttCpp::paho-mqttpp3 PahoMqttCpp::paho-mqttpp3 ${MARIADBCPP_LIB})
