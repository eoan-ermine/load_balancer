#include <boost/beast/core.hpp>
#include <iostream>

void fail(boost::beast::error_code ec, const char* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}
