#ifndef boxm_ocl_utils_h_
#define boxm_ocl_utils_h_

#include "bocl/bocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_stack.h>

#include <boct/boct_tree_cell.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>
template<class T>
class boxm_ocl_convert
{
 public:

  static void pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* cell_ptr,  vnl_vector_fixed<float, 16> &data);


  static void pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data);


  static void pack_cell_data(boct_tree_cell<short, float> * cell_ptr, vnl_vector_fixed<float, 16> &data);


  static void copy_to_arrays(boct_tree_cell<short, T >* cell_ptr,
                 vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
                 vcl_vector<vnl_vector_fixed<float, 16> >& data_array,
                 int cell_input_ptr);

};

class boxm_ocl_utils
{
 public:




  static void split(vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
                    int parent_ptr,
                    int& child_ptr);

  static void print_tree_array(int* tree, unsigned numcells, float* data);
  
  
  static void* alloc_aligned(unsigned n, unsigned unit_size, unsigned block_size);
  
  static void free_aligned(void* ptr);
  
  static bool verify_format(vcl_vector<vnl_vector_fixed<int, 4> > cell_array);
  
  // Binary read and writes for a tree.
  static cl_int* readtree(vcl_string tree_file, unsigned int & tree_size );
  static bool writetree(vcl_string tree_file,cl_int* cell_array, unsigned int tree_size );
  static bool writetreedata(vcl_string tree_data_file,cl_float* data_array, unsigned int  tree_data_size);
  static cl_float*  readtreedata(vcl_string tree_data_file,unsigned int & tree_data_size );

};



#endif
