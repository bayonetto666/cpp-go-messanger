#include "Auth.hpp"

std::string Auth::generateJWT(const std::string& subject) {
    try {
        auto token = jwt::create<traits>()
            .set_issuer("Auth server 0.2")
            .set_subject(subject)
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes(30))
            .sign(jwt::algorithm::hs256{_secretKey});

        std::cout << "Created JWT-token: " << token << std::endl;
        return token;
    } catch (const std::exception& e) {
        std::cout << e.what();
        return "";
    }
}

Auth::Auth(const std::string &secret_key) : _secretKey(secret_key)
{
}

bool Auth::authUser(const std::string &username, const std::string &password, std::string &error)
{
    std::string ec;

    auto password_data = _db.GetPassword(username);

    bool verified = Hasher::verifyPassword(password, password_data.first, password_data.second);

    return verified;
}

bool Auth::registerUser(const std::string& username, const std::string& password,  std::string& error){
    /*....*/
    std::string ec;
    if (!validateUsername(username, ec)){
        error = ec;
        return false;
    }
    if (!validatePassword(password, ec)){
        error = ec;
        return false;
    }

    std::pair<std::string, std::string> hashed_password = Hasher::hashPassword(password);

    // nlohmann::json userData;
    // userData["username"] = username;
    // userData["password"] = hashed_password.first;
    // userData["salt"] = hashed_password.second;
    //отправка на сервер БД
    _db.insertUser(username, hashed_password.first, hashed_password.second);

    return true;
}

std::pair<bool, std::string> Auth::verifyJWT(const std::string& token) {
    std::string error;
    try {
        jwt::decoded_jwt decoded = jwt::decode(token);
        jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{_secretKey})
        .verify(decoded);

        std::cout << "JWT-token \"" << token << "\" was verified." << std::endl;
        return std::make_pair(true, "");
        } catch (const std::exception& e) {
            std::cerr << "Error JWT-token verification: " << e.what() << std::endl;
            error = e.what();
        }
        return std::make_pair(false, error);
}

std::string Auth::getSubject(const std::string& token) {
    try {
        const auto decoded = jwt::decode<traits>(token);

        // Проверьте, что токен был подписан с использованием правильного секретного ключа
        jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{_secretKey})
            .verify(decoded);

        const auto subject = decoded.get_subject();
        return subject;
       } catch (const std::exception& e) {
           std::cerr << "Error while processing JWT token: " << e.what() << std::endl;
           return "";
       }
}

bool Auth::validatePassword(const std::string& password, std::string& error){
    if (password.size() < 5 || password.size() > 15) {
        error = "Password length must be between 5 and 15 characters.";
        return false;
    }
    if (std::any_of(password.begin(), password.end(), ::isspace)) {
        error = "Password must not contain whitespace characters.";
        return false;
    }
    return true;
}
bool Auth::validateUsername(const std::string& username, std::string& error){
    if (username.size() < 5 || username.size() > 15 ) {
        error = "Username length must be between 5 and 15 characters.";
        return false;
    }
    if(_db.UserExists(username)){
        error = "Username is already in use.";
        return false;
    }
    return true;
}
