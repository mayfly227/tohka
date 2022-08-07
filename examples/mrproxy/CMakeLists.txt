add_executable(mrproxy_client
        main.cc
        freedom.cc
        socks_in.cc
        context.cc
        run_in.cc
        run_out.cc
        point.cc
        )
add_executable(mrproxy_server
        main.cc
        freedom.cc
        socks_in.cc
        context.cc
        run_in.cc
        run_out.cc
        point.cc
        )
target_link_libraries(mrproxy_client tohka)
target_link_libraries(mrproxy_server tohka)
