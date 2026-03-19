#include "variables.hpp"

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

// Function generates a semi random client id to avoid blocking from same id
std::string generate_client_id(const std::string &prefix)
{
    const std::string chars = "0123456789abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, chars.size() - 1);

    std::string id = prefix + "_";
    for (int i = 0; i < 8; ++i)
    {
        id += chars[distribution(generator)];
    }
    std::cout << id << std::endl;
    return id;
}

const std::string CLIENT_ID = generate_client_id("paho_cpp");

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

class action_listener : public virtual mqtt::iaction_listener
{
    std::string name_;

    void on_failure(const mqtt::token &tok) override
    {
        std::cout << name_ << " failure";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        std::cout << std::endl;
    }

    void on_success(const mqtt::token &tok) override
    {
        std::cout << name_ << " success";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        auto top = tok.get_topics();
        if (top && !top->empty())
            std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
        std::cout << std::endl;
    }

public:
    action_listener(const std::string &name) : name_(name) {}
};

void handleMessage(const mqtt::const_message_ptr msg, sql::Connection *testDb)
{
    std::string topic = msg->get_topic();
    std::string payload = msg->to_string();

    if (topic == "calc/median_temperature")
    {
        saveToDatabase(testDb, "temperature_test", topic, payload);
    }
/*     else if (topic == "CO2_SCD40/CO2ppm")
    {
        saveToDatabase(testDb, "co2", topic, payload);
    }
    else if (topic == "CO2_SCD40/humidity")
    {
        saveToDatabase(testDb, "humidity", topic, payload);
    }
    else if (topic == "bmp_forced/pressure")
    {
        saveToDatabase(testDb, "pressure", topic, payload);
    }
    else if (topic == "ens160/tvoc")
    {
        saveToDatabase(testDb, "tvoc", topic, payload);
    }
    else if (topic == "mq_manual/air_quality_original")
    {
        saveToDatabase(testDb, "airq", topic, payload);
    } */
    else
    {
        std::cout << "Unknown topic received: " << topic << std::endl;
        //saveToDatabase(testDb, "log", topic, payload);
    }
}
