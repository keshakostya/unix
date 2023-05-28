BUILD_DIR = build
SOURCE_DIR = src

CC = gcc
CFLAGS = -O3 -Werror -Wall -Wextra -g

all: setup server client

$(SOURCE_DIR):
	if [ ! -d $@ ]; then 				\
		$(error source dir not found!);	\
	fi

$(BUILD_DIR):
	mkdir -p $@

setup: $(SOURCE_DIR) $(BUILD_DIR)

server: config
	$(CC) $(CFLAGS) $(BUILD_DIR)/config.o $(SOURCE_DIR)/server.c $(SOURCE_DIR)/logger.c -o $(BUILD_DIR)/server

client: config
	$(CC) $(CFLAGS) $(BUILD_DIR)/config.o $(SOURCE_DIR)/client.c -o $(BUILD_DIR)/client

config:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/config.c -o $(BUILD_DIR)/config.o

clean:
	if [ -d ${BUILD_DIR} ]; then \
		rm -rf ${BUILD_DIR}; \
	fi \