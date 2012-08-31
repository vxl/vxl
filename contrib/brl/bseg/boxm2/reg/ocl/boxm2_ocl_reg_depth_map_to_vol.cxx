#include "boxm2_ocl_reg_depth_map_to_vol.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <vcl_where_root_dir.h>
#include <vil/vil_save.h>

boxm2_ocl_reg_depth_map_to_vol::boxm2_ocl_reg_depth_map_to_vol(  vpgl_camera_double_sptr & cam,
                                                                 vil_image_view<float> * img,
                                                                 boxm2_stream_scene_cache& cache,
                                                                 bocl_device_sptr device,
                                                                 int nbins)
  : vnl_cost_function(6),cache_(cache), cam_(cam)
{
  device_ = device;
  nbins_  = nbins;

  depth_img_ = img ;
  this->estimate_xyz();
  this->compile_kernel();
  this->init_ocl_minfo();
}

bool boxm2_ocl_reg_depth_map_to_vol::estimate_xyz()
{
  Xdepth = new vil_image_view<float>(depth_img_->ni(),depth_img_->nj());
  Ydepth = new vil_image_view<float>(depth_img_->ni(),depth_img_->nj());
  Zdepth = new vil_image_view<float>(depth_img_->ni(),depth_img_->nj());

  if (vpgl_perspective_camera<double> * pcam = dynamic_cast<vpgl_perspective_camera<double> * > (cam_.ptr()))
  {
    for (unsigned i = 0 ; i < depth_img_->ni(); i++)
      for (unsigned j = 0 ; j < depth_img_->nj(); j++)
      {
        vgl_ray_3d<double> r = pcam->backproject_ray(vgl_point_2d<double>((double)i,(double)j));
        vgl_point_3d<double> temp(0,0,0);
        vgl_point_3d<double> pt = temp +(*depth_img_)(i,j)*r.direction();

        (*Xdepth)(i,j) = (float)pt.x();
        (*Ydepth)(i,j) = (float)pt.y();
        (*Zdepth)(i,j) = (float)pt.z();
      }
#if 0
    vil_save(*Zdepth,"z.tiff");
#endif
    return true;
  }
  else
    return false;
}

double boxm2_ocl_reg_depth_map_to_vol::f(vnl_vector<double> const& x)
{
  vgl_vector_3d<double> tx(x[0],x[1],x[2]) ;
  vgl_rotation_3d<double> r(x[3],x[4],x[5]);
  float val = 0.0f;
  this->boxm2_ocl_register_world(r,tx,nbins_,val);
  return -val;
}

double boxm2_ocl_reg_depth_map_to_vol:: mutual_info(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
  float val = 0.0f;
  this->boxm2_ocl_register_world(rot,trans,nbins_,val);
  return val;
}

double boxm2_ocl_reg_depth_map_to_vol:: error(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
  float val = 0.0f;
  this->boxm2_ocl_register_world(rot,trans,nbins_,val);
  return -val;
}

bool boxm2_ocl_reg_depth_map_to_vol::compile_kernel()
{
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  vcl_string reg_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/reg/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(reg_source_dir + "estimate_mi_depth_map_to_vol.cl");

  this->kern = new bocl_kernel();
  return kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "estimate_mi_depth_map_to_vol", "", "MI");
}

