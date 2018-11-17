#include <utility>
#include "bstm_ocl_minfo_function.h"

#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vul/vul_timer.h>


bstm_ocl_minfo_function::bstm_ocl_minfo_function(const bocl_device_sptr& device, const bstm_scene_sptr& scene, const bstm_opencl_cache_sptr& opencl_cache,
                        vgl_box_3d<double> bb,vgl_rotation_3d<double> rot, vgl_vector_3d<double> T, float time1, float time2, std::string kernel_opt,int nbins ):
                        device_(device), scene_(scene), opencl_cache_(opencl_cache),bb_(bb), T_(T), time1_(time1), time2_(time2), R_(std::move(rot)), kernel_opt_(std::move(kernel_opt)),
                        app_nbins_(nbins), surf_nbins_(2), app_view_dir_num_(8)
{
  //get blocks that intersect the provided bounding box.
  relevant_blocks_ = scene_->get_block_ids(bb_, time1_);

  //compute pairs of blks tht need to be processed together
  populate_blk_mapping();

  //initialize ocl stuff.
  init_ocl_minfo();
  compile_kernel();
}


bool bstm_ocl_minfo_function::compile_kernel()
{
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "bit/time_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "statistics_library_functions.cl");
  src_paths.push_back(source_dir     + "view_dep_app_helper_functions.cl");
  src_paths.push_back(source_dir + "tracking/minfo.cl");
  this->kern_ = new bocl_kernel();
  return kern_->create_kernel(&device_->context(),device_->device_id(), src_paths, "estimate_mi_vol", kernel_opt_, "MI");
}


void bstm_ocl_minfo_function::init_ocl_minfo()
{
  //load cell centers
  centerX_ = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  centerY_ = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  centerZ_ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


  //load rotation,translation
  translation_buff[0] = T_.x();        translation_buff[2] = T_.z();
  translation_buff[1] = T_.y();        translation_buff[3] = 0.0;
  T_mem_ = new bocl_mem(device_->context(), translation_buff, sizeof(cl_float)*4, " translation " );
  T_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  vnl_matrix_fixed<double, 3, 3> R = R_.as_matrix();
  rotation_buff[0] = R(0,0);rotation_buff[3] = R(1,0);rotation_buff[6] = R(2,0);
  rotation_buff[1] = R(0,1);rotation_buff[4] = R(1,1);rotation_buff[7] = R(2,1);
  rotation_buff[2] = R(0,2);rotation_buff[5] = R(1,2);rotation_buff[8] = R(2,2);
  rotation_buff[9] = 0;rotation_buff[10] = 0;rotation_buff[11] = 0;
  R_mem_ = new bocl_mem(device_->context(), rotation_buff, sizeof(cl_float)*16, " rotation " );
  R_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  std::cout << "ROT: " << R << std::endl;

  //load time1 and time 2 (local coordinates)
  double local_time1,local_time2;
  scene_->get_block_metadata(relevant_blocks_[0]).contains_t((double)time1_,local_time1);
  scene_->get_block_metadata(relevant_blocks_[0]).contains_t((double)time2_,local_time2);
  std::cout << "Local times: " << (cl_float) local_time1 << " " << (cl_float) local_time2 << std::endl;
  times[0] = (float) local_time1;
  times[1] = (float) local_time2;
  times[2] = 0;
  times[3] = 0;
  times_mem_ = new bocl_mem(device_->context(), times, sizeof(cl_float)* 4, " time buf" );
  times_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  //num bins buffer
  num_bins_buff_[0] = app_nbins_;
  num_bins_buff_[1] = surf_nbins_;
  num_bins_mem_  = new bocl_mem(device_->context(), num_bins_buff_, sizeof(cl_uint)* 4, " num_bins buf" );
  num_bins_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  //bounding box buffer
  bb_buf_[0] = bb_.min_x(); bb_buf_[1] = bb_.min_y(); bb_buf_[2] = bb_.min_z();
  bb_buf_[3] = bb_.max_x(); bb_buf_[4] = bb_.max_y(); bb_buf_[5] = bb_.max_z();
  bb_mem_ = new bocl_mem(device_->context(), bb_buf_, sizeof(cl_float)* 8, " bounding box buf" );
  bb_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  //setup intensity histogram
  app_joint_histogram_buff = new cl_float[app_view_dir_num_ *app_nbins_*app_nbins_];
  for (unsigned int k = 0; k<app_nbins_*app_nbins_* app_view_dir_num_; ++k)
    app_joint_histogram_buff[k] = 0;
  app_joint_hist_mem_  = new bocl_mem(device_->context(), app_joint_histogram_buff, sizeof(cl_uint)*app_nbins_*app_nbins_* app_view_dir_num_, " app joint histogram" );
  app_joint_hist_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //setup surface prob histogram
  surf_joint_histogram_buff = new cl_float[ surf_nbins_*surf_nbins_];
  for (unsigned int k = 0; k<surf_nbins_*surf_nbins_ ; ++k)
    surf_joint_histogram_buff[k] = 0.0f;
  surf_joint_hist_mem_  = new bocl_mem(device_->context(), surf_joint_histogram_buff, sizeof(cl_float)*surf_nbins_*surf_nbins_ , " surf joint histogram" );
  surf_joint_hist_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  // bit lookup buffer
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  lookup_=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // output buffer for debugging
  for (float & i : output_buff) i = 0;
  output_ = new bocl_mem(device_->context(), output_buff, sizeof(cl_float)*1000, "output" );
  output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
}


