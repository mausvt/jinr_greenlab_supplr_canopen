file(GLOB
    SRC
    *.c
    application/*.c
    CANopen/*.c
    confile/*.c
    logger/*.c
)

find_library(
    CARGS
    cargs
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../cargs/build
)

add_library(canmaster SHARED ${SRC})

target_include_directories(canmaster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../cargs/include)

target_link_libraries(canmaster PUBLIC chai yaml ${CARGS})
