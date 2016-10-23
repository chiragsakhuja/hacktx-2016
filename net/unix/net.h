#pragma once

#define SERVER_ADDR a-dev.me
#define SERVER_PORT 1221

class server
{
public:
    server()
    {
        socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socket_fd < 0) {
            throw std::runtime_error("cannot open socket_fd");
        }

        bzero((uint8_t *) &addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htons(INADDR_ANY);
        addr.sin_port = htons(SERVER_PORT);

        if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
            cout << strerror(errno) << endl;
            close(socket_fd);
            throw std::runtime_error("cannot bind socket");
        }

        t = std::thread{ &server::transcieve, this };
    } 

    ~server()
    {
        t.join();
        close(socket_fd);
    }

    void transcieve()
    {
        uint8_t msg[1024];
        socklen_t other_addr_len;
        int msg_size;
        while (true) {
            msg_size = recvfrom(socket_fd, msg, 1023, 0, (struct sockaddr *) &other_addr, &other_addr_len);
            if (msg_size < 0) {
                cout << strerror(errno) << endl;
                throw std::runtime_error("failed to recieve data");
            }
            msg[msg_size] = '\0';
            cout << msg << endl;
        }
    }

private:
    std::thread t;
    int socket_fd;
    sockaddr_in addr;
    sockaddr_in other_addr;
};
