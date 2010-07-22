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
#include <boxm/ocl/boxm_ocl_scene.h>

class boxm_refine_scene_manager : public bocl_manager<boxm_refine_scene_manager > 
{
 
  public:
    //make sure cells and data pointers are initialized to null
    boxm_refine_scene_manager() : 
      block_ptrs_(0),    
      scene_dims_(0),
      scene_origin_(0), 
      block_nums_(0),
      tree_cells_(0), 
      data_cells_(0),
      alpha_cells_(0), 
      mem_ptrs_(0),
      program_(0){}
    
    ~boxm_refine_scene_manager() {  
      if (program_)
        clReleaseProgram(program_);
    }

    //: initialize scene data/tree data from arrays
    bool init_refine(boxm_ocl_scene* scene, float prob_thresh);
    
    //: run refinement
    bool run_refine();
   
    //: returns scene_ (refined) scene object  
    boxm_ocl_scene* get_scene() { return scene_; }
    
    //: cleanup
    bool clean_refine();
    
    //necessary CL items (public for setting in tableauxs and such)
    cl_program program_;
    cl_command_queue command_queue_;
    cl_kernel kernel_;

  protected:
  
    //memory allocation and cleanup subroutines 
    bool setup_scene_data();
    bool clean_scene_data();
    bool setup_scene_data_buffers();
    bool clean_scene_data_buffers();
    
    //get buffers from GPU and renew scene
    bool read_buffers();
    
    //opencl helper subroutines 
    bool init_kernel();
    int  build_kernel_program();
    int  set_kernel_args();
  
    //scene object
    boxm_ocl_scene* scene_;
  
    // scene data (host buffers)
    cl_int*   block_ptrs_;    //block pointer to tree's root
    cl_int*   block_nums_;    //(numX, numY, numZ);
    cl_int*   tree_cells_;    //array of tree cells (int4)
    cl_float* data_cells_;    //array of data pointed to by tree (float16)
    cl_float* alpha_cells_;    //alpha values for data (redundant)
    cl_int*   mem_ptrs_;      //(int2) points to tree_cells_ free mem
    cl_float* scene_dims_;    //x,y,z,0
    cl_float* scene_origin_;  //x,y,z,0
    
    //GPU mem pointers (to mirror above)
    cl_mem    block_ptrs_buf_;  
    cl_mem    block_nums_buf_;  //def necessary
    cl_mem    tree_cells_buf_;  //
    cl_mem    data_cells_buf_;
    cl_mem    alpha_cells_buf_;
    cl_mem    mem_ptrs_buf_;
    cl_mem    scene_dims_buf_;  //probably not necessary for refinement
    cl_mem    scene_origin_buf_;// also probably not necessary

    //tree and data buffer dimensions 
    cl_uint   tree_cell_size_;
    cl_uint   data_cell_size_; 
    cl_int    numbuffer_;
    cl_int    lenbuffer_;

    //refine arguments (prob thresh, max level, bbox length); 
    cl_float  prob_thresh_;
    cl_uint   max_level_;    //hard coded at 3 for now
    cl_float  bbox_len_;

    
    ///////////////////////////////////
    //OUTPUT BUFFERS
    cl_mem    output_buf_;
    cl_float  output_results_;
    float     gpu_time;
    //////////////////////////////////

};

#endif // boxm_refine_scene_manager_h_
