#ifndef boct_test_util_h_
#define boct_test_util_h_

#include <boct/boct_tree.h>

template <class T_loc,class T_data>
void create_random_configuration_tree(boct_tree<T_loc,T_data>* tree);

#define BOCT_TEST_UTIL_INSTANTIATE(T_loc,T_data) extern "please include vgl/algo/vgl_intersection.txx first"

#endif
