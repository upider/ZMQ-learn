#include <iostream>
#include <thread>

#include <unistd.h>
#include <assert.h>

#include <zmq.h>

class A {
    public:
        void hello() {
            std::cout << "hello" << std::endl;
        }
};

int main()
{
    A* a = new A();
    int size = sizeof(a);

    void* ctx = zmq_ctx_new();
    void* client = zmq_socket(ctx, ZMQ_DEALER);
    void* server = zmq_socket(ctx, ZMQ_DEALER);

    int rc = zmq_bind(server, "inproc://#1");
    assert(rc);
    rc = zmq_connect(server, "inproc://#1");
    assert(rc);

    std::thread tClient([&a, = ]() {
        char* buf = static_cast<char*>(a);
        zmq_send(client, buf, size, 0)
    });

    std::thread tServer([ = ]() {
        char* buf = new char[size];
        int n = zmq_recv(server, buf, size, 0);
        A b = static_cast<A>(buf);
        assert (n != -1);
    });

    sleep(100);
    tClient.join();
    tServer.join();
}
