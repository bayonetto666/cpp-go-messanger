#pragma once

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ws = beast::websocket;
namespace ip = asio::ip;
using tcp = asio::ip::tcp;

class websocket_proxy : public std::enable_shared_from_this<websocket_proxy> {
public:
  websocket_proxy(asio::io_context& context, ws::stream<tcp::socket>& clientWs, ws::stream<tcp::socket>& serverWs);
  ~websocket_proxy();
  
  void run();

  void close();

private:
  asio::io_context& _context;
  // ws::stream<tcp::socket>& _clientWs;
  // ws::stream<tcp::socket>& _serverWs;
  ws::stream<tcp::socket> _clientWs;
  ws::stream<tcp::socket> _serverWs;

  beast::flat_buffer _serverBuffer;
  beast::flat_buffer _clientBuffer;

  void _readFromServer();
  void _readFromClient();
  void _writeToServer();
  void _writeToClient();
};
