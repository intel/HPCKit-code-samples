#ifndef _DP_HPP
#define _DP_HPP

#pragma once

#include <stdlib.h>
#include <exception>

#include <CL/sycl.hpp>

namespace dpc_common {
// this exception handler with catch async exceptions
static auto exception_handler = [](cl::sycl::exception_list eList) {
  for (std::exception_ptr const &e : eList) {
    try {
      std::rethrow_exception(e);
    } catch (std::exception const &e) {
#if _DEBUG
      std::cout << "Failure" << std::endl;
#endif
      std::terminate();
    }
  }
};


using Duration = std::chrono::duration<double>;

class MyTimer {
 public:
   MyTimer() : start(std::chrono::steady_clock::now()) {}

  Duration elapsed() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<Duration>(now - start);
  }

 private:
  std::chrono::steady_clock::time_point start;
};

};  // namespace dpc_common

#endif
