//
// Created by Thomas Brooks on 1/27/24.
//

#ifndef ROUTE_MESSAGEDESERIALIZER_HPP
#define ROUTE_MESSAGEDESERIALIZER_HPP


#include <optional>
#include "Message.hpp"


namespace route::bgp {
    struct DeserializeMessageResult {
        std::variant<OpenMessage, UpdateMessage, NotificationMessage, KeepAliveMessage, RouteRefreshMessage> message;
        std::optional<NotificationMessage> notification_message;
    };

    class MessageDeserializer {
    public:
        DeserializeMessageResult deserialize(const uint8_t *buffer, uint16_t buf_size);
    };
}

#endif //ROUTE_MESSAGEDESERIALIZER_HPP
