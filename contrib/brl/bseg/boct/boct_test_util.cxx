#include <boct/boct_test_util.h>
#include <boct/boct_tree_cell.h>
#include <vnl/vnl_random.h>

void create_random_configuration_tree(boct_tree* tree)
{
    for(unsigned int i=0;i<100;i++)
    {
        vnl_random rand;
        vgl_point_3d<double> p(rand.drand32(),rand.drand32(),rand.drand32());
        boct_tree_cell_sptr curr_cell=tree->locate_point(p);
        if(curr_cell && curr_cell->level()>0)
            curr_cell->split();
    }
}