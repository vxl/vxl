#ifndef boxm_algo_test_utils_h_
#define boxm_algo_test_utils_h_
//:
// \file
// \brief  A file containing utility functions for other tests
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <boxm/boxm_scene.h>

void init_tree(boct_tree<short,float> *tree, unsigned i);

boxm_scene<boct_tree<short, float> >* create_scene(unsigned world_dimx = 2,unsigned world_dimy = 2,unsigned world_dimz = 2,
                                                   bool uniform = false, std::string scene_name = "test_scene");

void clean_up();

#endif
