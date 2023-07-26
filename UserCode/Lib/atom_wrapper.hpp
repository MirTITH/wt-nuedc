/**
 * @file atom_wrapper.hpp
 * @author X. Y.
 * @brief 用于在 vector 中使用 atomic
 * @version 0.1
 * @date 2023-07-23
 * @note See https://stackoverflow.com/a/13194652
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <atomic>

template <typename T>
class atom_wrapper : public std::atomic<T>
{
public:
    using std::atomic<T>::atomic;
    using std::atomic<T>::operator=;

    atom_wrapper(const std::atomic<T> &a)
        : std::atomic<T>(a.load()){};

    atom_wrapper(const atom_wrapper &other)
        : std::atomic<T>(other.load()){};
};