bool boxm2_ocl_reg_depth_map_to_vol::init_ocl_minfo()
{
  boxm2_scene_sptr sceneB = cache_.scene();
  // Instantiate OPENCL
  // get scene B on the GPU's host memory
  blk_offsets_array = new unsigned int[cache_.blk_offsets_.size()];
  for (unsigned k = 0; k< cache_.blk_offsets_.size(); k++)
    blk_offsets_array[k] = cache_.blk_offsets_[k];

  alpha_offsets_array = new unsigned int[cache_.offsets_["alpha"].size()];
  for (unsigned k = 0; k< cache_.offsets_["alpha"].size(); k++)
    alpha_offsets_array[k] =cache_.offsets_["alpha"][k]/4;

  vcl_vector<boxm2_block_id> sceneB_ids = sceneB->get_block_ids();
  boxm2_scene_info * sceneB_info = sceneB->get_blk_metadata( sceneB_ids[0] );

  vgl_box_3d<int> bbox = sceneB->bounding_box_blk_ids();

  bbox_buff[0] = 0; bbox_buff[3] = bbox.max_x()-bbox.min_x();
  bbox_buff[1] = 0; bbox_buff[4] = bbox.max_y()-bbox.min_y();
  bbox_buff[2] = 0; bbox_buff[5] = bbox.max_z()-bbox.min_z();

  bbox_buff[6] = sceneB_info->scene_dims[0];
  bbox_buff[7] = sceneB_info->scene_dims[1];
  bbox_buff[8] = sceneB_info->scene_dims[2];

  sceneB_bbox_ids = new bocl_mem(device_->context(), bbox_buff, 9*sizeof(int), " scene B bbox" );
  sceneB_bbox_ids->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  vgl_box_3d<double> scene_bbox = sceneB->bounding_box();
  sceneB_origin_buff[0] = (float)scene_bbox.min_x();
  sceneB_origin_buff[1] = (float)scene_bbox.min_y();
  sceneB_origin_buff[2] = (float)scene_bbox.min_z();

  sceneB_origin = new bocl_mem(device_->context(), sceneB_origin_buff, 4*sizeof(float), " scene B origin" );
  sceneB_origin->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  block_dims[0] = sceneB_info->scene_dims[0]*sceneB_info->block_len;
  block_dims[1] = sceneB_info->scene_dims[1]*sceneB_info->block_len;
  block_dims[2] = sceneB_info->scene_dims[2]*sceneB_info->block_len;
  sceneB_block_dims = new bocl_mem(device_->context(), block_dims, 4*sizeof(float), " scene B block dims" );
  sceneB_block_dims->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  subblk_num_buff[0] = sceneB_info->scene_dims[0];
  subblk_num_buff[1] = sceneB_info->scene_dims[1];
  subblk_num_buff[2] = sceneB_info->scene_dims[2];
  subblk_num_buff[3] = sceneB_info->scene_dims[3];

  sceneB_sub_block_num = new bocl_mem(device_->context(), subblk_num_buff, sizeof(int)* 4, " scene B sub block num" );
  sceneB_sub_block_num->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  sceneB_sub_block_len = new bocl_mem(device_->context(), &(sceneB_info->block_len), sizeof(float), " scene B sub block len" );
  sceneB_sub_block_len->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  blks_ocl_B = new bocl_mem(device_->context(), cache_.blk_buffer_, cache_.total_bytes_per_block_, " block buffer B" );
  blks_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

  blks_ocl_B_offsets = new bocl_mem(device_->context(), blk_offsets_array, sizeof(unsigned int)*cache_.blk_offsets_.size(), " block buffer B" );
  blks_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  alpha_ocl_B = new bocl_mem(device_->context(), cache_.data_buffers_["alpha"], cache_.total_bytes_per_data_["alpha"], " alpha buffer B " );
  alpha_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

  alpha_ocl_B_offsets = new bocl_mem(device_->context(), alpha_offsets_array, sizeof(unsigned int)*cache_.offsets_["alpha"].size(), " block buffer B" );
  alpha_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  //aux2_ocl_B = new bocl_mem(device_->context(), cache_.data_buffers_["aux2"], cache_.total_bytes_per_data_["aux2"], " aux2 buffer B " );
  //aux2_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
  //vcl_cout<<"# of bytes for trees, alpha, aux2 "<<cache_.total_bytes_per_block_<< ' '<<cache_.total_bytes_per_data_["alpha"]<<' '<<cache_.total_bytes_per_data_["aux2"]<<vcl_endl;
  vcl_cout<<"# of bytes for trees, alpha "<<cache_.total_bytes_per_block_<< ' '<<cache_.total_bytes_per_data_["alpha"]<<vcl_endl;
  // output buffer for debugging
  output = new bocl_mem(device_->context(), output_buff, sizeof(float)*1000, "output" );
  output->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  lookup=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  return true;
}

