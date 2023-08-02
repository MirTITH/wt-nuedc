#pragma once

/**
 * @file watchdog.hpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2023-08-02
 *
 * @copyright Copyright (c) 2023
 *
 * Example:
 *
    void kBtnWatchDogCallback(void *)
    {
        KeyboardLed.SetColor(1, 1, 0);
    }

    WatchDog kBtnWatchDog(kBtnWatchDogCallback, 5);
 */

#include <functional>
#include <cstdint>

class WatchDog
{
public:
    std::function<void(void *)> callback_func_;
    uint32_t err_count_ = 0;
    uint32_t max_err_count_;

    WatchDog(const std::function<void(void *)> &callback_func, uint32_t max_err_count = 1)
        : callback_func_(callback_func), max_err_count_(max_err_count){};

    bool Exam(bool is_ok, void *arg = nullptr)
    {
        if (is_ok == false) {
            err_count_++;
            if (err_count_ >= max_err_count_) {
                if (callback_func_ != nullptr) {
                    callback_func_(arg);
                }
            }
            return false;
        } else if (err_count_ > 0) {
            err_count_--;
        }
        return true;
    }
};
