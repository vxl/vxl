// This is brl/bseg/boxm/opt/open_cl/boxm_ray_trace_manager.txx
#ifndef boxm_ray_trace_manager_txx_
#define boxm_ray_trace_manager_txx_
#include <vcl_fstream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vbl/io/vbl_io_array_2d.h>
#include "boxm_ray_trace_manager.h"
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_where_root_dir.h>
#include <vul/vul_timer.h>

#include "boxm_opt_opencl_utils.h"

//allocate child cells on the array
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
  //cell_input_ptr is the array index for the cell being constructed
  //it already exists in the cell array but only has the parent index set
  //no data or child pointers

  // convert the data to 16 vector size
  vnl_vector_fixed<float, 16> data;
  pack_cell_data(cell_ptr,data);

  // data pointer will be at index == size after the push_back
  cell_array[cell_input_ptr][2] = data_array.size();
  data_array.push_back(data);
  cell_array[cell_input_ptr][3] = cell_ptr->level();
  //if the cell has chidren then they must be copied
  if (!cell_ptr->is_leaf()) {
    //initialize data values to null
      data_array[cell_array[cell_input_ptr][2]].fill(0.0);
    //create the children on the cell array
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
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_tree()
{
  if (!tree_)
    return false;
  boct_tree_cell<short, T >* root = tree_->root();
  if (!root)
    return false;
  cell_input_.clear();
  data_input_.clear();
  int cell_ptr = 0;
  //put the root into the cell array and its data in the data array
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1; //no parent
  root_cell[1]=-1; //no children at the moment
  root_cell[1]=-1; //no data at the moment
  cell_input_.push_back(root_cell);
  copy_to_arrays<T>(root, cell_input_, data_input_, cell_ptr);
  //the tree is now resident in the 1-d vectors
  //cells as vnl_vector_fixed<int, 4> and
  //data as vnl_vector_fixed<float, 2>
  cells_ = NULL;
  cell_data_ = NULL;
#if defined (_WIN32)
  cells_ =
    (cl_int*)_aligned_malloc(cell_input_.size() * sizeof(cl_int4), 16);
  cell_data_ =
    (cl_float*)_aligned_malloc(data_input_.size() * sizeof(cl_float16), 16);
#elif defined(__APPLE__)
  cells_ = (cl_int*)malloc(cell_input_.size() * sizeof(cl_int4));
  cell_data_ = (cl_float*)malloc(data_input_.size() * sizeof(cl_float16));
#else
  cells_ = (cl_int*)memalign(16, cell_input_.size() * sizeof(cl_int4));
  cell_data_ = (cl_float*)memalign(16, data_input_.size() * sizeof(cl_float16));
#endif
  if (cells_== NULL||cell_data_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }
  for (unsigned i = 0, j = 0; i<cell_input_.size()*4; i+=4, j++)
    for (unsigned k = 0; k<4; ++k)
      cells_[i+k]=cell_input_[j][k];

  //note that the cell data pointer cells[i+2] does not correspond to the 1-d
  //data array location. It must be mapped as:
  // cell_data indices = 2*cell_data_ptr, 2*cell_data_ptr +1,

  unsigned cell_data_size=16;
  for (unsigned i = 0, j = 0; i<data_input_.size()*cell_data_size; i+=cell_data_size, j++)
    for (unsigned k = 0; k<cell_data_size; ++k)
      cell_data_[i+k]=data_input_[j][k];

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
bool boxm_ray_trace_manager<T>::setup_camera()
{
  if (!pcam)
    return false;

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
bool boxm_ray_trace_manager<T>::setup_roi_dims(unsigned min_i,unsigned max_i,unsigned min_j,unsigned max_j)
{
#if defined (_WIN32)
  roidims_ =  (cl_uint*)_aligned_malloc( sizeof(cl_uint4), 16);
#elif defined(__APPLE__)
  roidims_ =  (cl_uint*)malloc( sizeof(cl_uint4));
#else
  roidims_ =  (cl_uint*)memalign(16, sizeof(cl_uint));
#endif

  roidims_[0]=min_i;
  roidims_[1]=max_i;
  roidims_[2]=min_j;
  roidims_[3]=max_j;

  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::setup_expected_image(unsigned ni,unsigned nj)
{
  ni_=ni;
  nj_=nj;
#if defined (_WIN32)
  ray_results_ = (cl_float*)_aligned_malloc(ni*nj*sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  ray_results_ = (cl_float*)malloc(ni*nj*sizeof(cl_float4));
#else
  ray_results_ = (cl_float*)memalign(16, ni*nj* sizeof(cl_float4));
#endif

  for (unsigned i=0;i<ni*nj*4;)
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
bool boxm_ray_trace_manager<T>::setup_ray_origin()
{
  if (!pcam)
    return false;

  vgl_point_3d<double> camcenter=pcam->camera_center();

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
bool boxm_ray_trace_manager<T>::setup_ray_results()
{
#if defined (_WIN32)
  ray_results_ = (cl_float*)_aligned_malloc(sizeof(cl_float4)*this->n_rays(), 16);
#elif defined(__APPLE__)
  ray_results_ = (cl_float*)malloc(sizeof(cl_float4)*this->n_rays());
#else
  ray_results_ = (cl_float*)memalign(16, sizeof(cl_float4)*this->n_rays());
#endif

  for (unsigned i=0;i<this->n_rays();)
    for (unsigned j=0;j<4;j++)
      ray_results_[i++]=0.0;

  return true;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_tree_input_buffers()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  input_cell_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   cell_input_.size() * sizeof(cl_int4),
                                   cells_,
                                   &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (cell_array) failed."))
    return SDK_FAILURE;

  input_data_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   data_input_.size() * sizeof(cl_float16),
                                   cell_data_,
                                   &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (cell_data) failed."))
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
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell_array) failed."))
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
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell_array) failed."))
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
int boxm_ray_trace_manager<T>::setup_expected_img_buffer()
{
  cl_int status = CL_SUCCESS;
  expected_image_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                       ni_*nj_* sizeof(cl_float4),ray_results_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (expected image) failed."))
    return SDK_FAILURE;
  out_expected_image_buf_ = clCreateBuffer(this->context_,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                                           ni_*nj_* sizeof(cl_float4),ray_results_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (expected image) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_imgdims_buffer()
{
  cl_int status = CL_SUCCESS;
  imgdims_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_uint4),imgdims_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell_array) failed."))
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
int boxm_ray_trace_manager<T>::build_kernel_program()
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
int boxm_ray_trace_manager<T>::setup_tree_processing()
{
  if (setup_tree_input_buffers()  != SDK_SUCCESS ||
      build_kernel_program()      != SDK_SUCCESS)
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::setup_ray_processing()
{
#if 0
  if (setup_tree_input_buffers() != SDK_SUCCESS ||
      setup_ray_input_buffers()  != SDK_SUCCESS ||
      build_kernel_program()     != SDK_SUCCESS)
    return SDK_FAILURE;
  else
#endif
    return SDK_SUCCESS;
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
int boxm_ray_trace_manager<T>::cleanup_tree_processing()
{
  // Releases OpenCL resources (Context, Memory etc.)
  cl_int status;

  if (kernel_) {
    status = clReleaseKernel(kernel_);
    if (!this->check_val(status,
      CL_SUCCESS,
      "clReleaseKernel failed."))
      return SDK_FAILURE;
  }

  status = clReleaseMemObject(input_cell_buf_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(input_data_buf_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_ray_trace_manager<T>::cleanup_ray_processing()
{
  this->cleanup_tree_processing();
  cl_int status;
  status = clReleaseMemObject(ray_origin_buf_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(ray_dir_buf_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
void boxm_ray_trace_manager<T>::delete_memory()
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

  if (tree_results_)
  {
#ifdef _WIN32
    _aligned_free(tree_results_);
#else
    free(tree_results_);
#endif
    tree_results_ = NULL;
  }
  if (ray_results_)
  {
#ifdef _WIN32
    _aligned_free(ray_results_);
#else
    free(ray_results_);
#endif
    ray_results_ = NULL;
  }
}

template<class T>
boxm_ray_trace_manager<T>::~boxm_ray_trace_manager()
{
  if (program_)
    clReleaseProgram(program_);
  this->delete_memory();
}

template<class T>
bool boxm_ray_trace_manager<T>::load_kernel_source(vcl_string const& path)
{
  prog_ = "";
  vcl_ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  char temp[256];
  vcl_ostringstream ostr;
  while (!is.eof()) {
    is.getline(temp, 256);
    vcl_string s(temp);
    ostr << s << '\n';
  }
  prog_ =  ostr.str();
  return prog_.size() > 0;
}

template<class T>
bool boxm_ray_trace_manager<T>::append_process_kernels(vcl_string const& path)
{
  vcl_ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  char temp[256];
  vcl_ostringstream ostr;
  while (!is.eof()) {
    is.getline(temp, 256);
    vcl_string s(temp);
    ostr << s << '\n';
  }
  prog_ += ostr.str();
  return true;
}

template<class T>
bool boxm_ray_trace_manager<T>::write_program(vcl_string const& path)
{
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return false;
  os << prog_;
  return true;
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


#if 0
template<boxm_apm_type APM_MODEL>
bool boxm_ray_trace_manager<APM_MODEL>::load_perspective_camera(vcl_string filename)
{
  pcam =new vpgl_perspective_camera<double>;
  vcl_ifstream ifs(filename.c_str());
  if (!ifs)
    return false;
  else
    ifs >> *pcam;

   return true;
}
#endif // 0

template<class T>
bool boxm_ray_trace_manager<T>::run()
{
  cl_int status = CL_SUCCESS;  cl_event events[2];

  vul_timer t;       

  vcl_string error_message="";
  // set up a command queue
  cl_command_queue command_queue = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  clSetCommandQueueProperty(command_queue, CL_QUEUE_PROFILING_ENABLE, CL_TRUE, NULL);

  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  if (!load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                          +"/contrib/brl/bseg/boxm/opt/open_cl/octree_library_functions.cl") ||
      !append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                              +"/contrib/brl/bseg/boxm/opt/open_cl/expected_functor.cl") ||
      !append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                              +"/contrib/brl/bseg/boxm/opt/open_cl/backproject.cl") ||
     !append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                             +"/contrib/brl/bseg/boxm/opt/open_cl/expected_ray_trace.cl") )
     return false;
  if (build_kernel_program())
      return false;

  t.mark();
  kernel_ = clCreateKernel(program_,"expected_ray_trace",&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
      return false;

  // -- Set appropriate arguments to the kernel --
  // the ray origin buffer
  status = clSetKernelArg(kernel_,0,sizeof(cl_mem),(void *)&ray_origin_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ray_origin_array)"))
    return SDK_FAILURE;

  // camera buffer
  status = clSetKernelArg(kernel_,1,sizeof(cl_mem),(void *)&camera_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ray_dir_array)"))
                       return SDK_FAILURE;

  // tree buffer
  status = clSetKernelArg(kernel_,2,sizeof(cl_mem),(void *)&input_cell_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (octree)"))
                       return SDK_FAILURE;
  // tree buffer
  status = clSetKernelArg(kernel_,3,sizeof(cl_mem),(void *)&input_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
                       return SDK_FAILURE;

  // roi dimensions
  status = clSetKernelArg(kernel_,4,sizeof(cl_mem),(void *)&imgdims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
                       return SDK_FAILURE;
  status = clSetKernelArg(kernel_,5,sizeof(cl_mem),(void *)&roidims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ROI dimensions)"))
                       return SDK_FAILURE;
  status = clSetKernelArg(kernel_,6,sizeof(cl_mem),(void *)&global_bbox_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (global bbox)"))
                       return SDK_FAILURE;
  // output image buffer
  status = clSetKernelArg(kernel_,7,sizeof(cl_mem),(void *)&expected_image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_expected_image)"))
                       return SDK_FAILURE;
  // output image buffer
  status = clSetKernelArg(kernel_,8,sizeof(cl_mem),(void *)&out_expected_image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output_expected_image)"))
                       return SDK_FAILURE;

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

  vcl_size_t globalThreads[]= {RoundUp((imgdims_[0])*(imgdims_[1]),128)};
  vcl_size_t localThreads[] = {128};

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue,this->kernel_, 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);



  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed."+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);


  // Enqueue readBuffers
  status = clEnqueueReadBuffer(command_queue,out_expected_image_buf_,CL_TRUE,
                               0,ni_*nj_*sizeof(cl_float4),
                               this->ray_results(),
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (ray_results)failed."))
    return SDK_FAILURE;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  clReleaseEvent(events[0]);
  vcl_cout<<"Time taken is "<<t.all()<<vcl_endl;

  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
  return true;
}

#define BOXM_RAY_TRACE_MANAGER_INSTANTIATE(T) \
template class boxm_ray_trace_manager<T >\


#endif