bool boxm2_ocl_reg_depth_map_to_vol::boxm2_ocl_register_world(vgl_rotation_3d<double> rot,
                                                              vgl_vector_3d<double> tx,
                                                              int nbins,
                                                              float & mi)
{
  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={1,1};

  unsigned cl_ni=(unsigned)RoundUp(depth_img_->ni(),(int)local_threads[0]);
  unsigned cl_nj=(unsigned)RoundUp(depth_img_->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  float* xdepth_buff = new float[cl_ni*cl_nj];
  float* ydepth_buff = new float[cl_ni*cl_nj];
  float* zdepth_buff = new float[cl_ni*cl_nj];
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j)
    for (unsigned int i=0;i<cl_ni;++i)
    {
      if (i<Xdepth->ni() && j< Xdepth->nj()) {
        xdepth_buff[count] = (*Xdepth)(i,j);
        ydepth_buff[count] = (*Ydepth)(i,j);
        zdepth_buff[count] = (*Zdepth)(i,j);
      }
      ++count;
    }
  bocl_mem_sptr x_image=new bocl_mem(device_->context(),xdepth_buff,cl_ni*cl_nj*sizeof(float),"x image buffer");
  bocl_mem_sptr y_image=new bocl_mem(device_->context(),ydepth_buff,cl_ni*cl_nj*sizeof(float),"y image buffer");
  bocl_mem_sptr z_image=new bocl_mem(device_->context(),zdepth_buff,cl_ni*cl_nj*sizeof(float),"z image buffer");
  x_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  y_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  z_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int * joint_histogram_buff= new int[nbins*nbins];
  for (int k = 0 ; k<nbins*nbins; k++)
    joint_histogram_buff[k] = 0;
  bocl_mem_sptr joint_histogram = new bocl_mem(device_->context(), joint_histogram_buff, sizeof(int)*nbins*nbins, " joint histogram" );
  joint_histogram->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  float translation_buff[4];
  translation_buff[0] = tx.x();        translation_buff[2] = tx.z();
  translation_buff[1] = tx.y();        translation_buff[3] = 0.0;

  bocl_mem_sptr translation = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
  translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  float rotation_buff[9];
  vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();

  rotation_buff[0] = R(0,0);rotation_buff[3] = R(1,0);rotation_buff[6] = R(2,0);
  rotation_buff[1] = R(0,1);rotation_buff[4] = R(1,1);rotation_buff[7] = R(2,1);
  rotation_buff[2] = R(0,2);rotation_buff[5] = R(1,2);rotation_buff[8] = R(2,2);

  bocl_mem_sptr rotation = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
  rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  bocl_mem_sptr ocl_nbins = new bocl_mem(device_->context(), &(nbins), sizeof(int), "  #of bins" );
  ocl_nbins->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  float gpu_time = 0.0;

  kern->set_arg(lookup.ptr());
  kern->set_arg(x_image.ptr());
  kern->set_arg(y_image.ptr());
  kern->set_arg(z_image.ptr());
  kern->set_arg(sceneB_origin.ptr());
  kern->set_arg(sceneB_bbox_ids.ptr());
  kern->set_arg(sceneB_block_dims.ptr());
  kern->set_arg(sceneB_sub_block_len.ptr());
  kern->set_arg(blks_ocl_B.ptr());
  kern->set_arg(alpha_ocl_B.ptr());
  //kern->set_arg(aux2_ocl_B.ptr());
  kern->set_arg(blks_ocl_B_offsets.ptr());
  kern->set_arg(alpha_ocl_B_offsets.ptr());
  kern->set_arg(translation.ptr());
  kern->set_arg(rotation.ptr());
  kern->set_arg(ocl_nbins.ptr());
  kern->set_arg(joint_histogram.ptr());
  kern->set_arg(output.ptr());
  kern->set_local_arg(nbins*nbins*sizeof(float));
  kern->set_local_arg(16*local_threads[0]*local_threads[1]*sizeof(unsigned char)); // local trees

  kern->execute(queue, 2, local_threads, global_threads);

  status = clFinish(queue);
  check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
  gpu_time += kern->exec_time();

  output->read_to_buffer(queue);
  joint_histogram->read_to_buffer(queue);
  clFinish(queue);
  //clear render kernel args so it can reset em on next execution
  kern->clear_args();

  float * joint_histogram_float = reinterpret_cast<float * > (joint_histogram_buff);
  float * histA = new float[nbins];
  float * histB = new float[nbins];

  float sum  = 0.0;
  for (int k = 0 ; k <nbins*nbins; k++)
    sum+=joint_histogram_buff[k];
  mi = joint_histogram_buff[nbins*nbins-1]/sum;
  for (int k = 0 ;k<nbins; k++)
  {
    histA[k] = 0.0;
    histB[k] = 0.0;
  }
  sum  = 0.0;
  // normalize joint histogram
  for (int k = 0; k < nbins; k++) {
    for (int l = 0; l < nbins; l++) {
      sum+=joint_histogram_float[k*nbins+l];
    }
  }
  for (int k = 0; k < nbins; k++) {
    for (int l = 0; l < nbins; l++) {
      joint_histogram_float[k*nbins+l] = joint_histogram_float[k*nbins+l] / sum;
    }
  }
  for (int k = 0; k < nbins; k++) {
    for (int l = 0; l < nbins; l++) {
      histA[k]+=joint_histogram_float[k*nbins+l];
    }
  }
  for (int k = 0; k < nbins; k++) {
    for (int l = 0; l < nbins; l++) {
      histB[k]+=joint_histogram_float[l*nbins+k];
    }
  }

  float entropyA = 0;
  for (int k = 0; k < nbins; k++) {
    entropyA += -(histA[k]?histA[k]*vcl_log(histA[k]):0); // if prob=0 this value is defined as 0
  }
  float entropyB = 0;
  for (int l = 0; l < nbins; l++) {
    entropyB += -(histB[l]?histB[l]*vcl_log(histB[l]):0); // if prob=0 this value is defined as 0
  }
  float entropyAB =  0.0; ;
  for (int k = 0; k < nbins; k++) {
    for (int l = 0; l < nbins; l++) {
      entropyAB += -(joint_histogram_float[k*nbins+l]?joint_histogram_float[k*nbins+l]*vcl_log(joint_histogram_float[k*nbins+l]):0);
    }
  }

  //mi = (entropyA +entropyB - entropyAB)/vnl_math::ln2;
  // vcl_cout<<"Mutual Info "<<entropyA<<','<<entropyB<<','<<entropyAB;

  clReleaseCommandQueue(queue);

  delete [] joint_histogram_float;
  delete [] histA;
  delete [] histB;
  delete [] xdepth_buff;
  delete [] ydepth_buff;
  delete [] zdepth_buff;
  return true;
}

