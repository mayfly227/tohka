add_executable(simple_echo echo.cc)
add_executable(pingpong_client pingpong_client.cc)

target_link_libraries(simple_echo tohka)
target_link_libraries(pingpong_client tohka)
#target_link_libraries(simple_echo tohka jemalloc)
#target_link_libraries(simple_echo tohka mimalloc.2.0)