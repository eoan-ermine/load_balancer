#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/lockfree/queue.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../include/relay.hpp"
#include "../include/common.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace po = boost::program_options;

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

void do_listen(net::io_context& ioc, tcp::endpoint endpoint, std::vector<tcp::resolver::results_type>& targets, boost::lockfree::queue<int>& targetsIdx, net::yield_context yield) {
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

    while (true) {
        tcp::socket client_socket{ioc};
        acceptor.async_accept(client_socket, yield[ec]);
        if (ec)
            fail(ec, "accept");

        beast::tcp_stream target_stream{ioc};
        auto result = targetsIdx.consume_one([&](int value) {
            target_stream.async_connect(targets[value], yield[ec]);
            if (ec)
                fail(ec, "connect");
            targetsIdx.push(value);
        });

        boost::asio::spawn(
            acceptor.get_executor(), std::bind(
                &do_session, beast::tcp_stream(std::move(client_socket)), std::move(target_stream), std::placeholders::_1
            )
        );
    }
}

bool validate_arguments(po::variables_map& vm) {
    if (!vm.count("host") || !vm.count("port") || !vm.count("target_host") || !vm.count("target_port"))
        return false;
    if (vm["target_host"].as<std::vector<std::string>>().size() != vm["target_port"].as<std::vector<std::string>>().size())
        return false;
    return true;
}

int main(int argc, char* argv[]) {
    std::string listener_host, listener_port;
    std::vector<std::string> target_host, target_port;
    po::options_description desc("Allowed options");

    po::positional_options_description pd;
    pd.add("host", 1).add("port", 1);

    desc.add_options()
        ("help", "produce help message")
        ("host", po::value<std::string>(&listener_host), "listener host address")
        ("port", po::value<std::string>(&listener_port), "listener port")
        ("target_host", po::value<std::vector<std::string>>(&target_host), "target host address")
        ("target_port", po::value<std::vector<std::string>>(&target_port), "target port");
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(pd).run(), vm);
    po::notify(vm);

    if (vm.count("help") || !validate_arguments(vm)) {
        std::cout << "Usage: load_balancer HOST PORT TARGETS" << '\n';
        std::cout << desc;
        return vm.count("help") ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    unsigned int threads = std::thread::hardware_concurrency();
    net::io_context ioc{threads};

    tcp::resolver resolver(ioc);
    std::vector<tcp::resolver::results_type> targets;
    boost::lockfree::queue<int> targetsIdx(target_host.size());
    for (std::size_t i = 0; i != target_host.size(); ++i) {
        targets.push_back(resolver.resolve(target_host[i], target_port[i]));
        targetsIdx.push(i);
    }

    const auto host = net::ip::make_address(listener_host);
    const auto port = static_cast<unsigned short>(std::stoi(listener_port));
    boost::asio::spawn(ioc, std::bind(
        &do_listen, std::ref(ioc), tcp::endpoint{host, port}, std::ref(targets), std::ref(targetsIdx), std::placeholders::_1
    ));

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i != 0; --i) {
        v.emplace_back([&ioc] { ioc.run(); });
    }
    ioc.run();

    return EXIT_SUCCESS;
}