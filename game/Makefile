CC = g++
CFLG = -std=c++14 -Wall
INCS = -Isw/include
LFLG = -Lsw/lib
LIBS = -lglfw -framework Cocoa -framework OpenGL -framework IOKit
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
EXEC = pong

.PHONY: all clean

all: $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LFLG) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLG) $(INCS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
