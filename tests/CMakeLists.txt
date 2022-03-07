
# add_executable(device_test0
#     src/test0.cpp
# )
# target_link_libraries(device_test0 PRIVATE
#     munu_io
# )

add_executable(read_until_test
    src/read_until_test.cpp
)
target_link_libraries(read_until_test PRIVATE
    munu_io
)


# add_executable(serial_test0
#     src/serial_test0.cpp
# )
# target_link_libraries(serial_test0 PRIVATE
#     munu_io
# )

# add_executable(tcpclient_test0
#     src/tcpclient_test0.cpp
# )
# target_link_libraries(tcpclient_test0 PRIVATE
#     munu_io
# )

# compilation fails because boost upd socket has no async_read_some method (WHY ????)
# add_executable(udpclient_test0
#     src/udpclient_test0.cpp
# )
# target_link_libraries(udpclient_test0 PRIVATE
#     munu_io
# )
