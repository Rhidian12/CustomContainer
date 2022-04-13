#pragma once

#include <iostream>

#ifdef NDEBUG
#define ASSERT(expression, message) ((void)0);
#else
#define ASSERT(expression, message) if (!expression) { std::cout << message << "\n"; std::abort(); }
#endif