#include "boxm_update_bit_scene_manager.h"
//:
// \file
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>
#include <vul/vul_timer.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <vil/vil_save.h>

//: Initializes host side input buffers
//  Put tree structure and data into arrays
bool boxm_update_bit_scene_manager::init_scene(boxm_ocl_bit_scene *scene,
                                               vpgl_camera_double_sptr cam,
                                               vil_image_view<float> &obs,
                                               float prob_thresh=0.3)
{
  if (scene==NULL) {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  scene_ = scene;

  //set scene information
  scene_info_ = new RenderSceneInfo();
  int blkX, blkY, blkZ;
  scene->block_num(blkX, blkY, blkZ);
  int numblocks = blkX*blkY*blkZ;
  vcl_cout<<"Block size "<<(float)numblocks*16/1024.0/1024.0<<"MB"<<vcl_endl;
  scene->tree_buffer_shape(scene_info_->num_buffer, scene_info_->tree_buffer_length);
  scene->data_buffer_shape(scene_info_->num_buffer, scene_info_->data_buffer_length);

  //set block length, epsilon and root level
  double lenX, lenY, lenZ;
  scene->block_dim(lenX,lenY,lenZ);
  scene_info_->block_len = (float) lenX;    // size of each block (can only be 1 number now that we've established blocks are cubes)
  scene_info_->epsilon   = 1.0f/100.0f;
  scene_info_->root_level = scene->max_level() - 1;

  //set origin and scene dimensions
  scene_info_->scene_dims[0]= blkX;
  scene_info_->scene_dims[1]= blkY;
  scene_info_->scene_dims[2]= blkZ;
  scene_info_->scene_dims[3]= 1;
  vgl_point_3d<double> origin = scene->origin();
  scene_info_->scene_origin[0]= (float) origin.x();
  scene_info_->scene_origin[1]= (float) origin.y();
  scene_info_->scene_origin[2]= (float) origin.z();
  scene_info_->scene_origin[3]= (float) 0.0;

  //allocate and initialize 3d blocks
  block_ptrs_ = (cl_ushort*) boxm_ocl_utils::alloc_aligned(numblocks, sizeof(cl_ushort2),16);
  scene->get_block_ptrs(block_ptrs_);

  //1d array of memory pointers
  mem_ptrs_   = (cl_ushort*) boxm_ocl_utils::alloc_aligned(scene_info_->num_buffer, sizeof(cl_ushort2), 16);
  scene->get_mem_ptrs(mem_ptrs_);

  //1d array of number of blocks in each buffer
  blocks_in_buffers_ = (cl_ushort*)  boxm_ocl_utils::alloc_aligned(scene_info_->num_buffer, sizeof(cl_ushort), 16);
  scene->get_blocks_in_buffers(blocks_in_buffers_);

  //tree cells (each are uchar16)
  int numCells = scene_info_->num_buffer * scene_info_->tree_buffer_length;
  cells_ = (cl_uchar*) boxm_ocl_utils::alloc_aligned(numCells , sizeof(cl_uchar16), 16);
  scene->get_tree_cells(cells_);

  //allocate the data items, initialize them from scene
  int numData = scene_info_->num_buffer * scene_info_->data_buffer_length;
  cell_alpha_   = (cl_float *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_float),16);
  cell_mixture_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_uchar8),16);
  cell_num_obs_ = (cl_ushort*) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_ushort4),16);
  cell_aux_data_= (cl_float *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_float4),16);
  scene->get_alphas(cell_alpha_);
  scene->get_mixture(cell_mixture_);
  scene->get_num_obs(cell_num_obs_);
  for (int i=0; i<numData*4; i++)   //init aux data to zero
    cell_aux_data_[i] = 0.0;
    
  //new aux data (2 floats, 2 floats);
  cell_cum_beta_ = (cl_float *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_float2),16);  
  cell_mean_vis_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_uchar2),16);  
  for (int i=0; i<numData*2; i++){   //init aux data to zero
    cell_cum_beta_[i] = 0.0;
    cell_mean_vis_[i] = 0;
  }


  //allocate and initialize bit lookup
  bit_lookup_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(256,sizeof(cl_uchar),16);
  unsigned char bits[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5 ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  for (int i=0; i<256; i++) bit_lookup_[i] = bits[i];

  //initialize output_debug_
  output_debug_ = (cl_float*) boxm_ocl_utils::alloc_aligned(scene_info_->num_buffer, sizeof(cl_float), 16);
  for (int i=0; i<scene_info_->num_buffer; i++) output_debug_[i] = 0;

  //initialize cam, input image and prob_thresh
  cam_ = cam;
  input_img_ = obs;
  wni_=(cl_uint)RoundUp(input_img_.ni(),bni_);
  wnj_=(cl_uint)RoundUp(input_img_.nj(),bnj_);
  prob_thresh_=prob_thresh;
  use_gl_=true;

  /****** size output **********/
  vcl_cout<<"Numbuffer "<<scene_info_->num_buffer
          <<",  Len buffer "<<scene_info_->tree_buffer_length
          <<",   total tree cells "<<numCells
          <<",   total data cells "<<numData<<vcl_endl;
  /****** size output **********/
  point_3d_=(cl_float *) boxm_ocl_utils::alloc_aligned(3,sizeof(cl_float),16);
  this->set_scene_buffers();

  //TODO SET UP CAMERA, IMAGE, OFFSET, ALL OTHER BUFFERS AND STUFF THAT YOU NEED

  return true;
}

bool boxm_update_bit_scene_manager::init_scene(boxm_ocl_bit_scene *scene,
                                               unsigned ni,
                                               unsigned nj,
                                               float prob_thresh=0.3)
{
  if (scene==NULL) {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  scene_ = scene;

  //set scene information
  scene_info_ = new RenderSceneInfo();
  int blkX, blkY, blkZ;
  scene->block_num(blkX, blkY, blkZ);
  int numblocks = blkX*blkY*blkZ;
  vcl_cout<<"Block size "<<(float)numblocks*16/1024.0/1024.0<<"MB"<<vcl_endl;
  scene->tree_buffer_shape(scene_info_->num_buffer, scene_info_->tree_buffer_length);
  scene->data_buffer_shape(scene_info_->num_buffer, scene_info_->data_buffer_length);

  //set block length, epsilon and root level
  double lenX, lenY, lenZ;
  scene->block_dim(lenX,lenY,lenZ);
  scene_info_->block_len = (float) lenX;    // size of each block (can only be 1 number now that we've established blocks are cubes)
  scene_info_->epsilon   = 1.0f/100.0f;
  scene_info_->root_level = scene->max_level() - 1;

  //set origin and scene dimensions
  scene_info_->scene_dims[0]= blkX;
  scene_info_->scene_dims[1]= blkY;
  scene_info_->scene_dims[2]= blkZ;
  scene_info_->scene_dims[3]= 1;
  vgl_point_3d<double> origin = scene->origin();
  scene_info_->scene_origin[0]= (float) origin.x();
  scene_info_->scene_origin[1]= (float) origin.y();
  scene_info_->scene_origin[2]= (float) origin.z();
  scene_info_->scene_origin[3]= (float) 0.0;

  //allocate and initialize 3d blocks
  block_ptrs_ = (cl_ushort*) boxm_ocl_utils::alloc_aligned(numblocks, sizeof(cl_ushort2),16);
  scene->get_block_ptrs(block_ptrs_);

  //1d array of memory pointers
  mem_ptrs_   = (cl_ushort*) boxm_ocl_utils::alloc_aligned(scene_info_->num_buffer, sizeof(cl_ushort2), 16);
  scene->get_mem_ptrs(mem_ptrs_);

  //1d array of number of blocks in each buffer
  blocks_in_buffers_ = (cl_ushort*)  boxm_ocl_utils::alloc_aligned(scene_info_->num_buffer, sizeof(cl_ushort), 16);
  scene->get_blocks_in_buffers(blocks_in_buffers_);

  //tree cells (each are uchar16)
  int numCells = scene_info_->num_buffer * scene_info_->tree_buffer_length;
  cells_ = (cl_uchar*) boxm_ocl_utils::alloc_aligned(numCells , sizeof(cl_uchar16), 16);
  scene->get_tree_cells(cells_);

  //allocate the data items, initialize them from scene
  int numData = scene_info_->num_buffer * scene_info_->data_buffer_length;
  cell_alpha_   = (cl_float *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_float),16);
  cell_mixture_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_uchar8),16);
  cell_num_obs_ = (cl_ushort*) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_ushort4),16);
  cell_aux_data_= (cl_float *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_float4),16);
  scene->get_alphas(cell_alpha_);
  scene->get_mixture(cell_mixture_);
  scene->get_num_obs(cell_num_obs_);
  for (int i=0; i<numData*4; i++)   //init aux data to zero
    cell_aux_data_[i] = 0.0;

  //new aux data (2 floats, 2 floats);
  cell_cum_beta_ = (cl_float *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_float2),16);  
  cell_mean_vis_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(numData,sizeof(cl_uchar2),16);  
  for (int i=0; i<numData*2; i++){   //init aux data to zero
    cell_cum_beta_[i] = 0.0;
    cell_mean_vis_[i] = 0;
  }

  //allocate and initialize bit lookup
  bit_lookup_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(256,sizeof(cl_uchar),16);
  unsigned char bits[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5 ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  for (int i=0; i<256; i++) bit_lookup_[i] = bits[i];

  //initialize output_debug_
  output_debug_ = (cl_float*) boxm_ocl_utils::alloc_aligned(scene_info_->num_buffer, sizeof(cl_float), 16);
  for (int i=0; i<scene_info_->num_buffer; i++) output_debug_[i] = 0;

  //initialize cam, input image and prob_thresh
  cam_ = new vpgl_perspective_camera<double>();
  input_img_.set_size(ni,nj);
  input_img_.fill(0.0f);
  wni_=(cl_uint)RoundUp((int)ni,bni_);
  wnj_=(cl_uint)RoundUp((int)nj,bnj_);
  vcl_cout<<"DIMS"<<wni_<<" "<<wnj_<<vcl_endl;

  prob_thresh_=prob_thresh;

  use_gl_=false;
  /****** size output **********/
  vcl_cout<<"Numbuffer "<<scene_info_->num_buffer
          <<",  Len buffer "<<scene_info_->tree_buffer_length
          <<",   total tree cells "<<numCells
          <<",   total data cells "<<numData<<vcl_endl;
  /****** size output **********/
  point_3d_=(cl_float *) boxm_ocl_utils::alloc_aligned(3,sizeof(cl_float),16);
  this->set_scene_buffers();

  //TODO SET UP CAMERA, IMAGE, OFFSET, ALL OTHER BUFFERS AND STUFF THAT YOU NEED

  return true;
}


