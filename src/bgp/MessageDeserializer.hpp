//
// Created by Thomas Brooks on 1/27/24.
//

#ifndef ROUTE_MESSAGEDESERIALIZER_HPP
#define ROUTE_MESSAGEDESERIALIZER_HPP


#include <optional>
#include "Message.hpp"


namespace route::bgp {
    struct DeserializeMessageErrorResult {
        Header header;
        NotificationMessage message;

        DeserializeMessageErrorResult();
    };

    struct DeserializeMessageResult {
        std::variant<OpenMessage, UpdateMessage, NotificationMessage, KeepAliveMessage> message;
        std::optional<DeserializeMessageErrorResult> error;
    };

    class MessageDeserializer {
    public:
        static DeserializeMessageResult deserialize(const uint8_t *buffer, uint16_t buf_size);

    private:
        static inline bool bad_msg_size(uint16_t size);
    };
}

#endif //ROUTE_MESSAGEDESERIALIZER_HPP
