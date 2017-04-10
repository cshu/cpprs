#pragma once

#if defined(SSIZE_MAX) && (SSIZE_MAX>INT_MAX)
typedef ssize_t int_for_int_ssize_t;
#else
typedef int int_for_int_ssize_t;
#endif

