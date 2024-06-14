cmake_minimum_required(VERSION 3.15)
project(ChessGame C)

set(CMAKE_C_STANDARD 11)

# Add source files
set(SOURCES
        chess.c
        chess.h
        server.c
        client.c
        )

# Add executable targets
add_executable(server server.c chess.c)
add_executable(client client.c chess.c)
add_executable(client2 client.c chess.c)

# Link with pthread library for server
if (WIN32)
    target_link_libraries(server PRIVATE ws2_32)
    target_link_libraries(client PRIVATE ws2_32)
    target_link_libraries(client2 PRIVATE ws2_32)
else()
    target_link_libraries(server PRIVATE pthread)
endif()
