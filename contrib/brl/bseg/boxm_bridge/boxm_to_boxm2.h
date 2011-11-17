#ifndef boxm_to_boxm2_h_
#define boxm_to_boxm2_h_
//:
// \file

#include <boct/boct_tree.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm/sample/boxm_sample.h>

//: extracts the parameters of a boxm_sample for boxm2 representation
void deconstruct_sample(boxm_sample<BOXM_APM_MOG_GREY> sample,
                        float &alpha,
                        boxm2_data_traits<BOXM2_MOG3_GREY>::datatype& data,
                        boxm2_data_traits<BOXM2_NUM_OBS>::datatype& num_obs);

//: extracts the tree cell's data into an array format for boxm2 representation
template <class T_loc, class T_data>
void convert_data(boct_tree_cell<T_loc,T_data>* tree_cell,
                  float* alpha,
                  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype* data,
                  boxm2_data_traits<BOXM2_NUM_OBS>::datatype* num_obs,
                  int& data_idx);

//: converts the boct_octree to bit tree representation
template <class T_loc, class T_data>
void convert_to_bittree(boct_tree_cell<T_loc,T_data>* tree_cell, boct_bit_tree*& bit_tree);

#endif // boxm_to_boxm2_h_
