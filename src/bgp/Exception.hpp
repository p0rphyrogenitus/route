//
// Created by Thomas Brooks on 1/27/24.
//

#ifndef ROUTE_EXCEPTION_HPP
#define ROUTE_EXCEPTION_HPP

#include <stdexcept>


namespace route::bgp {
    class ExceptionWithErrorCode : public std::runtime_error {
    public:
        explicit ExceptionWithErrorCode(uint8_t err_code);

        [[nodiscard]] uint8_t get_err_code() const;

    private:
        const uint8_t err_code;
    };

    class MessageFormatException : public ExceptionWithErrorCode {
    public:
        static constexpr uint8_t ERR_CODE_INVAL_ATTR_TYPE = 1;

        explicit MessageFormatException(uint8_t err_code);
    };
}

#endif //ROUTE_EXCEPTION_HPP
