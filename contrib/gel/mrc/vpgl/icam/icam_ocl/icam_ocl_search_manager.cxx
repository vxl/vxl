#include <vcl_cassert.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_matrix_fixed.h>
#include <icam_ocl/icam_ocl_search_manager.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vcl_cstdio.h>

icam_ocl_search_manager::~icam_ocl_search_manager()
{
}

bool icam_ocl_search_manager::
encode_image_data(icam_minimizer& minimizer, unsigned level)
{
  //encode smoothed source image
  vil_image_view<float> source = minimizer.source(level);
  sni_ = source.ni(); snj_ = source.nj();
#if 1
  vil_image_view<float> source_sm(sni_, snj_);
  vil_gauss_filter_5tap(source,source_sm,vil_gauss_filter_5tap_params(2));
#endif
  vnl_matrix_fixed<double, 3, 3> Ks = minimizer.to_calibration_matrix(level);

  // source cl_memory array
  unsigned slen = sni_*snj_;
#if defined (_WIN32)
  cl_nbins_ = (cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
  source_array_=(cl_float*)_aligned_malloc(slen * sizeof(cl_float), 16);
  Ks_ = (cl_float4*)_aligned_malloc(sizeof(cl_float4),16);
  cl_sni_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
  cl_snj_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
#elif defined(__APPLE__)
  cl_nbins_=(cl_uint*)malloc(sizeof(cl_uint));
  source_array_ = (cl_float*)malloc(slen * sizeof(cl_float));
  Ks_ = (cl_float4*)malloc(sizeof(cl_float4));
  cl_sni_=(cl_uint*)malloc(sizeof(cl_uint));
  cl_snj_=(cl_uint*)malloc(sizeof(cl_uint));
#else
  cl_nbins_=(cl_uint*)memalign(16,sizeof(cl_uint));
  source_array_ = (cl_float*)memalign(16, slen * sizeof(cl_float));
  Ks_ = (cl_float4*)memalign(16,sizeof(cl_float4));
  cl_sni_=(cl_uint*)memalign(16,sizeof(cl_uint));
  cl_snj_=(cl_uint*)memalign(16,sizeof(cl_uint));
#endif
  if (!source_array_)
    return false;
  (*cl_nbins_) = this->nbins_;
  unsigned sindex = 0;
  for (unsigned j = 0; j<snj_; ++j)
    for (unsigned i = 0; i<sni_; ++i, sindex++)
      source_array_[sindex] = source_sm(i,j);

  *cl_sni_ = sni_;   *cl_snj_ = snj_;

  Ks_->s[0] = static_cast<float>(Ks[0][0]);
  Ks_->s[1] = static_cast<float>(Ks[0][2]);
  Ks_->s[2] = static_cast<float>(Ks[1][1]);
  Ks_->s[3] = static_cast<float>(Ks[1][2]);

  //encode smoothed dest image
  vil_image_view<float> dest = minimizer.dest(level);
  dni_ = dest.ni(); dnj_ = dest.nj();
  //work space dims must be a multiple of work group dims
  wsni_ = RoundUp(dni_, this->workgrp_ni());
  wsnj_ = RoundUp(dnj_, this->workgrp_nj());
#if 1
  vil_image_view<float> dest_sm(dni_, dnj_);
  vil_gauss_filter_5tap(dest,dest_sm,vil_gauss_filter_5tap_params(2));
#endif
  vnl_matrix_fixed<double, 3, 3> Kdi =
    minimizer.from_calibration_matrix_inv(level);

  // dest cl_memory array
  unsigned dlen = dni_*dnj_;
#if defined (_WIN32)
  dest_array_=(cl_float*)_aligned_malloc(dlen * sizeof(cl_float), 16);
  Kdi_ = (cl_float4*)_aligned_malloc(sizeof(cl_float4),16);
  cl_dni_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
  cl_dnj_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
#elif defined(__APPLE__)
  dest_array_ = (cl_float*)malloc(dlen * sizeof(cl_float));
  Kdi_ = (cl_float4*)malloc(sizeof(cl_float4));
  cl_dni_=(cl_uint*)malloc(sizeof(cl_uint));
  cl_dnj_=(cl_uint*)malloc(sizeof(cl_uint));
#else
  dest_array_ = (cl_float*)memalign(16, dlen * sizeof(cl_float));
  Kdi_ = (cl_float4*)memalign(16,sizeof(cl_float4));
  cl_dni_=(cl_uint*)memalign(16,sizeof(cl_uint));
  cl_dnj_=(cl_uint*)memalign(16,sizeof(cl_uint));
#endif
  if (!dest_array_)
    return false;
  unsigned dindex = 0;
  for (unsigned j = 0; j<dnj_; ++j)
    for (unsigned i = 0; i<dni_; ++i, dindex++)
      dest_array_[dindex] = dest_sm(i,j);
  *cl_dni_ = dni_;   *cl_dnj_ = dnj_;
  Kdi_->s[0] = static_cast<float>(Kdi[0][0]);
  Kdi_->s[1] = static_cast<float>(Kdi[0][2]);
  Kdi_->s[2] = static_cast<float>(Kdi[1][1]);
  Kdi_->s[3] = static_cast<float>(Kdi[1][2]);

  //encode depth image
  vil_image_view<double> depth = minimizer.inv_depth(level);
#if defined (_WIN32)
  depth_array_=(cl_float*)_aligned_malloc(dlen * sizeof(cl_float), 16);
#elif defined(__APPLE__)
  depth_array_ = (cl_float*)malloc(dlen * sizeof(cl_float));
#else
  depth_array_ = (cl_float*)memalign(16, dlen * sizeof(cl_float));
#endif
  if (!depth_array_)
    return false;
  unsigned k = 0;
  for (unsigned j =0; j<dnj_; ++j)
    for (unsigned i =0; i<dni_; ++i, ++k)
      depth_array_[k]=static_cast<float>(depth(i,j));

  return true;
}

void icam_ocl_search_manager::clean_image_data()
{
  if (source_array_)
  {
#ifdef _WIN32
    _aligned_free(source_array_);
#elif defined(__APPLE__)
    free(source_array_);
#else
    source_array_ = NULL;
#endif
  }
  *cl_sni_=0;   *cl_snj_=0;

  if (dest_array_)
  {
#ifdef _WIN32
    _aligned_free(dest_array_);
#elif defined(__APPLE__)
    free(dest_array_);
#else
    dest_array_ = NULL;
#endif
  }
  *cl_dni_=0;   *cl_dnj_=0;

  if (depth_array_)
  {
#ifdef _WIN32
    _aligned_free(depth_array_);
#elif defined(__APPLE__)
    free(depth_array_);
#else
    depth_array_ = NULL;
#endif
  }
}

void icam_ocl_search_manager::
setup_transf_search_space(vgl_box_3d<double> const& trans_box,
                          vgl_vector_3d<double> const& trans_steps,
                          icam_minimizer& minimizer,
                          unsigned level)
{
  //set up rotations
  unsigned n_rays, n_pangs;
  principal_ray_scan prs = minimizer.pray_scan(level, n_rays);
  double polar_range = vnl_math::pi;
  double pl_inc = minimizer.polar_inc(level, n_pangs, polar_range);
  vcl_cout << "Initializing " << n_rays*n_pangs << " rotations\n";
  for (prs.reset(); prs.next();)
    for (double ang = -polar_range; ang<=polar_range; ang+=pl_inc)
      rotations_.push_back(prs.rot(ang));

  // set up translations
  double xo = trans_box.min_x(),yo = trans_box.min_y(),zo = trans_box.min_y();
  double xspan = trans_box.max_x() - xo;
  double yspan = trans_box.max_y() - yo;
  double zspan = trans_box.max_z() - zo;
  unsigned nx = static_cast<unsigned>(xspan/trans_steps.x());
  unsigned ny = static_cast<unsigned>(yspan/trans_steps.y());
  unsigned nz = static_cast<unsigned>(zspan/trans_steps.z());
  //need to include (0 0 0);
  if (nx%2) nx++;  if (ny%2) ny++;  if (nz%2) nz++;
  double dx = xspan/nx, dy = yspan/ny, dz = zspan/nz;
  double x, y, z;
  unsigned ix, iy, iz;
  for (x=xo, ix = 0; ix<nx+1; x+=dx, ++ix)
    for (y=yo, iy = 0; iy<ny+1; y+=dy, ++iy)
      for (z=zo, iz = 0; iz<nz+1; z+=dz, ++iz)
        translations_.push_back(vgl_vector_3d<double>(x, y, z));
  vcl_cout << "Initializing " << translations_.size() << " translations\n"
           << "Total search space " << n_rays*n_pangs*translations_.size()
           << " mappings\n";
}
//set all rotations to be the same value for testing purposes
void icam_ocl_search_manager::
setup_rot_debug_space(unsigned n_rotations,
                      vgl_rotation_3d<double> const& rot)
{
  for(unsigned i =0; i<n_rotations; ++i)
    rotations_.push_back(rot);
}
bool icam_ocl_search_manager::
create_image_parallel_transf_data()
{
#if defined (_WIN32)
  rotation_ =(cl_float4*)_aligned_malloc(sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  rotation_ = (cl_float4*)malloc(sizeof(cl_float4));
#else
  rotation_ = (cl_float4*)memalign(16, sizeof(cl_float4));
#endif

#if defined (_WIN32)
  translation_=(cl_float4*)_aligned_malloc(sizeof(cl_float4),16);
#elif defined(__APPLE__)
  translation_ = (cl_float4*)malloc(sizeof(cl_float4));
#else
  translation_ = (cl_float4*)memalign(16, sizeof(cl_float4));
#endif
  if(!translation_)
    return false;
  return true;
}

bool icam_ocl_search_manager::
set_image_parallel_transf(vgl_vector_3d<double> const& tr,
                          vgl_rotation_3d<double> const& rot)
{
  if (!rotation_)
    return false;
  vnl_vector_fixed<double, 3> rod = rot.as_rodrigues();
  rotation_->s[0]=static_cast<float>(rod[0]);
  rotation_->s[1]=static_cast<float>(rod[1]);
  rotation_->s[2]=static_cast<float>(rod[2]);
  rotation_->s[3] = 0.0f;

  if (!translation_)
    return false;
  translation_->s[0]=static_cast<float>(tr.x());
  translation_->s[1]=static_cast<float>(tr.y());
  translation_->s[2]=static_cast<float>(tr.z());
  translation_->s[3] = 0.0f;

  return true;
}
void icam_ocl_search_manager::clean_image_parallel_transf_data()
{
  if (rotation_)
  {
#ifdef _WIN32
    _aligned_free(rotation_);
#elif defined(__APPLE__)
    free(rotation_);
#else
    rotation_ = NULL;
#endif
  }

  if (translation_)
  {
#ifdef _WIN32
    _aligned_free(translation_);
#elif defined(__APPLE__)
    free(translation_);
#else
    translation_ = NULL;
#endif
  }
}
bool icam_ocl_search_manager::
create_rot_parallel_transf_data()
{
  unsigned nrot = rotations_.size();
#if defined (_WIN32)
  rot_array_ =(cl_float4*)_aligned_malloc(nrot*sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  rot_array_ = (cl_float4*)malloc(nrot*sizeof(cl_float4));
#else
  rot_array_ = (cl_float4*)memalign(16, nrot*sizeof(cl_float4));
#endif
  if(!rot_array_) return false;

#if defined (_WIN32)
  translation_=(cl_float4*)_aligned_malloc(sizeof(cl_float4),16);
#elif defined(__APPLE__)
  translation_ = (cl_float4*)malloc(sizeof(cl_float4));
#else
  translation_ = (cl_float4*)memalign(16, sizeof(cl_float4));
#endif
  if(!translation_)
    return false;
  return true;
}

bool icam_ocl_search_manager::
set_rot_parallel_transf_data(vgl_vector_3d<double> const& tr)
{
  unsigned nrot = rotations_.size();
  if (!rot_array_)
    return false;
  for(unsigned i = 0; i<nrot; ++i)
    {
      vnl_vector_fixed<double, 3> rod = rotations_[i].as_rodrigues();
      rot_array_[i].s[0]=static_cast<float>(rod[0]);
      rot_array_[i].s[1]=static_cast<float>(rod[1]);
      rot_array_[i].s[2]=static_cast<float>(rod[2]);
      rot_array_[i].s[3]= 0.0f;
    }

  if (!translation_)
    return false;
  translation_->s[0]=static_cast<float>(tr.x());
  translation_->s[1]=static_cast<float>(tr.y());
  translation_->s[2]=static_cast<float>(tr.z());
  translation_->s[3] = 0.0f;

  return true;

}

void icam_ocl_search_manager::clean_rot_parallel_transf_data()
{
  if (rot_array_)
  {
#ifdef _WIN32
    _aligned_free(rot_array_);
#elif defined(__APPLE__)
    free(rot_array_);
#else
    rot_array_ = NULL;
#endif
  }

  if (translation_)
  {
#ifdef _WIN32
    _aligned_free(translation_);
#elif defined(__APPLE__)
    free(translation_);
#else
    translation_ = NULL;
#endif
  }
}

bool icam_ocl_search_manager::create_rot_parallel_transf_buffers()
{
  vcl_vector<void*> arrs;
  vcl_vector<unsigned int> sizes;

  int i=buffer_map_.size();
  unsigned nrot = rotations_.size();
  vcl_cout << "arg[" << i << "] - rot (cl_float4)\n";
  arrs.push_back(rot_array_); sizes.push_back(nrot*sizeof(cl_float4));
  buffer_map_[rot_array_]=i++;

  vcl_cout << "arg[" << i << "] - trans (cl_float4)\n";
  arrs.push_back(translation_); sizes.push_back(sizeof(cl_float4));
  buffer_map_[translation_]=i++;

  cl_int status;
  status = kernel_->create_in_buffers(this->context_,arrs,sizes);
  return status == SDK_SUCCESS;
}

bool icam_ocl_search_manager::setup_image_parallel_result()
{
  unsigned rlen =   wsni_*wsnj_;
  // the result image
#if defined (_WIN32)
  result_array_=(cl_float*)_aligned_malloc(rlen * sizeof(cl_float), 16);
#elif defined(__APPLE__)
  result_array_ = (cl_float*)malloc(rlen * sizeof(cl_float));
#else
  result_array_ = (cl_float*)memalign(16, rlen * sizeof(cl_float));
#endif
  if (!result_array_)
    return false;
  unsigned k = 0;
  for (unsigned j =0; j<wsnj_; ++j)
    for (unsigned i =0; i<wsni_; ++i, ++k)
      result_array_[k]=0.0f;

  // the mask image
#if defined (_WIN32)
  mask_array_=(cl_float*)_aligned_malloc(rlen * sizeof(cl_float), 16);
#elif defined(__APPLE__)
  mask_array_ = (cl_float*)malloc(rlen * sizeof(cl_float));
#else
  mask_array_ = (cl_float*)memalign(16, rlen * sizeof(cl_float));
#endif
  if (!mask_array_)
    return false;
  k = 0;
  for (unsigned j =0; j<wsnj_; ++j)
    for (unsigned i =0; i<wsni_; ++i, ++k)
      mask_array_[k]=0.0f;

#if defined (_WIN32)
  image_para_result_=(cl_float4*)_aligned_malloc(sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  image_para_result_ = (cl_float4*)malloc(sizeof(cl_float4));
#else
  image_para_result_ = (cl_float4*)memalign(16, sizeof(cl_float4));
#endif

#if defined (_WIN32)
  image_para_flag_=(cl_int4*)_aligned_malloc( sizeof(cl_int4), 16);
#elif defined(__APPLE__)
  image_para_flag_ = (cl_int4*)malloc(sizeof(cl_int4));
#else
  image_para_flag_ = (cl_int4*)memalign(16, sizeof(cl_int4));
#endif

  if (!image_para_result_ || !image_para_flag_)
    return false;

  // initialize image_para result
  image_para_result_->s[0]=vnl_numeric_traits<float>::maxval;
  image_para_result_->s[1]= 0.0f;
  image_para_result_->s[2]= 0.0f;
  image_para_result_->s[3] = 0.0f;
  // initialize flag
  for (unsigned i = 0; i<4; ++i)
    image_para_flag_->s[i] = 0;
  return true;
}

void icam_ocl_search_manager::clean_image_parallel_result()
{
  if (image_para_result_)
  {
#ifdef _WIN32
    _aligned_free(image_para_result_);
#elif defined(__APPLE__)
    free(image_para_result_);
#else
    image_para_result_ = NULL;
#endif
  }

  if (image_para_flag_)
  {
#ifdef _WIN32
    _aligned_free(image_para_flag_);
#elif defined(__APPLE__)
    free(image_para_flag_);
#else
    image_para_flag_ = NULL;
#endif
  }
  if (result_array_)
  {
#ifdef _WIN32
    _aligned_free(result_array_);
#elif defined(__APPLE__)
    free(result_array_);
#else
    result_array_ = NULL;
#endif
  }

  if (mask_array_)
  {
#ifdef _WIN32
    _aligned_free(mask_array_);
#elif defined(__APPLE__)
    free(mask_array_);
#else
    mask_array_ = NULL;
#endif
  }
}

bool icam_ocl_search_manager::setup_rot_parallel_result()
{
  unsigned n_rot = rotations_.size();
  // the result image
#if defined (_WIN32)
  minfo_array_=(cl_float*)_aligned_malloc(n_rot * sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  minfo_array_ = (cl_float*)malloc(n_rot * sizeof(cl_float));
#else
  minfo_array_ = (cl_float*)memalign(16, n_rot * sizeof(cl_float));
#endif
  if (!minfo_array_)
    return false;
  for (unsigned i =0; i<n_rot; ++i)
      minfo_array_[i]=0.0f;

#if defined (_WIN32)
  rot_para_flag_=(cl_int4*)_aligned_malloc( sizeof(cl_int4), 16);
#elif defined(__APPLE__)
  rot_para_flag_ = (cl_int4*)malloc(sizeof(cl_int4));
#else
  rot_para_flag_ = (cl_int4*)memalign(16, sizeof(cl_int4));
#endif

  if (!rot_para_flag_)
    return false;
  for (unsigned i = 0; i<4; ++i)
    rot_para_flag_->s[i] = 0;
  return true;

}
void icam_ocl_search_manager::clean_rot_parallel_result()
{
  if (rot_para_flag_)
  {
#ifdef _WIN32
    _aligned_free(rot_para_flag_);
#elif defined(__APPLE__)
    free(rot_para_flag_);
#else
    rot_para_flag_ = NULL;
#endif
  }
  if (minfo_array_)
  {
#ifdef _WIN32
    _aligned_free(minfo_array_);
#elif defined(__APPLE__)
    free(minfo_array_);
#else
    minfo_array_ = NULL;
#endif
  }
}

bool icam_ocl_search_manager::create_rot_parallel_result_buffers()
{
  vcl_vector<void*> arrs;
  vcl_vector<unsigned int> sizes;
  unsigned n_rot = rotations_.size();
  int i=buffer_map_.size();
  vcl_cout << "output arg[" << i << "] - minfo (cl_float) \n";
  arrs.push_back(minfo_array_);  sizes.push_back(n_rot*sizeof(cl_float));
  buffer_map_[minfo_array_]=i++;

  vcl_cout << "output arg[" << i << "] - rot_para_flag (cl_int4)\n";
  arrs.push_back(rot_para_flag_);  sizes.push_back(sizeof(cl_int4));
  buffer_map_[rot_para_flag_]=i++;

  cl_int status;
  status = kernel_->create_out_buffers(this->context_,arrs,sizes);
  return status == SDK_SUCCESS;
}
bool icam_ocl_search_manager::set_nbins_buffer()
{
  vcl_vector<void*> arrs;
  vcl_vector<unsigned int> sizes;
  int i=kernel_->buffer_cnt();
  vcl_cout << "arg[" << i << "] - nbins (cl_uint)\n";
  arrs.push_back(cl_nbins_);sizes.push_back(sizeof(cl_uint));
  buffer_map_[cl_nbins_]=i;
  cl_int  status = kernel_->create_in_buffers(this->context_,arrs,sizes);
  return status == SDK_SUCCESS;
}
bool icam_ocl_search_manager::copy_to_image_buffers()
{
  // Create and initialize memory objects
  // source array

  vcl_vector<void*> arrs;
  vcl_vector<unsigned int> sizes;
  int i=kernel_->buffer_cnt();

  vcl_cout << "arg[" << i << "] - sni (cl_uint)\n";
  arrs.push_back(cl_sni_);sizes.push_back(sizeof(cl_uint));
  buffer_map_[cl_sni_]=i++;

  vcl_cout << "arg[" << i << "] - snj (cl_uint)\n";
  arrs.push_back(cl_snj_);sizes.push_back(sizeof(cl_uint));
  buffer_map_[cl_snj_]=i++;

  vcl_cout << "arg[" << i << "] - Ks (cl_float4)\n";
  arrs.push_back(Ks_);sizes.push_back(sizeof(cl_float4));
  buffer_map_[Ks_]=i++;

  vcl_cout << "arg[" << i << "] - source (cl_float)\n";
  arrs.push_back(source_array_);sizes.push_back(sni_*snj_*sizeof(cl_float));
  buffer_map_[source_array_]=i++;

  vcl_cout << "arg[" << i << "] - dni (cl_uint)\n";
  arrs.push_back(cl_dni_);sizes.push_back(sizeof(cl_uint));
  buffer_map_[cl_dni_]=i++;

  vcl_cout << "arg[" << i << "] - dnj (cl_uint)\n";
  arrs.push_back(cl_dnj_);sizes.push_back(sizeof(cl_uint));
  buffer_map_[cl_dnj_]=i++;

  vcl_cout << "arg[" << i << "] - Kdi (cl_float4)\n";
  arrs.push_back(Kdi_);sizes.push_back(sizeof(cl_float4));
  buffer_map_[Kdi_]=i++;

  vcl_cout << "arg[" << i << "] - dest (cl_float)\n";
  arrs.push_back(dest_array_);sizes.push_back(dni_*dnj_*sizeof(cl_float));
  buffer_map_[dest_array_]=i++;

  vcl_cout << "arg[" << i << "] - depth (cl_float)\n";
  arrs.push_back(depth_array_); sizes.push_back(dni_*dnj_*sizeof(cl_float));
  buffer_map_[depth_array_]=i++;

  cl_int status;
  status = kernel_->create_in_buffers(this->context_,arrs,sizes);

  return status == SDK_SUCCESS;
}

bool icam_ocl_search_manager::release_buffers()
{
  return (kernel_->release_buffers());
}

bool icam_ocl_search_manager::copy_to_image_parallel_transf_buffers()
{
  cl_int status = CL_SUCCESS;
  cl_event events[1];

  status = kernel_->enqueue_write_buffer(command_queue_, buffer_map_[rotation_],CL_TRUE,
                                   0,sizeof(cl_float4), rotation_, 0,NULL,&events[0]);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer ( rotation ) failed."))
    return SDK_FAILURE;

  // translation
  status = kernel_->enqueue_write_buffer(command_queue_,buffer_map_[translation_],CL_TRUE,
                                   0,sizeof(cl_float4),translation_, 0,NULL,&events[0]);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer ( translation ) failed."))
    return SDK_FAILURE;

  // Wait for the wriet buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!check_val(status,CL_SUCCESS,"clWaitForEvents ( write transf ) failed."))
    return SDK_FAILURE;

  status = clReleaseEvent(events[0]);
  if (!check_val(status,CL_SUCCESS,"clReleaseEvent (write transf ) failed."))
    return SDK_FAILURE;
  else
    return CL_SUCCESS;
}

bool icam_ocl_search_manager::create_image_parallel_transf_buffers()
{
  vcl_vector<void*> arrs;
  vcl_vector<unsigned int> sizes;

  int i=buffer_map_.size();
  arrs.push_back(rotation_); sizes.push_back(sizeof(cl_float4));
  buffer_map_[rotation_]=i++;

  arrs.push_back(translation_); sizes.push_back(sizeof(cl_float4));
  buffer_map_[translation_]=i++;

  cl_int status;
  status = kernel_->create_in_buffers(this->context_,arrs,sizes);
  return status == SDK_SUCCESS;
}

bool icam_ocl_search_manager::create_image_parallel_result_buffers()
{
  vcl_vector<void*> arrs;
  vcl_vector<unsigned int> sizes;

  int i=buffer_map_.size();
  arrs.push_back(image_para_result_);  sizes.push_back(sizeof(cl_float4));
  buffer_map_[image_para_result_]=i++;

  arrs.push_back(image_para_flag_);  sizes.push_back(sizeof(cl_int4));
  buffer_map_[image_para_flag_]=i++;

  arrs.push_back(result_array_);  sizes.push_back(wsni_*wsnj_*sizeof(cl_float));
  buffer_map_[result_array_]=i++;

  arrs.push_back(mask_array_);  sizes.push_back(wsni_*wsnj_*sizeof(cl_float));
  buffer_map_[mask_array_]=i++;

  cl_int status;
  status = kernel_->create_out_buffers(this->context_,arrs,sizes);
  return status == SDK_SUCCESS;
}
bool icam_ocl_search_manager::setup_image_parallel_kernel()
{
  // -- Set appropriate arguments to the kernel --
  // source array args
  cl_int status = kernel_->set_args();

  //=================== end of kernel arguments =======================

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_->kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_->kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;
  assert(this->workgrp_size()<= vcl_size_t(kernel_work_group_size));

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  return check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status))!=CL_SUCCESS;
}

bool icam_ocl_search_manager::set_rot_parallel_local_args()
{
  int i = buffer_map_.size();
  // translation
  vcl_cout << "local arg[" << i << "] - translation (cl_float4)\n";
  kernel_->set_local_arg(i++, sizeof(cl_float4));

  // rows of the rotation matrix
  vcl_cout << "local arg[" << i << "] - r0 (cl_float4)\n";
  kernel_->set_local_arg(i++, sizeof(cl_float4));
  vcl_cout << "local arg[" << i << "] - r1 (cl_float4)\n";
  kernel_->set_local_arg(i++, sizeof(cl_float4));
  vcl_cout << "local arg[" << i << "] - r2 (cl_float4)\n";
  kernel_->set_local_arg(i++, sizeof(cl_float4));

  // dest histogram
  vcl_cout << "local arg[" << i << "] - mdhist (cl_uint)\n";
  kernel_->set_local_arg(i++, nbins_*sizeof(cl_uint));
  // joint histogram
  vcl_cout << "local arg[" << i << "] - joint hist (cl_uint)\n";
  kernel_->set_local_arg(i++, nbins_*nbins_*sizeof(cl_uint));
  // reduction buffer
  vcl_cout << "local arg[" << i << "] - reduc_buf hist(cl_float)\n";
  kernel_->set_local_arg(i, 2*this->workgrp_ni()*sizeof(cl_float));
  return true;
}

bool icam_ocl_search_manager::setup_rot_parallel_kernel()
{
  // -- Set appropriate arguments to the kernel --
  // source array args
  cl_int status = kernel_->set_args();
  if(status!=0) return false;
  if(!this->set_rot_parallel_local_args())
    return false;
  //=================== end of kernel arguments =======================

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_->kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_->kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;
  assert(this->workgrp_size()<= vcl_size_t(kernel_work_group_size));

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  return check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status))!=CL_SUCCESS;
}

bool icam_ocl_search_manager::run_image_parallel_kernel()
{
  vcl_size_t globalThreads[]= {wsni_ , wsnj_};
  vcl_size_t localThreads[] = {this->workgrp_ni(), this->workgrp_nj()};
  cl_int status = CL_SUCCESS;
  cl_event ceEvent;

  status = clEnqueueNDRangeKernel(command_queue_,kernel_->kernel(), 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  cl_event events[1];
  status = kernel_->enqueue_read_buffer(command_queue_,buffer_map_[image_para_result_],CL_TRUE,
                                        0,sizeof(cl_float4),image_para_result_,0,NULL,&events[0]);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_para result )failed."))
    return SDK_FAILURE;
  status = kernel_->enqueue_read_buffer(command_queue_,buffer_map_[image_para_flag_],CL_TRUE,
                                        0,sizeof(cl_int4),image_para_flag_,0,NULL,&events[0]);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_para flag)failed."))
    return SDK_FAILURE;


  status = kernel_->enqueue_read_buffer(command_queue_,buffer_map_[result_array_],CL_TRUE,
                               0,wsni_*wsnj_*sizeof(cl_float),result_array_,0,NULL,&events[0]);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (result_array)failed."))
    return SDK_FAILURE;

  status = kernel_->enqueue_read_buffer(command_queue_,buffer_map_[mask_array_],CL_TRUE,
                               0,wsni_*wsnj_*sizeof(cl_float), mask_array_,0,NULL,&events[0]);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (mask_array)failed."))
    return SDK_FAILURE;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  status = clReleaseEvent(events[0]);
  if (!check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return SDK_FAILURE;

  return CL_SUCCESS;
}

bool icam_ocl_search_manager::run_rot_parallel_kernel()
{
  vcl_size_t num_rot = rotations_.size();
  vcl_size_t globalThreads[]= {num_rot*this->workgrp_ni()};
  vcl_size_t localThreads[] = {this->workgrp_ni()};
  cl_int status = CL_SUCCESS;
  cl_event ceEvent;

  status = clEnqueueNDRangeKernel(command_queue_,kernel_->kernel(), 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  cl_event events[1];
  status = kernel_->enqueue_read_buffer(command_queue_,buffer_map_[minfo_array_],CL_TRUE,
                                        0,num_rot*sizeof(cl_float),minfo_array_,0,NULL,&events[0]);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (rot_para result )failed."))
    return SDK_FAILURE;
  status = kernel_->enqueue_read_buffer(command_queue_,buffer_map_[rot_para_flag_],CL_TRUE,
                                        0,sizeof(cl_int4),rot_para_flag_,0,NULL,&events[0]);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_para flag)failed."))
    return SDK_FAILURE;


  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  status = clReleaseEvent(events[0]);
  if (!check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return SDK_FAILURE;

  return CL_SUCCESS;
}
bool icam_ocl_search_manager::release_queue()
{
  // release the command Queue
  cl_int status = clReleaseCommandQueue(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return SDK_FAILURE;
  ///
  return CL_SUCCESS;
}

int icam_ocl_search_manager::build_kernel_program()
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!check_val(status,CL_SUCCESS,"clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = prog_.c_str();

  program_ = clCreateProgramWithSource(this->context_,1,&source,sourceSize, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_, 1, this->devices_,NULL,NULL,NULL);
  if (!check_val(status, CL_SUCCESS, error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program_, this->devices_[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }

  return SDK_SUCCESS;
}


int icam_ocl_search_manager::create_kernel(vcl_string const& kernel_name)
{
  cl_int status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_->create_kernel(program_,kernel_name.c_str(),status);
  return status;
}


int icam_ocl_search_manager::release_kernel()
{
  cl_int status;
  kernel_->release_kernel(status);
  return status;
}

