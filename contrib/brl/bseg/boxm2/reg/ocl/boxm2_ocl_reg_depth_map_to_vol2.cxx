#include "boxm2_ocl_reg_depth_map_to_vol2.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <vil/vil_save.h>
#include <vcl_where_root_dir.h>
#include <vcl_cassert.h>

boxm2_ocl_reg_depth_map_to_vol2::boxm2_ocl_reg_depth_map_to_vol2(vpgl_camera_double_sptr & cam,
                                                                 vil_image_view<float> * img,
                                                                 //vil_image_view<float> * skyimg,
                                                                 vcl_vector<boxm2_stream_scene_cache_sptr>& caches,
                                                                 vgl_box_3d<double>& global_bbox,
                                                                 bocl_device_sptr device,
                                                                 int nbins)
  : vnl_cost_function(6),caches_(caches), global_bbox_(global_bbox), cam_(cam)
{
  device_ = device;
  nbins_  = nbins;

  depth_img_ = img ;
  //sky_img_ = skyimg;
  this->estimate_xyz();
  this->compile_kernel();
  this->init_ocl_minfo();
}

bool boxm2_ocl_reg_depth_map_to_vol2::estimate_xyz()
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
        double d = (*depth_img_)(i,j);
        if (d <= 0) {  // mark invalid pixels
          (*Xdepth)(i,j) = global_bbox_.min_x()-10000;
          (*Ydepth)(i,j) = global_bbox_.min_y()-10000;
          (*Zdepth)(i,j) = global_bbox_.min_z()-10000;
        }
        else {
          vgl_point_3d<double> pt = temp + d*r.direction();

          (*Xdepth)(i,j) = (float)pt.x();
          (*Ydepth)(i,j) = (float)pt.y();
          (*Zdepth)(i,j) = (float)pt.z();
        }
        // OZGE HACK TO SEE IF MAKING SKY AT INFINITY MAKES A DIFFERENCE
        //if ((*sky_img_)(i,j) >= 0.9)
        //  (*Zdepth)(i,j) = global_bbox_.max_z()+10000;
      }
    //vil_save(*Zdepth, "C:\\projects\\FINDER\\query_matching\\depth_maps\\z_depth.tif");
    return true;
  }
  else
    return false;
}

double boxm2_ocl_reg_depth_map_to_vol2::f(vnl_vector<double> const& x)
{
  vgl_vector_3d<double> tx(x[0],x[1],x[2]) ;
  vgl_rotation_3d<double> r(x[3],x[4],x[5]);
  float val = 0.0f;
  this->boxm2_ocl_register_world(r,tx,nbins_,val);
  return -val;
}

double boxm2_ocl_reg_depth_map_to_vol2:: mutual_info(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
  float val = 0.0f;
  this->boxm2_ocl_register_world(rot,trans,nbins_,val);
  return val;
}

double boxm2_ocl_reg_depth_map_to_vol2:: error(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
  float val = 0.0f;
  this->boxm2_ocl_register_world(rot,trans,nbins_,val);
  return -val;
}

bool boxm2_ocl_reg_depth_map_to_vol2::compile_kernel()
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

