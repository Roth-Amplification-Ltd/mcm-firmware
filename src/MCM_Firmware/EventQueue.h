/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include <array>
#include <cstddef>

namespace Mcm {

template <typename T, std::size_t Capacity>
class EventQueue final {
    static_assert(Capacity > 0U, "Queue capacity must be nonzero.");

public:
    EventQueue() noexcept = default;

    [[nodiscard]] bool push(const T& value) noexcept {
        if (count_ >= Capacity) {
            return false;
        }

        storage_[head_] = value;
        head_ = (head_ + 1U) % Capacity;
        ++count_;
        return true;
    }

    [[nodiscard]] bool pop(T& value) noexcept {
        if (count_ == 0U) {
            return false;
        }

        value = storage_[tail_];
        tail_ = (tail_ + 1U) % Capacity;
        --count_;
        return true;
    }

    void clear() noexcept {
        head_ = 0U;
        tail_ = 0U;
        count_ = 0U;
    }

    [[nodiscard]] bool empty() const noexcept { return count_ == 0U; }
    [[nodiscard]] bool full() const noexcept { return count_ == Capacity; }
    [[nodiscard]] std::size_t size() const noexcept { return count_; }

private:
    std::array<T, Capacity> storage_{};
    std::size_t head_ = 0U;
    std::size_t tail_ = 0U;
    std::size_t count_ = 0U;
};

}  // namespace Mcm
