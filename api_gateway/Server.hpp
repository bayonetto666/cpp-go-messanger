#pragma once

#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>


#include "AuthClient.hpp"
#include "DBClient.hpp"
#include "ChatClient.hpp"

namespace asio = boost::asio;
namespace ip = asio::ip;
using tcp = ip::tcp;
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace ws = beast::websocket;

class Server {
public:
    Server();
    
    void listen();

    // void stop();
    
private:
    asio::io_context _context;
    asio::ip::tcp::socket _serverSocket;
    asio::ip::tcp::endpoint _serverEndpoint;
    asio::ip::tcp::acceptor _acceptor;
    
    // asio::ip::tcp::acceptor _wsAcceptor;

    bool _isRunning = false;
    std::string _secretKey;

    AuthClient _auth;
    DBClient _db;
    ChatClient _chat;
    
    void handleRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket,
        const asio::ip::tcp::endpoint& clientEndpoint);
    
    void handleSendMessageRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket,
        const asio::ip::tcp::endpoint& clientEndpoint);
    void handleRegisterRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket);
    void handleLoginRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket);
    void handleGetMessagesRequest(const http::request<http::string_body>& request, asio::ip::tcp::socket& clientSocket);
//    void sendMessage(asio::ip::tcp::socket& recipient_socket, const std::string& message, const std::string& sender);
    bool parseJson(const std::string& body, nlohmann::json& parsedJson, std::string& ex_what);
    void sendErrorResponse(asio::ip::tcp::socket& clientSocket, const http::status& errorStatus, const std::string& errorMessage, const unsigned short& version);
    void handleClient(asio::ip::tcp::socket& clientSocket, const asio::ip::tcp::endpoint& clientEndpoint);

    void handleWebSocketConnection(std::shared_ptr<ws::stream<tcp::socket>> clientWs,const std::string& room_id);
    // void proxyData(ws::stream<tcp::socket>& clientWs, ws::stream<tcp::socket>& serverWs);
    void proxyWebSocketData(ws::stream<tcp::socket>& fromWs, ws::stream<tcp::socket>& toWs);
};
