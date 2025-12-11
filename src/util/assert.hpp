#pragma once

#include <cassert>

// ============================================================================
//                      CONSTEXPR-COMPATIBLE ASSERTIONS
// ============================================================================
//
// Standard assert() is not constexpr-friendly in all contexts, causing
// IntelliSense/clangd to complain even when the code compiles correctly.
//
// This header provides CONSTEXPR_ASSERT which:
//   - In release builds (NDEBUG defined): no-op
//   - In debug builds at compile time: triggers a compile error
//   - In debug builds at runtime: triggers standard assert()
//
// Usage:
//   constexpr int foo(int x) {
//       CONSTEXPR_ASSERT(x > 0);
//       return x * 2;
//   }
// ============================================================================

#ifdef NDEBUG

#define CONSTEXPR_ASSERT(cond) ((void)0)

#else

namespace util::detail {

// C++23 constexpr-friendly assertion
// Uses `if consteval` to distinguish compile-time vs runtime evaluation
constexpr void constexpr_assert_impl(bool cond) {
    if consteval {
        // Compile-time: invalid pointer dereference causes compile error
        if (!cond) {
            int* p = nullptr;
            *p = 0;  // Compile error if condition fails at compile time
        }
    } else {
        // Runtime: use standard assert
        assert(cond);
    }
}

}  // namespace util::detail

#define CONSTEXPR_ASSERT(cond) ::util::detail::constexpr_assert_impl(cond)

#endif
