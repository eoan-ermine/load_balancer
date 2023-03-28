#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include <load_balancer/server/extensions/http/http_relay.hpp>
#include <load_balancer/server/request_info.hpp>
#include <load_balancer/server/target_info.hpp>
#include <load_balancer/server/transports/common_http_transport.hpp>
#include <load_balancer/server/transports/http_transport.hpp>
#include <load_balancer/server/transports/https_transport.hpp>

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace beast = boost::beast;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
namespace http = beast::http;

namespace eoanermine {

namespace load_balancer {

class HTTPExtension {
  net::io_context &ioc;
  ssl::context ctx{ssl::context::tlsv12_client};

public:
  HTTPExtension(net::io_context &ioc) : ioc(ioc) {
    ctx.set_default_verify_paths();
    ctx.set_verify_mode(ssl::verify_peer);
  }
  void session(std::shared_ptr<CommonHTTPTransport> client_transport,
               std::shared_ptr<CommonHTTPTransport> target_transport,
               TargetInfo &target_info, net::yield_context yield) {
    beast::error_code ec;
    beast::flat_buffer buffer;
    buffer.reserve(2048);

    relay<true>(client_transport, target_transport, buffer, ec, yield,
                [&target_info](auto &message) {
                  message.set("Host",
                              boost::string_view{target_info.domain.data(),
                                                 target_info.domain.size()});
                  message.keep_alive(true);
                });
    relay<false>(target_transport, client_transport, buffer, ec, yield);

    client_transport->get_stream().socket().shutdown(tcp::socket::shutdown_send,
                                                     ec);
    target_transport->disconnect(ec, yield);
  }
  void listen(tcp::endpoint endpoint, std::shared_ptr<Algorithm> &algorithm,
              net::yield_context yield) {
    constexpr int PERMISSION_DENIED_CODE = 13;
    constexpr int ALREADY_IN_USE_CODE = 98;

    beast::error_code ec;
    tcp::acceptor acceptor{ioc};
    acceptor.open(endpoint.protocol(), ec);
    if (ec)
      return fail(ec, "open");

    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
      return fail(ec, "set_option");

    acceptor.bind(endpoint, ec);
    if (ec) {
      switch (ec.value()) {
      case PERMISSION_DENIED_CODE:
        std::cerr << "bind: Permission denied" << '\n';
        std::cerr << "Ports below 1024 are considered privileged and can only "
                     "be bound to with an root user\n";
        std::cerr << "Please, try executing program as root or with sudo\n";
        break;
      case ALREADY_IN_USE_CODE:
        std::cerr << "bind: Address already in use" << '\n';
        std::cerr << "Port you are trying to allocate is already in use\n";
        std::cerr
            << "Please, stop the process which uses that port and try again\n";
        break;
      }
      std::exit(EXIT_FAILURE);
    }

    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec)
      return fail(ec, "listen");

    while (true) {
      tcp::socket client_socket{ioc};
      acceptor.async_accept(client_socket, yield[ec]);
      if (ec)
        return fail(ec, "accept");

      net::ip::tcp::endpoint client_endpoint = client_socket.remote_endpoint();
      net::ip::address client_address = client_endpoint.address();

      const TargetInfo &nextTarget =
          algorithm->getNext(RequestInfo{client_address});

      std::shared_ptr<CommonHTTPTransport> client_transport =
          std::make_shared<HTTPTransport>(
              beast::tcp_stream(std::move(client_socket)));
      std::shared_ptr<CommonHTTPTransport> target_transport;
      if (nextTarget.use_https) {
        target_transport = std::make_shared<HTTPSTransport>(
            beast::ssl_stream<beast::tcp_stream>(ioc, ctx));
      } else {
        target_transport =
            std::make_shared<HTTPTransport>(beast::tcp_stream(ioc));
      }
      target_transport->connect(nextTarget, ec, yield);

      boost::asio::spawn(acceptor.get_executor(),
                         std::bind(&HTTPExtension::session, this,
                                   client_transport, target_transport,
                                   std::move(nextTarget),
                                   std::placeholders::_1));
    }
  }
};

} // namespace load_balancer

} // namespace eoanermine