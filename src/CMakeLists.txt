project(route_)

set(HEADER_FILES
        bgp/Message.h
)

set(SOURCE_FILES
        bgp/Message.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
