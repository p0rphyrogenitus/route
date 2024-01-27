#ifndef ROUTE_MESSAGE_H
#define ROUTE_MESSAGE_H


#include <cstdint>


constexpr uint8_t
        BGP_MSGTYPE_OPEN = 1,
        BGP_MSGTYPE_UPDATE = 2,
        BGP_MSGTYPE_NOTIFICATION = 3,
        BGP_MSGTYPE_KEEPALIVE = 4,
        BGP_MSGTYPE_ROUTE_REFRESH = 5;

constexpr uint8_t
        BGP_ATTRTYPECODE_ORIGIN = 1,
        BGP_ATTRTYPECODE_AS_PATH = 2,
        BGP_ATTRTYPECODE_NEXT_HOP = 3,
        BGP_ATTRTYPECODE_AS_MULTI_EXIT_DISC = 4,
        BGP_ATTRTYPECODE_LOCAL_PREF = 5,
        BGP_ATTRTYPECODE_ATOMIC_AGGREGATE = 6,
        BGP_ATTRTYPECODE_AGGREGATOR = 7;

constexpr uint8_t
        BGP_ATTRVAL_ORIGIN_IGP = 0,
        BGP_ATTRVAL_ORIGIN_EGP = 1,
        BGP_ATTRVAL_ORIGIN_INCOMPLETE = 2;

constexpr uint8_t
        BGP_ATTRVAL_AS_PATH_AS_SET = 1,
        BGP_ATTRVAL_AS_PATH_AS_SEQUENCE = 2;

constexpr uint8_t
        BGP_ERR_MESSAGE_HEADER = 1,
        BGP_ERR_OPEN_MESSAGE = 2,
        BGP_ERR_UPDATE_MESSAGE = 3,
        BGP_ERR_HOLD_TIMER_EXPIRED = 4,
        BGP_ERR_FSM_ERROR = 5,
        BGP_ERR_CEASE = 6;

constexpr uint8_t
        BGP_SUBERR_UNSPEC = 0,
        BGP_SUBERR_MESSAGE_HEADER_CONN_NOT_SYNC = 1,
        BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_LEN = 2,
        BGP_SUBERR_MESSAGE_HEADER_BAD_MSG_TYPE = 3,
        BGP_SUBERR_OPEN_MESSAGE_UNSUP_VER = 1,
        BGP_SUBERR_OPEN_MESSAGE_BAD_PEER_AS = 2,
        BGP_SUBERR_OPEN_MESSAGE_BAD_BGP_ID = 3,
        BGP_SUBERR_OPEN_MESSAGE_UNSUP_OPT_PARAM = 4,
        BGP_SUBERR_OPEN_MESSAGE_UNACC_HOLD_TIME = 6,
        BGP_SUBERR_UPDATE_MESSAGE_MALFORM_ATTR_LIST = 1,
        BGP_SUBERR_UPDATE_MESSAGE_UNREC_WELLKN_ATTR = 2,
        BGP_SUBERR_UPDATE_MESSAGE_MISSING_WELLKN_ATTR = 3,
        BGP_SUBERR_UPDATE_MESSAGE_ATTR_FLAGS_ERR = 4,
        BGP_SUBERR_UPDATE_MESSAGE_ATTR_LEN_ERR = 5,
        BGP_SUBERR_UPDATE_MESSAGE_INVAL_ORIGIN_ATTR = 6,
        BGP_SUBERR_UPDATE_MESSAGE_INVAL_NEXT_HOP_ATTR = 8,
        BGP_SUBERR_UPDATE_MESSAGE_OPT_ATTR_ERR = 9,
        BGP_SUBERR_UPDATE_MESSAGE_INVAL_NET_FIELD = 10,
        BGP_SUBERR_UPDATE_MESSAGE_MALFORM_AS_PATH = 11;

[[deprecated("see https://datatracker.ietf.org/doc/html/rfc4271#appendix-A")]]
constexpr uint8_t
        BGP_SUBERR_OPEN_MESSAGE_AUTH_FAIL = 5,
        BGP_SUBERR_UPDATE_MESSAGE_AS_ROUTE_LOOP = 7;


namespace route::bgp {
#pragma pack(push, 1)
    struct Header {
        uint8_t marker[16];
        uint16_t length;
        uint8_t type;
    };

    struct OptionalParameter {
        uint8_t param_type;
        uint8_t param_len;
        uint8_t *param_value;
    };

    struct OpenMessage {
        uint8_t version;
        uint16_t my_as;
        uint16_t hold_time;
        uint32_t bgp_id;
        uint8_t opt_params_len;
        OptionalParameter *opt_params;
    };

    struct IPAddressPrefix {
        uint8_t length;
        uint8_t *prefix;
    };

    union AttributeFlags {
        struct {
            uint8_t optional: 1;
            uint8_t transitive: 1;
            uint8_t partial: 1;
            uint8_t extended_len: 1;
            uint8_t ignored: 4;
        };
        uint8_t value;
    };

    struct AttributeType {
        AttributeFlags attr_flags;
        uint8_t attr_type_code;

        explicit AttributeType(uint8_t attr_type_code, uint8_t options);
    };

    struct PathAttribute {
        AttributeType attr_type;
        uint8_t *attr_len;
        uint8_t *attr_value;
    };

    struct ASPathSegment {
        uint8_t path_seg_type;
        uint8_t path_seg_len;
        uint16_t *path_seg_value;
    };

    struct AggregatorAttributeValue {
        uint16_t last_aggr_route_forming_as;
        uint32_t last_aggr_route_forming_speaker;
    };

    using origin_attr_type = uint8_t;
    using as_path_attr_type = ASPathSegment *;
    using next_hop_attr_type = uint32_t;
    using multi_exit_disc_attr_type = uint32_t;
    using local_pref_attr_type = uint32_t;
    // ATOMIC_AGGREGATE value is of length 0
    using aggregator_attr_type = AggregatorAttributeValue;

    struct UpdateMessage {
        uint16_t withdrawn_routes_len;
        IPAddressPrefix *withdrawn_routes;
        uint16_t total_path_attr_len;
        PathAttribute *path_attrs;
        IPAddressPrefix *nlri;
    };

    // KEEPALIVE message consists of only the header

    struct NotificationMessage {
        uint8_t error_code;
        uint8_t error_subcode;
        uint8_t *data;
    };
#pragma pack(pop)
}

#endif //ROUTE_MESSAGE_H
