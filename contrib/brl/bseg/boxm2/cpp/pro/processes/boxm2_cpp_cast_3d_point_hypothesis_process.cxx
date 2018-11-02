// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_cast_3d_point_hypothesis_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing a 3d point hypothesis at each voxel as an aggregate of points induced by each ray piercing through
//
// \author Ozge C. Ozcanli
// \date April 04, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/cpp/algo/boxm2_3d_point_hypothesis_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_cast_3d_point_hypothesis_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_cast_3d_point_hypothesis_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_cast_3d_point_hypothesis_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";  // perspective camera --> to retrieve cam center and principal axis
  input_types_[3] = "vpgl_camera_double_sptr";  // generic camera --> to retrieve direction of rays of pixels
  input_types_[4] = "vil_image_view_base_sptr";  // depth image
  input_types_[5] = "vil_image_view_base_sptr";  // depth variance image
  input_types_[6] = "vcl_string";  //image identifier
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_cast_3d_point_hypothesis_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_cast_3d_point_hypothesis_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr perspective_cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr depth_img_sptr=pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr var_img_sptr=pro.get_input<vil_image_view_base_sptr>(i++);
  std::string identifier = pro.get_input<std::string>(i++);

  auto * depth_img=dynamic_cast<vil_image_view<float> * > (depth_img_sptr.ptr());
  auto * var_img=dynamic_cast<vil_image_view<float> * > (var_img_sptr.ptr());

  auto* gcam = reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr());
  std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(gcam);
  if (vis_order.empty())
  {
    std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
    return true;
  }
  auto* pcam = reinterpret_cast<vpgl_perspective_camera<double>*>(perspective_cam.ptr());

  //: compute R_s --> the rotation that rotates v to v_s for each pixel
  vnl_matrix_fixed<double, 3, 3> R(0.0);
  auto* Rss = new vbl_array_2d<vnl_matrix_fixed<double, 3, 3> >(depth_img->ni(), depth_img->nj(), R);
  vgl_vector_3d<double> v = pcam->principal_axis();
  for (unsigned i = 0; i < depth_img->ni(); i++)
    for (unsigned j = 0; j < depth_img->nj(); j++) {
      vgl_ray_3d<double> ray_ij = gcam->ray(i,j);
      vgl_vector_3d<double> v_s = ray_ij.direction();
      vgl_rotation_3d<double> rot_s(v,v_s);
      (*Rss)[i][j] = rot_s.as_matrix();
      if (i == 0 && j == 0) {
        std::cout << "principal ray: " << v << std::endl;
        std::cout << "generic ray: " << v_s << std::endl;
        std::cout << "R_ss: \n" << rot_s << std::endl;
        std::cout << "R_ss*v = \n" << rot_s*v << std::endl;
      }
    }

  vil_image_view<float> vis_img(depth_img->ni(),depth_img->nj());
  vis_img.fill(1.0f);

  bool success=true;
  boxm2_3d_point_hypothesis_functor pass;

  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_FLOAT16>::prefix());

  std::vector<boxm2_block_id>::iterator id;
  boxm2_block_id bid;
  int data_index = 0;

#if 0
   // for debugging purposes, TODO: remove this
   //survey point 271,163
   //vgl_point_3d<double> point(251.21295,310.2511,258.47885);
   vgl_point_3d<double> point(309.583,251.252,258.228);
   float side_len;
   if (!boxm2_util::get_point_index(scene,cache,point,bid,data_index,side_len)) {
     std::cout << "Cannot locate the point: " << point << " in the scene!\n";
     return false;
   } else
     std::cout << "point: " << point << " is in block: " << bid << " index: " << data_index << std::endl;
 #endif

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    std::cout<<"Block id "<<(*id)<<' ';
    boxm2_block *   blk = cache->get_block(scene,*id);

    boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    std::size_t buf_len = alph->buffer_length();
    std::cout << "in blk: " << *id << " data buf len: " << buf_len/alphaTypeSize << "\n";

    //: first make sure that the database is removed from memory if it already exists
    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_POINT>::prefix(identifier));
    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT16>::prefix(identifier));

    //: now retrieve it with get_data_base_new method so that even if it exists on disc, a fresh one will be created
    boxm2_data_base *  pts = cache->get_data_base_new(scene,*id,boxm2_data_traits<BOXM2_POINT>::prefix(identifier),buf_len/alphaTypeSize*pointTypeSize,false);
    boxm2_data_base *  sums = cache->get_data_base_new(scene,*id,boxm2_data_traits<BOXM2_FLOAT16>::prefix(identifier),buf_len/alphaTypeSize*auxTypeSize,false);

    auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);

    pass.init_data(alph, pts, sums, depth_img, var_img, pcam, gcam, Rss, &vis_img, bid, data_index);
    success=success && cast_ray_per_block<boxm2_3d_point_hypothesis_functor>(pass,
                                                                             scene_info_wrapper->info,
                                                                             blk,
                                                                             cam,
                                                                             depth_img->ni(),
                                                                             depth_img->nj());
    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_POINT>::prefix(identifier));  // cache needs to be read-write cache for output blocks to be written before being discarded
    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT16>::prefix(identifier));
  }

  delete Rss;
  return success;
}



