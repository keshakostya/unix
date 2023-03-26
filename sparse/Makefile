BUILD = build
SRC = src
build_dir := ${CURDIR}/${BUILD}
src_dir := ${CURDIR}/${SRC}
APP=sparse

CC = gcc
CFLAGS = -O3 -Werror -Wall -Wextra

all: ${APP}

${APP}: ${build_dir}
	${CC} ${CFLAGS} -o ${build_dir}/${APP} ${src_dir}/main.c

${build_dir}:
	mkdir -p $@

clean:
	if [ -d ${build_dir} ]; then \
		rm -r ${build_dir}; \
	fi \