bool boxm_update_bit_scene_manager::uninit_scene()
{
  boxm_ocl_utils::free_aligned(block_ptrs_);
  boxm_ocl_utils::free_aligned(mem_ptrs_);
  boxm_ocl_utils::free_aligned(cells_);
  boxm_ocl_utils::free_aligned(cell_alpha_);
  boxm_ocl_utils::free_aligned(cell_mixture_);
  boxm_ocl_utils::free_aligned(cell_num_obs_);
  boxm_ocl_utils::free_aligned(cell_aux_data_);
  boxm_ocl_utils::free_aligned(bit_lookup_);
  boxm_ocl_utils::free_aligned(output_debug_);
  if (scene_info_) delete scene_info_;
  this->clean_scene_buffers();

  //TODO CLEANUP CAMERA, IMAGE, OFFSET, ALL OTHER BUFFERS AND STUFF THAT YOU NEED

  return true;
}

//INIT_UPDATE allocates and initializes host side buffers,
//this is called after to allocate and initialize gpu side buffers
bool boxm_update_bit_scene_manager::set_scene_buffers()
{
  cl_int status=0;
  //create the scene info buffer
  scene_info_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(RenderSceneInfo),
                                   scene_info_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (scene info) failed."))
    return false;

  //memory pointers for each tree buffer
  mem_ptrs_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                 scene_info_->num_buffer*sizeof(cl_ushort2),
                                 mem_ptrs_,
                                 &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (mem_ptrs_) failed."))
    return false;

  //blocksi n buffers buff
  blocks_in_buffers_buf_ = clCreateBuffer(this->context_,
                                          CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                          scene_info_->num_buffer*sizeof(cl_ushort),
                                          blocks_in_buffers_,
                                          &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (mem_ptrs_) failed."))
    return false;

  //block pointers
  int numBlks = scene_info_->scene_dims[0] * scene_info_->scene_dims[1] * scene_info_->scene_dims[2];
  block_ptrs_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                   numBlks * sizeof(cl_ushort2),
                                   block_ptrs_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (block_ptrs) failed."))
    return false;

  //trees
  int numCells = scene_info_->tree_buffer_length * scene_info_->num_buffer;
  cells_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                              numCells * sizeof(cl_uchar16),
                              cells_,
                              &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cells) failed."))
    return false;

  //data (alpha)
  int datCells = scene_info_->data_buffer_length * scene_info_->num_buffer;
  cell_alpha_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                   datCells * sizeof(cl_float),
                                   cell_alpha_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (alpha_cells) failed."))
    return false;

  //data (mixture)
  cell_mixture_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                     datCells * sizeof(cl_uchar8),
                                     cell_mixture_,
                                     &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_mixture) failed."))
    return false;

  //data (num_obs)
  cell_num_obs_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                     datCells * sizeof(cl_ushort4),
                                     cell_num_obs_,
                                     &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_num_obs) failed."))
    return false;

  //data (aux_data)
  cell_aux_data_buf_ = clCreateBuffer(this->context_,
                                      CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                      datCells * sizeof(cl_float4),
                                      cell_aux_data_,
                                      &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_aux_data) failed."))
    return false;

  //data (cum_length, beta)
  cell_cum_beta_buf_ = clCreateBuffer(this->context_,
                                      CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                      datCells * sizeof(cl_float2),
                                      cell_cum_beta_,
                                      &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_cum_beta_buf) failed."))
    return false;
  
  //data (mean_obs/cum_vis)
  cell_mean_vis_buf_ = clCreateBuffer(this->context_,
                                      CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                      datCells * sizeof(cl_uchar2),
                                      cell_mean_vis_,
                                      &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_mean_vis_Buf) failed."))
    return false;


  //bit lookup buf to go to constant mem
  bit_lookup_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   256*sizeof(cl_uchar),
                                   bit_lookup_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (bit_lookup) failed."))
    return false;
  //Output debugger
  point_3d_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_float)*3,
                                 point_3d_,
                                 &status);
  if (! this->check_val(status, CL_SUCCESS, "clCreateBuffer (point_3d_buf_) failed."))
    return false;

  //Output debugger
  output_debug_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                     sizeof(cl_float)*scene_info_->num_buffer,
                                     output_debug_,
                                     &status);
  if (! this->check_val(status, CL_SUCCESS, "clCreateBuffer (output_debug_buf) failed."))
    return false;

  /** GPU MEMORY INFO **********************/
  float cellMB    = (float)numCells*(float)sizeof(cl_uchar16)/1024.0f/1024.0f;
  float alphaMB   = (float)datCells*(float)sizeof(cl_float  )/1024.0f/1024.0f;
  float mixtureMB = (float)datCells*(float)sizeof(cl_uchar8 )/1024.0f/1024.0f;
  float numobsMB  = (float)datCells*(float)sizeof(cl_ushort4)/1024.0f/1024.0f;
  //float auxMB     = (float)datCells*(float)sizeof(cl_float4 )/1024.0f/1024.0f;
  float auxMB     = (float)datCells*(float)(sizeof(cl_float2)+sizeof(cl_uchar2))/1024.0f/1024.0f;
  float total = cellMB + alphaMB + mixtureMB + numobsMB + auxMB;
  vcl_cout<<"GPU Mem Allocated:\n"
          <<"    Cells    "<<cellMB<<" MB\n"
          <<"    Alpha    "<<alphaMB<<" MB\n"
          <<"    Mixture  "<<mixtureMB<<" MB\n"
          <<"    Num Obs  "<<numobsMB<<" MB\n"
          <<"    Aux Data "<<auxMB<<" MB\n"
          <<"    TOTAL:   "<<total<<" MB"<<vcl_endl;
  /****************************************/
  //all good - return true
  return true;
}

