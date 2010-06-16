#ifndef boxm_refine_manager_h_
#define boxm_refine_manager_h_
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

template <class T_data>
class boxm_refine_manager : public bocl_manager<boxm_refine_manager<T_data> > 
{
 
  public:

    typedef boct_tree<short,T_data> tree_type;
    typedef boct_tree_cell<short,T_data> cell_type;

    boxm_refine_manager() : 
    cells_(0), numcells_(0), tree_max_size_(0),
    cell_data_(0), numdata_(0), data_max_size_(0),
    tree_results_(0), tree_results_size_(0),
    data_results_(0), data_results_size_(0),
    prob_thresh_(0), max_level_(0),
    program_(0) {}
    ~boxm_refine_manager() {  
      if (program_)
        clReleaseProgram(program_);
    }

    //: initialize scene data/tree data from different formats
    //These functions will allocate and prepare CPU side buffers
    //(cl_int*)cells_, 
    //(cl_float*)data_, 
    //(cl_int*) tree_max_size_
    //(cl_int*) numLevels_
    bool init(tree_type* tree, float prob_thresh, unsigned max_level);
    bool init(int* cells, unsigned numcells, unsigned tree_max_size,
              float* data, unsigned data_size, unsigned data_max_size,
              float prob_thresh, unsigned max_level);

    //: run refinement
    //bool run_scene();
    bool run_tree();
   
    //returns resulting tree (ideally post refinement) 
    //- dereferencing on return may cause segfaults if not defined...
    int* get_tree() { return tree_results_; }
    int get_tree_size() { return (*tree_results_size_); }
    float* get_data() { return data_results_; }
    float get_data_size() { return (*data_results_size_); }
    
    //: cleanup
    bool clean_refine();

  protected:

    //: helper functions
    bool run_block();
    //bool read_output();
    
    //host side helper functions
    bool format_tree(tree_type* tree);
    bool alloc_trees(int cells_size, int data_input_size);
    bool free_trees();
    
    //open cl side helper functions
    int setup_tree_buffers();
    bool read_tree_buffers();
    int clean_tree_buffers();
    int build_kernel_program();
    cl_kernel kernel() {return kernel_;}
 
    //necessary CL items
    cl_program program_;
    cl_command_queue command_queue_;
    cl_kernel kernel_;

    //array of tree cells,
    cl_int* cells_;
    cl_uint* numcells_;
    cl_uint* tree_max_size_;
    
    //array of data pointed to by tree
    cl_float* cell_data_;
    cl_uint* numdata_;
    cl_uint* data_max_size_;
    
    //probability threshold and max tree level
    cl_float* prob_thresh_;
    cl_uint* max_level_;

    //output array of tree cells and new size
    cl_int* tree_results_;
    cl_uint* tree_results_size_;
    cl_float* data_results_;
    cl_uint* data_results_size_;
    
    //pointer to cl memory on GPU
    cl_mem   cell_buf_;
    cl_mem   cell_size_buf_;
    cl_mem   cell_max_size_buf_;
    cl_mem   data_buf_;
    cl_mem   data_size_buf_;
    cl_mem   data_max_size_buf_;
    cl_mem   prob_thresh_buf_;
    cl_mem   max_level_buf_;
    
    
    ///////////////////////////////////
    //TODO DELETE ME
    cl_float* output_results_;
    cl_float* output_input_;
    cl_mem   output_buf_;
    //////////////////////////////////

};

#endif // boxm_refine_manager_h_
