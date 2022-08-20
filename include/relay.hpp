template<bool isRequest, class SyncWriteStream, class SyncReadStream, class DynamicBuffer>
void relay(SyncWriteStream& output, SyncReadStream& input, DynamicBuffer& buffer, boost::beast::error_code& ec) {
    static_assert(boost::beast::is_sync_write_stream<SyncWriteStream>::value,
        "SyncWriteStream requirements not met");

    static_assert(boost::beast::is_sync_read_stream<SyncReadStream>::value,
        "SyncReadStream requirements not met");

    boost::beast::http::parser<isRequest, boost::beast::http::buffer_body> p;
    boost::beast::http::read_header(input, buffer, p, ec);
    if(ec) return;

    boost::beast::http::serializer<isRequest, boost::beast::http::buffer_body, boost::beast::http::fields> sr{p.get()};
    boost::beast::http::write_header(output, sr, ec);
    if(ec) return;
}