void bstm_ocl_minfo_function::populate_blk_mapping()
{
  //std::cout << "bbox: "; bb_.print(std::cout); std::cout << std::endl;

  std::vector<bstm_block_id>::const_iterator iter;
  for(iter = relevant_blocks_.begin(); iter != relevant_blocks_.end(); iter++)
  {
    //std::cout << "Processing blk " << *iter << std::endl;

    vgl_box_3d<double> block_bb =  scene_->get_block_metadata(*iter).bbox();
    //std::cout << "block  bbox: "; block_bb.print(std::cout); std::cout << std::endl;

    //intersect with bounding bb
    vgl_box_3d<double> block_intersection_bb = vgl_intersection(block_bb,bb_);
    //std::cout << "block intersection bbox: "; block_intersection_bb.print(std::cout); std::cout << std::endl;

    block_intersection_bb.scale_about_centroid(0.99);

    //now rotate and translate the box according to R,T
    vgl_orient_box_3d<double> oriented_intersection_box(block_intersection_bb, R_.as_quaternion());
    //get the corners of resulting box and insert them to an axis aligned box.
    std::vector<vgl_point_3d<double> > corners = oriented_intersection_box.corners();
    vgl_box_3d<double> aa_intersection_box;
    std::vector<vgl_point_3d<double> >::const_iterator pt_iter;
    //std::cout << "corners: " << std::endl;;
    for(pt_iter = corners.begin(); pt_iter != corners.end(); pt_iter++) {
      //std::cout << *pt_iter << std::endl;
      aa_intersection_box.add(*pt_iter + T_) ;
    }
    //std::cout << std::endl;
    //get blocks intersecting this box
    std::vector<bstm_block_id> query_relevant_blocks = scene_->get_block_ids(aa_intersection_box, time2_);
    std::vector<bstm_block_id>::const_iterator iter_query_blks;
    //for(iter_query_blks = query_relevant_blocks.begin(); iter_query_blks != query_relevant_blocks.end(); iter_query_blks++)
    //  std::cout << "--mapped to " << *iter_query_blks << std::endl;

    //insert into map
    relevant_block_mapping_[*iter] = query_relevant_blocks;
  }
  //std::cout << "Mapping done..." << std::endl;
}

