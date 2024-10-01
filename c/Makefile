PCG_PATH = /Users/angeloocanamartins/dev/pcg-c-0.94
MEMCACHED_PATH = /usr/local

CFLAGS = -I$(PCG_PATH)/include -I$(MEMCACHED_PATH)/include
LDFLAGS = -L$(PCG_PATH)/src -L$(MEMCACHED_PATH)/lib -lpcg_random -lm -lmemcached
RPATH = -Wl,-rpath,$(MEMCACHED_PATH)/lib

CC = clang

TARGET = dist/game
SRC = src/game.c

$(TARGET): $(SRC)
	mkdir -p dist
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(RPATH)

.PHONY: clean

clean:
	rm -f $(TARGET)
	rmdir dist