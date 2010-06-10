// This is brl/bseg/boxm/ocl/boxm_ray_trace_manager.txx
#ifndef boxm_ray_trace_manager_txx_
#define boxm_ray_trace_manager_txx_
#include "boxm_ray_trace_manager.h"

#include <vcl_fstream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_where_root_dir.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_intersection.h>
#include "boxm_ocl_utils.h"


// allocate child cells on the array
template<class T>
static void split(vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
                  int parent_ptr,
                  int& child_ptr)
{
  child_ptr = cell_array.size();
  for (unsigned i=0; i<8; i++) {
    vnl_vector_fixed<int, 4> cell(0);
    cell[0]= parent_ptr;
    cell[1]=-1;
    cell[2]=-1;
    cell_array.push_back(cell);
  }
}

template<class T>
static void
copy_to_arrays(boct_tree_cell<short, T >* cell_ptr,
               vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
               vcl_vector<vnl_vector_fixed<float, 16> >& data_array,
               int cell_input_ptr)
{
  // cell_input_ptr is the array index for the cell being constructed
  // it already exists in the cell array but only has the parent index set
  // no data or child pointers

  // convert the data to 16 vector size
  vnl_vector_fixed<float, 16> data;
  pack_cell_data(cell_ptr,data);

  // data pointer will be at index == size after the push_back
  cell_array[cell_input_ptr][2] = data_array.size();
  data_array.push_back(data);
  cell_array[cell_input_ptr][3] = cell_ptr->level();
  // if the cell has chidren then they must be copied
  if (!cell_ptr->is_leaf()) {
    // initialize data values to null
    data_array[cell_array[cell_input_ptr][2]].fill(0.0);
    // create the children on the cell array
    int child_ptr = -1;
    split<T>(cell_array, cell_input_ptr, child_ptr);
    cell_array[cell_input_ptr][1]=child_ptr;
    boct_tree_cell<short,T >* children =
      cell_ptr->children();
    for (unsigned i = 0; i<8; ++i) {
      boct_tree_cell<short, T >* child_cell_ptr = children + i;
      int child_cell_input_ptr = child_ptr +i;
      copy_to_arrays(child_cell_ptr, cell_array, data_array, child_cell_input_ptr);
    }
  }
#if 1
  else{
    // Debug
    boct_loc_code<short> cd = cell_ptr->get_code();
    vgl_box_3d<double> lbb = cell_ptr->local_bounding_box(2);
    vcl_cout<< cell_ptr->level() << ' ' << cd << ' ' << lbb << '\n';
  }
#endif
}

