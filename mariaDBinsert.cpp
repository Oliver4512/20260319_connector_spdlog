#include <mariadb/conncpp.hpp>
#include "spdlog/spdlog.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#if __has_include("pass.hpp")
#include "pass.hpp"
#else
std::string pass = "password";
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h" // <-- til konsol
#include <vector>

int main(int argc, char *argv[])
{
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/logexample", true); // true = overskriv ved opstart

  spdlog::logger logger("logexample", {console_sink, file_sink});
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));

  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);

  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
  spdlog::info("Support for floats {:03.2f}", 1.23456);
  spdlog::info("Positional args are {1} {0}..", "too", "supported");
  spdlog::info("{:<30}", "left aligned");

  spdlog::set_level(spdlog::level::debug); // Set global log level to debug
  spdlog::debug("This message should be displayed..");

  // change log pattern
  spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

  // Compile time log levels
  // Note that this does not change the current log level, it will only
  // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
  SPDLOG_TRACE("Some trace message with param {}", 42);
  SPDLOG_DEBUG("Some debug message");

  try
  {
    sql::Driver *driver = sql::mariadb::get_driver_instance();
    sql::SQLString url("jdbc:mariadb://127.0.0.1:3306/iot");
    sql::Properties properties({{"user", "root"}, {"password", pass}});
    std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));

    try
    {
      conn->setAutoCommit(false);
      std::shared_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(
          "INSERT INTO devices(DeviceID, Temperatur) VALUES ('indoortemp', 30)"));
      stmnt->executeUpdate();
      conn->commit();
    }

    catch (sql::SQLException &e)
    {
      std::cerr << "Error updating contact with a transaction: " << e.what() << std::endl;
      conn->rollback();
    }
    conn->close();
  }

  catch (sql::SQLException &e)
  {
    std::cerr << "Error connecting to the database: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
