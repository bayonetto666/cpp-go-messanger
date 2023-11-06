//
//  DatabaseHandler.hpp
//  Server
//
//  Created by Кірил on 19/10/2023.
//

#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <nlohmann/json.hpp>


class DatabaseHandler {
public:
    DatabaseHandler(const std::string& connection_uri);
    
    void insertUser(const nlohmann::json& userData, std::string& error);

    bool userExists(const std::string& username, std::string& error);

    std::pair<std::string,std::string> getPassword(const std::string& username, std::string& error);
    // bool verifyPassword(const std::string& username, const std::string& password);
    
    // void storeMessage(const std::string& sender, const std::string& recipient, const std::string& message, std::string& ex_what);
    void storeMessage(const nlohmann::json& message_json, std::string& error);


    nlohmann::json getMessages(const std::string& username, std::string& error);
    
private:
    mongocxx::client _client;
    mongocxx::database _db;
    mongocxx::collection _usersCollection;
    mongocxx::collection _messagesCollection;
    
    void markMessageAsRead(const std::string& messageId);
    
};
