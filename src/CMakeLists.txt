project(route_)

set(HEADER_FILES
        bgp/Message.hpp
        bgp/RoutingInformationBase.hpp
        bgp/Exception.hpp
        bgp/MessageDeserializer.hpp
)

set(SOURCE_FILES
        bgp/Message.cpp
        bgp/RoutingInformationBase.cpp
        bgp/Exception.cpp
        bgp/MessageDeserializer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
