#pragma once

#include "argon2.h"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <sstream>
#include <cstring>

class Hasher {
public:
  static std::pair<std::string, std::string> hashPassword(const std::string& password);
    
  static bool verifyPassword(const std::string& password, const std::string& hashedPassword, const std::string& salt) ;

private:
  static void generateRandomSalt(uint8_t salt[], size_t saltLength);

  static std::string toHex(uint8_t value);
  static std::vector<uint8_t> fromHex(const std::string& hexStr);

  static const size_t HASHLEN = 32;
  static const size_t SALTLEN = 16;
};
