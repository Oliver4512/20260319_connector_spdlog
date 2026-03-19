
#ifndef database_manager_hpp
#define database_manager_hpp

#include <mariadb/conncpp.hpp>
#include <memory>
#include <map>
#include <string>
#include <iostream>

class DatabaseManager {
public:
    // Pass the credentials here. If they match a connection we already have, 
    // it returns that one. If not, it creates a new one.
    static sql::Connection* getConnection(const std::string& url, 
                                         const std::string& user, 
                                         const std::string& pass) {
        
        // Use the URL + User as a unique key for this specific instance
        std::string key = url + user;

        // Check if we already have this connection open
        auto& instances = getInstances();
        if (instances.find(key) == instances.end()) {
            // Not found! Let's create it.
            instances[key] = createConnection(url, user, pass);
        }

        return instances[key].get();
    }

private:
    // This holds all our active connections
    static std::map<std::string, std::unique_ptr<sql::Connection>>& getInstances() {
        static std::map<std::string, std::unique_ptr<sql::Connection>> instances;
        return instances;
    }

    // The actual "Heavy Lifting" of connecting
    static std::unique_ptr<sql::Connection> createConnection(const std::string& url, 
                                                            const std::string& user, 
                                                            const std::string& pass) {
        try {
            sql::Driver* driver = sql::mariadb::get_driver_instance();
            sql::Properties properties({{"user", user}, {"password", pass}});
            
            std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
            std::cout << "Successfully connected to: " << url << " as " << user << std::endl;
            return conn;
        }
        catch (sql::SQLException& e) {
            std::cerr << "Failed to connect to " << url << ": " << e.what() << std::endl;
            return nullptr;
        }
    }
};



/* USAGE

void myApp() {
    // 1. Get the Test Database
    sql::Connection* testDb = DatabaseManager::getConnection(
        "jdbc:mariadb://localhost:3306/testdb", "oliver", "pass123"
    );

    // 2. Get a different Database (or same DB with different user)
    sql::Connection* prodDb = DatabaseManager::getConnection(
        "jdbc:mariadb://remote-server:3306/production", "admin", "secure_pass"
    );

    // 3. If I call this again, it DOES NOT create a new connection.
    // It simply returns the one already stored in the map!
    sql::Connection* sameTestDb = DatabaseManager::getConnection(
        "jdbc:mariadb://localhost:3306/testdb", "oliver", "pass123"
    );
}


*/


#endif