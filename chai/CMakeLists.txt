set(SRC chai-cbunp.c chai-lnx.c unican.h)

add_library(chai SHARED ${SRC})

target_include_directories(chai PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_library(PTHREAD_LIBRARY pthread)
add_definitions(-DLINUX)
target_link_libraries(chai ftd2xx ${PTHREAD_LIBRARY})
