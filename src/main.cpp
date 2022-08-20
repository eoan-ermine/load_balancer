#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "../include/relay.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void do_session(tcp::socket& socket, beast::tcp_stream& output) {    
    beast::error_code ec;
    beast::flat_buffer buffer;

    relay<true>(output, socket, buffer, ec);
    
    http::response<http::dynamic_body> res;
    http::read(output, buffer, res, ec);
    http::write(socket, res, ec);

    socket.shutdown(tcp::socket::shutdown_send, ec);
    output.close();
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage:\n"
                  << "./a.out [listener_host] [listener_port] [target_host] [target_port]"
                  << std::endl;
        return 1;
    }

    const auto host = net::ip::make_address(argv[1]);
    const auto port = static_cast<unsigned short>(std::atoi(argv[2]));
    const auto target_host = argv[3], target_port = argv[4];

    net::io_context ioc{1};
    tcp::acceptor acceptor{ioc, {host, port}};

    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(target_host, target_port);

    while (true) {
        tcp::socket socket{ioc};
        beast::tcp_stream stream(ioc);

        acceptor.accept(socket);
        stream.connect(results);

        std::thread(std::bind(&do_session, std::move(socket), std::move(stream))).detach();
    }
}