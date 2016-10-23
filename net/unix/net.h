#pragma once

#define SERVER_ADDR a-dev.me
#define SERVER_PORT 1221

struct game_t
{
    enum status_enum {
        idle,
        play
    };

    struct player_t
    {
        struct pos_t
        {
            float x = 0.0f;
            float y = 0.0f;
        };

        struct vec_t
        {
            float x = 0.0f;
            float y = 0.0f;
        };

        sockaddr_in addr[2];
        bool joined[0];
        pos_t pos;
        vec_t vec;
    };

    struct ball_t
    {
        struct vec_t
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
        };

        struct acc_t
        {
            float x = 0.0f;
            float y = 0.0f;
        };

        vec_t vec;
        acc_t acc;
    };

    uint8_t status = status_enum::idle;
    player_t player[2];
    ball_t ball;
};


struct packet_t
{
    sockaddr_in addr;
    string data;
};

std::vector<packet_t> packet_queue;
std::mutex packet_queue_mutex;


class server_t
{
public:
    server_t()
    {
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

        t = std::thread{ &server_t::transcieve, this };
    } 

    ~server_t()
    {
        t.join();
        close(socket_fd);
    }

    void transcieve()
    {
        char msg[1024];
        sockaddr_in other_addr;
        socklen_t other_addr_len;
        int msg_size;
        while (true) {
            other_addr_len = sizeof(other_addr);
            msg_size = recvfrom(socket_fd, msg, 1023, 0, (struct sockaddr *) &other_addr, &other_addr_len);
            if (msg_size < 0) {
                cout << strerror(errno) << endl;
                throw std::runtime_error("failed to recieve data");
            }
            msg[msg_size] = '\0';
            cout << "> " << inet_ntoa(other_addr.sin_addr) << ":" << ntohs(other_addr.sin_port) << endl;
            cout << msg << endl;
            {
                std::lock_guard<std::mutex> lock{ packet_queue_mutex };
                packet_queue.push_back(
                    packet_t {
                        other_addr,
                        string{ msg }
                    }
                );
            }
        }

    }

    void send(sockaddr_in addr, string data)
    {
        socklen_t addr_len = sizeof(addr);

        int msg_size = sendto(socket_fd, data.c_str(), data.length(), 0, (struct sockaddr *) &addr, addr_len);
        if (msg_size < 0) {
            cout << strerror(errno) << endl;
            throw std::runtime_error("failed to send data");
        }
    }

private:
    std::thread t;
    int socket_fd;
    sockaddr_in addr;
};
