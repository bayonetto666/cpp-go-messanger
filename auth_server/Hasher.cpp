#include "Hasher.hpp"

std::pair<std::string, std::string> Hasher::hashPassword(const std::string& password) {
  uint8_t hash[HASHLEN];
  uint8_t salt[SALTLEN];
  generateRandomSalt(salt, SALTLEN);

  uint8_t *pwd = (uint8_t *)password.c_str();
  uint32_t pwdlen = static_cast<uint32_t>(password.length());
  uint32_t t_cost = 2;            // 1-pass computation
  uint32_t m_cost = (1 << 16);    // 64 mebibytes memory usage
  uint32_t parallelism = 1;       // number of threads and lanes

  argon2i_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, salt, SALTLEN, hash, HASHLEN);
  std::string hashStr, saltStr;
  for (int i = 0; i < HASHLEN; i++) {
    hashStr += toHex(hash[i]);
  }
  for (int i = 0; i < SALTLEN; i++) {
    saltStr += toHex(salt[i]);
  }

  return std::make_pair(hashStr, saltStr);
}

bool Hasher::verifyPassword(const std::string& password, const std::string& hashedPassword, const std::string& salt) {
  std::vector<uint8_t> saltBytes = fromHex(salt);
  std::vector<uint8_t> savedHashBytes = fromHex(hashedPassword);
  uint8_t computedHash[HASHLEN];
  uint8_t *pwd = (uint8_t *)password.c_str();
  uint32_t pwdlen = static_cast<uint32_t>(password.length());
  uint32_t t_cost = 2;            // 1-pass computation
  uint32_t m_cost = (1 << 16);    // 64 mebibytes memory usage
  uint32_t parallelism = 1;       // number of threads and lanes
  argon2i_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, saltBytes.data(), SALTLEN, computedHash, HASHLEN);

  return (memcmp(computedHash, savedHashBytes.data(), HASHLEN) == 0);
}

void Hasher::generateRandomSalt(uint8_t salt[], size_t saltLength) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> distrib(0, 255);

  for (size_t i = 0; i < saltLength; i++) {
    salt[i] = distrib(gen);
  }
}

std::string Hasher::toHex(uint8_t value) {
  std::ostringstream oss;
  oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
  return oss.str();
}

std::vector<uint8_t> Hasher::fromHex(const std::string& hexStr) {
  std::vector<uint8_t> bytes;
  for (size_t i = 0; i < hexStr.length(); i += 2) {
    std::string byteString = hexStr.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
    bytes.push_back(byte);
  }
  return bytes;
}

 