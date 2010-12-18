#include <icam_ocl/icam_ocl_minimizer.h>
#include <vul/vul_timer.h>
bool icam_ocl_minimizer::
exhaustive_rotation_search(vgl_vector_3d<double> const& trans,
                           unsigned level,
                           double min_allowed_overlap,
                           vgl_rotation_3d<double>& min_rot,
                           double& min_cost,
                           double& min_overlap_fraction,
                           bool setup,
                           bool finish)
{
  if (setup)
    init_opencl_rotation(level);
  if (finish){
    finish_opencl();
    return true;
  }
  search_mgr_->copy_trans_to_buffer(trans);
  if (!run_rotation_kernel())
    return false;
  float min_costf;
  search_mgr_->find_min_rot(min_rot, min_costf);
  min_cost = min_costf;
  min_overlap_fraction = min_allowed_overlap;//FIX-ME
  return true;
}

bool icam_ocl_minimizer::
initialized_rot_search(vgl_vector_3d<double> const& trans,
                       vgl_rotation_3d<double>& initial_rot,
                       unsigned initial_level,
                       unsigned search_level,
                       double min_allowed_overlap,
                       vgl_rotation_3d<double>& min_rot,
                       double& min_cost,
                       double& min_overlap_fraction,
                       bool setup,
                       bool finish)
{
  if (setup)
    init_opencl_rotation(initial_rot, initial_level, search_level);
  if (finish){
    finish_opencl();
    return true;
  }
  search_mgr_->copy_trans_to_buffer(trans);
  if (!run_rotation_kernel())
    return false;
  float min_costf;
  search_mgr_->find_min_rot(min_rot, min_costf);
  min_cost = min_costf;
  min_overlap_fraction = min_allowed_overlap;//FIX-ME
  return true;
}

bool icam_ocl_minimizer::init_opencl_rotation(unsigned level)
{
  search_mgr_->set_workgrp_ni(wgsize_);   search_mgr_->set_workgrp_nj(0);
  search_mgr_->set_nbins(params_.nbins_);
  search_mgr_->encode_image_data(*this, level);
  search_mgr_->set_nbins_buffer();
  search_mgr_->copy_to_image_buffers();
  search_mgr_->setup_top_level_rot_search_space(*this, level);
  search_mgr_->create_rot_parallel_transf_data();
  search_mgr_->set_rot_parallel_transf_data(vgl_vector_3d<double>());
  search_mgr_->create_rot_parallel_transf_buffers();
  search_mgr_->setup_rot_parallel_result();
  search_mgr_->create_rot_parallel_result_buffers();
  if (!search_mgr_->load_kernel_source(rot_kernel_path_))
    return false;
  if (search_mgr_->build_kernel_program()!=SDK_SUCCESS)
    return false;
  if (search_mgr_->create_kernel("trans_parallel_transf_search")!=SDK_SUCCESS)
    return false;
  if (!search_mgr_->setup_rot_parallel_kernel())
    return false;

  return true;
}

bool icam_ocl_minimizer::
init_opencl_rotation(vgl_rotation_3d<double> const& initial_rot,
                     unsigned initial_level,
                     unsigned search_level)
{
  search_mgr_->set_workgrp_ni(wgsize_);   search_mgr_->set_workgrp_nj(0);
  search_mgr_->set_nbins(params_.nbins_);
  search_mgr_->encode_image_data(*this, search_level);
  search_mgr_->set_nbins_buffer();
  search_mgr_->copy_to_image_buffers();
  search_mgr_->setup_initialized_rot_search_space(*this, initial_rot,
                                                  initial_level,
                                                  search_level);
  search_mgr_->create_rot_parallel_transf_data();
  search_mgr_->set_rot_parallel_transf_data(vgl_vector_3d<double>());
  search_mgr_->create_rot_parallel_transf_buffers();
  search_mgr_->setup_rot_parallel_result();
  search_mgr_->create_rot_parallel_result_buffers();
  if (!search_mgr_->load_kernel_source(rot_kernel_path_))
    return false;
  if (search_mgr_->build_kernel_program()!=SDK_SUCCESS)
    return false;
  if (search_mgr_->create_kernel("trans_parallel_transf_search")!=SDK_SUCCESS)
    return false;
  if (!search_mgr_->setup_rot_parallel_kernel())
    return false;

  return true;
}

bool icam_ocl_minimizer::run_rotation_kernel()
{
  vul_timer t;
  if (search_mgr_->run_rot_parallel_kernel()!=SDK_SUCCESS)
    return false;
  if (verbose_)
    vcl_cout << "OpenCL search time " << t.real()/1000.0 << " seconds\n" << vcl_flush;
  return true;
}

bool icam_ocl_minimizer::finish_opencl()
{
  search_mgr_->release_queue();
  search_mgr_->release_buffers();
  search_mgr_->release_kernel();
  search_mgr_->clean_image_data();
  search_mgr_->clean_rot_parallel_transf_data();
  search_mgr_->clean_rot_parallel_result();
  return true;
}
