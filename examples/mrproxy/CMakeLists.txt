

add_executable(mrproxy
        main.cc
        #        socks5.cc
        freedom.cc
        socks_in.cc
        context.cc
        #        context.h
        run_in.cpp
        #        run_in.h
        run_out.cpp
        #        run_out.h
        )

target_link_libraries(mrproxy tohka)
