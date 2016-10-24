#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <cerrno>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using std::cout;
using std::endl;
using std::string;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

#include "net.h"


class ball_physics
{
public:
    ball_physics(server_t& server) : server(server) {}

    ~ball_physics()
    {
        physics_thread.join();
    }

    void start()
    {
        game.ball.pos.z = 5.0f;
        game.ball.vec.z = game.start_player ? 2.0f : -2.0f;

        std::ostringstream out_data_str;
        ptree out_data_tree;
        out_data_tree.put("obj", "ball");
        out_data_tree.put("px", game.ball.pos.x);
        out_data_tree.put("py", game.ball.pos.y);
        out_data_tree.put("pz", game.ball.pos.z);
        write_json(out_data_str, out_data_tree);
        server.send_all(out_data_str.str());

        physics_thread = std::thread{ &ball_physics::update, this };
    }

    void update()
    {
        game_t::ball_t& ball = game.ball;

        constexpr float delta = 1.0f/60;
        constexpr float ball_radius = 0.1f;
        constexpr float paddle_size = 0.6f;
        constexpr float speed_up = 1.1f;
        constexpr float acc_factor = 5.0f;
        constexpr float aspect_ratio = 720.0f / 1280;

        while (true) {
            std::this_thread::sleep_for(std::chrono::microseconds((uint64_t) (delta * 1000000)));

            ball.pos.x += ball.vec.x * delta;
            ball.pos.y += ball.vec.y * delta;
            ball.pos.z += ball.vec.z * delta;
            ball.vec.x += ball.acc.x * delta;
            ball.vec.y += ball.acc.y * delta;

            if ((ball.pos.x + ball_radius) > 1.0f) {
                ball.pos.x = 1.0f - ball_radius;
                ball.vec.x *= -1;
            } else if ((ball.pos.x - ball_radius) < -1.0f) {
                ball.pos.x = -1.0f + ball_radius;
                ball.vec.x *= -1;
            }
            if ((ball.pos.y + ball_radius) > aspect_ratio) {
                ball.pos.y = aspect_ratio - ball_radius;
                ball.vec.y *= -1;
            } else if ((ball.pos.y - ball_radius) < -aspect_ratio) {
                ball.pos.y = -aspect_ratio + ball_radius;
                ball.vec.y *= -1;
            }

            if ((ball.pos.z - ball_radius) < 0.0f) {
                if ((ball.pos.x + ball_radius) > (game.player[0].pos.x - paddle_size / 2.0f) &&
                    (ball.pos.x - ball_radius) < (game.player[0].pos.x + paddle_size / 2.0f) &&
                    (ball.pos.y + ball_radius) > (game.player[0].pos.y - paddle_size / 2.0f) &&
                    (ball.pos.y - ball_radius) < (game.player[0].pos.y + paddle_size / 2.0f)) {
                    ball.vec.x += ((rand() % 20) - 10) * 0.01f;
                    ball.vec.y += ((rand() % 20) - 10) * 0.01f;
                    ball.vec.z *= -speed_up;
                    ball.acc.x = game.player[0].vec.x * acc_factor;
                    ball.acc.y = game.player[0].vec.y * acc_factor;
                } else {
                    game.player[1].score += 1;
                    game.start_player = !game.start_player;
                    game.ball.pos.x = 0.0f;
                    game.ball.pos.y = 0.0f;
                    game.ball.pos.z = 5.0f;
                    ball.vec.x = ((rand() % 20) - 10) * 0.01f;
                    ball.vec.y = ((rand() % 20) - 10) * 0.01f;
                    game.ball.vec.z = game.start_player ? 2.0f : -2.0f;
                }
            } else if ((ball.pos.z + ball_radius) > 10.0f) {
                cout << "ball: (" << ball.pos.x << ", " << ball.pos.y << ")" << endl;
                cout << "paddle: (" << game.player[1].pos.x << ", " << game.player[1].pos.y << ")" << endl;
                if ((ball.pos.x + ball_radius) > (game.player[1].pos.x - paddle_size / 2.0f) &&
                    (ball.pos.x - ball_radius) < (game.player[1].pos.x + paddle_size / 2.0f) &&
                    (ball.pos.y + ball_radius) > (game.player[1].pos.y - paddle_size / 2.0f) &&
                    (ball.pos.y - ball_radius) < (game.player[1].pos.y + paddle_size / 2.0f)) {
                    ball.vec.x += ((rand() % 20) - 10) * 0.01f;
                    ball.vec.y += ((rand() % 20) - 10) * 0.01f;
                    ball.vec.z *= -speed_up;
                    ball.acc.x = game.player[1].vec.x * acc_factor;
                    ball.acc.y = game.player[1].vec.y * acc_factor;
                } else {
                    game.player[0].score += 1;
                    game.start_player = !game.start_player;
                    game.ball.pos.x = 0.0f;
                    game.ball.pos.y = 0.0f;
                    game.ball.pos.z = 5.0f;
                    ball.vec.x = ((rand() % 20) - 10) * 0.01f;
                    ball.vec.y = ((rand() % 20) - 10) * 0.01f;
                    game.ball.vec.z = game.start_player ? 2.0f : -2.0f;
                }
            }

            std::ostringstream out_data_str;
            ptree out_data_tree;
            out_data_tree.put("obj", "ball");
            out_data_tree.put("px", game.ball.pos.x);
            out_data_tree.put("py", game.ball.pos.y);
            out_data_tree.put("pz", game.ball.pos.z);
            write_json(out_data_str, out_data_tree);
            server.send_all(out_data_str.str());
        }
    }

private:
    server_t& server;
    std::thread physics_thread;
};


