#pragma once

#define JWT_DISABLE_PICOJSON

#include <iostream>
#include <string>
#include <chrono>

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include <nlohmann/json.hpp>


#include "Hasher.hpp"
#include "DBClient.hpp"

class Auth {
public:
    Auth(const std::string& db_address, const std::string& secret_key);

    std::string authUser(const std::string& username, const std::string& password, std::string& error);
    
    bool registerUser(const std::string& username, const std::string& password, std::string& error);
    
    std::string getSubject(const std::string& token, std::string& error);
    
    bool verifyJWT(const std::string& token, std::string& error);
    
private:
    using traits = jwt::traits::nlohmann_json;
    using claim = jwt::basic_claim<traits>;

    DBClient _db;

    std::string _secretKey;
    
    std::string generateJWT(const std::string& subject);
    
    bool validatePassword(const std::string& password, std::string& error);
    bool validateUsername(const std::string& username, std::string& error);
    
    
};
