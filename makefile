MYSQL_INCLUDE_DIR?=/usr/include/mysql
MYSQL_PLUGIN_DIR?=/usr/lib/mysql/plugin
TARGET_FILE?=logger
CFLAGS?=-c -Wall -fpic
INCLUDE?=-I/usr/include/x86_64-linux-gnu/curl -I${MYSQL_INCLUDE_DIR}
LDFLAGS?=-L/usr/lib/x86_64-linux-gnu
LDLIBS?=-lcurl
SERVER_ADDRESS=http://localhost:3000

logger : logger.c
	${CC} -o $@.o $? -D SERVER_ADDRESS='"${SERVER_ADDRESS}"' ${CFLAGS} ${CCFLAGS} ${INCLUDE} ${LDFLAGS} ${LDLIBS}
	${CC} -o $@.so $@.o -shared ${INCLUDE} ${LDFLAGS} ${LDLIBS}
	chmod 0644 $@.so

clean:
	unlink *.o
	unlink *.so
