CC = g++
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

TARGET = morse

SRCS = morse.cpp

HEADERS = constants.h
OBJS = $(SRCS:.cpp=.o)

DATA = words.json

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

clobber: clean
	rm -f $(SRCS:.cpp=.o)

run: $(TARGET)
	./$(TARGET) $(DATA)