bool boxm_update_bit_scene_manager::clean_scene_buffers()
{
  cl_int status;
  status = clReleaseMemObject(scene_info_buf_);
  bool good = this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_info_buf_).");

  status = clReleaseMemObject(mem_ptrs_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (mem_ptrs_buf_).");

  status = clReleaseMemObject(block_ptrs_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (block_ptrs_buf_).");

  status = clReleaseMemObject(cells_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cells_buf_).");

  status = clReleaseMemObject(cell_alpha_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_alpha_buf_).");

  status = clReleaseMemObject(cell_mixture_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_mixture_buf_).");

  status = clReleaseMemObject(cell_aux_data_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_aux_data_buf_).");

  status = clReleaseMemObject(bit_lookup_buf_);
    good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (bit_lookup).");

  status = clReleaseMemObject(output_debug_buf_);
  good = good && this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (output_debug_buf_).");

  return good;
}


//: Does the non-buffer setup (sets kernels, sets kernel args, sets commandqueue, sets workspace)
bool boxm_update_bit_scene_manager::setup_online_processing()
{
  bool good=true;
  vcl_string error_message="";
  good = set_offset_buffers(0,0,2);
  // set persp cam and input_image host buffers
  good = good && set_persp_camera();
  good = good && set_input_image();
  good = good && set_rayoutput();
  // set persp camand input_image gpu buffers
  good = good && set_persp_camera_buffers()
              && set_input_image_buffers()
              && set_image_dims_buffers();
  good = good && set_rayoutput_buffers();
  //now that all buffers are set, compile kernels, queue, and set args
  good = good && this->set_kernels()
              && this->set_commandqueue();
  return good;
}

//: Need to figure out what this entails
bool boxm_update_bit_scene_manager::finish_online_processing()
{
  bool good=true;
  this->release_kernels();

  this->read_scene();
  scene_->set_tree_buffers(cells_);
  scene_->set_alpha_values(cell_alpha_);
  scene_->set_mixture_values(cell_mixture_);
  scene_->set_num_obs_values(cell_num_obs_);

  
  this->release_rayoutput_buffers();
  this->clean_rayoutput();
  //NEED TO:
  //- release offset buffers
  //- release persp cam, input image, image_dims (and buffers)
  // release kernel
  // release command queue
  return good;
}

//: create the three sets of kernels used by this manager
bool boxm_update_bit_scene_manager::set_kernels()
{
  vcl_cout<<"Setting kernels "<<vcl_endl;
  cl_int status = CL_SUCCESS;
  const int CHECK_SUCCESS = 1;
  if (!this->release_kernels())
    return false;

  // pass 0 (seg lens)
  //if (!this->build_update_program("seg_len_obs(d,image_vect,ray_bundle_array,cached_aux_data)", false))
  if (!this->build_update_program("SEGLEN", false))
    return false;
  cl_kernel kernel = clCreateKernel(program_,"seg_len_main",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  update_kernels_.push_back(kernel);
  
  // pass 1 (calc_pi)
  kernel = clCreateKernel(program_,"calc_pi",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  update_kernels_.push_back(kernel);
      
    vcl_cout<<"Set set calc_pi kernel "<<vcl_endl;


  // pass 2
  //if (!this->build_update_program("pre_infinity(d,image_vect,ray_bundle_array, cached_data, cached_aux_data)", true))
  if (!this->build_update_program("PREINF", true))
    return false;
  kernel = clCreateKernel(program_,"pre_inf_main",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  update_kernels_.push_back(kernel);
  
    vcl_cout<<"Set preinf kernel "<<vcl_endl;


  //pass 3 norm image (pre-requisite to C++ pass 2)
  kernel = clCreateKernel(program_,"proc_norm_image",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  update_kernels_.push_back(kernel);
  
      vcl_cout<<"Set proc norm kernel "<<vcl_endl;


  // pass 4 Bayes ratio (pass 2 in C++)
  //if (!this->build_update_program("bayes_ratio(d,image_vect,ray_bundle_array, cached_data, cached_aux_data)", true))
  if (!this->build_update_program("BAYES", true))
    return false;
  kernel = clCreateKernel(program_,"bayes_main",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  update_kernels_.push_back(kernel);
  
      vcl_cout<<"Set bayes kernel "<<vcl_endl;


  // pass 5 update_ocl_scene_main
  kernel = clCreateKernel(program_,"update_bit_scene_main",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  update_kernels_.push_back(kernel);
  
  vcl_cout<<"Set all four update kernels"<<vcl_endl;

  //create and render kernel
  if (!this->build_rendering_program()) {
    vcl_cout<<"rendering program failed to build"<<vcl_endl;
    return false;
  }
  render_kernel_ = clCreateKernel(program_,"ray_trace_bit_scene_opt",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS) {
    vcl_cout<<"render kernel failed to build"<<vcl_endl;
    return false;
  }

  //create and set refining kernel
  if (!this->build_refining_program()) {
    vcl_cout<<"refine program failed to build"<<vcl_endl;
    return false;
  }
  refine_kernel_ = clCreateKernel(program_,"refine_bit_scene", &status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS) {
    vcl_cout<<"REfine failed to build"<<vcl_endl;
    return false;
  }
  if (!this->build_query_point_program()) {
      vcl_cout<<"refine program failed to build"<<vcl_endl;
      return false;
  }
  query_point_kernel_ = clCreateKernel(program_,"query_bit_scene",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS) {
    vcl_cout<<"Query kernel failed to build"<<vcl_endl;
    return false;
  }
  if (!this->build_ray_probe_program()) {
      vcl_cout<<"refine program failed to build"<<vcl_endl;
      return false;
  }
  ray_probe_kernel_ = clCreateKernel(program_,"ray_probe",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS) {
    vcl_cout<<"ray_probe kernel failed to build"<<vcl_endl;
    return false;
  }
  if (!this->build_change_detection_program()) {
      vcl_cout<<"build_change_detection program failed to build"<<vcl_endl;
      return false;
  }

  change_detection_kernel_ = clCreateKernel(program_,"change_detecttion_scene",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS) {
    vcl_cout<<"change_detecttion_scene kernel failed to build"<<vcl_endl;
    return false;
  }
  if (!this->build_clean_aux_data_program()) {
      vcl_cout<<"build_clean_aux_data_program  failed to build"<<vcl_endl;
      return false;
  }

  clean_aux_data_kernel_ = clCreateKernel(program_,"clean_aux_data",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS) {
    vcl_cout<<"clean_aux_data kernel failed to build"<<vcl_endl;
    return false;
  }


  return true;
}

bool boxm_update_bit_scene_manager::build_rendering_program()
{
  //append render source code
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/";
  if (!this->load_kernel_source(source_dir+"scene_info.cl") ||
      !this->append_process_kernels(source_dir+"cell_utils.cl") ||
      !this->append_process_kernels(source_dir+"bit_tree_library_functions.cl") ||
      !this->append_process_kernels(source_dir+"backproject.cl")||
      !this->append_process_kernels(source_dir+"statistics_library_functions.cl")||
      !this->append_process_kernels(source_dir+"expected_functor.cl")||
      !this->append_process_kernels(source_dir+"ray_bundle_library_opt.cl")||
      !this->append_process_kernels(source_dir+"cast_ray_bit.cl") ||
      !this->append_process_kernels(source_dir+"render_bit_scene.cl")) {
    vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }

  //replace step_cell functor with the correct one... opt
  // transfer cell data from global to local memory if use_cell_data_ == true
  vcl_cout<<"Using functor step_cell_render_opt"<<vcl_endl;
  vcl_string patt = "/*$$step_cell$$*/";
  vcl_string functor = "step_cell_render_opt(mixture_array,alpha_array,data_ptr,d,&data_return);";

  //compilation options
  vcl_string options="";
  options+="-D INTENSITY ";
  if (vcl_strstr(this->platform_name,"ATI"))
    options+="-D ATI ";
  if (vcl_strstr(this->platform_name,"NVIDIA"))
    options+="-D NVIDIA ";
  options += "-D RENDER ";
  //options += " -cl-fast-relaxed-math ";

  // assign the functor calling signature
  vcl_string::size_type pos_start = this->prog_.find(patt);
  vcl_string::size_type n1 = patt.size();
  if (pos_start < this->prog_.size()) {
    vcl_string::size_type n2 = functor.size();
    if (!n2)
      return false;
    this->prog_ = this->prog_.replace(pos_start, n1, functor.c_str(), n2);
    return this->build_kernel_program(program_, options)==SDK_SUCCESS;
  }

  return this->build_kernel_program(program_, options)==SDK_SUCCESS;
}

bool boxm_update_bit_scene_manager::build_change_detection_program()
{
  //append render source code
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/";
  if (!this->load_kernel_source(source_dir+"scene_info.cl") ||
      !this->append_process_kernels(source_dir+"cell_utils.cl") ||
      !this->append_process_kernels(source_dir+"bit_tree_library_functions.cl") ||
      !this->append_process_kernels(source_dir+"backproject.cl")||
      !this->append_process_kernels(source_dir+"statistics_library_functions.cl")||
      !this->append_process_kernels(source_dir+"expected_functor.cl")||
      !this->append_process_kernels(source_dir+"ray_bundle_library_opt.cl")||
      !this->append_process_kernels(source_dir+"cast_ray_bit.cl") ||
      !this->append_process_kernels(source_dir+"change_detection.cl")) {
    vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }

  //replace step_cell functor with the correct one... opt
  // transfer cell data from global to local memory if use_cell_data_ == true
  vcl_cout<<"Using functor step_cell_render_opt"<<vcl_endl;
  vcl_string patt = "/*$$step_cell$$*/";
  vcl_string functor = "step_cell_render_opt(mixture_array,alpha_array,data_ptr,d,&data_return);";

  //compilation options
  vcl_string options="";
  if (vcl_strstr(this->platform_name,"ATI"))
    options+="-D ATI ";
  if (vcl_strstr(this->platform_name,"NVIDIA"))
    options+="-D NVIDIA ";
  options += "-D CHANGE ";
  //options += " -cl-fast-relaxed-math ";

  // assign the functor calling signature
  vcl_string::size_type pos_start = this->prog_.find(patt);
  vcl_string::size_type n1 = patt.size();
  if (pos_start < this->prog_.size()) {
    vcl_string::size_type n2 = functor.size();
    if (!n2)
      return false;
    this->prog_ = this->prog_.replace(pos_start, n1, functor.c_str(), n2);
    return this->build_kernel_program(program_, options)==SDK_SUCCESS;
  }

  return this->build_kernel_program(program_, options)==SDK_SUCCESS;
}


bool boxm_update_bit_scene_manager::build_refining_program()
{
    vcl_string root = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/" ;
    bool info = this->load_kernel_source(root + "scene_info.cl");
    bool bitr = this->append_process_kernels(root + "bit_tree_library_functions.cl");
    bool refn = this->append_process_kernels(root + "refine_bit_scene.cl");

    if (!info || !bitr || !refn) {
      vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (refine_bit_scene functions)\n";
      return false;
    }
    
    vcl_string options = "";
    if(scene_info_->root_level == 1) {
      options += "-D MAXINNER=1 ";
      options += "-D MAXCELLS=9 ";
    } else if(scene_info_->root_level == 2) {
      options += "-D MAXINNER=9 ";
      options += "-D MAXCELLS=73 ";
    } else if(scene_info_->root_level == 3) {
      options += "-D MAXINNER=73 ";
      options += "-D MAXCELLS=585 ";
    } else {
      vcl_cout<<"build_refining_program::root level is not 1, 2 or 3, invalid scene info"<<vcl_endl;
      return false;
    }
    
    return this->build_kernel_program(program_,options)==SDK_SUCCESS;
}

bool boxm_update_bit_scene_manager::build_query_point_program()
{
    vcl_string root = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/" ;
    bool info = this->load_kernel_source(root + "scene_info.cl");
    bool bitr = this->append_process_kernels(root + "bit_tree_library_functions.cl");
    bool refn = this->append_process_kernels(root + "query_bit_scene.cl");

    if (!info || !bitr || !refn) {
      vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (build_query_point_program functions)\n";
      return false;
    }
    return this->build_kernel_program(program_,"")==SDK_SUCCESS;
}


bool boxm_update_bit_scene_manager::build_ray_probe_program()
{
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/";
  if (!this->load_kernel_source(source_dir+"scene_info.cl") ||
      !this->append_process_kernels(source_dir+"cell_utils.cl") ||
      !this->append_process_kernels(source_dir+"bit_tree_library_functions.cl") ||
      !this->append_process_kernels(source_dir+"backproject.cl")||
      !this->append_process_kernels(source_dir+"statistics_library_functions.cl")||
      !this->append_process_kernels(source_dir+"expected_functor.cl")||
      !this->append_process_kernels(source_dir+"ray_bundle_library_functions.cl")||
      !this->append_process_kernels(source_dir+"ray_probe.cl")) {
    vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }
  return this->build_kernel_program(program_, "")==SDK_SUCCESS;

}

bool boxm_update_bit_scene_manager::build_clean_aux_data_program()
{
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/";
  if (!this->load_kernel_source(source_dir+"scene_info.cl")||
      !this->append_process_kernels(source_dir + "clean_aux_data.cl")) {
    vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }
  return this->build_kernel_program(program_, "")==SDK_SUCCESS;

}

bool boxm_update_bit_scene_manager::build_update_program(vcl_string const& functor, bool use_cell_data)
{
  vcl_string root = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm/ocl/cl/";
  bool locc = this->load_kernel_source(    root + "scene_info.cl"  );
  bool cell = this->append_process_kernels(root + "cell_utils.cl");
  bool octr = this->append_process_kernels(root + "bit_tree_library_functions.cl");
  bool bpr  = this->append_process_kernels(root + "backproject.cl");
  bool stat = this->append_process_kernels(root + "statistics_library_functions.cl");
  bool ropt = this->append_process_kernels(root + "ray_bundle_library_opt.cl");
  bool rayc = this->append_process_kernels(root + "cast_ray_bit.cl");
  bool main = this->append_process_kernels(root + "update_kernels.cl");

  if (!octr||!bpr||!stat||!main||!locc||!cell||!rayc||!ropt) {
    vcl_cerr << "Error: boxm_update_bit_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }
  
  //compilation options
  vcl_string options="";
  options+="-D INTENSITY ";
  if (vcl_strstr(this->platform_name,"ATI"))
    options+="-D ATI ";
  if (vcl_strstr(this->platform_name,"NVIDIA"))
    options+="-D NVIDIA ";
  
  //choose update functor
  if(vcl_strstr(functor.c_str(), "SEGLEN"))
    options += "-D SEGLEN";
  else if(vcl_strstr(functor.c_str(), "PREINF"))
    options += "-D PREINF";
  else if(vcl_strstr(functor.c_str(), "BAYES"))
    options += "-D BAYES";

  //vcl_string patt = "$$step_cell$$", empty = "", zero = "0", one = "1";
  //// transfer cell data from global to local memory if use_cell_data_ == true
  //vcl_string use = "%%";
  //vcl_string::size_type use_start = this->prog_.find(use);
  //if (use_start < this->prog_.size()) {
    //if (use_cell_data)
      //this->prog_ = this->prog_.replace (use_start, 2, one.c_str(), 1);
    //else
      //this->prog_ = this->prog_.replace (use_start, 2, zero.c_str(), 1);
  //}
  //else
    //return false;
    
  // assign the functor calling signature
  //vcl_string::size_type pos_start = this->prog_.find(patt);
  //vcl_string::size_type n1 = patt.size();
  //if (pos_start < this->prog_.size()) {
    //vcl_string::size_type n2 = functor.size();
    //if (!n2)
      //return false;
    //this->prog_ = this->prog_.replace(pos_start, n1, functor.c_str(), n2);
    return this->build_kernel_program(program_,options)==SDK_SUCCESS;
  //}
  return false;
}

bool boxm_update_bit_scene_manager::set_query_point_args()
{
  cl_int status = CL_SUCCESS;
  int i=0;
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return 0;
    //block pointers
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;
    // the tree buffer
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return 0;
    // alpha buffer
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer
    status = clSetKernelArg(query_point_kernel_, i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return 0;
    //cell num obs buffer
    status = clSetKernelArg(query_point_kernel_, i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return 0;
    //cell aux data buffer
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_cum_beta_)"))
      return 0;
    //cell mean_vis 
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return 0;
    //bit lookup buffer
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return 0;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(query_point_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return 0;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&point_3d_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Input 3d query point )"))
      return false;

    //output float buffer (one float for each buffer)
    status = clSetKernelArg(query_point_kernel_,i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;

  return true;
}


bool boxm_update_bit_scene_manager::set_render_args()
{
  const int CHECK_SUCCESS = 1;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&scene_info_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
    return 0;
  //block pointers
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
    return 0;
  // the tree buffer
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
    return 0;
  // alpha buffer
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
    return 0;
  //mixture buffer
  status = clSetKernelArg(render_kernel_, i++,sizeof(cl_mem),(void *)&cell_mixture_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
    return 0;
  // camera buffer
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
    return 0;
  // roi dimensions
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&img_dims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
    return 0;
  //local copy of the tree (one for each thread/ray)
  status = clSetKernelArg(render_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
    return 0;
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
    return 0;
  if (!use_gl_)
  {
  image_gl_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                 wni_*wnj_*sizeof(cl_uint),
                                 image_gl_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (gl_image)"))
      return 0;
  }
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&image_gl_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (gl_image)"))
    return 0;
  //bit lookup buffer
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
    return 0;
  //cum sum lookup buffer
  status = clSetKernelArg(render_kernel_,i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    return 0;
  //imIndex buffer
  status = clSetKernelArg(render_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_int), 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    return 0;
  //output float buffer (one float for each buffer)
  status = clSetKernelArg(render_kernel_,i++,sizeof(cl_mem),(void *)&output_debug_buf_);
  return this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)")==CHECK_SUCCESS;
}
bool boxm_update_bit_scene_manager::set_change_detection_args()
{
  const int CHECK_SUCCESS = 1;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&scene_info_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
    return 0;
  //block pointers
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
    return 0;
  // the tree buffer
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
    return 0;
  //// alpha buffer
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
    return 0;
  //mixture buffer
  status = clSetKernelArg(change_detection_kernel_, i++,sizeof(cl_mem),(void *)&cell_mixture_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
    return 0;
   //camera buffer
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
    return 0;
  // roi dimensions
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&img_dims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
    return 0;
  //local copy of the tree (one for each thread/ray)
  status = clSetKernelArg(change_detection_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
    return 0;
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
    return 0;
  //bit lookup buffer
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
    return 0;
  //cum sum lookup buffer
  status = clSetKernelArg(change_detection_kernel_,i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    return 0;
  //imIndex buffer
  status = clSetKernelArg(change_detection_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_int), 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    return 0;
  //output float buffer (one float for each buffer)
  status = clSetKernelArg(change_detection_kernel_,i++,sizeof(cl_mem),(void *)&output_debug_buf_);
  return this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)")==CHECK_SUCCESS;


  return true;
}

bool boxm_update_bit_scene_manager::set_ray_probe_args(int pi, int pj, float intensity)
{
  int CHECK_SUCCESS = 1;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&scene_info_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
    return 0;
  //block pointers
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
    return 0;
  // the tree buffer
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
    return 0;
  // alpha buffer
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
    return 0;
  //mixture buffer
  status = clSetKernelArg(ray_probe_kernel_, i++,sizeof(cl_mem),(void *)&cell_mixture_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
    return 0;
  // camera buffer
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
    return 0;
  //local copy of the tree (one for each thread/ray)
  status = clSetKernelArg(ray_probe_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
    return 0;
  //bit lookup buffer
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
    return 0;
  //cum sum lookup buffer
  status = clSetKernelArg(ray_probe_kernel_,i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    return 0;
  //imIndex buffer
  status = clSetKernelArg(ray_probe_kernel_,i++,this->bni_*this->bnj_*sizeof(cl_int), 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    return 0;
  status = clSetKernelArg(ray_probe_kernel_, i++, sizeof(cl_int),&pi);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (i) buffer)"))
      return SDK_FAILURE;
  status = clSetKernelArg(ray_probe_kernel_, i++, sizeof(cl_int),&pj);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (j) buffer)"))
      return SDK_FAILURE;
  status = clSetKernelArg(ray_probe_kernel_, i++, sizeof(cl_float),&intensity);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (intensity) buffer)"))
      return SDK_FAILURE;

  for(unsigned cnt=0;cnt<10;cnt++)
  {
      status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&rayoutput_buf_[cnt]);
      if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
          return 0;
  }
  //output float buffer (one float for each buffer)
  status = clSetKernelArg(ray_probe_kernel_,i++,sizeof(cl_mem),(void *)&output_debug_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)")!=CHECK_SUCCESS)
    return false;
    
  return true;
}
bool boxm_update_bit_scene_manager::set_refine_args()
{
  //SET REFINE ARGS-------------------------------------------------------------
  const int CHECK_SUCCESS = 1;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem), (void *)&scene_info_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
    return 0;
  //mem_ptrs
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem), (void *)&mem_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (mem pointers scene info)"))
    return 0;
  //blocks in buffers
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem), (void *)&blocks_in_buffers_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (mem pointers scene info)"))
    return 0;
  // the tree buffer
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem), (void *)&cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
    return 0;
  // alpha buffer
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
    return 0;
  //mixture buffer
  status = clSetKernelArg(refine_kernel_, i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
    return 0;
  //cell num obs buffer
  status = clSetKernelArg(refine_kernel_, i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
  if (this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)")!=CHECK_SUCCESS)
    return 0;
  //cell aux data buffer
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
    return 0;
  //bit lookup buffer
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
    return 0;
  ////cum sum lookup buffer
  //status = clSetKernelArg(refine_kernel_,i++,10*64*sizeof(cl_uchar), 0);
  //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
    //return 0;
  //local copy of the tree (old copy)
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_uchar16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
    return 0;
  //local copy of the tree (refined copy)
  status = clSetKernelArg(refine_kernel_,i++,sizeof(cl_uchar16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (refined local tree)"))
    return 0;
  //prob thresh for refine
  status = clSetKernelArg(refine_kernel_, i++, sizeof(cl_float),  &prob_thresh_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (prob_thresh buffer)"))
    return 0;
  //output float buffer (one float for each buffer)
  status = clSetKernelArg(refine_kernel_, i++,sizeof(cl_mem),(void *)&output_debug_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)")!=CHECK_SUCCESS)
    return false;
  //END REFINE ARGS-------------------------------------------------------------

  return true;
}

bool boxm_update_bit_scene_manager::set_update_args(unsigned pass)
{
  cl_int status = CL_SUCCESS;
  int i=0;
  
  //set raytrace update args -------------------------------------------------
  if (pass==0) {
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return 0;
    //block pointers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;
    // the tree buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return 0;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return 0;
    //cell num obs buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return 0;
    //cell aux data buffer
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      //return 0;
    //cell cum_beta
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_cum_beta_)"))
      return 0;
    //cell mean_vis 
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return 0;
      
    //bit lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return 0;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return 0;
    // camera buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return 0;
    // roi dimensions
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return 0;
    // input image
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return 0;
    // offset factor
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&factor_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)"))
      return false;
    // offset x and y
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_x_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_y_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)"))
      return false;
    // ray bundle array
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_uchar4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cache ptr bundle)"))
      return false;
    // cell pointers (cached)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_int)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cell pointers)"))
      return false;
    // cached aux data
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cached aux data)"))
      return false;
    // local image_vect
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image_vect)"))
      return false;
    //cum sum lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
      return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  if (pass==1) {
    //scene info
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return 0;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return 0;
    //cell num obs buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return 0;
    //cell aux data buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  if (pass==2) {
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return 0;
    //block pointers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;
    // the tree buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return 0;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return 0;
    //cell num obs buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return 0;
    //cell aux data buffer
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      //return 0;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_cum_beta_)"))
      return 0;
    //cell mean_vis 
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return 0;
      
    //bit lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return 0;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return 0;
    // camera buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return 0;
    // roi dimensions
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return 0;
    // input image
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return 0;
    // offset factor
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&factor_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)"))
      return false;
    // offset x and y
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_x_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_y_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)"))
      return false;
    // cached aux data
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cached aux data)"))
      return false;
    // local image_vect
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image_vect)"))
      return false;
    //cum sum lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
      return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  if (pass==4) {
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return 0;
    //block pointers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;
    // the tree buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return 0;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return 0;
    //cell num obs buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return 0;
    //cell aux data buffer
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      //return 0;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_cum_beta_)"))
      return 0;
    //cell mean_vis 
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return 0;
      

    //bit lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return 0;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return 0;
    // camera buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return 0;
    // roi dimensions
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return 0;
    // input image
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return 0;
    // offset factor
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&factor_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)"))
      return false;
    // offset x and y
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_x_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_y_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)"))
      return false;
    // ray bundle array
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_uchar4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cache ptr bundle)"))
      return false;
    // cell pointers (cached)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_int)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cell pointers)"))
      return false;
    // cached aux data
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cached aux data)"))
      return false;
    // local image_vect
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image_vect)"))
      return false;
    //cum sum lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
      return false;
    //imIndex buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_int), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (imindex buff)"))
      return 0;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  //END ray trace update pass args -------------------------------------------------

  //set pass 4 args ------------------------------------------------------------
  if (pass == 5) {
    i=0;
    //scene info
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem), (void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return false;
    //alpha
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_alpha_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_alpha_buf_)"))
      return false;
    //mixture
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_mixture_buf_)"))
      return false;
    //num obs
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return false;
    // aux data
    //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_aux_data_buf_);
    //if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_aux_data_buf_)"))
      //return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_cum_beta_)"))
      return 0;
    //cell mean_vis 
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return 0;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
    return true;
  }
  //end pass 4 args ------------------------------------------------------------

  //set pass 2 args norm image process------------------------------------------
  if (pass == 3) {
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&image_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (image array)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&app_density_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (remote surface appearance)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&img_dims_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (image dimensions)"))
      return false;
  }
  //END pass 2 args ------------------------------------------------------------

  return true;
}


bool boxm_update_bit_scene_manager::set_clean_aux_data_args()
{
    cl_int status = CL_SUCCESS;
    int i=0;

    //set raytrace update args -------------------------------------------------
    status = clSetKernelArg(clean_aux_data_kernel_,i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
        return 0;
    //cell cum_beta
    status = clSetKernelArg(clean_aux_data_kernel_,i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_cum_beta_)"))
        return 0;
    //cell mean_vis 
    status = clSetKernelArg(clean_aux_data_kernel_,i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
        return 0;
    return true;
}



bool boxm_update_bit_scene_manager::release_kernels()
{
  cl_int status = CL_SUCCESS;
  const int CHECK_SUCCESS = 1;
  if (query_point_kernel_) {
    status = clReleaseKernel(query_point_kernel_);
    if (this->check_val(status,CL_SUCCESS,"clReleaseKernel (query_point_kernel_) failed.")!=CHECK_SUCCESS)
      return false;
  }

  //release render
  if (render_kernel_) {
    status = clReleaseKernel(render_kernel_);
    if (this->check_val(status,CL_SUCCESS,"clReleaseKernel (render) failed.")!=CHECK_SUCCESS)
      return false;
  }

  //release refine
  if (refine_kernel_) {
    status = clReleaseKernel(refine_kernel_);
    if (this->check_val(status,CL_SUCCESS,"clReleaseKernel (refine) failed.")!=CHECK_SUCCESS)
      return false;
  }

  //release update kernels
  for (unsigned i = 0; i<update_kernels_.size(); ++i) {
    if (update_kernels_[i]) {
      status = clReleaseKernel(update_kernels_[i]);
      if (this->check_val(status,CL_SUCCESS,"clReleaseKernel (update) failed.")!=CHECK_SUCCESS)
        return false;
    }
  }
  update_kernels_.clear();

  if(ray_probe_kernel_){
    status = clReleaseKernel(ray_probe_kernel_);
    if (this->check_val(status,CL_SUCCESS,"clReleaseKernel (ray_probe_kernel_) failed.")!=CHECK_SUCCESS)
      return false;

  }
  if(clean_aux_data_kernel_){
      status = clReleaseKernel(clean_aux_data_kernel_);
      if (this->check_val(status,CL_SUCCESS,"clReleaseKernel (clean_aux_data_kernel_) failed.")!=CHECK_SUCCESS)
          return false;

  }

  return true;
}

bool boxm_update_bit_scene_manager::setup_norm_data(bool use_uniform,float mean,float sigma)
{
  return this->setup_app_density(use_uniform, mean, sigma)
      && this->setup_app_density_buffer()==SDK_SUCCESS;
}

bool boxm_update_bit_scene_manager::clean_norm_data()
{
  return this->clean_app_density()
      && this->clean_app_density_buffer()==SDK_SUCCESS;
}

bool boxm_update_bit_scene_manager::setup_app_density(bool use_uniform, float mean, float sigma)
{
#if defined (_WIN32)
  app_density_ =  (cl_float*)_aligned_malloc( sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  app_density_ =  (cl_float*)malloc( sizeof(cl_float4));
#else
  app_density_ =  (cl_float*)memalign(16, sizeof(cl_float4));
#endif
  if (use_uniform) {
    app_density_[0]=1.0f;
    app_density_[1]=0.0f;
    app_density_[2]=0.0f;
    app_density_[3]=0.0f;
  }
  else {
    app_density_[0]=0.0f;
    app_density_[1]=mean;
    app_density_[2]=sigma;
    app_density_[3]=0.0f;
  }
  return true;
}


bool boxm_update_bit_scene_manager::clean_app_density()
{
  if (app_density_) {
#ifdef _WIN32
    _aligned_free(app_density_);
#else
    free(app_density_);
#endif
    app_density_ = NULL;
    return true;
  }
  else
    return false;
}


int boxm_update_bit_scene_manager::setup_app_density_buffer()
{
  cl_int status = CL_SUCCESS;
  app_density_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_float4),app_density_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (app density) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int boxm_update_bit_scene_manager::clean_app_density_buffer()
{
  cl_int status = clReleaseMemObject(app_density_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (app_density_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

bool boxm_update_bit_scene_manager::set_commandqueue()
{
  cl_int status = CL_SUCCESS;
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  return true;
}


bool boxm_update_bit_scene_manager::release_commandqueue()
{
  if (command_queue_)
  {
    cl_int status = clReleaseCommandQueue(command_queue_);
    if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
      return false;
  }
  return true;
}


bool boxm_update_bit_scene_manager::set_workspace(cl_kernel kernel, unsigned pass)
{
  cl_int status = CL_SUCCESS;

  // check the local memeory
  //THIS CHECK can be done elsewhere, just once, after set args
  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel,this->devices()[0],
                                    CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return false;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel,this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return false;

  if (pass==0 || pass==4) // passes for computing aux
  {
      globalThreads[0]=this->wni_/2; globalThreads[1]=this->wnj_/2;
      localThreads[0] =this->bni_  ; localThreads[1] =this->bnj_  ;
  }
  else if (pass==2 || pass==3) // pass preinf, and proc_norm_image
  {
      globalThreads[0]=this->wni_;globalThreads[1]=this->wnj_;
      localThreads[0] =this->bni_;localThreads[0] =this->bnj_;
  }
  else if (pass==1 || pass==5|| pass==11)  // pass for updating data from aux data
  {
    int numbuf = scene_info_->num_buffer;
    int datlen = scene_info_->data_buffer_length;
    globalThreads[0] = RoundUp(numbuf*datlen,64);
    globalThreads[1] = 1;
    localThreads[0]  = 64;
    localThreads[1]  = 1;
  }


  else if (pass==6|| pass==10)
  {
      globalThreads[0] = this->wni_;
      globalThreads[1] = this->wnj_;
      localThreads[0]  = this->bni_;
      localThreads[1]  = this->bnj_;
  }
  else if (pass==7)
  {
      globalThreads[0] = scene_info_->num_buffer;
      globalThreads[1] = 1;
      localThreads[0]  = 1;
      localThreads[1]  = 1;
  }
  else if (pass==8)
  {
      globalThreads[0] = 1;
      globalThreads[1] = 1;
      localThreads[0]  = 1;
      localThreads[1]  = 1;
  }
  else if (pass==9)
  {
      globalThreads[0] = 1;
      globalThreads[1] = 1;
      localThreads[0]  = 1;
      localThreads[1]  = 1;

  }
  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return false;
  }
  else {
    return true;
  }
}


bool boxm_update_bit_scene_manager::run(cl_kernel kernel, unsigned pass)
{
  const int CHECK_SUCCESS = 1;
  cl_int status = SDK_SUCCESS;
  cl_ulong tstart,tend;

  //pass 0, and 3 require four separate executions to run
  if (pass==0 || pass==4)
  {
      for (unsigned k=0;k<2;k++)
      {
          for (unsigned l=0;l<2;l++)
          {
              status=clEnqueueWriteBuffer(command_queue_,offset_y_buf_,0,0,sizeof(cl_uint),(void *)&k,0,0,0);
              status=clEnqueueWriteBuffer(command_queue_,offset_x_buf_,0,0,sizeof(cl_uint),(void *)&l,0,0,0);
              clFinish(command_queue_);
              cl_event ceEvent=0;
              status = clEnqueueNDRangeKernel(command_queue_, kernel, 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);
              if (this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status))!=CHECK_SUCCESS)
                  return false;
              status = clFinish(command_queue_);
              if (this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status))!=CHECK_SUCCESS)
                  return false;
              status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
              status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
              gpu_time_ += 1.0e-6f * float(tend - tstart); // convert nanoseconds to milliseconds
          }
      }
  }
  //everything else just runs once
  else
  {
      cl_event ceEvent =0;
      status = clEnqueueNDRangeKernel(this->command_queue_, kernel, 2, NULL, globalThreads, localThreads, 0, NULL, &ceEvent);
      if (this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status))!=CHECK_SUCCESS)
          return false;
      status = clFinish(command_queue_);
      status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
      status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
      gpu_time_ += 1.0e-6f * float(tend - tstart); // convert nanoseconds to milliseconds
  }

  return this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status))==CHECK_SUCCESS;
}


