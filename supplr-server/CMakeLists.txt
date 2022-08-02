file(GLOB
    SRC
    *.c
)

add_executable(supplr-server ${SRC})

target_include_directories(supplr-server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(supplr-server PUBLIC canmaster ulfius pthread jansson)
