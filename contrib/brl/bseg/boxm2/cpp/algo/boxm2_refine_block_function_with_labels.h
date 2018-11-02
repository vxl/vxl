#ifndef boxm2_refine_block_function_with_labels_h
#define boxm2_refine_block_function_with_labels_h
//:
// \file

#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<int, 16> int16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;
  typedef vnl_vector_fixed<float,4> float4;
template <class T>
class boxm2_refine_block_function_with_labels
{
 public:

  //: "default" constructor
  boxm2_refine_block_function_with_labels() {replace_=false;rgb_=false;}

  //: initialize generic data base pointers as their data type
  bool init_data(boxm2_block* blk, std::vector<boxm2_data_base*> & datas, float prob_thresh,
                  std::string app_type,std::string flow_prefix="resultant");

  //: refine function;
  bool refine();

  bool refine_deterministic(std::vector<boxm2_data_base*>& datas);

  //: refine bit tree
  boct_bit_tree refine_bit_tree(boct_bit_tree& curr_tree,
                                 int buff_offset,
                                 bool is_random=true);

  void recursive_refine(boct_bit_tree & tree,int i);

  //: move data into new location
  int move_data(boct_bit_tree& unrefined_tree,
                boct_bit_tree& refined_tree,
                float*  alpha_cpy,
                float* alpha_sav_cpy,
                T* mog_cpy,
                T* mog_sav_cpy,
                float4*   flow_cpy,
                float4*   flow_res_cpy);


  //: helper method
  int free_space(int startPtr, int endPtr);
  int num_split_;

 private:
  boxm2_block* blk_;
  bool replace_,rgb_;
  float*       alpha_,*alpha_sav_;
  short*       labels_;
  T*      mog_,*mog_sav_;
  ushort4*     num_obs_;
  T* appearance_type_;
  float4* flow_,*flow_res_;
  int app_type_size_;
  //block max level
  int max_level_;
  int MAX_INNER_CELLS_;
  int MAX_CELLS_;
  std::string app_type_,flow_prefix_;
  //max alpha integrated
  float max_alpha_int_;

  //length of data buffer
  int data_len_;

  //length of one side of a sub block
  double block_len_;


};

////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block_with_labels( boxm2_block* blk,
                         std::vector<boxm2_data_base*> & datas,
                         float prob_thresh,
                         bool is_random = true);

#endif