bool boxm_update_bit_scene_manager::update()
{
  vcl_cout<<"Update called "<<vcl_endl;
  gpu_time_=0.0;
  for (unsigned pass = 0; pass<6; pass++)
  {
    if(pass == 1) continue;
      vcl_cout<<"  pass_"<<pass;
      this->set_update_args(pass);  //need to set args?
      this->set_workspace(update_kernels_[pass], pass);
      if (!this->run(update_kernels_[pass], pass))
        return false;
      vcl_cout<<"("<<gpu_time_<<"ms)  ";

#if 0
      if (pass == 1) {
        cl_event events[1];
        int status = clEnqueueReadBuffer(command_queue_, image_buf_, CL_TRUE, 0,
                                         this->wni_*this->wnj_*sizeof(cl_float4),
                                         image_, 0, NULL, &events[0]);
        if (!this->check_val(status,CL_SUCCESS,"clEnqueueReadBuffer (read)failed."))
          return false;
        status = clWaitForEvents(1, &events[0]);
        if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents (output read) failed."))
          return false;
        this->save_image();
      }
      cl_event events[1];
      int status = clEnqueueReadBuffer(command_queue_, output_debug_buf_, CL_TRUE, 0,
                                       scene_info_->num_buffer*sizeof(cl_float),
                                       output_debug_, 0, NULL, &events[0]);
      if (!this->check_val(status,CL_SUCCESS,"clEnqueueReadBuffer (output buffer )failed."))
        return false;
      status = clWaitForEvents(1, &events[0]);
      if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents (output read) failed."))
        return false;
      vcl_cout<<"KERNEL OUTPUT:"<<vcl_endl;
      for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
          vcl_cout<<output_debug_[4*i+j]<<' ';
        }
        vcl_cout<<vcl_endl;
      }

      if (pass == 4) {  //only read for data setting pass
        cl_event events[1];
        int status = clEnqueueReadBuffer(command_queue_, output_debug_buf_, CL_TRUE, 0,
                                         scene_info_->num_buffer*sizeof(cl_float),
                                         output_debug_, 0, NULL, &events[0]);
        if (!this->check_val(status,CL_SUCCESS,"clEnqueueReadBuffer (output buffer )failed."))
          return false;
        status = clWaitForEvents(1, &events[0]);
        if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents (output read) failed."))
          return false;
        vcl_cout<<"OUTPUT:\n"
                <<"  alpha:  "<<output_debug_[0]<<'\n'
                <<"  gauss0: "<<output_debug_[1]<<','
                <<output_debug_[2]<<','
                <<output_debug_[3]<<'\n'
                <<"  gauss1: "<<output_debug_[4]<<','
                <<output_debug_[5]<<','
                <<output_debug_[6]<<'\n'
                <<"  gauss2: "<<output_debug_[7]<<','
                <<output_debug_[8]<<'\n'
                <<"  nobsmix:"<<output_debug_[9]<<vcl_endl;
      }
