cmake_minimum_required(VERSION 3.10)
project(tohka)
set(VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)


include_directories(${PROJECT_SOURCE_DIR})


message(${PROJECT_SOURCE_DIR})
set(TOHKA_SRC
        tohka/util/log.cc
        #        tohka/log.h
        #        tohka/noncopyable.h
        tohka/ioloop.cc
        #        tohka/ioloop.h
        tohka/timepoint.cc
        #        tohka/timepoint.h
        tohka/timermanager.cc
        #        tohka/timermanager.h
        tohka/ioevent.cc
        #        tohka/ioevent.h
        tohka/timer.cc
        #        tohka/timer.h
        tohka/iowatcher.cc
        #        tohka/iowatcher.h
        tohka/poll.cc
        #        tohka/poll.h
        tohka/acceptor.cc
        #        tohka/acceptor.h
        tohka/socket.cc
        #        tohka/socket.h
        tohka/netaddress.cc
        #        tohka/netaddress.h
        tohka/tcpevent.cc
        #        tohka/tcpevent.h
        tohka/iobuf.cc
        #        tohka/iobuf.h
        tohka/tohka.h
        tohka/tcpserver.cc
        #        tohka/tcpserver.h)

        tohka/platform.h tohka/connector.cc tohka/connector.h tohka/tcpclient.cc tohka/tcpclient.h)

add_library(tohka SHARED ${TOHKA_SRC})

add_executable(test test.cc)
add_executable(test_channel tests/test_channel.cc)
add_executable(test_loop tests/test_loop.cc)
add_executable(test_timepoint tests/test_timepoint.cc)
add_executable(test_timermanager tests/test_timermanager.cc)
add_executable(test_netaddress tests/test_netaddress.cc)
add_executable(test_tcpserver.cc tests/test_tcpserver.cc)
add_executable(test_connector tests/test_connector.cc)
add_executable(test_tcp_client tests/test_tcp_client.cc)


#Test
target_link_libraries(test)
target_link_libraries(test_timepoint tohka)
target_link_libraries(test_channel tohka)
target_link_libraries(test_timermanager tohka)
target_link_libraries(test_loop tohka)
target_link_libraries(test_netaddress tohka)
target_link_libraries(test_tcpserver.cc tohka)
target_link_libraries(test_connector tohka)
target_link_libraries(test_tcp_client tohka)


#example
add_executable(echo examples/echo/echo.cc)


target_link_libraries(echo tohka)