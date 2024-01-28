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
        static DeserializeMessageResult deserialize(uint8_t *buffer, uint16_t buf_size);

    private:
        static void deserialize_open(uint8_t *body_buffer,
                                     uint16_t body_buf_size,
                                     DeserializeMessageResult &result);

        static void deserialize_update(uint8_t *body_buffer,
                                       uint16_t body_buf_size,
                                       DeserializeMessageResult &result);

        static void deserialize_notification(uint8_t *body_buffer,
                                             uint16_t body_buf_size,
                                             DeserializeMessageResult &result);

        static void build_err(std::optional<DeserializeMessageErrorResult> &error,
                              uint8_t code,
                              uint8_t subcode,
                              const std::optional<std::function<uint16_t(
                                      std::optional<DeserializeMessageErrorResult> &error)>> &build_err_data);

        static inline bool bad_msg_size(uint16_t size) {
            return size < BGP_MSGSIZE_MIN || size > BGP_MSGSIZE_MAX;
        }
    };
}

#endif //ROUTE_MESSAGEDESERIALIZER_HPP
