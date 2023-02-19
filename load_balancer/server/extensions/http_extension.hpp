#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include <load_balancer/server/relay.hpp>
#include <load_balancer/server/target_info.hpp>
#include <load_balancer/server/transports/transport.hpp>
#include <load_balancer/server/transports/http_transport.hpp>
#include <load_balancer/server/transports/https_transport.hpp>

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
	HTTPExtension(net::io_context& ioc): ioc(ioc) {
		ctx.set_default_verify_paths();
		ctx.set_verify_mode(ssl::verify_peer);
	}
	void session(
		beast::tcp_stream &client_stream,
		std::shared_ptr<Transport> target_transport,
		TargetInfo &target_info, net::yield_context yield
	) {
		beast::error_code ec;
		beast::flat_buffer buffer;
		buffer.reserve(2048);

	  	relay(target_transport, client_stream, target_info, buffer, ec, yield);

		http::response<http::dynamic_body> response;
		target_transport->async_read(buffer, response, ec, yield);
		if (ec)
		return fail(ec, "response read");

		http::async_write(client_stream, response, yield[ec]);
		if (ec)
		return fail(ec, "response write");

		client_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
		target_transport->disconnect(ec, yield);
	}
	void listen(
		tcp::endpoint endpoint, std::shared_ptr<Algorithm> &algorithm,
		net::yield_context yield
	) {
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

			const TargetInfo &nextTarget = algorithm->getNext();

			std::shared_ptr<Transport> transport;
			if (nextTarget.use_https) {
			  transport = std::make_shared<HTTPSTransport>(
			      beast::ssl_stream<beast::tcp_stream>(ioc, ctx));
			} else {
			  transport = std::make_shared<HTTPTransport>(beast::tcp_stream(ioc));
			}
			transport->connect(nextTarget, ec, yield);

			boost::asio::spawn(
			    acceptor.get_executor(),
			    std::bind(&HTTPExtension::session, this, beast::tcp_stream(std::move(client_socket)),
			              transport, std::move(nextTarget), std::placeholders::_1));
		}
	}
};

} // namespace load_balancer

} // namespace eoanermine