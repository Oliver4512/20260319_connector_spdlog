
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include <random>
#include <algorithm>

#include <mariadb/conncpp.hpp>
#include <iostream>

#include <stdio.h>
#include <time.h>

#include "mqtt/async_client.h"
#include "pass.hpp"
#include "database_manager_class.hpp"

using namespace std;

const std::string DFLT_SERVER_URI("mqtt://localhost:1883");
const std::string TOPIC("#");

const int QOS = 1;
const int N_RETRY_ATTEMPTS = 5;

// pass is refering to hidden variable with password
sql::Connection *testDb = DatabaseManager::getConnection("jdbc:mariadb://localhost:3306/testdb", "oliver", pass);

// Variable CLIENT_ID is placed in functions.hpp because of dependency on function: generate_client_id()
