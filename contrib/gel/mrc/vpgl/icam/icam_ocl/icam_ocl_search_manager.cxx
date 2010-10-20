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
  source_array_=(cl_float*)_aligned_malloc(slen * sizeof(cl_float), 16);
  Ks_ = (cl_float4*)_aligned_malloc(sizeof(cl_float4),16);
  cl_sni_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
  cl_snj_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
#elif defined(__APPLE__)
  source_array_ = (cl_float*)malloc(slen * sizeof(cl_float));
  Ks_ = (cl_float4*)malloc(sizeof(cl_float4));
  cl_sni_=(cl_uint*)malloc(sizeof(cl_uint));
  cl_snj_=(cl_uint*)malloc(sizeof(cl_uint));
#else
  source_array_ = (cl_float*)memalign(16, slen * sizeof(cl_float));
  Ks_ = (cl_float4*)memalign(16,sizeof(cl_float4));
  cl_sni_=(cl_uint*)memalign(16,sizeof(cl_uint));
  cl_snj_=(cl_uint*)memalign(16,sizeof(cl_uint));
#endif
  if(!source_array_)
    return false;
  unsigned sindex = 0;
  for(unsigned j = 0; j<snj_; ++j)
    for(unsigned i = 0; i<sni_; ++i, sindex++)
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
  if(!dest_array_)
    return false;
  unsigned dindex = 0;
  for(unsigned j = 0; j<dnj_; ++j)
    for(unsigned i = 0; i<dni_; ++i, dindex++)
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
  if(!depth_array_)
    return false;
  unsigned k = 0;
  for(unsigned j =0; j<dnj_; ++j)
    for(unsigned i =0; i<dni_; ++i, ++k)
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
  for(prs.reset(); prs.next();)
    for(double ang = -polar_range; ang<=polar_range; ang+=pl_inc)
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
  if(nx%2) nx++;  if(ny%2) ny++;  if(nz%2) nz++;
  double dx = xspan/nx, dy = yspan/ny, dz = zspan/nz;
  double x, y, z;
  unsigned ix, iy, iz;
  for(x=xo, ix = 0; ix<nx+1; x+=dx, ++ix)
    for(y=yo, iy = 0; iy<ny+1; y+=dy, ++iy)
      for(z=zo, iz = 0; iz<nz+1; z+=dz, ++iz)
        translations_.push_back(vgl_vector_3d<double>(x, y, z));
  vcl_cout << "Initializing " << translations_.size() << " translations\n";
  vcl_cout << "Total search space " << n_rays*n_pangs*translations_.size()
           << " mappings\n";
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
  if(!rotation_)
    return false;
  vnl_vector_fixed<double, 3> rod = rot.as_rodrigues();
  rotation_->s[0]=static_cast<float>(rod[0]);
  rotation_->s[1]=static_cast<float>(rod[1]);
  rotation_->s[2]=static_cast<float>(rod[2]);
  rotation_->s[3] = 0.0f;

  if(!translation_)
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
  if(!result_array_)
    return false;
  unsigned k = 0;
  for(unsigned j =0; j<wsnj_; ++j)
    for(unsigned i =0; i<wsni_; ++i, ++k)
      result_array_[k]=0.0f;
  
  // the mask image
#if defined (_WIN32)
  mask_array_=(cl_float*)_aligned_malloc(rlen * sizeof(cl_float), 16);
#elif defined(__APPLE__)
  mask_array_ = (cl_float*)malloc(rlen * sizeof(cl_float));
#else
  mask_array_ = (cl_float*)memalign(16, rlen * sizeof(cl_float));
#endif
  if(!mask_array_)
    return false;
  k = 0;
  for(unsigned j =0; j<wsnj_; ++j)
    for(unsigned i =0; i<wsni_; ++i, ++k)
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

// intialize image_para result
 image_para_result_->s[0]=vnl_numeric_traits<float>::maxval;
 image_para_result_->s[1]= 0.0f;
 image_para_result_->s[2]= 0.0f;
 image_para_result_->s[3] = 0.0f;
