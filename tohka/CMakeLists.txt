set(TOHKA_SRC
        acceptor.cc
        connector.cc
        iobuf.cc
        ioevent.cc
        ioloop.cc
        iowatcher.cc
        netaddress.cc
        poll.cc
        socket.cc
        tcpclient.cc
        tcpevent.cc
        tcpserver.cc
        timepoint.cc
        timer.cc
        timermanager.cc
        util/log.cc
        )

add_library(tohka STATIC ${TOHKA_SRC})
#target_link_libraries(tohka )

