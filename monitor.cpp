// Read one event off the monitor socket; return value and address
// by reference, if not null, and event number by value. Returns -1
// in case of error.

static int get_monitor_event (void *monitor, int *value, char **address)
{
    // First frame in message contains event number and value
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor, 0) == -1)
    {
        return -1; // Interrupted, presumably
    }
    assert (zmq_msg_more (&msg));

    uint8_t *data = (uint8_t *) zmq_msg_data (&msg);
    uint16_t event = *(uint16_t *) (data);

    if (value)
    {
        *value = *(uint32_t *) (data + 2);
    }
    // Second frame in message contains event address
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor, 0) == -1)
    {
        return -1; // Interrupted, presumably
    }
    assert (!zmq_msg_more (&msg));

    if (address) {
        uint8_t *data = (uint8_t *) zmq_msg_data (&msg);
        size_t size = zmq_msg_size (&msg);
        *address = (char *) malloc (size + 1);
        memcpy (*address, data, size);
        (*address)[size] = 0;

    }
    return event;
}

int main()
{
    void *ctx = zmq_ctx_new ();
    assert (ctx);
    // We'll monitor these two sockets
    void *client = zmq_socket (ctx, ZMQ_DEALER);
    assert (client);
    void *server = zmq_socket (ctx, ZMQ_DEALER);
    assert (server);

    rc = zmq_socket_monitor(server, "inproc://monitor-server", ZMQ_EVENT_ALL);
    assert(rc == 0);
    void *server_mon = zmq_socket (ctx, ZMQ_PAIR);
    assert (server_mon);
    rc = zmq_connect(server_mon, "inproc://monitor-server");
    assert(rc == 0);

    // Now do a basic ping test
    rc = zmq_bind (server, "tcp://127.0.0.1:9998");
    assert (rc == 0);

}
