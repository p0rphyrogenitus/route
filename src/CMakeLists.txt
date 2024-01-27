project(route_)

set(HEADER_FILES
        bgp/Message.h
        bgp/RoutingInformationBase.hpp
        bgp/Exception.hpp
)

set(SOURCE_FILES
        bgp/Message.cpp
        bgp/RoutingInformationBase.cpp
        bgp/Exception.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