// intialize flag 
 for(unsigned i = 0; i<4; ++i)
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
bool icam_ocl_search_manager::copy_to_image_buffers()
{
 cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  // source array
  source_array_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                     sni_*snj_*sizeof(cl_float),source_array_,
                                     &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (source array) failed."))
    return SDK_FAILURE;

  sni_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_uint), cl_sni_,
                            &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (source array length) failed."))
    return SDK_FAILURE;
  snj_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_uint), cl_snj_,
                            &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (source array length) failed."))
    return SDK_FAILURE;

  Ks_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_float4), Ks_,
                            &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (source array length) failed."))
    return SDK_FAILURE;
  // dest array
  dest_array_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   dni_*dnj_*sizeof(cl_float),dest_array_,
                                   &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (dest array) failed."))
    return SDK_FAILURE;

  dni_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_uint), cl_dni_,
                            &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (dest array length) failed."))
    return SDK_FAILURE;

  dnj_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_uint), cl_dnj_,
                            &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (dest array length) failed."))
    return SDK_FAILURE;

  Kdi_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_float4), Kdi_,
                            &status);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (source array length) failed."))
    return SDK_FAILURE;
  // depth buffer
  depth_array_buf_ = clCreateBuffer(this->context_,
                                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    dni_*dnj_*sizeof(cl_float),depth_array_,
                                    &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (depth array) failed."))
    return SDK_FAILURE;
  return CL_SUCCESS;
}
bool icam_ocl_search_manager::release_image_buffers()
{
 cl_int status = CL_SUCCESS;
  // source 
  status = clReleaseMemObject(sni_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( sni_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(snj_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( sni_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(Ks_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( Ks_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(source_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject (source_array_buf_) failed."))
    return SDK_FAILURE;

  // dest 
  status = clReleaseMemObject(dni_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( dni_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(dnj_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( dnj_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(Kdi_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( Kdi_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(dest_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject (dest_array_buf_) failed."))
    return SDK_FAILURE;

  // depth

  status = clReleaseMemObject(depth_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject (depth_array_buf_) failed."))
    return SDK_FAILURE;
return CL_SUCCESS;
}

bool icam_ocl_search_manager::copy_to_image_parallel_transf_buffers()
{
 cl_int status = CL_SUCCESS;
 cl_event events[1];
 status = clEnqueueWriteBuffer(command_queue_,
                               rot_buf_,CL_TRUE,
                               0,sizeof(cl_float4),
                               rotation_,
                               0,NULL,&events[0]);
 if (!this->check_val(status,
                      CL_SUCCESS,
                      "clCreateBuffer ( rotation ) failed."))
   return SDK_FAILURE;
  // translation 
 status = clEnqueueWriteBuffer(command_queue_,
                               trans_buf_,CL_TRUE,
                               0,sizeof(cl_float4),
                               translation_,
                               0,NULL,&events[0]);
 if (!this->check_val(status,
                      CL_SUCCESS,
                      "clCreateBuffer ( translation ) failed."))
   return SDK_FAILURE;

  // Wait for the wriet buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents ( write transf ) failed."))
    return SDK_FAILURE;

  status = clReleaseEvent(events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent (write transf ) failed."))
    return SDK_FAILURE;

return CL_SUCCESS;
}

bool icam_ocl_search_manager::create_image_parallel_transf_buffers()
{
 cl_int status = CL_SUCCESS;
 // rotation 
  rot_buf_ = clCreateBuffer(this->context_,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(cl_float4),
                            rotation_,
                            &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer ( rotation ) failed."))
    return SDK_FAILURE;


  // translation 
  trans_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_float4),
                              translation_,
                              &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (translation) failed."))
    return SDK_FAILURE;
return CL_SUCCESS;
}
bool icam_ocl_search_manager::release_image_parallel_transf_buffers()
{
	 cl_int status = CL_SUCCESS;
  // rotation
  status = clReleaseMemObject(rot_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject ( rot_buf_) failed."))
    return SDK_FAILURE;

  // translation
  status = clReleaseMemObject(trans_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clReleaseMemObject (trans_buf_) failed."))
    return SDK_FAILURE;
  return CL_SUCCESS;
}

bool icam_ocl_search_manager::create_image_parallel_result_buffers()
{
 cl_int status = CL_SUCCESS;
  //resulting image_para 
  image_para_result_buf_ = clCreateBuffer(this->context_,
                                    CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_float4),
                                    image_para_result_, &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (image_para result ) failed."))
    return SDK_FAILURE;

  image_para_flag_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                  sizeof(cl_int4),image_para_flag_,&status);
  if (!this->check_val(status,CL_SUCCESS,
                       "clCreateBuffer (image_para_flag) failed."))
    return SDK_FAILURE;

  result_array_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                     wsni_*wsnj_*sizeof(cl_float),
                                     result_array_,&status);
  if (!this->check_val(status,CL_SUCCESS,
                       "clCreateBuffer (result array) failed."))
    return SDK_FAILURE;

  mask_array_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                  wsni_*wsnj_*sizeof(cl_float),
                                   mask_array_,&status);
  if (!this->check_val(status,CL_SUCCESS,
                       "clCreateBuffer (mask array) failed."))
    return SDK_FAILURE;

  return CL_SUCCESS;
}
bool icam_ocl_search_manager::release_image_parallel_result_buffers()
{
 cl_int status = CL_SUCCESS;
  // release output result buffers
  status = clReleaseMemObject(image_para_result_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (image_para_array_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(image_para_flag_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (image_para_flag_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(result_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (image_para_result_buf_) failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(mask_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (mask_array_buf_) failed."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}
 
bool icam_ocl_search_manager::setup_image_parallel_kernel()
{
  // -- Set appropriate arguments to the kernel --
  // source array args
 cl_int status = CL_SUCCESS;
  status = clSetKernelArg(kernel_,0,sizeof(cl_mem),
                          (void *)&sni_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. ( source.ni )"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,1,sizeof(cl_mem),
                          (void *)&snj_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. ( source.nj )"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,2,sizeof(cl_mem),
                          (void *)&Ks_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. ( K source )"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,3,sizeof(cl_mem),(void *)&source_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (source array)"))
    return SDK_FAILURE;

  // dest array args
  status = clSetKernelArg(kernel_,4,sizeof(cl_mem),
                          (void *)&dni_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (length of dest buf)"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,5,sizeof(cl_mem),
                          (void *)&dnj_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (length of dest buf)"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,6,sizeof(cl_mem),
                          (void *)&Kdi_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (inv K dest )"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,7,sizeof(cl_mem),(void *)&dest_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (dest array)"))
    return SDK_FAILURE;

  //depth image
  status = clSetKernelArg(kernel_,8,sizeof(cl_mem),(void *)&depth_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (depth array)"))
    return SDK_FAILURE;

  //rotation
  status = clSetKernelArg(kernel_,9,sizeof(cl_mem),(void *)&rot_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (rotation)"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,10,sizeof(cl_mem),(void *)&trans_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (rotation array)"))
    return SDK_FAILURE;

  //resulting image_paras and flag
  status = clSetKernelArg(kernel_,11,sizeof(cl_mem),(void *)&image_para_result_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (image_para result)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,12,sizeof(cl_mem),(void *)&image_para_flag_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (image_para flag)"))
    return SDK_FAILURE;

  // results buffers
  status = 
    clSetKernelArg(kernel_,13,sizeof(cl_mem),(void *)&result_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (result_array)"))
    return SDK_FAILURE;

  status = 
    clSetKernelArg(kernel_,14,sizeof(cl_mem),(void *)&mask_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,
                       "clSetKernelArg failed. (mask_array)"))
    return SDK_FAILURE;

  //=================== end of kernel arguments =======================

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;
  assert(this->workgrp_size()<= vcl_size_t(kernel_work_group_size));
#if 0
  vcl_size_t globalThreads[]= {wsni_ , wsnj_};
  vcl_size_t localThreads[] = {this->workgrp_ni(), this->workgrp_nj()};
#endif
  if (used_local_memory > this->total_local_memory())
    {
      vcl_cout << "Unsupported: Insufficient local memory on device.\n";
      return SDK_FAILURE;
    }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;
return true;
}
bool icam_ocl_search_manager::run_kernel()
{ 
vcl_size_t globalThreads[]= {wsni_ , wsnj_};
  vcl_size_t localThreads[] = {this->workgrp_ni(), this->workgrp_nj()};
cl_int status = CL_SUCCESS;
  cl_event ceEvent;

  status = clEnqueueNDRangeKernel(command_queue_,this->kernel_, 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  cl_event events[1];
  status = clEnqueueReadBuffer(command_queue_,image_para_result_buf_,CL_TRUE,
                               0,sizeof(cl_float4),
                               image_para_result_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_para result )failed."))
    return SDK_FAILURE;
  status = clEnqueueReadBuffer(command_queue_,image_para_flag_buf_,CL_TRUE,
                               0,sizeof(cl_int4),
                               image_para_flag_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_para flag)failed."))
    return SDK_FAILURE;


  status = clEnqueueReadBuffer(command_queue_,result_array_buf_,CL_TRUE,
                               0,wsni_*wsnj_*sizeof(cl_float),
                               result_array_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (result_array)failed."))
    return SDK_FAILURE;

  status = clEnqueueReadBuffer(command_queue_,mask_array_buf_,CL_TRUE,
                               0,wsni_*wsnj_*sizeof(cl_float),
                               mask_array_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (mask_array)failed."))
    return SDK_FAILURE;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  status = clReleaseEvent(events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return SDK_FAILURE;

return CL_SUCCESS;
}
bool icam_ocl_search_manager::release_queue()
{
  // release the command Queue
cl_int status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
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
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = prog_.c_str();

  program_ = clCreateProgramWithSource(this->context_,
                                       1,
                                       &source,
                                       sourceSize,
                                       &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  
   
      // create a cl program executable for all the devices specified
      status = clBuildProgram(program_,
                              1,
                              this->devices_,
                              NULL,
                              NULL,
                              NULL);
      if (!this->check_val(status,
                           CL_SUCCESS,
                           error_to_string(status)))
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
  kernel_ = clCreateKernel(program_,kernel_name.c_str(),&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int icam_ocl_search_manager::release_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;
  if (!this->check_val(status,CL_SUCCESS,"clReleaseKernel failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