#endif
  }

  vcl_cout << ":::: total update time:"<<gpu_time_<<" ms" << vcl_endl
#ifdef DEBUG
           << "Running block "<<total_gpu_time/1000<<"s\n"
           << "total block loading time = " << total_load_time << "s\n"
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl
#endif
      ;
  return true;
}


bool boxm_update_bit_scene_manager::rendering()
{
  gpu_time_=0;
  unsigned pass = 6;
  this->set_render_args();
  this->set_workspace(render_kernel_, pass);
  if (!this->run(render_kernel_, pass))
      return false;
  vcl_cout << "Render Time: "<<gpu_time_<<" ms" << vcl_endl
#ifdef DEBUG
           << "Running block "<<total_gpu_time/1000<<"s\n"
           << "total block loading time = " << total_load_time << "s\n"
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl
#endif
      ;

  return true;
}
bool boxm_update_bit_scene_manager::clean_aux_data()
{
  gpu_time_=0;
  unsigned pass = 11;
  this->set_clean_aux_data_args();
  this->set_workspace(clean_aux_data_kernel_, pass);
  if (!this->run(clean_aux_data_kernel_, pass))
      return false;
  vcl_cout << "Clean Aux data: "<<gpu_time_<<" ms" << vcl_endl;
  return true;
}


