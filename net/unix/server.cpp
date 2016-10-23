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


game_t game;


int main(int argc, char** argv)
{
    try {
        server_t server;

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
            string dev = in_data_tree.get<string>("dev");
            if (req == "connect") {
                if (dev == "phone") {
                    size_t id;
                    if (!game.player[0].joined[0])
                        id = 0;
                    else if (!game.player[1].joined[0])
                        id = 1;
                    else
                        throw std::runtime_error("too many phones");
                    game.player[id].addr[0] = packet.addr;
                    //cout << "player 0 phone @ " << game.player[id].addr[0].ip << ":" << game.player[id].addr[0].port << endl;

                    std::ostringstream out_data_str;
                    ptree out_data_tree;
                    out_data_tree.put("id", id);
                    write_json(out_data_str, out_data_tree);
                    server.send(game.player[id].addr[0], out_data_str.str());
                }
                else if (dev == "laptop") {
                    size_t id;
                    if (!game.player[0].joined[1])
                        id = 0;
                    else if (!game.player[1].joined[1])
                        id = 1;
                    else
                        throw std::runtime_error("too many laptops");
                    game.player[id].addr[1] = packet.addr;
                    //cout << "player 0 laptop @ " << game.player[id].addr[1].ip << ":" << game.player[id].addr[1].port << endl;

                    std::ostringstream out_data_str;
                    ptree out_data_tree;
                    out_data_tree.put("id", id);
                    write_json(out_data_str, out_data_tree);
                    server.send(game.player[id].addr[1], out_data_str.str());
                }
            } else if (req == "update") {
                string px = in_data_tree.get<string>("px");
                string py = in_data_tree.get<string>("py");
                int id = std::atoi(in_data_tree.get<string>("id").c_str());

                std::ostringstream out_data_str;
                ptree out_data_tree;
                out_data_tree.put("px", px);
                out_data_tree.put("py", py);
                write_json(out_data_str, out_data_tree);
                server.send(game.player[id].addr[1], out_data_str.str());
            }
        }

    } catch (std::runtime_error e) {
        cout << "error: " << e.what() << endl;
    }
}
