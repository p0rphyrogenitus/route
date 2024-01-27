#include "Exception.hpp"
#include "Message.hpp"


route::bgp::AttributeType::AttributeType(uint8_t attr_type_code, uint8_t options = 0) : attr_type_code(
        attr_type_code) {

    // Set (optional, transitive, partial) bits
    switch (attr_type_code) {
        case BGP_ATTRTYPECODE_ORIGIN:
        case BGP_ATTRTYPECODE_AS_PATH:
        case BGP_ATTRTYPECODE_NEXT_HOP:
        case BGP_ATTRTYPECODE_LOCAL_PREF:
        case BGP_ATTRTYPECODE_ATOMIC_AGGREGATE:
            attr_flags.optional = 0;
            attr_flags.transitive = 1;
            attr_flags.partial = 0;
            break;
        case BGP_ATTRTYPECODE_AS_MULTI_EXIT_DISC:
            attr_flags.optional = 1;
            attr_flags.transitive = 0;
            attr_flags.partial = 0;
            break;
        case BGP_ATTRTYPECODE_AGGREGATOR:
            attr_flags.optional = 1;
            attr_flags.transitive = 1;
            attr_flags.partial = options & 0b01;
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
            attr_flags.extended_len = 0;
            break;
        case BGP_ATTRTYPECODE_AS_PATH:
        case BGP_ATTRTYPECODE_AGGREGATOR:
            attr_flags.extended_len = options & 0b10;
            break;
        default:
            throw MessageFormatException(MessageFormatException::ERR_CODE_INVAL_ATTR_TYPE);
    }

    // Set ignored bits to 0
    attr_flags.ignored = 0;
}

route::bgp::NotificationMessage::NotificationMessage() : error_code(0), error_subcode(0), data(nullptr) {
}
