#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/config.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../include/relay.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void do_session(beast::tcp_stream& client, beast::tcp_stream& target, net::yield_context yield) {    
    beast::error_code ec;
    beast::flat_buffer buffer;

    relay<true>(target, client, buffer, ec, yield);
    
    http::response<http::dynamic_body> res;
    http::async_read(target, buffer, res, yield[ec]);
    if (ec) return fail(ec, "response read");

    http::async_write(client, res, yield[ec]);
    if (ec) return fail(ec, "response write");

    client.socket().shutdown(tcp::socket::shutdown_send, ec);
    target.close();
}

void do_listen(net::io_context& ioc, tcp::endpoint endpoint, std::pair<std::string, std::string> forward, net::yield_context yield) {
    beast::error_code ec;

    tcp::acceptor acceptor{ioc};
    acceptor.open(endpoint.protocol(), ec);
    if (ec)
        return fail(ec, "open");

    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
        return fail(ec, "set_option");

    acceptor.bind(endpoint, ec);
    if (ec)
        return fail(ec, "bind");

    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec)
        return fail(ec, "listen");

    tcp::resolver resolver(ioc);
    const auto results = resolver.async_resolve(forward.first, forward.second, yield[ec]);
    if (ec)
        return fail(ec, "async_resolve");

    while (true) {
        tcp::socket client_socket{ioc};
        acceptor.async_accept(client_socket, yield[ec]);
        if (ec)
            fail(ec, "accept");

        beast::tcp_stream target_stream{ioc};
        target_stream.async_connect(results, yield[ec]);
        if (ec)
            fail(ec, "connect");

        else
            boost::asio::spawn(
                acceptor.get_executor(), std::bind(
                    &do_session, beast::tcp_stream(std::move(client_socket)), std::move(target_stream), std::placeholders::_1
                )
            );
    }
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage:\n"
                  << "./a.out [listener_host] [listener_port] [target_host] [target_port]"
                  << std::endl;
        return EXIT_FAILURE;
    }

    const auto host = net::ip::make_address(argv[1]);
    const auto port = static_cast<unsigned short>(std::atoi(argv[2]));
    const auto target_host = argv[3], target_port = argv[4];

    unsigned int threads = std::thread::hardware_concurrency();
    net::io_context ioc{threads};

    boost::asio::spawn(ioc, std::bind(
        &do_listen, std::ref(ioc), tcp::endpoint{host, port}, std::make_pair(target_host, target_port), std::placeholders::_1
    ));

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i != 0; --i) {
        v.emplace_back([&ioc] { ioc.run(); });
    }
    ioc.run();

    return EXIT_SUCCESS;
}