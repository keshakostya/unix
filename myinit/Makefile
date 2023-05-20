BUILD_DIR	= build
SOURCE_DIR	= src
APP_NAME    = myinit

CC 			= gcc
CFLAGS 		= -Werror -Wall -Wextra

all: myinit

$(SOURCE_DIR):
	if [ ! -d $@ ]; then 				\
		$(error source dir not found!);	\
	fi


$(BUILD_DIR):
	mkdir -p $@

error:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/error/error.c -o $(BUILD_DIR)/error.o

configuration:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/configuration/configuration.c -o $(BUILD_DIR)/configuration.o

logger:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/logger/logger.c -o $(BUILD_DIR)/logger.o

task:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/task/task.c -o $(BUILD_DIR)/task.o

myinit: $(SOURCE_DIR) $(BUILD_DIR) error configuration logger task
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(APP_NAME) $(SOURCE_DIR)/main.c $(BUILD_DIR)/*.o

clean:
	if [ -d ${BUILD_DIR} ]; then \
		rm -rf ${BUILD_DIR}; \
	fi \