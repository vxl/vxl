#ifndef boxm2_to_boxm_h_
#define boxm2_to_boxm_h_
//:
// \file

#include <boct/boct_tree.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm/sample/boxm_sample.h>

//: combine the data values to create a sample
boxm_sample<BOXM_APM_MOG_GREY> create_sample(float alpha,
                                             boxm2_data_traits<BOXM2_MOG3_GREY>::datatype data,
                                             boxm2_data_traits<BOXM2_NUM_OBS>::datatype num_obs);

//: converts the bit tree to boct_octree representation
template <class T_loc, class T_data>
void convert_tree(boct_bit_tree const& bit_tree, boct_tree<T_loc,T_data>*& tree,
                  boxm2_data<BOXM2_ALPHA>* alpha_data,
                  boxm2_data<BOXM2_MOG3_GREY>* mog3_data,
                  boxm2_data<BOXM2_NUM_OBS>* num_obs);

#endif // boxm2_to_boxm_h_
