#ifndef boxm_ocl_utils_h_
#define boxm_ocl_utils_h_

#include "bocl/bocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_stack.h>

#include <boct/boct_tree_cell.h>
#include <boct/boct_bit_tree.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>

template <class T>
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

  static void convert_scene(boxm_scene<boct_tree<short, T> >* scene, boxm_ocl_scene &ocl_scene, int max_mb);
  
  
  static void convert_bit_scene(boxm_scene<boct_tree<short, T> >* scene, boxm_ocl_bit_scene &ocl_scene, int max_mb, int buff_length);

};

class boxm_ocl_utils
{
 typedef vnl_vector_fixed<unsigned short,2> ushort2;
 public:

  static void split(vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
                    int parent_ptr,
                    int& child_ptr);

  static void print_tree_array(int* tree, unsigned numcells, float* data);

  static void print_multi_block_tree(int* blocks, int* block_ptrs, int numBlocks, float* data);

  static void* alloc_aligned(unsigned n, unsigned unit_size, unsigned block_size);

  static void free_aligned(void* ptr);

  static bool verify_format(vcl_vector<vnl_vector_fixed<int, 4> > cell_array);

  static int getBufferIndex(bool rand, 
                            vbl_array_1d<ushort2> mem_ptrs,
                            vbl_array_1d<unsigned short> blocksInBuffer,
                            int BUFF_LENGTH,
                            int blocks_per_buffer,
                            int tree_size,
                            vnl_random& random);
  static void random_permutation(int* buffer, int size); 

  // Binary read and writes for a tree.
  static cl_int* readtree(vcl_string tree_file, unsigned int & tree_size );
  static bool writetree(vcl_string tree_file,cl_int* cell_array, unsigned int tree_size );
  static bool writetreedata(vcl_string tree_data_file,cl_float* data_array, unsigned int  tree_data_size);
  static cl_float*  readtreedata(vcl_string tree_data_file,unsigned int & tree_data_size );
  
  static void compare_bit_scenes(boxm_ocl_bit_scene* one, boxm_ocl_bit_scene* two); 
  
  static void bit_lookup_table(unsigned char* bits);
};


#endif
