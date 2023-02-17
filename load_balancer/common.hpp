#pragma once

#include <boost/beast/core.hpp>
#include <iostream>

namespace eoanermine {

namespace load_balancer {

void fail(boost::beast::error_code ec, const char* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

} // namespace load_balancer

} // namespace eoanermine