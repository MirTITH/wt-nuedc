#pragma once

#include <vector>
#include "freertos_io/uart_device.hpp"
#include <cstdint>
#include <type_traits>
#include <array>
#include "in_handle_mode.h"

namespace vofa
{

namespace vofa_internal
{
constexpr char kTail[4] = {0x00, 0x00, 0x80, 0x7f};

} // namespace vofa_internal

class JustFloatStream
{
protected:
    AsyncUart &uart_;
    bool is_locked_ = false;

private:
    void Write(const float *value, size_t size)
    {
        bool is_in_handle_mode = InHandlerMode();

        if (is_in_handle_mode) {
            if (is_locked_ == false) {
                uart_.lock_.lock_from_isr();
                is_locked_ = true;
            }

            uart_.WriteInIsr((const char *)value, size * sizeof(float));
        } else {
            if (is_locked_ == false) {
                uart_.lock_.lock_from_thread();
                is_locked_ = true;
            }

            uart_.WriteInThreadWithoutLock((const char *)value, size * sizeof(float));
        }
    }

    void SendEndMarkAndUnlock()
    {
        bool is_in_handle_mode = InHandlerMode();

        if (is_in_handle_mode) {
            if (is_locked_ == false) {
                uart_.lock_.lock_from_isr();
                is_locked_ = true;
            }

            uart_.WriteInIsr(vofa_internal::kTail, sizeof(vofa_internal::kTail));
            uart_.lock_.unlock_from_isr();
        } else {
            if (is_locked_ == false) {
                uart_.lock_.lock_from_thread();
                is_locked_ = true;
            }

            uart_.WriteInThreadWithoutLock(vofa_internal::kTail, sizeof(vofa_internal::kTail));
            uart_.lock_.unlock_from_thread();
        }
        is_locked_ = false;
    }

public:
    JustFloatStream(AsyncUart &uart = MainUart)
        : uart_(uart){};

    template <typename T, size_t array_size>
    friend JustFloatStream &operator<<(JustFloatStream &jfo, const std::array<T, array_size> &values)
    {
        if (std::is_same_v<T, float>) {
            jfo.Write(values.data(), array_size);
        } else {
            std::array<float, array_size> buffer;
            for (size_t i = 0; i < array_size; i++) {
                buffer[i] = values[i];
            }
            jfo.Write(buffer.data(), array_size);
        }
        return jfo;
    }

    template <typename T>
    friend JustFloatStream &operator<<(JustFloatStream &jfo, const std::vector<T> &values)
    {
        size_t size = values.size();
        if (std::is_same_v<T, float>) {
            jfo.Write(values.data(), size);
        } else {
            std::vector<float> buffer(size);
            for (size_t i = 0; i < size; i++) {
                buffer[i] = values[i];
            }
            jfo.Write(buffer.data(), size);
        }
        return jfo;
    }

    template <typename T>
    friend JustFloatStream &operator<<(JustFloatStream &jfo, const T &value)
    {
        float data = value;
        jfo.Write(&data, 1);
        return jfo;
    }

    friend void EndJFStream(vofa::JustFloatStream &jfo);
    friend JustFloatStream &operator<<(JustFloatStream &jfo, void (*func)(JustFloatStream &jfo))
    {
        func(jfo);
        return jfo;
    }
};

/**
 * @brief 发送数据结束标志
 */
void EndJFStream(vofa::JustFloatStream &jfo);

/**
 * @brief 用于向 vofa 输出 JustFloat 数据。（直接输出 float 原始数据，速度极快）
 * @note 用法：JFStream << float1 << float2 << vector1 << array1 << EndJFStream;
 *@note 最后一定要加 EndJFStream，否则 vofa 不知道数据在哪结束
 */
extern JustFloatStream JFStream;

} // namespace vofa

// 使用 vofa 命名空间，方便使用
using namespace vofa;
