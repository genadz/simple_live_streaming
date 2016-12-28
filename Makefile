CC = c++
CFLAGS = -std=c++1y -Wall -fPIC -pthread
SOURCES = $(shell find ./src -name '*.cpp')
DEPS = $(shell pkg-config --cflags --libs libmongocxx)

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -shared -o libSongs.so -lfastcgi-daemon2 $(DEPS)
