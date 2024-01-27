//
// Created by Thomas Brooks on 1/27/24.
//

#include "MessageDeserializer.hpp"


route::bgp::DeserializeMessageResult route::bgp::MessageDeserializer::deserialize(const uint8_t *buffer,
                                                                                  uint16_t buf_size) {
    NotificationMessage error_response;
    DeserializeMessageResult result;

    // Validate basic synchronization
    bool buf_sync_err = bad_msg_size(buf_size);
    if (!buf_sync_err) {
        for (uint8_t i = BGP_MSG_MARKER_LEN; i != 0; --i) {
            if (buffer[i] != 0xFF) {
                buf_sync_err = true;
            }
        }
    }
    if (buf_sync_err) {
        error_response.error_code = BGP_ERR_MESSAGE_HEADER;
        error_response.error_subcode = BGP_SUBERR_MESSAGE_HEADER_CONN_NOT_SYNC;
        return result;
    }

    // Validate header
    auto header = reinterpret_cast<const Header *>(buffer);
    if (bad_msg_size(header->length) ||
        header->type == BGP_MSGTYPE_OPEN && header->length < BGP_MSGSIZE_OPENMSG_MIN ||
        header->type == BGP_MSGTYPE_UPDATE && header->length < BGP_MSGSIZE_UPDATEMSG_MIN ||
        header->type == BGP_MSGTYPE_KEEPALIVE && header->length != BGP_MSGSIZE_KEEPALIVEMSG ||
        header->type == BGP_MSGTYPE_NOTIFICATION && header->length < BGP_MSGSIZE_NOTIFICATIONMSG_MIN
            ) {
        error_response.error_code = BGP_ERR_MESSAGE_HEADER;
        error_response.error_subcode = BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_LEN;
        // TODO Set response data
    }
    // TODO deserialize

    return result;
}

bool route::bgp::MessageDeserializer::bad_msg_size(uint16_t size) {
    return size < BGP_MSGSIZE_MIN || size > BGP_MSGSIZE_MAX;
}
