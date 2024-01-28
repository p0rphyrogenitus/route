#ifndef ROUTE_MESSAGE_HPP
#define ROUTE_MESSAGE_HPP


#include <cstdint>
#include <memory>

constexpr uint8_t BGP_VERSION = 4;

constexpr uint16_t
        BGP_MSGSIZE_MIN = 19,
        BGP_MSGSIZE_MAX = 4096,
        BGP_MSGSIZE_OPENMSG_MIN = 29,
        BGP_MSGSIZE_UPDATEMSG_MIN = 23,
        BGP_MSGSIZE_KEEPALIVEMSG = 19,
        BGP_MSGSIZE_NOTIFICATIONMSG_MIN = 21;

constexpr uint8_t BGP_MSG_MARKER_LEN = 16;

constexpr uint8_t
        BGP_MSGTYPE_OPEN = 1,
        BGP_MSGTYPE_UPDATE = 2,
        BGP_MSGTYPE_NOTIFICATION = 3,
        BGP_MSGTYPE_KEEPALIVE = 4;

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
        uint8_t marker[BGP_MSG_MARKER_LEN]{};
        uint16_t length;
        uint8_t type;

        Header();

        /**
         * Reinterprets data at given pointer as a Header and, if appropriate, swaps bytes of multi-byte struct fields
         * to translate network byte order to host byte order. The given pointer is assumed to be safe to reinterpret
         * as specified here--you must do all memory-safety validations before calling this method.
         * @param buffer
         * @return
         */
        static inline Header *from_buffer(uint8_t *buffer) {
            auto header = reinterpret_cast<Header *>(buffer);
            header->ntoh();
            return header;
        }

    private:
        inline void hton() {
            length = htons(length);
        }

        inline void ntoh() {
            length = ntohs(length);
        }
    };

    static_assert(sizeof(Header) == BGP_MSGSIZE_MIN);

    struct OptionalParameter {
        uint8_t param_type;
        uint8_t param_len;
        std::unique_ptr<uint8_t[]> param_value;
    };

    struct OpenMessage_Invariant_ {
        uint8_t version;
        uint16_t my_as;
        uint16_t hold_time;
        uint32_t bgp_id;
        uint8_t opt_params_len;

        static inline OpenMessage_Invariant_ *from_buffer(uint8_t *buffer) {
            auto partial_msg = reinterpret_cast<OpenMessage_Invariant_ *>(buffer);
            partial_msg->ntoh();
            return partial_msg;
        }

    private:
        inline void hton() {
            my_as = htons(my_as);
            hold_time = htons(hold_time);
            bgp_id = htonl(bgp_id);
        }

        inline void ntoh() {
            my_as = ntohs(my_as);
            hold_time = ntohs(hold_time);
            bgp_id = ntohl(bgp_id);
        }
    };

    static_assert(sizeof(OpenMessage_Invariant_) + BGP_MSGSIZE_MIN == BGP_MSGSIZE_OPENMSG_MIN);

    struct OpenMessage : public OpenMessage_Invariant_ {
        std::unique_ptr<OptionalParameter[]> opt_params;
    };

    static_assert(sizeof(OpenMessage) - sizeof(std::unique_ptr<OptionalParameter[]>) + BGP_MSGSIZE_MIN ==
                  BGP_MSGSIZE_OPENMSG_MIN);

    struct IPAddressPrefix {
        uint8_t length;
        std::unique_ptr<uint8_t[]> prefix;
    };

    struct AttributeType {
        uint8_t attr_flags;
        uint8_t attr_type_code;

        /**
         * Builds an AttributeType that is well-behaved for the given attr_type_code, that is, where AttributeFlags are
         * initialized properly for the given code. Options cover flags (partial, extended_len) where the AttributeType
         * might be ambiguous given just a type code.
         * @param attr_type_code Attribute type code.
         * @param options Ignored if it conflicts with good behavior as described above. Second least-significant bit
         * sets extended_len. Least-significant bit sets partial. Remaining bits are always ignored.
         */
        AttributeType(uint8_t attr_type_code, uint8_t options);

        static inline AttributeType *from_buffer(uint8_t *buffer) {
            return reinterpret_cast<AttributeType *>(buffer);
        };

        [[nodiscard]] inline bool get_optional() const {
            return (attr_flags & 0x80) >> 7;
        }

        inline void set_optional(bool set) {
            if (set) {
                attr_flags |= 0x80;
            } else {
                attr_flags &= ~0x80;
            }
        }

        [[nodiscard]] inline bool get_transitive() const {
            return (attr_flags & 0x40) >> 6;
        }

        inline void set_transitive(bool set) {
            if (set) {
                attr_flags |= 0x40;
            } else {
                attr_flags &= ~0x40;
            }
        }

        [[nodiscard]] inline bool get_partial() const {
            return (attr_flags & 0x20) >> 5;
        }

        inline void set_partial(bool set) {
            if (set) {
                attr_flags |= 0x20;
            } else {
                attr_flags &= ~0x20;
            }
        }

        [[nodiscard]] inline bool get_extended_len() const {
            return (attr_flags & 0x10) >> 4;
        }

        inline void set_extended_len(bool set) {
            if (set) {
                attr_flags |= 0x10;
            } else {
                attr_flags &= ~0x10;
            }
        }
    };

    struct PathAttribute {
        AttributeType attr_type;
        std::unique_ptr<uint8_t[]> attr_len;
        std::unique_ptr<uint8_t[]> attr_value;
    };

    struct ASPathSegment {
        uint8_t path_seg_type;
        uint8_t path_seg_len;
        std::unique_ptr<uint16_t[]> path_seg_value;
    };

    struct AggregatorAttributeValue {
        uint16_t last_aggr_route_forming_as;
        uint32_t last_aggr_route_forming_speaker;
    };

    using origin_attr_type = uint8_t;
    using as_path_attr_type = std::unique_ptr<ASPathSegment[]>;
    using next_hop_attr_type = uint32_t;
    using multi_exit_disc_attr_type = uint32_t;
    using local_pref_attr_type = uint32_t;
    // ATOMIC_AGGREGATE value is of length 0
    using aggregator_attr_type = AggregatorAttributeValue;

    struct UpdateMessage {
        uint16_t withdrawn_routes_len;
        std::unique_ptr<IPAddressPrefix[]> withdrawn_routes;
        uint16_t total_path_attr_len;
        std::unique_ptr<PathAttribute[]> path_attrs;
        std::unique_ptr<IPAddressPrefix[]> nlri;
    };
    static_assert(sizeof(UpdateMessage) - 2 * sizeof(std::unique_ptr<IPAddressPrefix[]>) -
                  sizeof(std::unique_ptr<PathAttribute[]>) + BGP_MSGSIZE_MIN == BGP_MSGSIZE_UPDATEMSG_MIN);

    using KeepAliveMessage = nullptr_t;

    struct NotificationMessage {
        uint8_t error_code;
        uint8_t error_subcode;
        std::unique_ptr<uint8_t[]> data;

        NotificationMessage();
    };

    static_assert(sizeof(NotificationMessage) - sizeof(std::unique_ptr<uint8_t[]>) + BGP_MSGSIZE_MIN ==
                  BGP_MSGSIZE_NOTIFICATIONMSG_MIN);
#pragma pack(pop)
}

#endif //ROUTE_MESSAGE_HPP
