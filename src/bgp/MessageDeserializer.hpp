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
        static void deserialize_open(const uint8_t *body_buffer,
                                     uint16_t body_buf_size,
                                     DeserializeMessageResult &result);

        static void deserialize_update(const uint8_t *body_buffer,
                                       uint16_t body_buf_size,
                                       DeserializeMessageResult &result);

        static void deserialize_notification(const uint8_t *body_buffer,
                                             uint16_t body_buf_size,
                                             DeserializeMessageResult &result);

        static inline bool bad_msg_size(uint16_t size);
    };
}

#endif //ROUTE_MESSAGEDESERIALIZER_HPP