namespace boxm2_cpp_cast_3d_point_hypothesis_process2_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_cast_3d_point_hypothesis_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_cast_3d_point_hypothesis_process2_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";  // generic camera --> to retrieve direction of rays of pixels
  input_types_[3] = "vcl_string";  //image identifier
  input_types_[4] = "vcl_string";  // file that contains 3x3 camera center covariance
  input_types_[5] = "vcl_string";  // file that contains 3x3 camera orientation covariance
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_cast_3d_point_hypothesis_process2(bprb_func_process& pro)
{
  using namespace boxm2_cpp_cast_3d_point_hypothesis_process2_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  std::string identifier = pro.get_input<std::string>(i++);
  std::string cov_c_file = pro.get_input<std::string>(i++);
  std::string cov_v_file = pro.get_input<std::string>(i++);

  // read matrices from text file.
  std::ifstream ifs(cov_c_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << cov_c_file << std::endl;
    return false;
  }
  vnl_matrix_fixed<double, 3, 3> cov_C;
  ifs >> cov_C;
  ifs.close();
  ifs.open(cov_v_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << cov_v_file << std::endl;
    return false;
  }

  vnl_matrix_fixed<double, 3, 3> cov_v;
  ifs >> cov_v;
  ifs.close();


  auto* gcam = reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr());
  std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(gcam);
  if (vis_order.empty())
  {
    std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
    return true;
  }

  int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_FLOAT16>::prefix());
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

  boxm2_block_id bid;
  int data_index = 0;

  std::vector<boxm2_block_id>::iterator id;
// #if 1
//   // for debugging purposes, TODO: remove this
//   //survey point 271,163
//   //vgl_point_3d<double> point(251.21295,310.2511,258.47885);
//   vgl_point_3d<double> point(309.583,251.252,258.228);
//   boxm2_block_id bid; int data_index; float side_len;
//   if (!boxm2_util::get_point_index(scene,cache,point,bid,data_index,side_len)) {
//     std::cout << "Cannot locate the point: " << point << " in the scene!\n";
//     return false;
//   } else
//     std::cout << "point: " << point << " is in block: " << bid << " index: " << data_index << std::endl;
// #endif

  int covTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_COVARIANCE>::prefix());

  //: now do the second pass to compute 3x3 covariance matrix
  boxm2_3d_point_hypothesis_cov_functor pass2;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    std::cout<<"Block id "<<(*id)<<' ';
    boxm2_block *   blk = cache->get_block(scene,*id);

    boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    std::size_t buf_len = alph->buffer_length();
    std::cout << "in blk: " << *id << " data buf len: " << buf_len/alphaTypeSize << "\n";

    boxm2_data_base *  aux = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT16>::prefix(identifier),buf_len/alphaTypeSize*auxTypeSize,true);

    //: first make sure that the database is removed from memory if it already exists
    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_COVARIANCE>::prefix(identifier));

    //: now retrieve it with get_data_base_new method so that even if it exists on disc, a fresh one will be created
    boxm2_data_base *  covs = cache->get_data_base_new(scene,*id,boxm2_data_traits<BOXM2_COVARIANCE>::prefix(identifier),buf_len/alphaTypeSize*covTypeSize,false);

    auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(*id);
    pass2.init_data(covs, aux, cov_C, cov_v, bid, data_index);

    int data_buf_len = covs->buffer_length()/covTypeSize;
    std::cout << "in blk: " << *id << " data buf len: " << data_buf_len << "\n";
    boxm2_data_serial_iterator<boxm2_3d_point_hypothesis_cov_functor>(data_buf_len,pass2);

    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_COVARIANCE>::prefix(identifier)); // read-only flag is false so will be written to disc before being removed
    //cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT16>::prefix(identifier));
  }

  return true;
}