template<class T>
bool boxm_ray_trace_manager<T>::init_raytrace(boxm_scene<boct_tree<short,T > > *scene,
                                              vpgl_camera_double_sptr cam,
                                              unsigned int ni, unsigned int nj,
                                              vcl_vector<vcl_string> functor_source_filenames,
                                              unsigned int i0, unsigned int j0, bool useimage)
{
  scene_ = scene;
  cam_ = cam;
  ni_ = ni;
  nj_ = nj;
  i0_ = i0;
  j0_ = j0;
  useimage_=useimage;
  // load base raytrace code
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                +"/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/backproject.cl")) {
    vcl_cerr << "Error: boxm_ray_trace_manager : failed to load kernel source (helper functions)\n";
    return false;
  }
  // load application-specific functor code
  const unsigned int n_sources = functor_source_filenames.size();
  if (n_sources == 0) {
    vcl_cerr << "Error: boxm_ray_trace_manager : must provide at least one functor source\n";
    return  false;
  }
  for (unsigned int i=0; i < n_sources; ++i) {
    if (!this->append_process_kernels(functor_source_filenames[i])) {
      vcl_cerr << "Error: boxm_ray_trace_manager : failed to load application-specific functor source\n";
      return false;
    }
  }
  if (!this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/ray_trace_main.cl")) {
    vcl_cerr << "Error: boxm_ray_trace_manager : failed to load kernel source (main function)\n";
    return false;
  }
  if (build_kernel_program(useimage_)) {
    return false;
  }

  cl_int status = CL_SUCCESS;
  kernel_ = clCreateKernel(program_,"ray_trace_main",&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status))) {
    return false;
  }
  if (!this->setup_image_cam_arrays())
    return false;
  if (!this->setup_work_image())
    return false;
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_image_cam_arrays()
{
  bool good = true;
  good = good && setup_ray_origin();
  good = good && setup_img_dims(ni_,nj_);
  good = good && setup_camera();
  return good;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_image_cam_arrays()
{
  bool good = true;
  good = good && clean_img_dims();
  good = good && clean_camera();
  good = good && clean_ray_origin();
  return good;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_raytrace()
{
  bool good = true;
  good = good && clean_image_cam_arrays();
  good = good && clean_work_image();

  cl_int status = CL_SUCCESS;
  // release kernel
  status = clReleaseKernel(kernel_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseKernel failed.");
  return good;
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_img_bb(vpgl_camera_double_sptr cam, vgl_box_3d<double> const& block_bb, vgl_box_2d<double> &img_bb, unsigned ni,unsigned nj)
{
  // determine intersection of block bounding box projection and image bounds
  vgl_box_2d<double> img_bounds;
  img_bounds.add(vgl_point_2d<double>(0,0));
  img_bounds.add(vgl_point_2d<double>(0 + ni - 1, 0 + nj - 1));

  vgl_box_2d<double> block_projection;
  double u,v;
  cam->project(block_bb.min_x(),block_bb.min_y(),block_bb.min_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.min_x(),block_bb.min_y(),block_bb.max_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.min_x(),block_bb.max_y(),block_bb.min_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.min_x(),block_bb.max_y(),block_bb.max_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.max_x(),block_bb.min_y(),block_bb.min_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.max_x(),block_bb.min_y(),block_bb.max_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.max_x(),block_bb.max_y(),block_bb.min_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));
  cam->project(block_bb.max_x(),block_bb.max_y(),block_bb.max_z(),u,v);
  block_projection.add(vgl_point_2d<double>(u,v));

  img_bb=vgl_intersection(img_bounds,block_projection);

  if (img_bb.is_empty())
    return false;
  else
    return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_tree()
{
  if (!tree_)
    return false;
  boct_tree_cell<short, T >* root = tree_->root();
  nlevels_=root->level()+1;
  if (!root)
    return false;
  cell_input_.clear();
  data_input_.clear();
  int cell_ptr = 0;
  // put the root into the cell array and its data in the data array
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1; // no parent
  root_cell[1]=-1; // no children at the moment
  root_cell[1]=-1; // no data at the moment
  cell_input_.push_back(root_cell);
  copy_to_arrays<T>(root, cell_input_, data_input_, cell_ptr);

  // the tree is now resident in the 1-d vectors
  // cells as vnl_vector_fixed<int, 4> and
  // data as vnl_vector_fixed<float, 2>

  unsigned cells_size=cell_input_.size();
  if (cells_size>this->image2d_max_width_)
    cells_size=RoundUp(cells_size,this->image2d_max_width_);
  cells_ = NULL;
  cell_data_ = NULL;
#if defined (_WIN32)
  cells_ =
    (cl_int*)_aligned_malloc(cells_size * sizeof(cl_int4), 16);
  cell_data_ =
    (cl_float*)_aligned_malloc(data_input_.size() * sizeof(cl_float16), 16);
numlevels_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
#elif defined(__APPLE__)
  cells_ = (cl_int*)malloc(cells_size * sizeof(cl_int4));
  cell_data_ = (cl_float*)malloc(data_input_.size() * sizeof(cl_float16));
numlevels_=(cl_uint*)malloc(sizeof(cl_uint));
#else
  cells_ = (cl_int*)memalign(16, cells_size * sizeof(cl_int4));
  cell_data_ = (cl_float*)memalign(16, data_input_.size() * sizeof(cl_float16));
  numlevels_=(cl_uint*)memalign(16,sizeof(cl_uint));

#endif
  if (cells_== NULL||cell_data_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }
  for (unsigned i = 0, j = 0; i<cell_input_.size()*4; i+=4, j++)
    for (unsigned k = 0; k<4; ++k)
      cells_[i+k]=cell_input_[j][k];

  // note that the cell data pointer cells[i+2] does not correspond to the 1-d
  // data array location. It must be mapped as:
  //  cell_data indices = 2*cell_data_ptr, 2*cell_data_ptr +1,

  unsigned cell_data_size=16;
  for (unsigned i = 0, j = 0; i<data_input_.size()*cell_data_size; i+=cell_data_size, j++)
    for (unsigned k = 0; k<cell_data_size; ++k)
      cell_data_[i+k]=data_input_[j][k];

(*numlevels_)=nlevels_;
#if defined (_WIN32)
  global_bbox_ = (cl_float*)_aligned_malloc(sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  global_bbox_ = (cl_float*)malloc(sizeof(cl_float4));
#else
  global_bbox_ = (cl_float*)memalign(16, sizeof(cl_float4));
#endif
  global_bbox_[0] = (cl_float)tree_->bounding_box().min_x();
  global_bbox_[1] = (cl_float)tree_->bounding_box().min_y();
  global_bbox_[2] = (cl_float)tree_->bounding_box().min_z();
  global_bbox_[3] = (cl_float)tree_->bounding_box().width();

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_tree()
{
  if (cells_)
  {
#ifdef _WIN32
    _aligned_free(cells_);
#else
    free(cells_);
#endif
    cells_ = NULL;
  }

  if (cell_data_)
  {
#ifdef _WIN32
    _aligned_free(cell_data_);
#else
    free(cell_data_);
#endif
    cell_data_ = NULL;
  }

  if (global_bbox_)
  {
#ifdef _WIN32
    _aligned_free(global_bbox_);
#else
    free(global_bbox_);
#endif
    global_bbox_ = NULL;
  }
  if (numlevels_)
#ifdef _WIN32
    _aligned_free(numlevels_);
#else
    free(numlevels_);
#endif
    numlevels_ = NULL;
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_tree_results()
{
  if (!tree_)
    return false;

#if defined (_WIN32)
  tree_results_ = (cl_int*)_aligned_malloc(this->tree_result_size()* sizeof(cl_int4), 16);
#elif defined(__APPLE__)
  tree_results_ = (cl_int*)malloc(this->tree_result_size() * sizeof(cl_int4));
#else
  tree_results_ = (cl_int*)memalign(16, this->tree_result_size() * sizeof(cl_int4));
#endif

  if (tree_results_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree_results)\n";
    return SDK_FAILURE;
  }
  this->clear_tree_results();

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_tree_results()
{
  if (tree_results_)
  {
#ifdef _WIN32
    _aligned_free(tree_results_);
#else
    free(tree_results_);
#endif
    tree_results_ = NULL;
  }
  return true;
}


template<class T>
bool boxm_ray_trace_manager<T>::setup_camera()
{
  if (vpgl_perspective_camera<double>* pcam =
      dynamic_cast<vpgl_perspective_camera<double>*>(cam_.ptr()))
  {
    vnl_svd<double>* svd=pcam->svd();

    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();

    svd_UtWV_=NULL;
#if defined (_WIN32)
    svd_UtWV_ =  (cl_float*)_aligned_malloc( sizeof(cl_float16)*3, 16);
#elif defined(__APPLE__)
    svd_UtWV_ =  (cl_float*)malloc( sizeof(cl_float16)*3);
#else
    svd_UtWV_ =  (cl_float*)memalign(16, sizeof(cl_float16)*3);
#endif

    int cnt=0;
    for (unsigned i=0;i<Ut.rows();i++)
    {
      for (unsigned j=0;j<Ut.cols();j++)
      {
        svd_UtWV_[cnt]=(cl_float)Ut(i,j);
        ++cnt;
      }
      svd_UtWV_[cnt]=0;
      ++cnt;
    }
    for (unsigned i=0;i<V.rows();i++)
      for (unsigned j=0;j<V.cols();j++)
      {
        svd_UtWV_[cnt]=(cl_float)V(i,j);
        ++cnt;
      }
    for (unsigned i=0;i<Winv.size();i++)
    {
      svd_UtWV_[cnt]=(cl_float)Winv(i);
      ++cnt;
    }
  }
  else {
    vcl_cerr << "Error : boxm_raytrace_manager::setup_camera() : unsupported camera type\n";
    return false;
  }

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_camera()
{
  if (svd_UtWV_)
  {
#ifdef _WIN32
    _aligned_free(svd_UtWV_);
#else
    free(svd_UtWV_);
#endif
    svd_UtWV_ = NULL;
  }
  return true;
}


template<class T>
bool boxm_ray_trace_manager<T>::setup_img_dims(unsigned ni,unsigned nj)
{
  ni_=ni;
  nj_=nj;
#if defined (_WIN32)
  imgdims_ =  (cl_uint*)_aligned_malloc( sizeof(cl_uint4), 16);
#elif defined(__APPLE__)
  imgdims_ =  (cl_uint*)malloc( sizeof(cl_uint4));
#else
  imgdims_ =  (cl_uint*)memalign(16, sizeof(cl_uint));
#endif

  imgdims_[0]=ni;
  imgdims_[1]=nj;
  imgdims_[2]=0;
  imgdims_[3]=0;

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_img_dims()
{
  if (imgdims_) {
#ifdef _WIN32
    _aligned_free(imgdims_);
#else
    free(imgdims_);
#endif
    imgdims_ = NULL;
    return true;
  }
  else {
    return false;
  }
}


template<class T>
bool boxm_ray_trace_manager<T>::setup_roi_dims(unsigned min_i,unsigned max_i,unsigned min_j,unsigned max_j)
{
  roi_min_i_ = min_i;   roi_min_j_ = min_j;
  roi_max_i_ = max_i;   roi_max_j_ = max_j;
#if defined (_WIN32)
  roidims_ =  (cl_uint*)_aligned_malloc( sizeof(cl_uint4), 16);
#elif defined(__APPLE__)
  roidims_ =  (cl_uint*)malloc( sizeof(cl_uint4));
#else
  roidims_ =  (cl_uint*)memalign(16, sizeof(cl_uint));
#endif

  roidims_[0]=roi_min_i_;
  roidims_[1]=roi_max_i_;
  roidims_[2]=roi_min_j_;
  roidims_[3]=roi_max_j_;

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_roi_dims()
{
  if (roidims_) {
#ifdef _WIN32
    _aligned_free(roidims_);
#else
    free(roidims_);
#endif
    roidims_ = NULL;
    return true;
  }
  else {
    return false;
  }
}


template<class T>
bool boxm_ray_trace_manager<T>::setup_work_image()
{
#if defined (_WIN32)
  ray_results_ = (cl_float*)_aligned_malloc(ni_*nj_*sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  ray_results_ = (cl_float*)malloc(ni_*nj_*sizeof(cl_float4));
#else
  ray_results_ = (cl_float*)memalign(16, ni_*nj_* sizeof(cl_float4));
#endif

  for (unsigned i=0;i<ni_*nj_*4;)
  {
    ray_results_[i]=0.0f;
    ray_results_[i+1]=1.0f;
    ray_results_[i+2]=0.0f;
    ray_results_[i+3]=0.0f;
    i+=4;
  }
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_work_image()
{
  if (ray_results_) {
#ifdef _WIN32
    _aligned_free(ray_results_);
#else
    free(ray_results_);
#endif
    ray_results_ = NULL;
    return true;
  }
  else {
    return false;
  }
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_ray_origin()
{
  vgl_point_3d<double> camcenter;

  if (vpgl_perspective_camera<double> *pcam =
      dynamic_cast<vpgl_perspective_camera<double>*>(cam_.ptr())) {
    camcenter=pcam->camera_center();
  }
  else {
    vcl_cerr << "Error: boxm_ray_trace_manager::setup_ray_origin() : unsupported camera type\n";
    return false;
  }

#if defined (_WIN32)
  ray_origin_ = (cl_float*)_aligned_malloc(sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  ray_origin_ = (cl_float*)malloc(sizeof(cl_float4));
#else
  ray_origin_ = (cl_float*)memalign(16, sizeof(cl_float4));
#endif

  ray_origin_[0]=(float)camcenter.x();
  ray_origin_[1]=(float)camcenter.y();
  ray_origin_[2]=(float)camcenter.z();
  ray_origin_[3]=1.0;

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::clean_ray_origin()
{
  if (ray_origin_) {
#ifdef _WIN32
    _aligned_free(ray_origin_);
#else
    free(ray_origin_);
#endif
    ray_origin_ = NULL;
    return true;
  }
  else {
    return false;
  }
}

template<class T>
int boxm_ray_trace_manager<T>::setup_tree_input_buffers(bool useimage)
{
  cl_int status = CL_SUCCESS;
  useimage_=useimage;
  // Create and initialize memory objects
  if (useimage_)
  {
    // store the cells as 4 integers.
    inputformat.image_channel_data_type=CL_SIGNED_INT32;
    inputformat.image_channel_order=CL_RGBA;

    if (cell_input_.size()>this->image2d_max_width_*this->image2d_max_height_)
      return SDK_FAILURE;

    vcl_size_t width=vcl_min(cell_input_.size(),this->image2d_max_width_);
    vcl_size_t height=(vcl_size_t)vcl_ceil((float)cell_input_.size()/(float)width);
    input_cell_buf_=clCreateImage2D(this->context_,
                                    CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                    &inputformat,width,height,width*sizeof(cl_int4),
                                    cells_,&status);
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clCreateBuffer (cell_array) failed."))
      return SDK_FAILURE;
  }
  else
  {
    input_cell_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                     cell_input_.size() * sizeof(cl_int4),
                                     cells_,
                                     &status);
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clCreateBuffer (cell_array) failed."))
      return SDK_FAILURE;
  }
  input_data_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   data_input_.size() * sizeof(cl_float16),
                                   cell_data_,
                                   &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (cell_data) failed."))
    return SDK_FAILURE;
  nlevels_buf_ = clCreateBuffer(this->context_,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_int),
                                numlevels_,
                                &status);
  if (!this->check_val(status,CL_SUCCESS,
                       "clCreateBuffer (nlevels) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_tree_input_buffers()
{
  // Releases OpenCL resources (Context, Memory etc.)
  cl_int status;


  status = clReleaseMemObject(input_cell_buf_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed (input_cell_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(input_data_buf_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed (input_data_buf_)."))
    return SDK_FAILURE;
 status = clReleaseMemObject(nlevels_buf_);
  if (!this->check_val(status,
    CL_SUCCESS,
    "clReleaseMemObject failed (nlevels_buf_)."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_camera_input_buffer()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  camera_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(cl_float16)*3,svd_UtWV_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (camera_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_camera_input_buffer()
{
  cl_int status = clReleaseMemObject(camera_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (camera_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_roidims_input_buffer()
{
  cl_int status = CL_SUCCESS;
  roidims_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_uint4),roidims_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (roi) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_roidims_input_buffer()
{
  cl_int status = clReleaseMemObject(roidims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (roidims_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_ray_origin_buffer()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  ray_origin_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(cl_float4),ray_origin_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (ray origin) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_ray_origin_buffer()
{
  cl_int status = clReleaseMemObject(ray_origin_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (ray_origin_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


template<class T>
int boxm_ray_trace_manager<T>::setup_work_img_buffer()
{
  cl_int status = CL_SUCCESS;
  work_image_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                   ni_*nj_* sizeof(cl_float4),ray_results_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (work image) failed."))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_work_img_buffer()
{
  cl_int status = clReleaseMemObject(work_image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (work_image_buf_) failed."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}


template<class T>
int boxm_ray_trace_manager<T>::setup_imgdims_buffer()
{
  cl_int status = CL_SUCCESS;
  imgdims_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_uint4),imgdims_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (image size) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_imgdims_buffer()
{
  cl_int status = clReleaseMemObject(imgdims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (imgdims_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_tree_global_bbox_buffer()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  global_bbox_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(cl_float4),global_bbox_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (ray origin) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_tree_global_bbox_buffer()
{
  cl_int status = clReleaseMemObject(global_bbox_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (global_bbox_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_image_cam_buffers()
{
  if (setup_camera_input_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (setup_roidims_input_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (setup_ray_origin_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (setup_tree_global_bbox_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (setup_imgdims_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::clean_image_cam_buffers()
{
  if (clean_camera_input_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (clean_roidims_input_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (clean_ray_origin_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (clean_tree_global_bbox_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  if (clean_imgdims_buffer()!=SDK_SUCCESS)
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::build_kernel_program(bool useimage)
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!this->check_val(status,
      CL_SUCCESS,
      "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

  program_ = clCreateProgramWithSource(this->context_,
                                       1,
                                       &source,
                                       sourceSize,
                                       &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  vcl_string options="";

  if (useimage)
    options+="-D USEIMAGE";

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_,
                          1,
                          this->devices_,
                          options.c_str(),
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
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::create_kernel(vcl_string const& kernel_name)
{
  cl_int status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_ = clCreateKernel(program_,kernel_name.c_str(),&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::release_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;
  if (!this->check_val(status,
    CL_SUCCESS,
    "clReleaseKernel failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
void boxm_ray_trace_manager<T>::clear_tree_results()
{
  if (tree_results_) {
    unsigned n = 4*this->tree_result_size();
    for (unsigned i = 0; i<n; ++i)
      tree_results_[i]=0;
  }
}

template<class T>
void boxm_ray_trace_manager<T>::clear_ray_results()
{
  if (ray_results_) {
    unsigned n = 4*this->n_rays();
    for (unsigned i = 0; i<n; ++i)
      ray_results_[i]=0.0f;
  }
}

template<class T>
vcl_size_t boxm_ray_trace_manager<T>::n_ray_groups()
{
  vcl_size_t nr = this->n_rays();
  vcl_size_t ng = nr/this->group_size();
  if (ng == 0)
    ng = 1;
  return ng;
}

template<class T>
void boxm_ray_trace_manager<T>::print_tree_input()
{
  vcl_cout << "Tree Input\n";
  if (cells_)
    for (unsigned i = 0; i<cell_input_.size()*4; i+=4) {
      int data_ptr = 2*cells_[i+2];
      vcl_cout << "tree input[" << i/4 << "]("
               << cells_[i]   << ' '
               << cells_[i+1] << ' '
               << cells_[i+2] << ' '
               << cells_[i+3];
      if (data_ptr>0)
        vcl_cout << ' ' << cell_data_[data_ptr] << ':'
                 << cell_data_[data_ptr+1];
      vcl_cout << ")\n";
    }
}

template<class T>
void boxm_ray_trace_manager<T>::print_ray_input()
{
  vcl_cout << "Ray Input\n";
  vcl_size_t n = this->n_rays();
  if (ray_origin_&&ray_dir_)
    for (unsigned i = 0; i<n*4; i+=4)
      vcl_cout << "ray origin[" << i/4 << "](" << ray_origin_[i] << ' '
               << ray_origin_[i+1] << ' '
               << ray_origin_[i+2] << ' '
               << ray_origin_[i+3] << ")\n"
               << "ray_dir["<< i/4 << "](" << ray_dir_[i] << ' '
               << ray_dir_[i+1] << ' '
               << ray_dir_[i+2] << ' '
               << ray_dir_[i+3] << ")\n\n";
}

template<class T>
void boxm_ray_trace_manager<T>::print_ray_results()
{
  vcl_size_t n = this->n_rays()*4;
  if (ray_results_) {
    vcl_cout << "--Ray Results--\n";
    for (unsigned i = 0; i<n; i+=4)
      vcl_cout << "ray_out[" << i/4 << "](" << ray_results_[i] << ' '
               << ray_results_[i+1] << ' '
               << ray_results_[i+2] << ' '
               << ray_results_[i+3] << ")\n";
  }
}


template<class T>
boxm_ray_trace_manager<T>::~boxm_ray_trace_manager()
{
  if (program_)
    clReleaseProgram(program_);
}


template<class T>
bool boxm_ray_trace_manager<T>::load_tree(vcl_string const& path)
{
  if (tree_)
    delete tree_;
  tree_ =new boct_tree<short, T >();
  vsl_b_ifstream is(path.c_str(), vcl_ios_binary);
  if (!is)
    return false;
  tree_->b_read(is);
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::write_tree(vcl_string const& path)
{
  if (!tree_)
    return false;

  vsl_b_ofstream os(path.c_str());
  if (!os)
    return false;
  tree_->b_write(os, false);
  os.close();
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::run()
{
  cl_int status = CL_SUCCESS;
  vul_timer timer;
  vul_timer t;
  vcl_string error_message="";
  //setup_camera_input_buffer();
  //setup_ray_origin_buffer();
  setup_work_img_buffer();


  boxm_block_vis_graph_iterator<tree_type > block_vis_iter(cam_, scene_, ni_, nj_);
  float total_raytrace_time = 0.0f;
  float total_gpu_time = 0.0f;
  float total_load_time = 0.0f;
  while (block_vis_iter.next())
  {
    vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (unsigned i=0; i<block_indices.size(); i++) // code for each block
    {
      timer.mark();
      scene_->load_block(block_indices[i]);
      boxm_block<tree_type> * curr_block=scene_->get_active_block();
      float load_time = (float)timer.all() / 1e3f;
      vcl_cout << "loading block took " << load_time << 's' << vcl_endl;
      total_load_time += load_time;
      vcl_cout << "processing block at index (" <<block_indices[i] << ')' << vcl_endl;
      timer.mark();
      // make sure block projects to inside of image
      vgl_box_3d<double> block_bb = curr_block->bounding_box();

      if (!boxm_utils::is_visible(block_bb, cam_, ni_, nj_))
        continue;
      vgl_box_2d<double> img_bb;

      if (!setup_img_bb(cam_, block_bb, img_bb, ni_, nj_)) {
        continue;
      }

      tree_type * tree=curr_block->get_tree();

      set_tree(tree);
      setup_tree();

      setup_roi_dims((unsigned int)img_bb.min_x(),(unsigned int)img_bb.max_x(),(unsigned int)img_bb.min_y(),(unsigned int)img_bb.max_y());

      // allocate and initialize memory
      setup_tree_input_buffers(useimage_);
      setup_image_cam_buffers();

      t.mark();
      // run the raytracing for this block
      run_block();
      total_gpu_time+=t.all();
      // release memory

      clean_image_cam_buffers();
      //clean_work_img_buffer();
      clean_tree_input_buffers();

      clean_tree();

      float raytrace_time = (float)timer.all() / 1e3f;
      vcl_cout<<"processing block took " << raytrace_time << 's' << vcl_endl;
      total_raytrace_time += raytrace_time;
    }
  }
  read_output_image();
  clean_work_img_buffer();
  //clean_ray_origin_buffer();
  //clean_camera_input_buffer();

  vcl_cout << "Running block "<<total_gpu_time/1000<<'s'<<vcl_endl
           << "total block loading time = " << total_load_time << 's' << vcl_endl
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl;
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::run_block()
{
  // -- Set appropriate arguments to the kernel --
  cl_int status = CL_SUCCESS;
  status = clSetKernelArg(kernel_,0,sizeof(cl_mem),(void *)&nlevels_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (nlevels_buf_)"))
    return SDK_FAILURE;

  // the ray origin buffer
  status = CL_SUCCESS;
  status = clSetKernelArg(kernel_,1,sizeof(cl_mem),(void *)&ray_origin_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ray_origin_array)"))
    return SDK_FAILURE;

  // camera buffer
  status = clSetKernelArg(kernel_,2,sizeof(cl_mem),(void *)&camera_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ray_dir_array)"))
    return SDK_FAILURE;

  // tree buffer
  status = clSetKernelArg(kernel_,3,sizeof(cl_mem),(void *)&input_cell_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (octree)"))
    return SDK_FAILURE;
  // tree buffer
  status = clSetKernelArg(kernel_,4,sizeof(cl_mem),(void *)&input_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
    return SDK_FAILURE;

  // roi dimensions
  status = clSetKernelArg(kernel_,5,sizeof(cl_mem),(void *)&imgdims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,6,sizeof(cl_mem),(void *)&roidims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ROI dimensions)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,7,sizeof(cl_mem),(void *)&global_bbox_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (global bbox)"))
    return SDK_FAILURE;
  // output image buffer
  status = clSetKernelArg(kernel_,8,sizeof(cl_mem),(void *)&work_image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_work_image)"))
    return SDK_FAILURE;

  //  set local variable
  status = clSetKernelArg(kernel_,9,sizeof(cl_float16)*3,0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,10,sizeof(cl_float4),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local origin)"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,11,sizeof(cl_float4),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local box)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,12,sizeof(cl_uint4),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local roi)"))
    return SDK_FAILURE;
#if 0
  status = clSetKernelArg(kernel_,13,sizeof(cl_float4)*this->group_size(),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local roi)"))
   return SDK_FAILURE;
  status = clSetKernelArg(kernel_,13,sizeof(cl_int)*this->group_size()*(this->num_levels()),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local stack)"))
    return SDK_FAILURE;
#endif

  // check the local memeory
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

  vcl_size_t globalThreads[]= {RoundUp((imgdims_[0])*(imgdims_[1]),this->group_size())};
  vcl_size_t localThreads[] = {this->group_size()};

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],0,&status);
  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_,this->kernel_, 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);

  return SDK_SUCCESS;
}

template<class T>
bool boxm_ray_trace_manager<T>:: read_output_image()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,work_image_buf_,CL_TRUE,
                                   0,ni_*nj_*sizeof(cl_float4),
                                   this->ray_results(),
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (ray_results)failed."))
    return false;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clReleaseEvent(events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return false;

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return false;
return true;
}

#define BOXM_RAY_TRACE_MANAGER_INSTANTIATE(T) \
  template class boxm_ray_trace_manager<T >

#endif
