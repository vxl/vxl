#ifndef boct_test_util_hxx_
#define boct_test_util_hxx_

#include "boct_test_util.h"

#include <boct/boct_tree_cell.h>
#include <vnl/vnl_random.h>

template <class T_loc,class T_data>
void create_random_configuration_tree(boct_tree<T_loc,T_data>* tree)
{
    for (unsigned int i=0;i<100;i++)
    {
        vnl_random rand;
        vgl_point_3d<double> p(rand.drand32(),rand.drand32(),rand.drand32());
        boct_tree_cell<T_loc,T_data>* curr_cell=static_cast<boct_tree_cell<T_loc,T_data>*>(tree->locate_point(p));
        if (curr_cell && curr_cell->level()>0)
            curr_cell->split();
    }
}

#undef BOCT_TEST_UTIL_INSTANTIATE
#define BOCT_TEST_UTIL_INSTANTIATE(T_loc,T_data) \
template void create_random_configuration_tree(boct_tree<T_loc,T_data >*)

#endif // boct_test_util_hxx_
