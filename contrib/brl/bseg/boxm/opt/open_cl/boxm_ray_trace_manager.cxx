
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
#include <malloc.h>

//allocate child cells on the array
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
static void 
copy_to_arrays(boct_tree_cell<short, vnl_vector_fixed<float, 2> >* cell_ptr,
               vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
               vcl_vector<vnl_vector_fixed<float, 2> >& data_array,
               int cell_input_ptr
               )
{
  //cell_input_ptr is the array index for the cell being constructed
  //it already exists in the cell array but only has the parent index set
  //no data or child pointers
  vnl_vector_fixed<float, 2> cell_data = cell_ptr->data();
  // data pointer will be at index == size after the push_back
  cell_array[cell_input_ptr][2] = data_array.size();
  data_array.push_back(cell_data);
  cell_array[cell_input_ptr][3] = cell_ptr->level();
  //if the cell has chidren then they must be copied
  if(!cell_ptr->is_leaf()){
    //initialize data values to null
    data_array[cell_array[cell_input_ptr][2]][0]=-1;
    data_array[cell_array[cell_input_ptr][2]][1]=-2;
    //create the children on the cell array
    int child_ptr = -1;
    split(cell_array, cell_input_ptr, child_ptr);
    cell_array[cell_input_ptr][1]=child_ptr;
    boct_tree_cell<short, vnl_vector_fixed<float, 2> >* children = 
      cell_ptr->children();
    for(unsigned i = 0; i<8; ++i){
      boct_tree_cell<short, vnl_vector_fixed<float, 2> >* child_cell_ptr = children + i;
      int child_cell_input_ptr = child_ptr +i;
      copy_to_arrays(child_cell_ptr, cell_array, data_array, child_cell_input_ptr);
    }
  }
}

bool boxm_ray_trace_manager::setup_tree()
{
  if(!tree_)
    return false;
  boct_tree_cell<short, vnl_vector_fixed<float, 2> >* root = tree_->root();
  if(!root)
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
  copy_to_arrays(root, cell_input_, data_input_, cell_ptr);
  //the tree is now resident in the 1-d vectors
  //cells as vnl_vector_fixed<int, 4> and
  //data as vnl_vector_fixed<float, 2>
  cells_ = NULL;
  cell_data_ = NULL;
#if defined (_WIN32)
  cells_ = 
    (cl_int*)_aligned_malloc(cell_input_.size() * sizeof(cl_int4), 16);
  cell_data_ = 
    (cl_float*)_aligned_malloc(data_input_.size() * sizeof(cl_float2), 16);
#else
  cells_ = (cl_int*)memalign(16, cell_input_.size() * sizeof(cl_int4));
  cell_data_ = (cl_float*)memalign(16, data_input_.size() * sizeof(cl_float2));
#endif
	if(cells_== NULL||cell_data_ == NULL)	
    { 
      vcl_cout << "Failed to allocate host memory. (tree input)\n";
      return false;
    }
  for(unsigned i = 0, j = 0; i<cell_input_.size()*4; i+=4, j++)
    for(unsigned k = 0; k<4; ++k)
      cells_[i+k]=cell_input_[j][k];

  //note that the cell data pointer cells[i+2] does not correspond to the 1-d 
  //data array location. It must be mapped as:
  // cell_data indices = 2*cell_data_ptr, 2*cell_data_ptr +1, 

  for(unsigned i = 0, j = 0; i<data_input_.size()*2; i+=2, j++)
    for(unsigned k = 0; k<2; ++k)
      cell_data_[i+k]=data_input_[j][k];


#if defined (_WIN32)
  tree_results_ = (cl_int*)_aligned_malloc(this->tree_result_size()* sizeof(cl_int4), 16);
#else
  tree_results_ = (cl_float*)memalign(16, result_size_ * sizeof(cl_int4));
#endif

	if(tree_results_ == NULL)	
    { 
      vcl_cout << "Failed to allocate host memory. (tree_results)\n";
      return SDK_FAILURE;
    }
  this->clear_tree_results();
  return true;
}

void boxm_ray_trace_manager::free_ray_memory()
{
  if(ray_origin_){
    #ifdef _WIN32
      _aligned_free(ray_origin_);
#else
      free(ray_origin_);
#endif
      ray_origin_ = NULL;
  }
  if(ray_dir_){
    #ifdef _WIN32
      _aligned_free(ray_dir_);
#else
      free(ray_dir_);
#endif
      ray_dir_ = NULL;
  }
}

