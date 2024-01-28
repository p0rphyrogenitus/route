#include <algorithm>
#include <arpa/inet.h>
#include "Exception.hpp"
#include "Message.hpp"


route::bgp::Header::Header() {
    std::fill(std::begin(marker), std::end(marker), 0xFF);
    length = type = 0;
}

route::bgp::AttributeType::AttributeType(uint8_t attr_type_code, uint8_t options = 0) : attr_flags(0), attr_type_code(
        attr_type_code) {

    // Set (optional, transitive, partial) bits
    switch (attr_type_code) {
        case BGP_ATTRTYPECODE_ORIGIN:
        case BGP_ATTRTYPECODE_AS_PATH:
        case BGP_ATTRTYPECODE_NEXT_HOP:
        case BGP_ATTRTYPECODE_LOCAL_PREF:
        case BGP_ATTRTYPECODE_ATOMIC_AGGREGATE:
            set_optional(attr_flags, false);
            set_transitive(attr_flags, true);
            set_partial(attr_flags, false);
            break;
        case BGP_ATTRTYPECODE_AS_MULTI_EXIT_DISC:
            set_optional(attr_flags, true);
            set_transitive(attr_flags, false);
            set_partial(attr_flags, false);
            break;
        case BGP_ATTRTYPECODE_AGGREGATOR:
            set_optional(attr_flags, true);
            set_transitive(attr_flags, true);
            set_partial(attr_flags, options & 0b01);
            break;
        default:
            throw MessageFormatException(MessageFormatException::ERR_CODE_INVAL_ATTR_TYPE);
    }

    // Set extended_len bit
    switch (attr_type_code) {
        case BGP_ATTRTYPECODE_ORIGIN:
        case BGP_ATTRTYPECODE_NEXT_HOP:
        case BGP_ATTRTYPECODE_AS_MULTI_EXIT_DISC:
        case BGP_ATTRTYPECODE_LOCAL_PREF:
        case BGP_ATTRTYPECODE_ATOMIC_AGGREGATE:
            set_extended_len(attr_flags, false);
            break;
        case BGP_ATTRTYPECODE_AS_PATH:
        case BGP_ATTRTYPECODE_AGGREGATOR:
            set_extended_len(attr_flags, (options & 0b10) == 0b10);
            break;
        default:
            throw MessageFormatException(MessageFormatException::ERR_CODE_INVAL_ATTR_TYPE);
    }
}

route::bgp::NotificationMessage::NotificationMessage() : error_code(0), error_subcode(0), data(nullptr) {
}
