#ifndef icam_ocl_search_manager_h_
#define icam_ocl_search_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <icam_ocl/icam_ocl_manager.h>
#include <icam_ocl/icam_ocl_utils.h>
#include <icam_ocl/icam_ocl_mem.h>
#include <icam_ocl/icam_ocl_kernel.h>
#include <icam/icam_minimizer.h>
class icam_ocl_search_manager : public icam_ocl_manager<icam_ocl_search_manager> 
{
 public:
   
  icam_ocl_search_manager()
    : wgni_(0), wgnj_(0), wsni_(0), wsnj_(0),program_(0), time_in_secs_(0.0f),
    nbins_(0), sni_(0), snj_(0), dni_(0), dnj_(0)
    {kernel_ = new icam_ocl_kernel(); }

  ~icam_ocl_search_manager(); 


   vcl_string program_source() const {return prog_;}
  cl_program program() {return program_;}


  bool run_image_parallel_kernel();
  bool run_rot_parallel_kernel();

  int build_kernel_program();

  int create_kernel(vcl_string const& name);
  int release_kernel();

  float time_taken(){return time_in_secs_;}
  //cl_kernel kernel() {return kernel_;}

  //: 2d workgroup
  void set_workgrp_ni(unsigned ni){wgni_=ni;}
  void set_workgrp_nj(unsigned nj){wgnj_=nj;}

  vcl_size_t workgrp_ni(){return wgni_;}
  vcl_size_t workgrp_nj(){return wgnj_;}
  vcl_size_t workgrp_size(){return wgni_*wgnj_;}

  vcl_size_t work_space_ni(){return wsni_;}
  vcl_size_t work_space_nj(){return wsnj_;}

  vcl_size_t dest_ni(){return dni_;}
  vcl_size_t dest_nj(){return dnj_;}

  void set_nbins(unsigned nbins){nbins_ = nbins;}

    //------------------  icam specifics ------------------------
  // source, destination and depth images
  bool encode_image_data(icam_minimizer& minimizer, unsigned level);
  bool set_nbins_buffer();
  bool copy_to_image_buffers();
  bool release_buffers();
  void clean_image_data();
  // precompute the translation and rotations in the search space
  void setup_transf_search_space(vgl_box_3d<double> const& trans_box,
                                 vgl_vector_3d<double> const& trans_steps,
                                 icam_minimizer& minimizer,
                                 unsigned level);
  // for debug purposes 
  void  setup_rot_debug_space(unsigned n_rotations,
                              vgl_rotation_3d<double> const& rot);
  //: create the cl data 
  bool create_image_parallel_transf_data();
  //: assign the cl data 
  bool set_image_parallel_transf(vgl_vector_3d<double> const& tr,
                                 vgl_rotation_3d<double> const& rot);
  //: create the cl data for trans parallel
  bool create_rot_parallel_transf_data();
  //: set the cl data for trans parallel
  bool set_rot_parallel_transf_data(vgl_vector_3d<double> const& tr);
  //: deallocate rot parallel cl data
  void clean_rot_parallel_transf_data();
  //: create rot_parallel buffers
  bool create_rot_parallel_transf_buffers();

  //: set up kernel - assign arguments, create command queue etc.
  bool setup_image_parallel_kernel();
  //: set local args
  bool set_rot_parallel_local_args();
  bool setup_rot_parallel_kernel();
  //: release the command queue
  bool release_queue();
  //: create cl buffers
  bool create_image_parallel_transf_buffers();
  //: copy to cl buffers
  bool copy_to_image_parallel_transf_buffers();
  //: deallocate cl data
  void clean_image_parallel_transf_data();

  bool setup_image_parallel_result();
  bool create_image_parallel_result_buffers();
  void clean_image_parallel_result();

  bool setup_rot_parallel_result();
  bool create_rot_parallel_result_buffers();
  void clean_rot_parallel_result();

  cl_int4 image_para_flag() {return *image_para_flag_;}
  cl_float* image_para_result() {return image_para_result_;}
  cl_float* source_array() {return source_array_;}
  cl_float* result_array() {return result_array_;}
  cl_float* mask_array() {return mask_array_;}
  cl_int4 rot_para_flag() {return *rot_para_flag_;}
  cl_float* minfo_array(){return minfo_array_;}
 protected:
  unsigned wgni_, wgnj_;//work group size
  unsigned wsni_, wsnj_;//work group size

  cl_program program_;

  cl_command_queue command_queue_;
  //cl_kernel kernel_;
  float time_in_secs_;
  //histogram bins
  unsigned nbins_;
  cl_uint* cl_nbins_;
  //source dimensions
  unsigned sni_;
  unsigned snj_;
  //source image
  cl_uint * cl_sni_;
  cl_uint * cl_snj_;
  
  cl_float* Ks_; // source image K matrix
  cl_float* source_array_;
  
  //dest image
  unsigned dni_;
  unsigned dnj_;
  cl_uint * cl_dni_;
  cl_uint * cl_dnj_;
  cl_float* Kdi_;// inverse  K matrix for destination image
  cl_float* dest_array_;

  // depth image, same dimensions as dest
  cl_float* depth_array_;

  // result mapped image, same dimensions as dest
  cl_float* result_array_;
 
  // result mask image, same dimensions as dest
  cl_float* mask_array_;

  // search rotations
  vcl_vector<vgl_rotation_3d<double> > rotations_;
  cl_float* rotation_;
  cl_float* rot_array_;
  // search translations 
  vcl_vector<vgl_vector_3d<double> > translations_;
  cl_float* translation_;

  // the result of the search (image parallel)
  cl_float* image_para_result_;
  cl_int4 * image_para_flag_;

  // the result of the search (rot parallel)
  // minfo result
  cl_float* minfo_array_;
  cl_int4 * rot_para_flag_;

  icam_ocl_kernel* kernel_;
  vcl_map<void*, int> buffer_map_;

};

#endif // icam_ocl_search_manager_h_