bool boxm_update_bit_scene_manager::change_detection()
{
  gpu_time_=0;
  unsigned pass = 10;
  this->set_change_detection_args();
  this->set_workspace(change_detection_kernel_, pass);
  if (!this->run(change_detection_kernel_, pass))
      return false;
  vcl_cout << "Render Time: "<<gpu_time_<<" ms" << vcl_endl
#ifdef DEBUG
           << "Running block "<<total_gpu_time/1000<<"s\n"
           << "total block loading time = " << total_load_time << "s\n"
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl
#endif
      ;

  return true;
}


bool boxm_update_bit_scene_manager::query_point(vgl_point_3d<float> p)
{
  gpu_time_=0;
  unsigned pass = 8;
  point_3d_[0]=p.x();
  point_3d_[1]=p.y();
  point_3d_[2]=p.z();
  cl_int status=clEnqueueWriteBuffer(command_queue_,point_3d_buf_,CL_TRUE, 0,3*sizeof(cl_float), point_3d_, 0, 0, 0);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (point_3d_buf_) failed."))
    return false;
  clFinish(command_queue_);

  this->set_query_point_args();
  this->set_workspace(query_point_kernel_, pass);
  if (!this->run(query_point_kernel_, pass))
      return false;
  vcl_cout << "Timing Analysis\n"
           << "===============\n"
           << "openCL Running time "<<gpu_time_<<" ms" << vcl_endl;

  return true;
}

bool boxm_update_bit_scene_manager::ray_probe(unsigned i,unsigned j, float intensity)
{
  gpu_time_=0;
  unsigned pass = 9;
  this->set_ray_probe_args((int)i,(int)j, intensity);
  this->set_workspace(ray_probe_kernel_, pass);
  if (!this->run(ray_probe_kernel_, pass))
      return false;
  if(!this->read_output_array())
      vcl_cout<<" No output "<<vcl_endl;
  vcl_cout << "Timing Analysis\n"
           << "===============\n"
           << "openCL Running time "<<gpu_time_<<" ms" << vcl_endl;

  return true;
}

