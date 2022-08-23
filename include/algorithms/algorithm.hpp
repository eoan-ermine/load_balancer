#pragma once

#include <vector>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace eoanermine {

namespace load_balancer {

class Algorithm {
public:
	virtual boost::asio::ip::tcp::resolver::results_type& getNext() = 0; 
	virtual ~Algorithm() { }
};

} // namespace load_balancer

} // namespace eoanermine