/** Relay an HTTP message.

    This function efficiently relays an HTTP message from a downstream
    client to an upstream server, or from an upstream server to a
    downstream client. After the message header is read from the input,
    a user provided transformation function is invoked which may change
    the contents of the header before forwarding to the output. This may
    be used to adjust fields such as Server, or proxy fields.

    @param output The stream to write to.

    @param input The stream to read from.

    @param buffer The buffer to use for the input.

    @param transform The header transformation to apply. The function will
    be called with this signature:
    @code
        template<class Body>
        void transform(message<
            isRequest, Body, Fields>&,  // The message to transform
            error_code&);               // Set to the error, if any
    @endcode

    @param ec Set to the error if any occurred.

    @tparam isRequest `true` to relay a request.

    @tparam Fields The type of fields to use for the message.
*/
template<
    bool isRequest,
    class SyncWriteStream,
    class SyncReadStream,
    class DynamicBuffer,
    class Transform>
void
relay(
    SyncWriteStream& output,
    SyncReadStream& input,
    DynamicBuffer& buffer,
    error_code& ec,
    Transform&& transform)
{
    static_assert(is_sync_write_stream<SyncWriteStream>::value,
        "SyncWriteStream requirements not met");

    static_assert(is_sync_read_stream<SyncReadStream>::value,
        "SyncReadStream requirements not met");

    // A small buffer for relaying the body piece by piece
    char buf[2048];

    // Create a parser with a buffer body to read from the input.
    parser<isRequest, buffer_body> p;

    // Create a serializer from the message contained in the parser.
    serializer<isRequest, buffer_body, fields> sr{p.get()};

    // Read just the header from the input
    read_header(input, buffer, p, ec);
    if(ec)
        return;

    // Apply the caller's header transformation
    transform(p.get(), ec);
    if(ec)
        return;

    // Send the transformed message to the output
    write_header(output, sr, ec);
    if(ec)
        return;

    // Loop over the input and transfer it to the output
    do
    {
        if(! p.is_done())
        {
            // Set up the body for writing into our small buffer
            p.get().body().data = buf;
            p.get().body().size = sizeof(buf);

            // Read as much as we can
            read(input, buffer, p, ec);

            // This error is returned when buffer_body uses up the buffer
            if(ec == error::need_buffer)
                ec = {};
            if(ec)
                return;

            // Set up the body for reading.
            // This is how much was parsed:
            p.get().body().size = sizeof(buf) - p.get().body().size;
            p.get().body().data = buf;
            p.get().body().more = ! p.is_done();
        }
        else
        {
            p.get().body().data = nullptr;
            p.get().body().size = 0;
        }

        // Write everything in the buffer (which might be empty)
        write(output, sr, ec);

        // This error is returned when buffer_body uses up the buffer
        if(ec == error::need_buffer)
            ec = {};
        if(ec)
            return;
    }
    while(! p.is_done() && ! sr.is_done());
}
