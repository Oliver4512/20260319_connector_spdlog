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

#include "spdlog/sinks/rotating_file_sink.h"
void rotating_example()
{
  // Create a file rotating logger with 5 MB size max and 3 rotated files
  auto max_size = 1048576 * 5;
  auto max_files = 3;
  auto logger = spdlog::rotating_logger_mt("some_logger_name", "logs/rotating.txt", max_size, max_files);

  logger->warn("warning-----------");
}

int main(int argc, char *argv[])
{
  rotating_example();


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
