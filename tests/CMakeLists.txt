

add_executable(serial_test0
    src/serial_test0.cpp
)
target_link_libraries(serial_test0 PRIVATE
    munu_io
)

add_executable(tcpclient_test0
    src/tcpclient_test0.cpp
)
target_link_libraries(tcpclient_test0 PRIVATE
    munu_io
)