float bstm_ocl_minfo_function::evaluate()
{
  float gpu_time = 0.0;
  float transfer_time=0.0f;

  //specify queue, local threads...
  std::size_t local_threads[1]={64};
  std::size_t global_threads[1]={1};
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()), CL_QUEUE_PROFILING_ENABLE,&status);


  std::vector<bstm_block_id>::const_iterator blk_iter;
  std::vector<bstm_block_id>::const_iterator target_blk_iter;
  for(blk_iter = relevant_blocks_.begin(); blk_iter != relevant_blocks_.end(); blk_iter++)
  {

    vul_timer transfer;

    //get data items for blk_iter
    bstm_block_metadata mdata = scene_->get_block_metadata(*blk_iter);
    global_threads[0] = (unsigned) RoundUp(mdata.sub_block_num_.x()*mdata.sub_block_num_.y()*mdata.sub_block_num_.z(),(int)local_threads[0]);

    bocl_mem* blk = opencl_cache_->get_block(*blk_iter);
    bocl_mem* blk_t = opencl_cache_->get_time_block(*blk_iter);
    bocl_mem* alpha = opencl_cache_->get_data(*blk_iter, bstm_data_traits<BSTM_ALPHA>::prefix());
    bocl_mem* app = opencl_cache_->get_data(*blk_iter, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());

    transfer_time += (float) transfer.all();

    std::vector<bstm_block_id> target_blks = relevant_block_mapping_[*blk_iter];
    for(target_blk_iter = target_blks.begin(); target_blk_iter != target_blks.end(); target_blk_iter++)
    {

      transfer.mark();

      //load target mem
      bocl_mem* target_blk = opencl_cache_->get_block(*target_blk_iter);
      bocl_mem* target_blk_t = opencl_cache_->get_time_block(*target_blk_iter);
      bocl_mem* target_alpha = opencl_cache_->get_data(*target_blk_iter, bstm_data_traits<BSTM_ALPHA>::prefix());
      bocl_mem* target_app = opencl_cache_->get_data(*target_blk_iter, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());

      //load blk info and write appropriate info to several buffers
      bocl_mem* blk_info = opencl_cache_->loaded_block_info();
      auto* info_buffer = (bstm_scene_info*) blk_info->cpu_buffer();

      //load scene origin
      target_blk_origin_buff[0] = info_buffer->scene_origin[0];
      target_blk_origin_buff[1] = info_buffer->scene_origin[1];
      target_blk_origin_buff[2] = info_buffer->scene_origin[2];
      target_blk_origin_buff[3] = info_buffer->scene_origin[3];
      target_blk_origin_ = new bocl_mem(device_->context(), target_blk_origin_buff, 4*sizeof(cl_float), " scene origin buff" );
      target_blk_origin_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );


      //loads scene dims (number of sub-blocks/trees in each block)
      target_subblk_num_buff[0] = info_buffer->scene_dims[0];
      target_subblk_num_buff[1] = info_buffer->scene_dims[1];
      target_subblk_num_buff[2] = info_buffer->scene_dims[2];
      target_subblk_num_buff[3] = info_buffer->scene_dims[3];
      target_subblk_num_ = new bocl_mem(device_->context(), target_subblk_num_buff, sizeof(cl_int)* 4, " scene sub block num" );
      target_subblk_num_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

      //load scene block len
      block_len_buff = info_buffer->block_len;
      scene_sub_block_len_ = new bocl_mem(device_->context(), &(block_len_buff), sizeof(cl_float), " scene sub block len" );
      scene_sub_block_len_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );


      //load current mem
      bocl_mem* blk = opencl_cache_->get_block(*blk_iter);
      bocl_mem* blk_t = opencl_cache_->get_time_block(*blk_iter);
      bocl_mem* alpha = opencl_cache_->get_data(*blk_iter, bstm_data_traits<BSTM_ALPHA>::prefix());
      bocl_mem* app = opencl_cache_->get_data(*blk_iter, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
      blk_info = opencl_cache_->loaded_block_info();

      transfer_time += (float) transfer.all();

      std::cout << "Processing blk " << *blk_iter << " and " << *target_blk_iter << std::endl;

      kern_->set_arg(blk_info);
      kern_->set_arg(centerX_.ptr());
      kern_->set_arg(centerY_.ptr());
      kern_->set_arg(centerZ_.ptr());
      kern_->set_arg(lookup_.ptr());

      kern_->set_arg(blk);
      kern_->set_arg(blk_t);
      kern_->set_arg(alpha);
      kern_->set_arg(app);

      kern_->set_arg(target_blk);
      kern_->set_arg(target_blk_t);
      kern_->set_arg(target_alpha);
      kern_->set_arg(target_app);

      kern_->set_arg(T_mem_.ptr());
      kern_->set_arg(R_mem_.ptr());
      kern_->set_arg(bb_mem_.ptr());


      kern_->set_arg(target_blk_origin_.ptr());
      kern_->set_arg(target_subblk_num_.ptr());
      kern_->set_arg(scene_sub_block_len_.ptr());
      kern_->set_arg(times_mem_.ptr());

      kern_->set_arg(num_bins_mem_.ptr());
      kern_->set_arg(app_joint_hist_mem_.ptr());
      kern_->set_arg(surf_joint_hist_mem_.ptr());

      kern_->set_arg(output_.ptr());

      kern_->set_local_arg(16*local_threads[0]*sizeof(cl_uchar)); // local trees
      kern_->set_local_arg( local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
      kern_->set_local_arg(16*local_threads[0]*sizeof(cl_uchar )); // local trees
      kern_->set_local_arg(TT_NUM_BYTES*local_threads[0]*sizeof(cl_uchar )); // local time trees
      kern_->set_local_arg(app_view_dir_num_*app_nbins_*app_nbins_*sizeof(cl_uint));
      kern_->set_local_arg(surf_nbins_*surf_nbins_*sizeof(cl_float));

      kern_->execute(queue, 1, local_threads, global_threads);
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      gpu_time += kern_->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern_->clear_args();
      kern_->release_current_event();
    }
  }


  app_joint_hist_mem_->read_to_buffer(queue);
  surf_joint_hist_mem_->read_to_buffer(queue);
  output_->read_to_buffer(queue);

  clReleaseCommandQueue(queue);

  std::cout << "Gpu time " << gpu_time / (double)1000 << " seconds. " << std::endl
           << "Transfer time " << transfer_time / (double)1000 << " seconds. " << std::endl;


  std::cout << "Output[0] = " << output_buff[0] << std::endl;
  std::cout << "Output[1] = " << output_buff[1] << std::endl;
  std::cout << "Output[2] = " << output_buff[2] << std::endl;
  std::cout << "Output[3] = " << output_buff[3] << std::endl;
  std::cout << "Output[4] = " << output_buff[4] << std::endl;
  std::cout << "Output[5] = " << output_buff[5] << std::endl;
  std::cout << "Output[6] = " << output_buff[6] << std::endl;
  std::cout << "Output[7] = " << output_buff[7] << std::endl;
  std::cout << "Output[8] = " << output_buff[8] << std::endl;
  std::cout << "Output[9] = " << output_buff[9] << std::endl;


  //Compute the MI of surface probabilities
  cl_float sum_surf_joint_hist = 0.0f;
  for (unsigned int i = 0; i < surf_nbins_* surf_nbins_; ++i)
    sum_surf_joint_hist += surf_joint_histogram_buff[i];
  for (unsigned int i = 0; i < surf_nbins_* surf_nbins_; ++i)
    surf_joint_histogram_buff[i] /= sum_surf_joint_hist;


  auto * surf_histA = new float[surf_nbins_];
  auto * surf_histB = new float[surf_nbins_];

  for (unsigned int k = 0; k<surf_nbins_; ++k)
    {
    surf_histA[k] = 0.0;  surf_histB[k] = 0.0;
    }
  surf_histA[0] = surf_joint_histogram_buff[0] + surf_joint_histogram_buff[1];
  surf_histA[1] = surf_joint_histogram_buff[2] + surf_joint_histogram_buff[3];
  surf_histB[0] = surf_joint_histogram_buff[0] + surf_joint_histogram_buff[2];
  surf_histB[1] = surf_joint_histogram_buff[1] + surf_joint_histogram_buff[3];

  /*
  std::cout << "The sum of joint hist: " << sum_surf_joint_hist << std::endl;
  std::cout << "Surf. joint hist  "<< surf_joint_histogram_buff[0] << " "
                                  << surf_joint_histogram_buff[1] << " "
                                  << surf_joint_histogram_buff[2] << " "
                                  << surf_joint_histogram_buff[3] << std::endl;


  std::cout << "Hist A " << surf_histA[0] << " " << surf_histA[1] << std::endl;
  std::cout << "Hist B " << surf_histB[0] << " " << surf_histB[1] << std::endl;
  */

  float surf_entropyA = 0;
  for (unsigned int k = 0; k < surf_nbins_; ++k)
    {
    surf_entropyA += -(surf_histA[k]?surf_histA[k]*std::log(surf_histA[k]):0); // if prob=0 this value is defined as 0
    }
  float surf_entropyB = 0;
  for (unsigned int l = 0; l < surf_nbins_; ++l)
    {
    surf_entropyB += -(surf_histB[l]?surf_histB[l]*std::log(surf_histB[l]):0); // if prob=0 this value is defined as 0
    }
  //delete[] surf_histA; delete[] surf_histB;

  std::cout << std::endl << "///////////////////////////////////////////////" << std::endl;
  float surf_entropyAB =  0.0; ;
  for (unsigned int k = 0; k < surf_nbins_; ++k)
    {
    for (unsigned int l = 0; l < surf_nbins_; ++l)
      {
      surf_entropyAB += -(surf_joint_histogram_buff[k*surf_nbins_+l]?surf_joint_histogram_buff[k*surf_nbins_+l]*std::log(surf_joint_histogram_buff[k*surf_nbins_+l]):0);
      std::cout << surf_joint_histogram_buff[k*surf_nbins_+l] << " ";
      }
    std::cout << std::endl;
    }
  std::cout << std::endl ;
  float mi_surf  = ( (surf_entropyA + surf_entropyB) /  surf_entropyAB); // /vnl_math::ln2;
  std::cout << "MI surf: " << mi_surf << std::endl;


  //Compute the MI of app probabilities
  std::cout << "MI app: ";


  float mi_app = 0.0f;

  auto * app_hist = new float[app_nbins_* app_nbins_*app_view_dir_num_];
  auto * app_histA = new float[app_nbins_];
  auto * app_histB = new float[app_nbins_];

  for (unsigned int view_dir_num = 0; view_dir_num < app_view_dir_num_; ++view_dir_num)
    {
    cl_float sum_app_joint_hist = 0.0f;
    for (unsigned int i = 0; i < app_nbins_* app_nbins_; ++i)
      {
      sum_app_joint_hist += app_joint_histogram_buff[ (app_nbins_* app_nbins_*view_dir_num) + i];
      }
    //if the num is below a threshold, no apps here.
    if (sum_app_joint_hist < 1e-07f)
      continue;

    for (unsigned int i = 0; i < app_nbins_* app_nbins_; ++i)
      app_hist[(app_nbins_* app_nbins_*view_dir_num) + i] =  app_joint_histogram_buff[(app_nbins_* app_nbins_*view_dir_num) + i] / sum_app_joint_hist;


    for (unsigned int k = 0; k<app_nbins_; ++k)
      {
      app_histA[k] = 0.0;
      app_histB[k] = 0.0;
      }

    for (unsigned int k = 0; k < app_nbins_; ++k)
      {
      for (unsigned int l = 0; l < app_nbins_; ++l)
        {
        app_histA[k]+=app_hist[(app_nbins_* app_nbins_*view_dir_num) + k*app_nbins_+l];
        }
      }
    for (unsigned int k = 0; k < app_nbins_; ++k)
      {
      for (unsigned int l = 0; l < app_nbins_; ++l)
        {
        app_histB[k]+=app_hist[(app_nbins_* app_nbins_*view_dir_num) + l*app_nbins_+k];
        }
      }

    float app_entropyA = 0;
    for (unsigned int k = 0; k < app_nbins_; ++k)
      {
      app_entropyA += -(app_histA[k]?app_histA[k]*std::log(app_histA[k]):0); // if prob=0 this value is defined as 0
      }
    float app_entropyB = 0;
    for (unsigned int l = 0; l < app_nbins_; ++l)
      {
      app_entropyB += -(app_histB[l]?app_histB[l]*std::log(app_histB[l]):0); // if prob=0 this value is defined as 0
      }
    //delete[] app_histA; delete[] app_histB;

    std::cout << std::endl << "///////////////////////////////////////////////" << std::endl;
    float app_entropyAB =  0.0; ;
    for (unsigned int k = 0; k < app_nbins_; ++k)
      {
      for (unsigned int l = 0; l < app_nbins_; ++l)
        {
        float val = app_hist[(app_nbins_* app_nbins_*view_dir_num) + k*app_nbins_+l];
        app_entropyAB += -(val?val*std::log(val):0);
        std::cout << val << " ";
        }
      std::cout << std::endl;
      }
    if(app_entropyAB > 0.0f)
      mi_app  += ( (app_entropyA + app_entropyB) / app_entropyAB) ;///vnl_math::ln2;
    std::cout << ( (app_entropyA + app_entropyB) / app_entropyAB)  << std::endl;
  }

  delete[] app_hist;
  delete[] app_histA;
  delete[] app_histB;
  delete[] surf_histA;
  delete[] surf_histB;
  return  mi_app/8 ;
}
