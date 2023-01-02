#pragma once

#include <iostream>

/* [[nodiscard]] */
#define __NODISCARD [[nodiscard]]

	/* inline */
#ifdef _WIN32
#define __INLINE __forceinline
#endif

	/* ASSERT() */
#ifdef _DEBUG
#define __BREAK() __debugbreak()
#define __ASSERT(expr) \
	if ((expr)) {} \
	else \
	{ \
		std::cout << #expr << "\n"; \
		__BREAK(); \
	}
#define __CASSERT(expr) \
	if constexpr ((expr)) {} \
	else \
	{ \
		std::cout << #expr << "\n"; \
		__BREAK(); \
	}
#else
#define __ASSERT(expr)
#define __CASSERT(expr)
#endif

	/* std::move */
#ifdef _DEBUG
#define __MOVE(val) static_cast<std::remove_reference_t<decltype((val))>&&>((val))
#else
#define __MOVE(val) std::move((val))
#endif

	/* std::forward */
#ifdef _DEBUG
#define __FORWARD(val) static_cast<decltype((val))&&>((val))
#else
#define __FORWARD(val) std::forward<decltype((val))>((val))
#endif