CC = g++
CFLAGS = -g
LIBS = -lpthread
CLEANFILES = core core.* *.core *.o temp.* *.out typescript* *.lc *.lh *.bsdi *.sparc *.uw


PROGS = client

all:    ${PROGS}

client: client_gui.o everyone_talk.o serialize_msg.o connect_server.o wrapsock.o wrappthread.o \
         deal_friend_data.o deal_talk_data.o read_config.o
		${CC} ${CFLAGS} -o $@ client_gui.o everyone_talk.o serialize_msg.o connect_server.o wrapsock.o \
		wrappthread.o deal_friend_data.o deal_talk_data.o read_config.o ${LIBS}

client_gui.o:   client_gui.cpp
		${CC} ${CFLAGS} -c -o $@ client_gui.cpp

connect_server.o:    connect_server.cpp
		${CC} ${CFLAGS} -c -o $@ connect_server.cpp

everyone_talk.o:    everyone_talk.cpp
		${CC} ${CFLAGS} -c -o $@ everyone_talk.cpp

serialize_msg.o:    serialize_msg.cpp
		${CC} ${CFLAGS} -c -o $@ serialize_msg.cpp

wrapsock.o:    wrapsock.cpp
		${CC} ${CFLAGS} -c -o $@ wrapsock.cpp
		
wrappthread.o:    wrappthread.cpp
		${CC} ${CFLAGS} -c -o $@ wrappthread.cpp

deal_friend_data.o:    deal_friend_data.cpp
		${CC} ${CFLAGS} -c -o $@ deal_friend_data.cpp

deal_talk_data.o:    deal_talk_data.cpp
		${CC} ${CFLAGS} -c -o $@ deal_talk_data.cpp

read_config.o:    read_config.cpp
		${CC} ${CFLAGS} -c -o $@ read_config.cpp
		
clean:
		rm -f ${PROGS} ${CLEANFILES}
