#ifndef _test_util_h_
#define _test_util_h_

#include <boct/boct_tree.h>

template <class T_loc,class T_data, class T_aux>
void create_random_configuration_tree(boct_tree<T_loc,T_data,T_aux>* tree);

#define BOCT_TEST_UTIL_INSTANTIATE(T_loc,T_data,T_aux) extern "please include vgl/algo/vgl_intersection.txx first"

#endif