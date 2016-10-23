#include <iostream>
#include <thread>
#include <cstring>
#include <cerrno>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::cout;
using std::endl;

#include "net.h"

int main(int argc, char** argv)
{
    try {
        server s;
    } catch (std::runtime_error e) {
        cout << "error: " << e.what() << endl;
    }
}
