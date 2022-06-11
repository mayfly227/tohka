add_executable(mrproxy
        main.cc
        freedom.cc
        socks_in.cc
        context.cc
        run_in.cc
        run_out.cc
        point.cc
        )
target_link_libraries(mrproxy tohka)
