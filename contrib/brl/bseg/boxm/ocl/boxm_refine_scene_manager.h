#ifndef boxm_refine_scene_manager_h_
#define boxm_refine_scene_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/boxm_scene.h>

class boxm_refine_scene_manager : public bocl_manager<boxm_refine_scene_manager > 
{
 
  public:
    //make sure cells and data pointers are initialized to null
    boxm_refine_scene_manager() : 
    cells_(0), cell_data_(0), block_ptrs_(0),
    tree_results_(0), data_results_(0),
    program_(0) {}
    
    ~boxm_refine_scene_manager() {  
      if (program_)
        clReleaseProgram(program_);
    }

    //: initialize scene data/tree data from arrays
    //These functions will allocate and prepare CPU side buffers
    bool init(int* cells, int* block_ptrs, int num_blocks, int tree_max_size, 
              int start_ptr, int end_ptr,
              float* data, int data_size, int data_max_size,
              float prob_thresh, int max_level, float bbox_len);

    //: run refinement
    //bool run_scene();
    bool run_scene();
   
    //returns resulting tree (ideally post refinement) 
    int* get_scene() { return tree_results_; }
    int* get_block_ptrs() { return block_ptrs_results_; }
    float* get_data() { return data_results_; }
    int get_data_size() { return data_max_size_; }
    
    //this will be replaced by get_buffer_bookkeeping or something that includes all pointers
    int get_start_ptr() { return start_ptr_; }
    int get_end_ptr() { return end_ptr_; }
    
    
    //: cleanup
    bool clean_refine();

  protected:

    //: helper functions
    bool run_block();
    //bool read_output();
    
    //host side helper functions
    bool alloc_trees(int cells_size, int data_input_size, int num_blocks);
    bool free_trees();
    
    //open cl side helper functions
    int setup_tree_buffers();
    bool read_tree_buffers();
    int clean_tree_buffers();
    int build_kernel_program();
    int set_kernel_args();
    bool init_kernel();
    cl_kernel kernel() {return kernel_;}
 
    //necessary CL items
    cl_program program_;
    cl_command_queue command_queue_;
    cl_kernel kernel_;

    //array of tree cells,
    cl_int* cells_;
    cl_int* block_ptrs_;
    cl_int  num_blocks_;
    cl_uint tree_max_size_;  //This and data_max_size can be #defines in cxx...
    cl_int  start_ptr_, end_ptr_;
    
    //array of data pointed to by tree
    cl_float* cell_data_;
    cl_uint numdata_;
    cl_uint data_max_size_;
    
    //probability threshold and max tree level
    cl_float prob_thresh_;
    cl_uint max_level_;
    cl_float bbox_len_;

    //output array of tree cells and new size
    cl_int* tree_results_;
    cl_int* block_ptrs_results_;
    cl_float* data_results_;
    
    //pointer to cl memory on GPU
    cl_mem   cell_buf_;
    cl_mem   block_ptrs_buf_;
    cl_mem   data_buf_;
    cl_mem   start_ptr_buf_, end_ptr_buf_;
    
    ///////////////////////////////////
    //OUTPUT BUFFERS
    cl_mem   output_buf_;
    cl_float output_results_;
    float gpu_time;
    //////////////////////////////////

};

#endif // boxm_refine_scene_manager_h_
