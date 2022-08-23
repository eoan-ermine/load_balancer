#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/config.hpp>
#include <boost/lockfree/queue.hpp>

#include <string_view>
#include <utility>
#include <thread>
#include <vector>

#include "relay.hpp"
#include "common.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace po = boost::program_options;

namespace eoanermine {

namespace load_balancer {

void do_session(beast::tcp_stream& client, beast::tcp_stream& target, net::yield_context yield) {
    beast::error_code ec;
    beast::flat_buffer buffer; buffer.reserve(2048);

    relay<true>(target, client, buffer, ec, yield);
    
    http::response<http::dynamic_body> res;
    http::async_read(target, buffer, res, yield[ec]);
    if (ec) return fail(ec, "response read");

    http::async_write(client, res, yield[ec]);
    if (ec) return fail(ec, "response write");

    client.socket().shutdown(tcp::socket::shutdown_send, ec);
    target.close();
}

void do_listen(net::io_context& ioc, tcp::endpoint endpoint, std::vector<tcp::resolver::results_type>& targets, boost::lockfree::queue<int, boost::lockfree::capacity<64>>& targetsIdx, net::yield_context yield) {
    beast::error_code ec;

    tcp::acceptor acceptor{ioc};
    acceptor.open(endpoint.protocol(), ec);
    if (ec) return fail(ec, "open");

    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) return fail(ec, "set_option");

    acceptor.bind(endpoint, ec);
    if (ec) return fail(ec, "bind");

    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec) return fail(ec, "listen");

    while (true) {
        tcp::socket client_socket{ioc};
        acceptor.async_accept(client_socket, yield[ec]);
        if (ec) return fail(ec, "accept");

        beast::tcp_stream target_stream{ioc};
        auto result = targetsIdx.consume_one([&](int value) {
            target_stream.async_connect(targets[value], yield[ec]);
            if (ec) return fail(ec, "connect");
            targetsIdx.push(value);
        });

        boost::asio::spawn(
            acceptor.get_executor(), std::bind(
                &do_session, beast::tcp_stream(std::move(client_socket)), std::move(target_stream), std::placeholders::_1
            )
        );
    }
}

class server {
    int threads;
    net::io_context ioc;
    std::vector<tcp::resolver::results_type> targets;
    boost::lockfree::queue<int, boost::lockfree::capacity<64>> targetsIdx;
public:
    server(int threads = std::thread::hardware_concurrency()): threads{threads}, ioc(threads) { }
    void run(std::string_view host, std::string_view port, std::vector<std::pair<std::string_view, std::string_view>> targets_addrs) {
        tcp::resolver resolver(ioc);
        for (std::size_t i = 0; i != targets_addrs.size(); ++i) {
            targets.push_back(resolver.resolve(targets_addrs[i].first, targets_addrs[i].second));
            targetsIdx.push(i);
        }

        auto endpoint = tcp::endpoint{net::ip::make_address(host), static_cast<unsigned short>(std::atoi(port.data()))};
        boost::asio::spawn(ioc, std::bind(
            &do_listen, std::ref(ioc), endpoint, std::ref(targets), std::ref(targetsIdx), std::placeholders::_1
        ));

        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i != 0; --i) {
            v.emplace_back([&] { ioc.run(); });
        }
        ioc.run();
    }
};

} // namespace load_balancer

} // namespace eoanermine
