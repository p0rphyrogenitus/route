//
// Created by Thomas Brooks on 1/27/24.
//

#include "MessageDeserializer.hpp"


route::bgp::DeserializeMessageErrorResult::DeserializeMessageErrorResult() {
    header.type = BGP_MSGTYPE_NOTIFICATION;
}

route::bgp::DeserializeMessageResult route::bgp::MessageDeserializer::deserialize(const uint8_t *buffer,
                                                                                  const uint16_t buf_size) {
    DeserializeMessageResult result;

    // Validate basic synchronization
    bool buf_sync_err = bad_msg_size(buf_size);
    if (!buf_sync_err) {
        for (uint8_t i = BGP_MSG_MARKER_LEN; i != 0; --i) {
            if (buffer[i] != 0xFF) {
                buf_sync_err = true;
                break;
            }
        }
    }
    if (buf_sync_err) {
        result.error.emplace();
        result.error->message.error_code = BGP_ERR_MESSAGE_HEADER;
        result.error->message.error_subcode = BGP_SUBERR_MESSAGE_HEADER_CONN_NOT_SYNC;
        result.error->header.length = BGP_MSGSIZE_NOTIFICATIONMSG_MIN;
        return result;
    }

    // Validate header
    // TODO This reinterpret_cast may not be safe. Consider endianness, padding (should be taken care of by #pragmas
    //  in Message.hpp, but some compilers might not support this, technically).
    auto header = reinterpret_cast<const Header *>(buffer);
    if (bad_msg_size(header->length) ||
        header->length != buf_size ||
        header->type == BGP_MSGTYPE_OPEN && header->length < BGP_MSGSIZE_OPENMSG_MIN ||
        header->type == BGP_MSGTYPE_UPDATE && header->length < BGP_MSGSIZE_UPDATEMSG_MIN ||
        header->type == BGP_MSGTYPE_KEEPALIVE && header->length != BGP_MSGSIZE_KEEPALIVEMSG ||
        header->type == BGP_MSGTYPE_NOTIFICATION && header->length < BGP_MSGSIZE_NOTIFICATIONMSG_MIN
            ) {
        result.error.emplace();
        result.error->message.error_code = BGP_ERR_MESSAGE_HEADER;
        result.error->message.error_subcode = BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_LEN;
        result.error->message.data = std::make_unique<uint8_t[]>(2);
        for (uint8_t i = 16; i != 0; i -= 8) {
            result.error->message.data[(16 - i) / 8] = (header->length >> (i - 8)) & 0xFF;
        }
        result.error->header.length = BGP_MSGSIZE_NOTIFICATIONMSG_MIN + 2;
        return result;
    }

    const uint8_t *body_buffer = buffer + BGP_MSGSIZE_MIN;
    const uint16_t body_buf_size = buf_size - BGP_MSGSIZE_MIN;
    switch (header->type) {
        case BGP_MSGTYPE_OPEN:
            deserialize_open(body_buffer, body_buf_size, result);
            break;
        case BGP_MSGTYPE_UPDATE:
            deserialize_update(body_buffer, body_buf_size, result);
            break;
        case BGP_MSGTYPE_NOTIFICATION:
            deserialize_notification(body_buffer, body_buf_size, result);
            break;
        case BGP_MSGTYPE_KEEPALIVE:
            result.message = nullptr;
            break;
        default:
            result.error.emplace();
            result.error->message.error_code = BGP_ERR_MESSAGE_HEADER;
            result.error->message.error_subcode = BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_TYPE;
            result.error->message.data = std::make_unique<uint8_t[]>(1);
            result.error->message.data[0] = header->length;
            result.error->header.length = BGP_MSGSIZE_NOTIFICATIONMSG_MIN + 1;
            return result;
    }

    return result;
}

void route::bgp::MessageDeserializer::deserialize_open(const uint8_t *body_buffer,
                                                       const uint16_t body_buf_size,
                                                       DeserializeMessageResult &result) {
    // TODO
}

void route::bgp::MessageDeserializer::deserialize_update(const uint8_t *body_buffer,
                                                         const uint16_t body_buf_size,
                                                         route::bgp::DeserializeMessageResult &result) {
    // TODO
}

void route::bgp::MessageDeserializer::deserialize_notification(const uint8_t *body_buffer,
                                                               const uint16_t body_buf_size,
                                                               route::bgp::DeserializeMessageResult &result) {
    //TODO
}

bool route::bgp::MessageDeserializer::bad_msg_size(uint16_t size) {
    return size < BGP_MSGSIZE_MIN || size > BGP_MSGSIZE_MAX;
}
