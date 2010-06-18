#ifndef boxm_online_update_manager_h_
#define boxm_online_update_manager_h_
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
#include <boxm/boxm_apm_traits.h>

#include <vil/vil_image_view.h>

template <class T_data>
class boxm_online_update_manager : public bocl_manager<boxm_online_update_manager<T_data> > 
{
 
  public:

    typedef boct_tree<short,T_data> tree_type;
    typedef boct_tree_cell<short,T_data> cell_type;

	typedef float obs_type;

    boxm_online_update_manager() : 
    cells_(0), 
    cell_data_(0), 
    cell_aux_data_(0), 
    root_level_(0),
	img_dims_(0),
	offset_y_(0),
	offset_x_(0),
	bni_(1),bnj_(1),
	wni_(1),wnj_(1),
	cells_size_(0),
	cell_data_size_(0),
	input_img_(),
    program_pass0_(0) {}
    ~boxm_online_update_manager() {  
      if (program_pass0_)
        clReleaseProgram(program_pass0_);
    }

    // read the scene, cam and image
	bool init_update(boxm_scene<tree_type> *scene,
                     vpgl_camera_double_sptr cam,
                     vil_image_view<obs_type> &obs);
	//: 2d workgroup
	void set_bundle_ni(unsigned bundle_x){bni_=bundle_x;}
	void set_bundle_nj(unsigned bundle_y){bnj_=bundle_y;}
                     
    //: run update
    bool run_scene();
   
	//: set  root level
	bool set_scene_data();
	bool set_scene_data_buffers();
	bool release_scene_data_buffers();
	bool clean_scene_data();

	//: set input image and image dimensions and camera
	bool set_input_view();
	bool set_input_view_buffers();
	bool release_input_view_buffers();
	bool clean_input_view();

	//: set the tree, data , aux_data and bbox
    bool set_tree(tree_type* tree);
	bool set_tree_buffers();
	bool release_tree_buffers();
	bool clean_tree();

	unsigned wni(){return wni_;}
	unsigned wnj(){return wnj_;}

	bool read_output_image();
	bool read_trees();
void print_tree();

    //: cleanup
    bool clean_update();

	cl_float * output_image(){return image_;}

  protected:

    //: helper functions
    bool run_block();
  
	// Set up Camera 
	bool set_persp_camera();
    bool set_persp_camera_buffers();
	bool release_persp_camera_buffers();
	bool clean_persp_camera();

	// Set up input image
	bool set_input_image();
	bool set_input_image_buffers();
	bool release_input_image_buffers();
	bool clean_input_image();
	// bool set_root_level
	bool set_root_level();
	bool set_root_level_buffers();
	bool release_root_level_buffers();
	bool clean_root_level();

	bool set_offset_buffers(int off_x, int off_y);
	bool release_offset_buffers();
    //open cl side helper functions
    int build_kernel_program(cl_program & program);
    cl_kernel kernel() {return kernel_;}
 
    //necessary CL items
    // for pass0 to compute seg len 
	cl_program program_pass0_;
	// for pass1 to compute vis inf and pre inf
	cl_program program_pass1_;

    cl_command_queue command_queue_;
    cl_kernel kernel_;

    //array of tree cells,
    cl_int* cells_;
	cl_uint  cells_size_;
    
    //array of data pointed to by tree
    cl_float* cell_data_;
    cl_uint  cell_data_size_;

	// array of aux data point to by tree
	cl_float* cell_aux_data_;

	//root level
    cl_uint root_level_;

	// image dimensions
	cl_uint* img_dims_;

	//offset for non-overlapping sections
	cl_uint  offset_x_;
	cl_uint  offset_y_;
	// bounding box for each tree
	cl_float * tree_bbox_;

	// image
	cl_float * image_;

	// camera

	cl_float * persp_cam_;


	cl_uint bni_;
	cl_uint bnj_;
	//: workspace dimensions which will be 
	//  greater than or equal to image dimensions

	cl_uint wni_;
	cl_uint wnj_;
    //pointer to cl memory on GPU
    cl_mem   cells_buf_;
    cl_mem   cell_data_buf_;
    cl_mem   cell_aux_data_buf_;
	cl_mem   tree_bbox_buf_;

    cl_mem   persp_cam_buf_;
	cl_mem   image_buf_;
	cl_mem   img_dims_buf_;

	cl_mem   offset_x_buf_;
	cl_mem   offset_y_buf_;

	cl_mem   root_level_buf_;

    boxm_scene<tree_type> * scene_;
	vpgl_camera_double_sptr cam_;
    vil_image_view<obs_type>  input_img_;
   
};

#endif // boxm_online_update_manager_h_