int main(int argc, char** argv)
{
    try {
        srand(time(0));

        game.start_player = rand() % 2;
        server_t server;
        ball_physics bp{ server };

        while (true) {
            while (packet_queue.empty()) {}

            packet_t packet;
            {
                std::lock_guard<std::mutex> lock{ packet_queue_mutex };
                packet = packet_queue.front();
                packet_queue.pop_back();
            }
            std::istringstream in_data_str{ packet.data };
            ptree in_data_tree;
            read_json(in_data_str, in_data_tree);

            string req = in_data_tree.get<string>("req");
            if (game.status == game_t::status_enum::idle) {
                string dev = in_data_tree.get<string>("dev");
                if (req == "connect") {
                    if (dev == "phone") {
                        size_t id;
                        if (!game.player[0].joined[0]) {
                            id = 0;
                        } else if (!game.player[1].joined[0]) {
                            id = 1;
                        } else {
                            throw std::runtime_error("too many phones");
                        }
                        game.player[id].joined[0] = true;
                        game.player[id].addr[0] = packet.addr;
                        //cout << "player 0 phone @ " << game.player[id].addr[0].ip << ":" << game.player[id].addr[0].port << endl;
                        cout << "phone " << id << " connected" << endl;

                        std::ostringstream out_data_str;
                        ptree out_data_tree;
                        out_data_tree.put("id", id);
                        write_json(out_data_str, out_data_tree);
                        server.send(game.player[id].addr[0], out_data_str.str());
                    } else if (dev == "laptop") {
                        size_t id;
                        if (!game.player[0].joined[1]) {
                            id = 0;
                        } else if (!game.player[1].joined[1]) {
                            id = 1;
                        } else {
                            throw std::runtime_error("too many phones");
                        }
                        game.player[id].joined[1] = true;
                        game.player[id].addr[1] = packet.addr;
                        //cout << "player 0 laptop @ " << game.player[id].addr[1].ip << ":" << game.player[id].addr[1].port << endl;
                        cout << "laptop " << id << " connected" << endl;

                        std::ostringstream out_data_str;
                        ptree out_data_tree;
                        out_data_tree.put("id", id);
                        write_json(out_data_str, out_data_tree);
                        server.send(game.player[id].addr[1], out_data_str.str());
                    }
                    if (game.player[0].joined[0] && game.player[0].joined[1] &&
                        game.player[1].joined[0] && game.player[1].joined[1]) {
                        game.status = game_t::status_enum::ready;
                    }
                }
            } else if (game.status == game_t::status_enum::ready) {
                if (req == "update") {
                    string px = in_data_tree.get<string>("px");
                    string py = in_data_tree.get<string>("py");
                    string vx = in_data_tree.get<string>("vx");
                    string vy = in_data_tree.get<string>("vy");
                    int id = std::atoi(in_data_tree.get<string>("id").c_str());

                    game.player[id].pos.x = std::atof(px.c_str());
                    game.player[id].pos.y = std::atof(py.c_str());
                    game.player[id].vec.x = std::atof(vx.c_str());
                    game.player[id].vec.y = std::atof(vy.c_str());

                    std::ostringstream out_data_str;
                    ptree out_data_tree;
                    out_data_tree.put("obj", "paddle");
                    out_data_tree.put("id", id);
                    out_data_tree.put("px", px);
                    out_data_tree.put("py", py);
                    write_json(out_data_str, out_data_tree);
                    server.send_all(out_data_str.str());
                } else if (req == "start") {
                    cout << "start" << endl;
                    bp.start();

                    game.status = game_t::status_enum::play;
                } else if (req == "switch") {
                    std::swap(game.player[0].addr[1], game.player[1].addr[1]);
                }
            } else if (game.status == game_t::status_enum::play) {
                if (req == "update") {
                    string px = in_data_tree.get<string>("px");
                    string py = in_data_tree.get<string>("py");
                    string vx = in_data_tree.get<string>("vx");
                    string vy = in_data_tree.get<string>("vy");
                    int id = std::atoi(in_data_tree.get<string>("id").c_str());

                    game.player[id].pos.x = id ? -std::atof(px.c_str()) : std::atof(px.c_str());
                    game.player[id].pos.y = std::atof(py.c_str());
                    game.player[id].vec.x = id ? -std::atof(vx.c_str()) : std::atof(vx.c_str());
                    game.player[id].vec.y = std::atof(vy.c_str());

                    std::ostringstream out_data_str;
                    ptree out_data_tree;
                    out_data_tree.put("obj", "paddle");
                    out_data_tree.put("id", id);
                    out_data_tree.put("px", px);
                    out_data_tree.put("py", py);
                    write_json(out_data_str, out_data_tree);
                    server.send_all(out_data_str.str());
                }
            }
        }

    } catch (std::runtime_error e) {
        cout << "error: " << e.what() << endl;
    }
}
