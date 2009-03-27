#ifndef _test_util_h_
#define _test_util_h_

#include <boct/boct_tree.h>

template <typename T>
void create_random_configuration_tree(boct_tree<T>* tree);

#define BOCT_TEST_UTIL_INSTANTIATE(T) extern "please include vgl/algo/vgl_intersection.txx first"

#endif