bool boxm_ray_trace_manager::setup_rays()
{
  this->free_ray_memory();
  size_t nrows = ray_origin_input_.rows(), ncols = ray_origin_input_.cols();
  if(nrows != ray_dir_input_.rows() ||
     ncols != ray_dir_input_.cols()){
    vcl_cout << "In boxm_ray_trace_manager::setup_rays() - inconsistent "
             << "array size \n";
    return false;
  }  
  size_t nrays = nrows*ncols;
  ray_origin_ = NULL;
  ray_dir_ = NULL;
#if defined (_WIN32)
  ray_origin_ = 
    (cl_float*)_aligned_malloc(nrays * sizeof(cl_float4), 16);
  ray_dir_ = 
    (cl_float*)_aligned_malloc(nrays * sizeof(cl_float4), 16);
#else
  ray_origin_ = (cl_float*)memalign(16, nrays * sizeof(cl_float4));
  ray_dir_ = (cl_float*)memalign(16, nrays * sizeof(cl_float4));
#endif
	if(cells_== NULL||cell_data_ == NULL)	
    { 
      vcl_cout << "boxm_ray_trace_manager::setup_rays()"
               << " failed to allocate host memory\n";
      return false;
    }

  int ptr = 0;
  for(size_t r = 0; r<nrows; ++r)
    for(size_t c = 0; c<ncols; ++c)
      { 
        vnl_vector_fixed<float, 3> origin = ray_origin_input_[r][c];
        vnl_vector_fixed<float, 3> dir = ray_dir_input_[r][c];
        for(size_t i = 0; i<3; ++i){
          ray_origin_[ptr] = origin[i];
          ray_dir_[ptr] = dir[i];
          ptr++;
        }
        ray_origin_[ptr]=0.0f;
        ray_dir_[ptr]=0.0f;
		ptr++;
      }
#if defined (_WIN32)
  ray_results_ = (cl_float*)_aligned_malloc(this->n_rays()* sizeof(cl_float4), 16);
#else
  ray_results_ = (cl_float*)memalign(16, this->n_rays()) * sizeof(cl_float4));
#endif

	if(ray_results_ == NULL)	
    { 
      vcl_cout << "Failed to allocate host memory. (ray_results)\n";
      return SDK_FAILURE;
    }
  this->clear_ray_results();

  return true;
}

int boxm_ray_trace_manager::setup_tree_input_buffers()		
{
  cl_int status = CL_SUCCESS;
  /*
   * Create and initialize memory objects
   */
  input_cell_buf_ = clCreateBuffer(
                                   context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   cell_input_.size() * sizeof(cl_int4),
                                   cells_,
                                   &status);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clCreateBuffer (cell_array) failed."))
    {
      return SDK_FAILURE;
    }

  input_data_buf_ = clCreateBuffer(
                                   context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   data_input_.size() * sizeof(cl_float2),
                                   cell_data_,
                                   &status);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clCreateBuffer (cell_data) failed."))
    {
      return SDK_FAILURE;
    }

  return SDK_SUCCESS;
}

