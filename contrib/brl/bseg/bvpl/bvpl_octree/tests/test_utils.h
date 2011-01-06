#ifndef test_utils_h_
#define test_utils_h_
//:
// \file
// \brief  A file containing utility functions for other tests
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <boxm/boxm_scene.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpl/vpl.h>

void init_tree(boct_tree<short,float> *tree, unsigned i);

boxm_scene<boct_tree<short, float> >* create_scene(unsigned world_dimx = 2,unsigned world_dimy = 2,unsigned world_dimz = 2);

void clean_up();

void clean_up(vcl_string dir, vcl_string ext);
#endif
