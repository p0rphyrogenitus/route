//
// Created by Thomas Brooks on 1/27/24.
//

#include "MessageDeserializer.hpp"


route::bgp::DeserializeMessageErrorResult::DeserializeMessageErrorResult() {
    header.type = BGP_MSGTYPE_NOTIFICATION;
}

route::bgp::DeserializeMessageResult route::bgp::MessageDeserializer::deserialize(uint8_t *buffer,
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
        build_err(result.error, BGP_ERR_MESSAGE_HEADER, BGP_SUBERR_MESSAGE_HEADER_CONN_NOT_SYNC, std::nullopt);
        return result;
    }

    // Validate header
    auto header = Header::from_buffer(buffer);
    if (bad_msg_size(header->length) ||
        header->length != buf_size ||
        header->type == BGP_MSGTYPE_OPEN && header->length < BGP_MSGSIZE_OPENMSG_MIN ||
        header->type == BGP_MSGTYPE_UPDATE && header->length < BGP_MSGSIZE_UPDATEMSG_MIN ||
        header->type == BGP_MSGTYPE_KEEPALIVE && header->length != BGP_MSGSIZE_KEEPALIVEMSG ||
        header->type == BGP_MSGTYPE_NOTIFICATION && header->length < BGP_MSGSIZE_NOTIFICATIONMSG_MIN
            ) {
        build_err(
                result.error,
                BGP_ERR_MESSAGE_HEADER,
                BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_LEN,
                [length = header->length](std::optional<DeserializeMessageErrorResult> &error) -> uint16_t {
                    error->message.data = std::make_unique<uint8_t[]>(2);
                    for (uint8_t i = 16; i != 0; i -= 8) {
                        error->message.data[(16 - i) / 8] = (length >> (i - 8)) & 0xFF;
                    }
                    return 2;
                });
        return result;
    }

    uint8_t *body_buffer = buffer + BGP_MSGSIZE_MIN;
    uint16_t body_buf_size = buf_size - BGP_MSGSIZE_MIN;
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
            build_err(
                    result.error,
                    BGP_ERR_MESSAGE_HEADER,
                    BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_TYPE,
                    [length = header->length](std::optional<DeserializeMessageErrorResult> &error) -> uint16_t {
                        error->message.data = std::make_unique<uint8_t[]>(1);
                        error->message.data[0] = length;
                        return 1;
                    });
    }

    return result;
}

void route::bgp::MessageDeserializer::deserialize_open(uint8_t *body_buffer,
                                                       uint16_t body_buf_size,
                                                       DeserializeMessageResult &result) {
    OpenMessage_Invariant_ *partial_msg = OpenMessage_Invariant_::from_buffer(body_buffer);

    // Validate version
    if (partial_msg->version != BGP_VERSION) {
        build_err(
                result.error,
                BGP_ERR_OPEN_MESSAGE,
                BGP_SUBERR_OPEN_MESSAGE_UNSUP_VER,
                [](std::optional<DeserializeMessageErrorResult> &error) -> uint16_t {
                    error->message.data = std::make_unique<uint8_t[]>(2);

                    // This implementation only supports BGPv4
                    error->message.data[0] = 0;
                    error->message.data[1] = BGP_VERSION;
                    return 2;
                });
        return;
    }

    // TODO Validate my_as (implementation-dependent)
    // TODO Validate hold_time (implementation-dependent)
    // TODO Validate bgp_id (probably not implementation-dependent)
}

void route::bgp::MessageDeserializer::deserialize_update(uint8_t *body_buffer,
                                                         uint16_t body_buf_size,
                                                         route::bgp::DeserializeMessageResult &result) {
    // TODO
}

void route::bgp::MessageDeserializer::deserialize_notification(uint8_t *body_buffer,
                                                               uint16_t body_buf_size,
                                                               route::bgp::DeserializeMessageResult &result) {
    //TODO
}

void route::bgp::MessageDeserializer::build_err(std::optional<DeserializeMessageErrorResult> &error,
                                                uint8_t code,
                                                uint8_t subcode,
                                                const std::optional<std::function<uint16_t(
                                                        std::optional<DeserializeMessageErrorResult> &error)>>
                                                &build_err_data) {
    error.emplace();
    error->message.error_code = code;
    error->message.error_subcode = subcode;

    uint8_t data_size = 0;
    if (build_err_data.has_value()) {
        data_size = build_err_data.value()(error);
    }

    error->header.length = BGP_MSGSIZE_NOTIFICATIONMSG_MIN + data_size;
}
