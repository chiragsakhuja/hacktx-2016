CC = g++
CFLG = -g -std=c++14 -Wall -fmax-errors=1
INCS =
LFLG =
LIBS = -lpthread
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
EXEC = server

.PHONY: all clean

all: $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LFLG) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLG) $(INCS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
