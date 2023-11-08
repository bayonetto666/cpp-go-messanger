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
#include "DBClient"

class Auth {
public:
    Auth(const std::string& secret_key);

    bool authUser(const std::string& username, const std::string& password, std::string& error);
    
    bool registerUser(const std::string& username, const std::string& password, std::string& error);
    
    std::string getSubject(const std::string& token);
    
private:
    using traits = jwt::traits::nlohmann_json;
    using claim = jwt::basic_claim<traits>;

    DBClient _db;

    std::string _secretKey;
    
    std::pair<bool, std::string> verifyJWT(const std::string& token);
    
    std::string generateJWT(const std::string& subject);
    
    bool validatePassword(const std::string& password, std::string& error);
    bool validateUsername(const std::string& username, std::string& error);
    
    
};
