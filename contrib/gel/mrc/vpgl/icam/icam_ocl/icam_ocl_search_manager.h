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
#include <icam/icam_minimizer.h>
class icam_ocl_search_manager : public icam_ocl_manager<icam_ocl_search_manager> 
{
 public:


  icam_ocl_search_manager()
    : program_(0),time_in_secs_(0.0f), wgni_(0), wgnj_(0), wsni_(0), wsnj_(0),
     sni_(0), snj_(0), dni_(0), dnj_(0)
    {}

  ~icam_ocl_search_manager();


   vcl_string program_source() const {return prog_;}
  cl_program program() {return program_;}


  bool run_kernel();

  int build_kernel_program();

  int create_kernel(vcl_string const& name);
  int release_kernel();

  float time_taken(){return time_in_secs_;}
  cl_kernel kernel() {return kernel_;}

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

    //------------------  icam specifics ------------------------
  // source, destination and depth images
  bool encode_image_data(icam_minimizer& minimizer, unsigned level);
  bool copy_to_image_buffers();
  bool release_image_buffers();
  void clean_image_data();
  // precompute the translation and rotations in the search space
  void setup_transf_search_space(vgl_box_3d<double> const& trans_box,
                                 vgl_vector_3d<double> const& trans_steps,
                                 icam_minimizer& minimizer,
                                 unsigned level);
  //: create the cl data 
  bool create_image_parallel_transf_data();
  //: assign the cl data 
  bool set_image_parallel_transf(vgl_vector_3d<double> const& tr,
                                 vgl_rotation_3d<double> const& rot);
  //: set up kernel - assign arguments, create command queue etc.
  bool setup_image_parallel_kernel();
  //: release the command queue
  bool release_queue();
  //: create cl buffers
  bool create_image_parallel_transf_buffers();
  //: copy to cl buffers
  bool copy_to_image_parallel_transf_buffers();
  //: release buffers
  bool release_image_parallel_transf_buffers();
  //: deallocate cl data
  void clean_image_parallel_transf_data();

  bool setup_image_parallel_result();
  bool create_image_parallel_result_buffers();
  bool release_image_parallel_result_buffers();
  void clean_image_parallel_result();

  cl_int4 image_para_flag() {return *image_para_flag_;}
  cl_float4 image_para_result() {return *image_para_result_;}
  cl_float* source_array() {return source_array_;}
  cl_float* result_array() {return result_array_;}
  cl_float* mask_array() {return mask_array_;}
 protected:
  unsigned wgni_, wgnj_;//work group size
  unsigned wsni_, wsnj_;//work group size
  cl_program program_;

  cl_command_queue command_queue_;
  cl_kernel kernel_;
  float time_in_secs_;
  //source dimensions
  unsigned sni_;
  unsigned snj_;
  //source image
  cl_uint * cl_sni_;
  cl_uint * cl_snj_;
  cl_mem   sni_buf_;
  cl_mem   snj_buf_;
  cl_float4* Ks_; // source image K matrix
  cl_mem   Ks_buf_;
  cl_float* source_array_;
  cl_mem   source_array_buf_;
  //dest image
  unsigned dni_;
  unsigned dnj_;
  cl_uint * cl_dni_;
  cl_uint * cl_dnj_;
  cl_mem   dni_buf_;
  cl_mem   dnj_buf_;
  cl_float4* Kdi_;// inverse  K matrix for destination image
  cl_mem   Kdi_buf_;
  cl_float* dest_array_;
  cl_mem   dest_array_buf_;

  // depth image, same dimensions as dest
  cl_float* depth_array_;
  cl_mem   depth_array_buf_;

  // result mapped image, same dimensions as dest
  cl_float* result_array_;
  cl_mem   result_array_buf_;

  // result mask image, same dimensions as dest
  cl_float* mask_array_;
  cl_mem   mask_array_buf_;

  // search rotations
  vcl_vector<vgl_rotation_3d<double> > rotations_;
  cl_float4* rotation_;
  cl_mem   rot_buf_;
  // search translations 
  vcl_vector<vgl_vector_3d<double> > translations_;
  cl_float4* translation_;
  cl_mem   trans_buf_;

  // the result of the search
  cl_float4* image_para_result_;
  cl_int4 * image_para_flag_;
  cl_mem   image_para_result_buf_;
  cl_mem   image_para_flag_buf_;
};

#endif // icam_ocl_search_manager_h_
