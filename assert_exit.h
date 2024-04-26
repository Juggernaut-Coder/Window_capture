#pragma once /* RT_ASSERT.H */

#include <stdlib.h>
#include <stdio.h>

#define ASSERT_EXIT(expr, str, ...)                                                                                                      \
  ((void) sizeof ((expr) ? 1 : 0), __extension__ ({                                                                                      \
    if (!(expr)) {                                                                                                                       \
        fprintf(stderr,"At function [%s] in file [%s:%d], assert failed: [(%s)].\n" str "\n", __func__, __FILE__, __LINE__, (#expr));  \
        __VA_ARGS__;                                                                                                                     \
        exit(EXIT_FAILURE);                                                                                                              \
    }                                                                                                                                    \
  }))

