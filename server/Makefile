CC = g++
CFLAGS = -g -O2
LIBS = -lpthread -lmysqlclient
CLEANFILES = core core.* *.core *.o temp.* *.out typescript* *.lc *.lh *.bsdi *.sparc *.uw


PROGS = server

all:    ${PROGS}

server: readline_server.o server.o wrappthread.o wrapsock_server.o \
writen_server.o tbl_userinfo.o execute_sql.o notify_client.o tbl_logininfo.o tbl_friendinfo.o
		${CC} ${CFLAGS} -o $@ readline_server.o server.o wrappthread.o \
		wrapsock_server.o writen_server.o tbl_userinfo.o execute_sql.o notify_client.o tbl_logininfo.o tbl_friendinfo.o ${LIBS}

readline_server.o:   readline_server.cpp
		${CC} ${CFLAGS} -c -o $@ readline_server.cpp

server.o:    server.cpp
		${CC} ${CFLAGS} -c -o $@ server.cpp

wrappthread.o:    wrappthread.cpp
		${CC} ${CFLAGS} -c -o $@ wrappthread.cpp

wrapsock_server.o:    wrapsock_server.cpp
		${CC} ${CFLAGS} -c -o $@ wrapsock_server.cpp

writen_server.o:    writen_server.cpp
		${CC} ${CFLAGS} -c -o $@ writen_server.cpp

tbl_userinfo.o:    tbl_userinfo.cpp
		${CC} ${CFLAGS} -c -o $@ tbl_userinfo.cpp

execute_sql.o:    execute_sql.cpp
		${CC} ${CFLAGS} -c -o $@ execute_sql.cpp
		
notify_client.o:    notify_client.cpp
		${CC} ${CFLAGS} -c -o $@ notify_client.cpp
		
tbl_logininfo.o:    tbl_logininfo.cpp
		${CC} ${CFLAGS} -c -o $@ tbl_logininfo.cpp

tbl_friendinfo.o:    tbl_friendinfo.cpp
		${CC} ${CFLAGS} -c -o $@ tbl_friendinfo.cpp

clean:
		rm -f ${PROGS} ${CLEANFILES}