bool boxm_update_bit_scene_manager::refine()
{
  gpu_time_=0;
  unsigned pass = 7;
  this->set_refine_args();
  this->set_workspace(refine_kernel_, pass);
  if (!this->run(refine_kernel_, pass))
      return false;
  vcl_cout << "===============\n"
           << "Timing Analysis:\n"
           << "openCL Running time "<<gpu_time_<<" ms" << vcl_endl;

#if 0
  //-read trees for mem_ptrs---TO BE DELTED -------
  this->read_scene();
  scene_->set_blocks(block_ptrs_);
  scene_->set_tree_buffers_opt(cells_);
  scene_->set_mem_ptrs(mem_ptrs_);
  scene_->set_alpha_values(cell_alpha_);
  scene_->set_mixture_values(cell_mixture_);
  scene_->set_num_obs_values(cell_num_obs_);
  vcl_cout<<(*scene_)<<vcl_endl;
  //----TO BE DELTED -------

#endif
#if 1
  /******** read some output **************************************/
  cl_event events[1];
  int status = clEnqueueReadBuffer(command_queue_,output_debug_buf_,CL_TRUE, 0,
                                   scene_info_->num_buffer*sizeof(cl_float),
                                   output_debug_, 0, NULL, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueReadBuffer (output buffer )failed."))
    return false;
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents (output read) failed."))
    return false;

  vcl_cout<<"Kernel OUTPUT:"<<vcl_endl;
  this->read_scene();   //DEBUG PRINTER - can be deleted when fully working.
  for (int i=0; i<scene_info_->num_buffer; i++) {
    int startPtr = mem_ptrs_[2*i];
    int endPtr   = mem_ptrs_[2*i+1];
    int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : scene_info_->data_buffer_length - (endPtr-startPtr);
    //vcl_cout<<"Buffer @"<<i<<": ["<<startPtr<<','<<endPtr<<"] ("
            //<<freeSpace<<" free cells)  "
            //<<output_debug_[i]<<" cells split"<<vcl_endl;
    //if(startPtr > endPtr) vcl_cout<<"     Rolled over Buffer... "<<vcl_endl;
    if (output_debug_[i] == -666) {
      vcl_cout<<"buffer @ "<<i<<" is out of space post refine (shouldn't happen). freeSpace = "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;
    }
    else if (output_debug_[i] == -665) {
      vcl_cout<<"buffer @ "<<i<<" is out of space PRE refine freeSpace = "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;
    }
    else if (output_debug_[i] == -555) {
      vcl_cout<<"buffer @ "<<i<<" has bad block pointer!!! freeSpace = "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;
    }
    else if (output_debug_[i] == -663) {
      vcl_cout<<"buffer @ "<<i<<" failed on refine!!! freeSpace = "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;    
    }
    else if (output_debug_[i] == -662) {
      vcl_cout<<"buffer @ "<<i<<" failed to initialize correct number of cells !!! freeSpace = "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;    
    }
    else if (output_debug_[i] == -661) {
      vcl_cout<<"buffer @ "<<i<<" newInit and old cells don't add up to newsize !!! freeSpace = "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;    
    }
    else if (output_debug_[i] == -660) {
      vcl_cout<<"buffer @ "<<i<<" end pointer and newdata pointer don't match "<<freeSpace
              <<"  mem_ptrs = "<<startPtr<<','<<endPtr<<vcl_endl;    
    }
  }
  vcl_cout<<vcl_endl;
  /****************************************************************/
#endif // 1

  return true;
}

vil_image_view_base_sptr boxm_update_bit_scene_manager::get_output_image()
{
  vil_image_view<float> * oimage=new vil_image_view<float>(this->wni_,this->wnj_);

  for (unsigned i=0;i<oimage->ni();i++)
    for (unsigned j=0;j<oimage->nj();j++)
      (*oimage)(i,j)=image_[(j*wni_+i)*4+3];

  return oimage;
}

void boxm_update_bit_scene_manager::save_image()
{
  if (!image_)
    return;
  vil_image_view<float> img0(this->wni_,this->wnj_);
  vil_image_view<float> img1(this->wni_,this->wnj_);
  vil_image_view<float> img2(this->wni_,this->wnj_);
  vil_image_view<float> img3(this->wni_,this->wnj_);


  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img0(i,j)=image_[(j*this->wni_+i)*4];
  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img1(i,j)=image_[(j*this->wni_+i)*4+1];
  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img2(i,j)=image_[(j*this->wni_+i)*4+2];
  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img3(i,j)=image_[(j*this->wni_+i)*4+3];


  vil_save(img0,"/media/VXL/img0.tiff");
  vil_save(img1,"/media/VXL/img1.tiff");
  vil_save(img2,"/media/VXL/img2.tiff");
  vil_save(img3,"/media/VXL/img3.tiff");
}

bool boxm_update_bit_scene_manager::read_output_image()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,image_buf_,CL_TRUE,
                                   0,this->wni_*this->wnj_*sizeof(cl_float4),
                                   image_,
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
    return false;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clReleaseEvent(events[0]);
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}


bool boxm_update_bit_scene_manager::read_scene()
{
  cl_event events[2];

  // Enqueue readBuffers
  int numCells = scene_info_->tree_buffer_length * scene_info_->num_buffer;
  int status = clEnqueueReadBuffer(command_queue_, cells_buf_, CL_TRUE,
                                   0, numCells*sizeof(cl_uchar16),
                                   cells_,
                                   0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cells )failed."))
    return false;

  int datCells = scene_info_->data_buffer_length * scene_info_->num_buffer;
  status = clEnqueueReadBuffer(command_queue_,cell_alpha_buf_,CL_TRUE,
                               0,datCells*sizeof(cl_float),
                               cell_alpha_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell alpha )failed."))
    return false;

  status = clEnqueueReadBuffer(command_queue_,cell_mixture_buf_,CL_TRUE,
                               0,datCells*sizeof(cl_uchar8),
                               cell_mixture_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell mixture )failed."))
    return false;

  status = clEnqueueReadBuffer(command_queue_,cell_num_obs_buf_,CL_TRUE,
                               0,datCells*sizeof(cl_ushort4),
                               cell_num_obs_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell num obs )failed."))
    return false;

  status = clEnqueueReadBuffer(command_queue_,cell_aux_data_buf_,CL_TRUE,
                               0,datCells*sizeof(cl_float4),
                               cell_aux_data_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell aux )failed."))
    return false;

  status = clEnqueueReadBuffer(command_queue_,mem_ptrs_buf_,CL_TRUE,
                               0,scene_info_->num_buffer*sizeof(cl_ushort2),
                               mem_ptrs_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell mem_ptrs )failed."))
    return false;

  int numBlocks = scene_info_->scene_dims[0] * scene_info_->scene_dims[1] * scene_info_->scene_dims[2];
  status = clEnqueueReadBuffer(command_queue_,block_ptrs_buf_,CL_TRUE,
                               0,numBlocks*sizeof(cl_ushort2),
                               block_ptrs_,
                               0,NULL,&events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell block_ptrs )failed."))
    return false;

  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;
  // Wait for the read buffer to finish execution

  status = clReleaseEvent(events[0]);
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}

bool boxm_update_bit_scene_manager::save_scene()
{
  this->read_scene();
  scene_->set_blocks(block_ptrs_);
  scene_->set_tree_buffers(cells_);
  scene_->set_mem_ptrs(mem_ptrs_);
  scene_->set_alpha_values(cell_alpha_);
  scene_->set_mixture_values(cell_mixture_);
  scene_->set_num_obs_values(cell_num_obs_);
  return scene_->save();
}


bool boxm_update_bit_scene_manager::set_persp_camera(vpgl_proj_camera<double> * pcam)
{
  if (pcam)
  {
    vnl_svd<double>* svd=pcam->svd();
    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();
    persp_cam_=(cl_float *)boxm_ocl_utils::alloc_aligned(3,sizeof(cl_float16),16);

    int cnt=0;
    for (unsigned i=0;i<Ut.rows();i++)
    {
      for (unsigned j=0;j<Ut.cols();j++)
        persp_cam_[cnt++]=(cl_float)Ut(i,j);

      persp_cam_[cnt++]=0;
    }

    for (unsigned i=0;i<V.rows();i++)
      for (unsigned j=0;j<V.cols();j++)
        persp_cam_[cnt++]=(cl_float)V(i,j);

    for (unsigned i=0;i<Winv.size();i++)
      persp_cam_[cnt++]=(cl_float)Winv(i);

    vgl_point_3d<double> cam_center=pcam->camera_center();
    persp_cam_[cnt++]=(cl_float)cam_center.x();
    persp_cam_[cnt++]=(cl_float)cam_center.y();
    persp_cam_[cnt++]=(cl_float)cam_center.z();
    return true;
  }
  else {
    vcl_cerr << "Error set_persp_camera() : Missing camera\n";
    return false;
  }
}


bool boxm_update_bit_scene_manager::set_persp_camera()
{
  if (vpgl_proj_camera<double>* pcam =
      dynamic_cast<vpgl_proj_camera<double>*>(cam_.ptr()))
  {
    vnl_svd<double>* svd=pcam->svd();

    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();

    persp_cam_=(cl_float *)boxm_ocl_utils::alloc_aligned(3,sizeof(cl_float16),16);

    int cnt=0;
    for (unsigned i=0;i<Ut.rows();i++)
    {
      for (unsigned j=0;j<Ut.cols();j++)
        persp_cam_[cnt++]=(cl_float)Ut(i,j);

      persp_cam_[cnt++]=0;
    }

    for (unsigned i=0;i<V.rows();i++)
      for (unsigned j=0;j<V.cols();j++)
        persp_cam_[cnt++]=(cl_float)V(i,j);

    for (unsigned i=0;i<Winv.size();i++)
      persp_cam_[cnt++]=(cl_float)Winv(i);

    vgl_point_3d<double> cam_center=pcam->camera_center();
    persp_cam_[cnt++]=(cl_float)cam_center.x();
    persp_cam_[cnt++]=(cl_float)cam_center.y();
    persp_cam_[cnt++]=(cl_float)cam_center.z();
    return true;
  }
  else {
    vcl_cerr << "Error set_persp_camera() : unsupported camera type\n";
    return false;
  }
}


