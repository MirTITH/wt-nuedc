#pragma once

#include <vector>
#include "freertos_io/uart_device.hpp"
#include <cstdint>
#include <type_traits>

namespace vofa
{
// constexpr char kTail[4] = {0x00, 0x00, 0x80, 0x7f};

// template <typename T>
// void append(std::vector<float> &v, const T &t)
// {
//     static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");
//     v.push_back(static_cast<float>(t));
// }

// template <typename T, typename... Args>
// void append(std::vector<float> &v, const T &t, const Args &...args)
// {
//     static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");
//     v.push_back(static_cast<float>(t));
//     append(v, args...);
// }

// template <typename... Args>
// std::vector<float> convert_to_float(const Args &...args)
// {
//     std::vector<float> result;
//     append(result, args...);
//     return result;
// }

} // namespace vofa