int boxm_ray_trace_manager::setup_ray_input_buffers()		
{
  size_t nrows = ray_origin_input_.rows(), ncols = ray_origin_input_.cols();
  size_t nrays = nrows*ncols;
  cl_int status = CL_SUCCESS;
  /*
   * Create and initialize memory objects
   */
  ray_origin_buf_ = clCreateBuffer(
                                   context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   nrays * sizeof(cl_float4),
                                   ray_origin_,
                                   &status);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clCreateBuffer (ray_origin) failed."))
    {
      return SDK_FAILURE;
    }

  ray_dir_buf_ = clCreateBuffer(
                                context_,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                nrays * sizeof(cl_float4),
                                ray_dir_,
                                &status);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clCreateBuffer (ray dir) failed."))
    {
      return SDK_FAILURE;
    }

  return SDK_SUCCESS;
}
int boxm_ray_trace_manager::build_kernel_program()
{
  cl_int status = CL_SUCCESS;
  size_t sourceSize[] = { prog_.size() };
  if(!sourceSize[0]) return SDK_FAILURE;
  if(program_) {
  status = clReleaseProgram(program_);
  program_ = 0;
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseProgram failed."))
    {
      return SDK_FAILURE;
    }
  }
  const char * source = prog_.c_str();

  program_ = clCreateProgramWithSource(
                                       context_,
                                       1,
                                       &source,
                                       sourceSize,
                                       &status);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clCreateProgramWithSource failed."))
    return SDK_FAILURE;
    
  /* create a cl program executable for all the devices specified */
  status = clBuildProgram(
                          program_,
                          1,
                          devices_,
                          NULL,
                          NULL,
                          NULL);
  if(!this->check_val(
                      status,
                      CL_SUCCESS, 
                      "clBuildProgram failed."))
    {
      size_t len;
      char buffer[2048];
      clGetProgramBuildInfo(program_, devices_[0],
                            CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
      printf("%s\n", buffer);
      return SDK_FAILURE;
    }

  return SDK_SUCCESS;
}

int boxm_ray_trace_manager::create_kernel(vcl_string const& kernel_name){
  cl_int status = CL_SUCCESS;
  /* get a kernel object handle for a kernel with the given name */
  kernel_ = clCreateKernel(
                           program_,
                           kernel_name.c_str(),
                           &status);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clCreateKernel failed."))
    {
      return SDK_FAILURE;
    }

  return SDK_SUCCESS;

}
bool boxm_ray_trace_manager::
update_ray_input(vbl_array_2d<vnl_vector_fixed<float, 3> > const& ray_origin,
                 vbl_array_2d<vnl_vector_fixed<float, 3> > const& ray_dir)
{
  this->set_rays(ray_origin, ray_dir);
  if(this->setup_rays())
    return false;
  if(setup_ray_input_buffers()!=SDK_SUCCESS)
		return false;
  return true;
}

int boxm_ray_trace_manager::release_kernel(){

  cl_int status = SDK_SUCCESS;

  status = clReleaseKernel(kernel_);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseKernel failed."))
    {
      return SDK_FAILURE;
    }
  return SDK_SUCCESS;
}

void boxm_ray_trace_manager::clear_tree_results()
{
  if(tree_results_){
    unsigned n = 4*this->tree_result_size();
    for(unsigned i = 0; i<n; ++i)
      tree_results_[i]=0;
  }
}
void boxm_ray_trace_manager::clear_ray_results()
{
  if(ray_results_){
    unsigned n = 4*this->n_rays();
    for(unsigned i = 0; i<n; ++i)
      ray_results_[i]=0.0f;
  }

}
size_t boxm_ray_trace_manager::n_ray_groups()
{
  size_t nr = this->n_rays();
  size_t ng = nr/this->group_size();
  if(ng == 0)
    ng = 1;
  return ng;
}

int boxm_ray_trace_manager::setup_tree_processing()
{
  if(setup_tree_input_buffers()!=SDK_SUCCESS)
    {
      return SDK_FAILURE;
    }
  if(build_kernel_program()!=SDK_SUCCESS)
    {
      return SDK_FAILURE;
    }
  return SDK_SUCCESS;
}

int boxm_ray_trace_manager::setup_ray_processing()
{
  if(setup_tree_input_buffers()!=SDK_SUCCESS)
    {
      return SDK_FAILURE;
    }
  if(setup_ray_input_buffers()!=SDK_SUCCESS)
    {
      return SDK_FAILURE;
    }
  if(build_kernel_program()!=SDK_SUCCESS)
    {
      return SDK_FAILURE;
    }
  return SDK_SUCCESS;
}


void boxm_ray_trace_manager::print_tree_input()
{
  vcl_cout << "Tree Input\n";
  if(cells_)
    for(unsigned i = 0; i<cell_input_.size()*4; i+=4){
      int data_ptr = 2*cells_[i+2];
      vcl_cout << "tree input[" << i/4 << "](" << cells_[i] << ' '
               << cells_[i+1] << ' '
               << cells_[i+2] << ' '
               << cells_[i+3]; 
      if(data_ptr>0)
               vcl_cout << ' ' << cell_data_[data_ptr] << ':'
               << cell_data_[data_ptr+1] 
               << ")\n";
      else
        vcl_cout << ")\n";
    }
}
void boxm_ray_trace_manager::print_ray_input()
{
  vcl_cout << "Ray Input \n";
  size_t n = this->n_rays();
  if(ray_origin_&&ray_dir_)
    for(unsigned i = 0; i<n*4; i+=4)
      vcl_cout << "ray origin[" << i/4 << "](" << ray_origin_[i] << ' '
               << ray_origin_[i+1] << ' '
               << ray_origin_[i+2] << ' '
               << ray_origin_[i+3] << ")\n"
               << "ray_dir["<< i/4 << "](" << ray_dir_[i] << ' '
               << ray_dir_[i+1] << ' '
               << ray_dir_[i+2] << ' '
               << ray_dir_[i+3] << ")\n\n";
  
}

void boxm_ray_trace_manager::print_ray_results()
{
  size_t n = this->n_rays()*4;
  if(ray_results_){
    vcl_cout << "--Ray Results--\n";
    for(unsigned i = 0; i<n; i+=4)
      vcl_cout << "ray_out[" << i/4 << "](" << ray_results_[i] << ' '
               << ray_results_[i+1] << ' '
               << ray_results_[i+2] << ' '
               << ray_results_[i+3] << ")\n";
}
}
int boxm_ray_trace_manager::cleanup_tree_processing()
{
  /* Releases OpenCL resources (Context, Memory etc.) */
  cl_int status;

  status = clReleaseKernel(kernel_);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseKernel failed."))
    {
      return SDK_FAILURE;
    }

  status = clReleaseMemObject(input_cell_buf_);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseMemObject failed."))
    {
      return SDK_FAILURE;
    }

  status = clReleaseMemObject(input_data_buf_);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseMemObject failed."))
    {
      return SDK_FAILURE;
    }

  return SDK_SUCCESS;
}

int boxm_ray_trace_manager::cleanup_ray_processing()
{
  this->cleanup_tree_processing();
  cl_int status;
  status = clReleaseMemObject(ray_origin_buf_);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseMemObject failed."))
    {
      return SDK_FAILURE;
    }
  
  status = clReleaseMemObject(ray_dir_buf_);
  if(!this->check_val(
                      status,
                      CL_SUCCESS,
                      "clReleaseMemObject failed."))
    {
      return SDK_FAILURE;
    }
  return SDK_SUCCESS;
}

void boxm_ray_trace_manager::delete_memory()
{
  if(cells_) 
    {
#ifdef _WIN32
      _aligned_free(cells_);
#else
      free(cell_);
#endif
      cells_ = NULL;
    }


  if(cell_data_) 
    {
#ifdef _WIN32
      _aligned_free(cell_data_);
#else
      free(cell_data_);
#endif
      cell_data_ = NULL;
    }

  if(tree_results_) 
    {
#ifdef _WIN32
      _aligned_free(tree_results_);
#else
      free(tree_results_);
#endif
      tree_results_ = NULL;
    }
  if(ray_results_) 
    {
#ifdef _WIN32
      _aligned_free(ray_results_);
#else
      free(ray_results_);
#endif
      ray_results_ = NULL;
    }
  this->free_ray_memory();
}

boxm_ray_trace_manager::~boxm_ray_trace_manager()
{
  if(program_)
    clReleaseProgram(program_);
  this->delete_memory();
}

bool boxm_ray_trace_manager::load_kernel_source(vcl_string const& path)
{
  prog_ = "";
  vcl_ifstream is(path.c_str());
  if(!is.is_open())
    return false;
  char temp[256];
  vcl_ostringstream ostr;
  while(!is.eof()){
    is.getline(temp, 256);
    vcl_string s(temp);
    ostr << s << '\n';
  }
  prog_ =  ostr.str();
  if(!prog_.size()) return false;
  return true;
}

bool boxm_ray_trace_manager::append_process_kernels(vcl_string const& path)
{
  vcl_ifstream is(path.c_str());
  if(!is.is_open())
    return false;
  char temp[256];
  vcl_ostringstream ostr;
  while(!is.eof()){
    is.getline(temp, 256);
    vcl_string s(temp);
    ostr << s << '\n';
  }
  prog_ += ostr.str();
  return true;
}

bool boxm_ray_trace_manager::write_program(vcl_string const& path)
{
  vcl_ofstream os(path.c_str());
  if(!os.is_open())
    return false;
  os << prog_;
  return true;
}

bool boxm_ray_trace_manager::load_tree(vcl_string const& path)
{
  if(tree_)
    delete tree_;
  tree_ =new boct_tree<short, vnl_vector_fixed<float, 2> >();
  vsl_b_ifstream is(path.c_str(), vcl_ios_binary);
  if(!is) return false;
  tree_->b_read(is);
  return true;
}

bool boxm_ray_trace_manager::write_tree(vcl_string const& path)
{
  if(!tree_)
    return false;
  
  vsl_b_ofstream os(path.c_str());
  if(!os) return false;
  tree_->b_write(os);
  os.close();
  return true;
}

  
bool boxm_ray_trace_manager::write_rays(vcl_string const& path)
{
  if(!ray_origin_input_.rows()||!ray_origin_input_.rows()||
     !ray_dir_input_.rows()||!ray_dir_input_.rows())
    return false;
  vsl_b_ofstream os(path.c_str());
  if(!os) return false;
  vsl_b_write(os, ray_origin_input_);
  vsl_b_write(os, ray_dir_input_);
  return true;
}

bool boxm_ray_trace_manager::load_rays(vcl_string const& path)
{
  vsl_b_ifstream is(path.c_str());
  if(!is) return false;
  vsl_b_read(is, ray_origin_input_);
  vsl_b_read(is, ray_dir_input_);
  return true;
}
