#pragma once
#include <exception>
#include <stdexcept>

class vm_fault : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