bool boxm_update_bit_scene_manager::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils::free_aligned(persp_cam_);
  return true;
}


bool boxm_update_bit_scene_manager::set_persp_camera_buffers()
{
  cl_int status;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.")==1;
}


bool boxm_update_bit_scene_manager::write_persp_camera_buffers()
{
  cl_int status;
  status=clEnqueueWriteBuffer(command_queue_,persp_cam_buf_,CL_TRUE, 0,3*sizeof(cl_float16), persp_cam_, 0, 0, 0);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (persp_cam_buf_) failed."))
    return false;
  clFinish(command_queue_);

  return true;
}


bool boxm_update_bit_scene_manager::release_persp_camera_buffers()
{
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).")==1;
}


bool boxm_update_bit_scene_manager::set_input_image()
{
    image_=(cl_float *)boxm_ocl_utils::alloc_aligned(wni_*wnj_,sizeof(cl_float4),16);
    img_dims_=(cl_uint *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_uint4),16);
    if (!use_gl_)
        image_gl_=(cl_uint *)boxm_ocl_utils::alloc_aligned(wni_*wnj_,sizeof(cl_uint),16);
    for (unsigned i=0;i<wni_*wnj_*4;i++)
        image_[i]=0.0;
    // pad the image
    for (unsigned i=0;i<input_img_.ni();i++)
    {
        for (unsigned j=0;j<input_img_.nj();j++)
        {
            image_[(j*wni_+i)*4]=input_img_(i,j);
            image_[(j*wni_+i)*4+1]=0.0f;
            image_[(j*wni_+i)*4+2]=1.0f;
            image_[(j*wni_+i)*4+3]=0.0f;
        }
    }

  img_dims_[0]=0;
  img_dims_[1]=0;
  img_dims_[2]=input_img_.ni();
  img_dims_[3]=input_img_.nj();

  if (image_==NULL || img_dims_==NULL)
  {
    vcl_cerr<<"Failed allocation of image or image dimensions\n";
    return false;
  }
  else
    return true;
}

bool boxm_update_bit_scene_manager::set_input_image(vil_image_view<float>  obs)
{
  for (unsigned i=0;i<wni_*wnj_*4;i++)
    image_[i]=0.0;

 // pad the image
  for (unsigned i=0;i<obs.ni();i++)
  {
    for (unsigned j=0;j<obs.nj();j++)
    {
      image_[(j*wni_+i)*4]=obs(i,j);
      image_[(j*wni_+i)*4+1]=0.0f;
      image_[(j*wni_+i)*4+2]=1.0f;
      image_[(j*wni_+i)*4+3]=0.0f;
    }
  }

  img_dims_[0]=0;
  img_dims_[1]=0;
  img_dims_[2]=obs.ni();
  img_dims_[3]=obs.nj();

  if (image_==NULL || img_dims_==NULL)
  {
    vcl_cerr<<"Failed allocation of image or image dimensions\n";
    return false;
  }
  else
    return true;
}

bool boxm_update_bit_scene_manager::clean_input_image()
{
  if (image_)
    boxm_ocl_utils::free_aligned(image_);
  if (img_dims_)
    boxm_ocl_utils::free_aligned(img_dims_);
  return true;
}


bool boxm_update_bit_scene_manager::set_input_image_buffers()
{
  cl_int status;

  image_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              wni_*wnj_*sizeof(cl_float4),
                              image_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (image_buf_) failed.")==1;
}


bool boxm_update_bit_scene_manager::write_image_buffer()
{
  cl_int status;
  status=clEnqueueWriteBuffer(command_queue_,image_buf_,CL_TRUE, 0,wni_*wnj_*sizeof(cl_float4), image_, 0, 0, 0);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (image_buf_) failed."))
    return false;
  status=clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish (writing) failed."))
    return false;

  status=clEnqueueWriteBuffer(command_queue_,img_dims_buf_,CL_TRUE, 0,sizeof(cl_uint4), img_dims_, 0, 0, 0);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (imd_dims_buf_) failed."))
    return false;
  status=clFinish(command_queue_);

  return this->check_val(status,CL_SUCCESS,"clFinish (writing) failed.")==1;
}

bool boxm_update_bit_scene_manager::set_image_dims_buffers()
{
  cl_int status;

  img_dims_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint4),
                                 img_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (imd_dims_buf_) failed.")==1;
}


bool boxm_update_bit_scene_manager::release_input_image_buffers()
{
  cl_int status;
  status = clReleaseMemObject(image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (image_buf_)."))
    return false;

  status = clReleaseMemObject(img_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (img_dims_buf_).")==1;
}


bool boxm_update_bit_scene_manager::set_offset_buffers(int offset_x,int offset_y,int factor)
{
  vcl_cout<<"OFFSET BUFFERS SET: "<<offset_x<<','<<offset_y<<vcl_endl;
  cl_int status;
  offset_x_=offset_x;
  offset_y_=offset_y;
  factor_=factor;
  factor_buf_ = clCreateBuffer(this->context_,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_int),
                               &factor_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (factor_) failed."))
    return false;

  offset_x_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_int),
                                 &offset_x_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (offset_x_) failed."))
    return false;
  offset_y_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_int),
                                 &offset_y_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (offset_y_) failed.")==1;
}


bool boxm_update_bit_scene_manager::release_offset_buffers()
{
  cl_int status;
  status = clReleaseMemObject(factor_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (factor_buf_)."))
    return false;

  status = clReleaseMemObject(offset_x_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_x_buf_)."))
    return false;

  status = clReleaseMemObject(offset_y_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_y_buf_).")==1;
}

bool boxm_update_bit_scene_manager::read_output_debug()
{
    cl_event events[2];
    int status =-1;
    // Enqueue readBuffers
    status= clEnqueueReadBuffer(command_queue_,output_debug_buf_,CL_TRUE,
                                0,scene_info_->num_buffer*sizeof(cl_float),
                                output_debug_, 0,NULL,&events[0]);
    if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
        return false;

    // Wait for the read buffer to finish execution
    status = clWaitForEvents(1, &events[0]);
    if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
        return false;

    status = clReleaseEvent(events[0]);
    if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
        return false;

    return true;
}

bool boxm_update_bit_scene_manager::get_output_debug_array(vcl_vector<float> & data)
{
    vcl_cout<<"Num buffers "<<scene_info_->num_buffer<<vcl_endl;
    for (int i=0;i<scene_info_->num_buffer;i++)
    {
        if (output_debug_[i]==-1.0f)
            break;

            data.push_back(output_debug_[i]);
            vcl_cout<<output_debug_[i]<<" ";
    }
    return true;
}

bool boxm_update_bit_scene_manager::set_rayoutput()
{
 raydepth_=1000;
 for(unsigned j=0;j<10;j++)
 {
 rayoutput_[j]=(float*)boxm_ocl_utils::alloc_aligned(raydepth_,sizeof(float),16);

 for (unsigned i=0;i<raydepth_;i++)
     rayoutput_[j][i]=-1.0;
 }  
 return true;
}

bool boxm_update_bit_scene_manager::set_rayoutput_buffers()
{
  cl_int status;
  for(unsigned i=0;i<10;i++)
  {
  rayoutput_buf_[i] = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                  raydepth_*sizeof(cl_float),
                                  rayoutput_[i],&status);

  }
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (rayoutput_) failed.")==1;
}

bool boxm_update_bit_scene_manager::release_rayoutput_buffers()
{
    cl_int status;
    for(unsigned i=0;i<10;i++)
        status = clReleaseMemObject(rayoutput_buf_[i]);


    return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (rayoutput_buf_).")==1;
}

bool boxm_update_bit_scene_manager::clean_rayoutput()
{
    if (rayoutput_)
        for(unsigned i=0;i<10;i++)
            boxm_ocl_utils::free_aligned(rayoutput_[i]);
    return true;
}

bool boxm_update_bit_scene_manager::read_output_array()
{
  cl_event events[2];
 int status =-1;
  for(unsigned i=0;i<10;i++)
  {
      // Enqueue readBuffers
     status= clEnqueueReadBuffer(command_queue_,rayoutput_buf_[i],CL_TRUE,
                                       0,this->raydepth_*sizeof(cl_float),
                                       rayoutput_[i], 0,NULL,&events[0]);

      if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
          return false;

      // Wait for the read buffer to finish execution
      status = clWaitForEvents(1, &events[0]);
      if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
          return false;

      status = clReleaseEvent(events[0]);
  }

  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}

void boxm_update_bit_scene_manager::getoutputarray(vcl_vector< vcl_vector<float> >& out)
{
    out.resize(10);
    for(unsigned j=0;j<10;j++)
        for (unsigned i=0;i<raydepth_;i++)
            if (rayoutput_[j][i]>-1)
                out[j].push_back(rayoutput_[j][i]);
}



//: Binary write bit scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm_update_bit_scene_manager const& /*bit_scene*/)
{
}


//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm_update_bit_scene_manager& /*bit_scene*/)
{
}

//: Binary write boxm scene pointer to stream
void vsl_b_read(vsl_b_istream& /*is*/, boxm_update_bit_scene_manager* /*ph*/)
{
}

//: Binary write boxm scene pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm_update_bit_scene_manager* const& /*ph*/){}
