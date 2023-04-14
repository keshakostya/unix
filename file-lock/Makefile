BUILD_DIR 	= build
SOURCE_DIR 	= src

CC 		= gcc
CFLAGS 	= -O3 -Werror -Wall -Wextra

all: test-app

test-app: lib-file-lock
	${CC} ${CFLAGS} -L${BUILD_DIR} -o ${BUILD_DIR}/testapp ${SOURCE_DIR}/main.c -lfilelock

lib-file-lock: file_lock.o
	${CC} ${CFLAGS} -shared ${BUILD_DIR}/file_lock.o -o ${BUILD_DIR}/libfilelock.so

file_lock.o: ${SOURCE_DIR}/file_lock.c ${BUILD_DIR}
	${CC} ${CFLAGS} -c -fpic -o ${BUILD_DIR}/file_lock.o  ${SOURCE_DIR}/file_lock.c


${SOURCE_DIR}:
	if [ ! -d $@ ]; then 				\
		$(error source dir not found!);	\
	fi


${BUILD_DIR}:
	mkdir -p $@

clean:
	if [ -d ${BUILD_DIR} ]; then \
		rm -rf ${BUILD_DIR}; \
	fi \