bool boxm2_ocl_reg_depth_map_to_vol2::init_ocl_minfo()
{
  // Instantiate OPENCL
  // get scene B on the GPU's host memory

  unsigned cnt = (unsigned)caches_.size();

  blk_offsets_array.resize(cnt);
  alpha_offsets_array.resize(cnt);
#if 0
  sceneB_origin.resize(cnt);
  sceneB_bbox_ids.resize(cnt);
  sceneB_block_dims.resize(cnt);
  sceneB_sub_block_len.resize(cnt);
  sceneB_sub_block_num.resize(cnt);
  blks_ocl_B.resize(cnt);
  alpha_ocl_B.resize(cnt);
  blks_ocl_B_offsets.resize(cnt);
  alpha_ocl_B_offsets.resize(cnt);
#endif
  assert(caches_.size()!=0);

  for (unsigned i = 0; i < caches_.size(); i++) {
    boxm2_scene_sptr sceneB = caches_[i]->scene();

    blk_offsets_array[i] = new unsigned int[caches_[i]->blk_offsets_.size()];
    for (unsigned k = 0; k< caches_[i]->blk_offsets_.size(); k++)
      blk_offsets_array[i][k] = caches_[i]->blk_offsets_[k];

    alpha_offsets_array[i] = new unsigned int[caches_[i]->offsets_["alpha"].size()];
    for (unsigned k = 0; k< caches_[i]->offsets_["alpha"].size(); k++)
      alpha_offsets_array[i][k] =caches_[i]->offsets_["alpha"][k]/4;
#if 0
    vcl_vector<boxm2_block_id> sceneB_ids = sceneB->get_block_ids();
    boxm2_scene_info * sceneB_info = sceneB->get_blk_metadata( sceneB_ids[0] );

    vgl_box_3d<int> bbox = sceneB->bounding_box_blk_ids();

    bbox_buff[0] = 0; bbox_buff[3] = bbox.max_x()-bbox.min_x();
    bbox_buff[1] = 0; bbox_buff[4] = bbox.max_y()-bbox.min_y();
    bbox_buff[2] = 0; bbox_buff[5] = bbox.max_z()-bbox.min_z();

    bbox_buff[6] = sceneB_info->scene_dims[0];
    bbox_buff[7] = sceneB_info->scene_dims[1];
    bbox_buff[8] = sceneB_info->scene_dims[2];

    sceneB_bbox_ids[i] = new bocl_mem(device_->context(), bbox_buff, 9*sizeof(int), " scene B bbox" );
    //sceneB_bbox_ids[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_bbox_ids[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    vgl_box_3d<double> scene_bbox = sceneB->bounding_box();

    sceneB_origin_buff[0] = (float)scene_bbox.min_x();
    sceneB_origin_buff[1] = (float)scene_bbox.min_y();
    sceneB_origin_buff[2] = (float)scene_bbox.min_z();

    sceneB_origin[i] = new bocl_mem(device_->context(), sceneB_origin_buff, 4*sizeof(float), " scene B origin" );
    //sceneB_origin[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_origin[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    block_dims[0] = sceneB_info->scene_dims[0]*sceneB_info->block_len;
    block_dims[1] = sceneB_info->scene_dims[1]*sceneB_info->block_len;
    block_dims[2] = sceneB_info->scene_dims[2]*sceneB_info->block_len;
#if 0
    vcl_cout << "sub-block cnt/block " << sceneB_info->scene_dims[0] << ' ' << sceneB_info->scene_dims[1] << ' ' << sceneB_info->scene_dims[2] << '\n'
             << "sub-block len " << sceneB_info->block_len << '\n'
             << "scene block dims " << block_dims[0] << ' ' << block_dims[1] << ' ' << block_dims[2] << vcl_endl;
#endif
    sceneB_block_dims[i] = new bocl_mem(device_->context(), block_dims, 4*sizeof(float), " scene B block dims" );
    //sceneB_block_dims[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_block_dims[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    subblk_num_buff[0] = sceneB_info->scene_dims[0];
    subblk_num_buff[1] = sceneB_info->scene_dims[1];
    subblk_num_buff[2] = sceneB_info->scene_dims[2];
    subblk_num_buff[3] = sceneB_info->scene_dims[3];

    sceneB_sub_block_num[i] = new bocl_mem(device_->context(), subblk_num_buff, sizeof(int)* 4, " scene B sub block num" );
    //sceneB_sub_block_num[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_sub_block_num[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    sceneB_sub_block_len[i] = new bocl_mem(device_->context(), &(sceneB_info->block_len), sizeof(float), " scene B sub block len" );
    //sceneB_sub_block_len[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_sub_block_len[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    blks_ocl_B[i] = new bocl_mem(device_->context(), caches_[i]->blk_buffer_, caches_[i]->total_bytes_per_block_, " block buffer B" );
    blks_ocl_B[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

    blks_ocl_B_offsets[i] = new bocl_mem(device_->context(), blk_offsets_array[i], (unsigned)(sizeof(unsigned int)*caches_[i]->blk_offsets_.size()), " block buffer B" );
    blks_ocl_B_offsets[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

    alpha_ocl_B[i] = new bocl_mem(device_->context(), caches_[i]->data_buffers_["alpha"], caches_[i]->total_bytes_per_data_["alpha"], " alpha buffer B " );
    alpha_ocl_B[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

    alpha_ocl_B_offsets[i] = new bocl_mem(device_->context(), alpha_offsets_array[i], (unsigned)(sizeof(unsigned int)*caches_[i]->offsets_["alpha"].size()), " block buffer B" );
    alpha_ocl_B_offsets[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );

    vcl_cout<<"# of bytes for trees, alpha "<<caches_[i]->total_bytes_per_block_<< ' '<<caches_[i]->total_bytes_per_data_["alpha"]<<vcl_endl;
#endif
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  lookup=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  local_threads[0] = 8;
  local_threads[1] = 8;
  unsigned cl_ni=(unsigned)RoundUp(depth_img_->ni(),(int)local_threads[0]);
  unsigned cl_nj=(unsigned)RoundUp(depth_img_->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  xdepth_buff = new float[cl_ni*cl_nj];
  ydepth_buff = new float[cl_ni*cl_nj];
  zdepth_buff = new float[cl_ni*cl_nj];
  skyimg_buff = new float[cl_ni*cl_nj];
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j)
    for (unsigned int i=0;i<cl_ni;++i)
    {
      if (i<Xdepth->ni() && j< Xdepth->nj()) {
        xdepth_buff[count] = (*Xdepth)(i,j);
        ydepth_buff[count] = (*Ydepth)(i,j);
        zdepth_buff[count] = (*Zdepth)(i,j);
        //skyimg_buff[count] = (*sky_img_)(i,j);
      }
      else
        zdepth_buff[count] = -1.0;
      ++count;
    }
  x_image=new bocl_mem(device_->context(),xdepth_buff,cl_ni*cl_nj*sizeof(float),"x image buffer");
  y_image=new bocl_mem(device_->context(),ydepth_buff,cl_ni*cl_nj*sizeof(float),"y image buffer");
  z_image=new bocl_mem(device_->context(),zdepth_buff,cl_ni*cl_nj*sizeof(float),"z image buffer");
  //sky_image = new bocl_mem(device_->context(),skyimg_buff,cl_ni*cl_nj*sizeof(float),"sky image buffer");
  //x_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);
  //y_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);
  //z_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);
  x_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  y_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  z_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  //sky_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  global_bbox_buff = new float[6];
  global_bbox_buff[0] = global_bbox_.min_x(); global_bbox_buff[1] = global_bbox_.min_y(); global_bbox_buff[2] = global_bbox_.min_z();
  global_bbox_buff[3] = global_bbox_.max_x(); global_bbox_buff[4] = global_bbox_.max_y(); global_bbox_buff[5] = global_bbox_.max_z();
  global_bbox_mem = new bocl_mem(device_->context(),global_bbox_buff, 6*sizeof(float),"global bbox buffer");
  global_bbox_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  return true;
}

boxm2_ocl_reg_depth_map_to_vol2::~boxm2_ocl_reg_depth_map_to_vol2()
{
  delete [] xdepth_buff;
  delete [] ydepth_buff;
  delete [] zdepth_buff;
  //delete [] skyimg_buff;
  x_image->release_memory();
  y_image->release_memory();
  z_image->release_memory();
  //sky_image->release_memory();
  delete [] global_bbox_buff;
  global_bbox_mem->release_memory();
}

bool boxm2_ocl_reg_depth_map_to_vol2::boxm2_ocl_register_world(vgl_rotation_3d<double> rot,
                                                               vgl_vector_3d<double> tx,
                                                               int nbins,
                                                               float & mi)
{
  int * joint_histogram_buff= new int[nbins*nbins];
  int * global_joint_histogram = new int[nbins*nbins];
  for (int k = 0 ; k<nbins*nbins; k++) {
    joint_histogram_buff[k] = 0;
    global_joint_histogram[k] = 0;
  }

  float translation_buff[4];
  translation_buff[0] = (float)tx.x();        translation_buff[2] = (float)tx.z();
  translation_buff[1] = (float)tx.y();        translation_buff[3] = 0.0f;

  bocl_mem_sptr translation = new bocl_mem(device_->context(), translation_buff, sizeof(float)*4, " translation " );
  //translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
  translation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  float rotation_buff[9];
  vnl_matrix_fixed<double, 3, 3> R = rot.as_matrix();

  rotation_buff[0] = (float)R(0,0);rotation_buff[3] = (float)R(1,0);rotation_buff[6] = (float)R(2,0);
  rotation_buff[1] = (float)R(0,1);rotation_buff[4] = (float)R(1,1);rotation_buff[7] = (float)R(2,1);
  rotation_buff[2] = (float)R(0,2);rotation_buff[5] = (float)R(1,2);rotation_buff[8] = (float)R(2,2);

  bocl_mem_sptr rotation = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
  //rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
  rotation->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  bocl_mem_sptr ocl_nbins = new bocl_mem(device_->context(), &(nbins), sizeof(int), "  #of bins" );
  //ocl_nbins->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
  ocl_nbins->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  float gpu_time = 0.0;

  for (unsigned i = 0; i < caches_.size(); i++) {
    bocl_mem_sptr joint_histogram = new bocl_mem(device_->context(), joint_histogram_buff, sizeof(int)*nbins*nbins, " joint histogram" );
    //joint_histogram->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    joint_histogram->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

    // output buffer for debugging
    bocl_mem_sptr output = new bocl_mem(device_->context(), output_buff, sizeof(float)*1000, "output" );
    //output->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

    ////////////////////////////////////////////
    boxm2_scene_sptr sceneB = caches_[i]->scene();

    vcl_vector<boxm2_block_id> sceneB_ids = sceneB->get_block_ids();
    boxm2_scene_info * sceneB_info = sceneB->get_blk_metadata( sceneB_ids[0] );

    vgl_box_3d<int> bbox = sceneB->bounding_box_blk_ids();

    bbox_buff[0] = 0; bbox_buff[3] = bbox.max_x()-bbox.min_x();
    bbox_buff[1] = 0; bbox_buff[4] = bbox.max_y()-bbox.min_y();
    bbox_buff[2] = 0; bbox_buff[5] = bbox.max_z()-bbox.min_z();

    bbox_buff[6] = sceneB_info->scene_dims[0];
    bbox_buff[7] = sceneB_info->scene_dims[1];
    bbox_buff[8] = sceneB_info->scene_dims[2];

    bocl_mem_sptr sceneB_bbox_ids = new bocl_mem(device_->context(), bbox_buff, 9*sizeof(int), " scene B bbox" );
    //sceneB_bbox_ids->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_bbox_ids->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    vgl_box_3d<double> scene_bbox = sceneB->bounding_box();

    sceneB_origin_buff[0] = (float)scene_bbox.min_x();
    sceneB_origin_buff[1] = (float)scene_bbox.min_y();
    sceneB_origin_buff[2] = (float)scene_bbox.min_z();

    bocl_mem_sptr sceneB_origin = new bocl_mem(device_->context(), sceneB_origin_buff, 4*sizeof(float), " scene B origin" );
    //sceneB_origin->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_origin->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    block_dims[0] = sceneB_info->scene_dims[0]*sceneB_info->block_len;
    block_dims[1] = sceneB_info->scene_dims[1]*sceneB_info->block_len;
    block_dims[2] = sceneB_info->scene_dims[2]*sceneB_info->block_len;
#if 0
    vcl_cout << "sub-block cnt/block " << sceneB_info->scene_dims[0] << ' ' << sceneB_info->scene_dims[1] << ' ' << sceneB_info->scene_dims[2] << '\n'
             << "sub-block len " << sceneB_info->block_len << '\n'
             << "scene block dims " << block_dims[0] << ' ' << block_dims[1] << ' ' << block_dims[2] << vcl_endl;
#endif
    bocl_mem_sptr sceneB_block_dims = new bocl_mem(device_->context(), block_dims, 4*sizeof(float), " scene B block dims" );
    //sceneB_block_dims->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_block_dims->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    subblk_num_buff[0] = sceneB_info->scene_dims[0];
    subblk_num_buff[1] = sceneB_info->scene_dims[1];
    subblk_num_buff[2] = sceneB_info->scene_dims[2];
    subblk_num_buff[3] = sceneB_info->scene_dims[3];

    bocl_mem_sptr sceneB_sub_block_num = new bocl_mem(device_->context(), subblk_num_buff, sizeof(int)* 4, " scene B sub block num" );
    //sceneB_sub_block_num->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_sub_block_num->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    bocl_mem_sptr sceneB_sub_block_len = new bocl_mem(device_->context(), &(sceneB_info->block_len), sizeof(float), " scene B sub block len" );
    //sceneB_sub_block_len->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    sceneB_sub_block_len->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    bocl_mem_sptr blks_ocl_B = new bocl_mem(device_->context(), caches_[i]->blk_buffer_, caches_[i]->total_bytes_per_block_, " block buffer B" );
    //blks_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    blks_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    bocl_mem_sptr blks_ocl_B_offsets = new bocl_mem(device_->context(), blk_offsets_array[i], (unsigned)(sizeof(unsigned int)*caches_[i]->blk_offsets_.size()), " block buffer B" );
    //blks_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    blks_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    bocl_mem_sptr alpha_ocl_B = new bocl_mem(device_->context(), caches_[i]->data_buffers_["alpha"], caches_[i]->total_bytes_per_data_["alpha"], " alpha buffer B " );
    //alpha_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    alpha_ocl_B->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    bocl_mem_sptr alpha_ocl_B_offsets = new bocl_mem(device_->context(), alpha_offsets_array[i], (unsigned)(sizeof(unsigned int)*caches_[i]->offsets_["alpha"].size()), " block buffer B" );
    //alpha_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR );
    alpha_ocl_B_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

    //vcl_cout<<"# of bytes for trees, alpha "<<caches_[i]->total_bytes_per_block_<< ' '<<caches_[i]->total_bytes_per_data_["alpha"]<<vcl_endl;
    ////////////////////////////////


    kern->set_arg(lookup.ptr());
    kern->set_arg(x_image.ptr());
    kern->set_arg(y_image.ptr());
    kern->set_arg(z_image.ptr());
    //kern->set_arg(sky_image.ptr());
#if 0
    kern->set_arg(sceneB_origin[i].ptr());
    kern->set_arg(sceneB_bbox_ids[i].ptr());
    kern->set_arg(sceneB_block_dims[i].ptr());
    kern->set_arg(sceneB_sub_block_len[i].ptr());
    kern->set_arg(blks_ocl_B[i].ptr());
    kern->set_arg(alpha_ocl_B[i].ptr());
    kern->set_arg(blks_ocl_B_offsets[i].ptr());
    kern->set_arg(alpha_ocl_B_offsets[i].ptr());
#endif
    kern->set_arg(sceneB_origin.ptr());
    kern->set_arg(sceneB_bbox_ids.ptr());
    kern->set_arg(sceneB_block_dims.ptr());
    //kern->set_arg(global_bbox_mem.ptr());
    kern->set_arg(sceneB_sub_block_len.ptr());
    kern->set_arg(blks_ocl_B.ptr());
    kern->set_arg(alpha_ocl_B.ptr());
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
    //if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status))) {
    // vcl_cout << i << " ERROR!!!!!!!!\n";
    // return false;
    //}
    gpu_time += kern->exec_time();

    output->read_to_buffer(queue);
    joint_histogram->read_to_buffer(queue);
    clFinish(queue);
    //clear render kernel args so it can reset em on next execution
    kern->clear_args();
#if 0
    vcl_cout << "joint:\n";
    for (int k = 0; k < nbins*nbins; k++)
      vcl_cout << joint_histogram_buff[k] << '\t';
#endif
    for (int k = 0; k < nbins*nbins; k++)
      global_joint_histogram[k] += joint_histogram_buff[k];

    for ( int k = 0 ; k <nbins*nbins; k++)
      joint_histogram_buff[k] = 0;
#if 0
    vcl_cout << "\nglobal:\n";
    for (int k = 0; k < nbins*nbins; k++)
      vcl_cout << global_joint_histogram[k] << '\t';
    vcl_cout << '\n';
#endif
  }

  clReleaseCommandQueue(queue);

  float sum = 0.0;
  for ( int k = 0 ; k <nbins*nbins; k++)
    sum+=global_joint_histogram[k];
  if (sum == 0)
   mi = 0.0;
  else
   mi = global_joint_histogram[nbins*nbins-1]/sum;
   //mi = (global_joint_histogram[0]+global_joint_histogram[nbins*nbins-1])/sum;

  delete [] global_joint_histogram;
  delete [] joint_histogram_buff;

  return true;
}

