#include <string>
#include <iostream>
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

/*
 * * 将客户端请求公平的转发到Dealer。
 * * 然后由Dealer内部负载均衡的派发任务到各个Worker。
 * */
int doRouter(void* router, void* dealer)
{
    while (true) {
        int64_t more = 1;
        zmq_msg_t message;
        zmq_msg_init(&message);
        int rc = zmq_msg_recv(&message, router, 0);
        size_t more_size = sizeof(more);
        zmq_getsockopt(router, ZMQ_RCVMORE, &more, &more_size);
        zmq_msg_send(&message, dealer, more ? ZMQ_SNDMORE : 0);
        printf("[%ld] router deliver request to dealer. rc = %d, more = %ld\n", time(NULL), rc, more);
        zmq_msg_close(&message);
        if (!more) {
            break; // Last message part
        }
    }

    printf("[%ld]----------DoRouter----------\n\n", time(NULL));
    return 0;
}

/*
 * * Dealer将后端Worker的应答数据转发到Router。
 * * 然后由Router寻址将应答数据准确的传递给对应的client。
 * * 值得注意的是，Router对client的寻址方式，得看client的‘身份’。
 * * 临时身份的client，Router会为其生成一个uuid进行标识。
 * * 永久身份的client，Router直接使用该client的身份。
 * */
int doDealer(void* router, void* dealer)
{
    while (1) {
        int64_t more = 1;
        zmq_msg_t message;
        // Process all parts of the message
        zmq_msg_init(&message);
        int rc = zmq_msg_recv(&message, dealer, 0);
        size_t more_size = sizeof (more);
        zmq_getsockopt(dealer, ZMQ_RCVMORE, &more, &more_size);
        zmq_msg_send(&message, router, more ? ZMQ_SNDMORE : 0);
        printf("[%ld] dealer deliver reply to router. rc = %d, more = %ld\n", time(NULL), rc, more);
        zmq_msg_close(&message);
        if (!more) {
            break; // Last message part
        }
    }

    printf("[%ld]----------DoDealer----------\n\n", time(NULL));
    return 0;
}

int main(int argc, char *argv[])
{
    void* context = zmq_ctx_new();
    zmq_ctx_set(context, ZMQ_IO_THREADS, 3);
    void* socket = zmq_socket(context, ZMQ_REP);
    std::string address = "tcp://127.0.0.1:";
    address.append(argv[1]);
    int rc = zmq_bind(socket, address.c_str());
    assert(rc == 0);

    while (true) {
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, 6);
        zmq_msg_recv(&msg, socket, 0);
        std::cout << "msg: \n" << (char*)zmq_msg_data(&msg) << std::endl;
        sleep(1);
        zmq_msg_send(&msg, socket, 0);
        zmq_close(&msg);
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
