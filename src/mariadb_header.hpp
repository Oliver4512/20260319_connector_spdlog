
#include <mariadb/conncpp.hpp>
//#include <stdio.h>
//#include <time.h>
#include "database_manager_class.hpp"

#if __has_include("pass.hpp")
#include "pass.hpp"
#else
std::string pass = "password";
#endif

// pass is refering to hidden variable with password
sql::Connection *testDb = DatabaseManager::getConnection("jdbc:mariadb://localhost:3306/testdb", "oliver", pass);

using namespace std;

const std::string DFLT_SERVER_URI("mqtt://localhost:1883");
const std::string TOPIC("#");

const int QOS = 1;
const int N_RETRY_ATTEMPTS = 5;

// Variable CLIENT_ID is placed in functions.hpp because of dependency on function: generate_client_id()

// Get the current date/time. The format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about the date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

// Function saves the topic and value in the users table
bool saveUser(sql::Connection *database, string username = "anon", string email = "none@none", int age = 0)
{
    const std::string sql = "INSERT INTO users (username, email, age) VALUES (?, ?, ?)";

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmnt(database->prepareStatement(sql));

        stmnt->setString(1, username);
        stmnt->setString(2, email);
        stmnt->setInt(3, age);

        stmnt->executeUpdate();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return false;
    }
}

// Function saves the topic and value in the temperature table
bool saveToTempTable(sql::Connection *database, string topic = "none/none", string value = "NULLstr", int id = 0, string timestamp = "0")
{
    const std::string sql = "INSERT INTO temperature (id, topic, value, timestamp) VALUES (?, ?, ?, ?)";

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmnt(database->prepareStatement(sql));

        stmnt->setInt(1, id);
        stmnt->setString(2, topic);
        stmnt->setString(3, value);
        stmnt->setString(4, currentDateTime());

        stmnt->executeUpdate();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * Generic function to save 4-column data (id, topic, value, timestamp)
 * to a specified MariaDB table.
 */
bool saveToDatabase(sql::Connection *conn, std::string tableName, std::string topic = "none/none", std::string value = "NULLstr", int id = 0)
{
    // Construct the query string dynamically
    // Note: Table names cannot be bound with '?' placeholders in SQL
    std::string sql = "INSERT INTO " + tableName + " (id, topic, value, timestamp) VALUES (?, ?, ?, ?)";

    try
    {
        std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement(sql));

        stmnt->setInt(1, id);
        stmnt->setString(2, topic);
        stmnt->setString(3, value);
        stmnt->setString(4, currentDateTime()); // Assuming this returns a string

        stmnt->executeUpdate();
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "Database error in table [" << tableName << "]: " << e.what() << std::endl;
        return false;
    }
}
