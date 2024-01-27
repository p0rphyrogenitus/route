//
// Created by Thomas Brooks on 1/27/24.
//

#include "Exception.hpp"


route::bgp::ExceptionWithErrorCode::ExceptionWithErrorCode(const uint8_t err_code) : std::runtime_error(
        "exception was thrown from bgp module"), err_code(err_code) {
}

uint8_t route::bgp::ExceptionWithErrorCode::get_err_code() const {
    return err_code;
}

route::bgp::MessageFormatException::MessageFormatException(uint8_t err_code) : ExceptionWithErrorCode(err_code) {
}
