#ifndef bvpl_octree_test_utils_h_
#define bvpl_octree_test_utils_h_
//:
// \file
// \brief  A file containing utility functions for other tests
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <boxm/boxm_scene.h>

void init_tree(boct_tree<short,float> *tree, unsigned i, float init_val = 0.8);

boxm_scene<boct_tree<short, float> >* create_scene(unsigned world_dimx = 2,unsigned world_dimy = 2,unsigned world_dimz = 2,
                                                   bool uniform = false, float val = 0.5, const std::string& scene_prefix = "test_scene");

void clean_up();

void clean_up(const std::string& dir, const std::string& ext);

#endif
