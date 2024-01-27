//
// Created by Thomas Brooks on 1/27/24.
//

#include "Exception.hpp"


route::bgp::MessageFormatException::MessageFormatException(const uint8_t err_code) : std::runtime_error(
        "bad bgp message format"), err_code(err_code) {
}

uint8_t route::bgp::MessageFormatException::get_err_code() const {
    return err_code;
}
