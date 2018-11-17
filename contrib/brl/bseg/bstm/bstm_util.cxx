#include "bstm_util.h"
#include <bstm/bstm_data_traits.h>
//:
// \file

#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>

#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_util.h>
#include <bpgl/algo/bpgl_project.h>

#include <bstm/bstm_block.h>
#include <bstm/bstm_time_block.h>
#include <bstm/bstm_time_tree.h>

bool bstm_util::query_point_color(bstm_scene_sptr &scene,
                                  bstm_cache_sptr &cache,
                                  const vgl_point_3d<double> &point,
                                  unsigned time,
                                  float &prob,
                                  vnl_vector_fixed<unsigned char, 3> &rgb) {
  vgl_point_3d<double> local;
  double local_t;
  bstm_block_id id;
  if (!scene->contains(point, id, local, time, local_t))
    return false;

  int index_x = (int)std::floor(local.x());
  int index_y = (int)std::floor(local.y());
  int index_z = (int)std::floor(local.z());
  bstm_block *blk = cache->get_block(id);
  bstm_block_metadata blk_data = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char, 16> treebits =
      blk->trees()(index_x, index_y, index_z);
  boct_bit_tree tree(treebits.data_block(), blk_data.max_level_);
  int bit_index = tree.traverse(local);
  int depth = tree.depth_at(bit_index);
  int data_offset = tree.get_data_index(bit_index, false);
  auto side_len =
      static_cast<float>(blk_data.sub_block_dim_.x() / ((float)(1 << depth)));

  // do the query with data_offset....
  bstm_time_block *time_blk = cache->get_time_block(id);
  vnl_vector_fixed<unsigned char, 8> time_treebits =
      time_blk->get_cell_tt(data_offset, local_t);

  bstm_time_tree time_tree(time_treebits.data_block(), blk_data.max_level_t_);
  int bit_index_t = time_tree.traverse(local_t - time_blk->tree_index(local_t));
  float min, max;
  time_tree.cell_range(bit_index_t, min, max);
  std::cout << "cell range: [" << min << ',' << max << "] ";
  int data_offset_t = time_tree.get_data_index(bit_index_t);

  bstm_data_base *alpha_base =
      cache->get_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix());
  float alpha = ((float *)alpha_base->data_buffer())[data_offset_t];

  std::string data_type;
  bool foundDataType = false;
  std::vector<std::string> apps = scene->appearances();
  int appTypeSize = 0; // just to avoid compiler warning about using potentially
                       // uninitialised value
  for (const auto & app : apps) {
    if (app == bstm_data_traits<BSTM_GAUSS_RGB>::prefix()) {
      data_type = app;
      foundDataType = true;
    }
  }

  if (!foundDataType) {
    std::cerr << "No gauss rgb...exiting\n";
    return false;
  }

  // store cell probability
  prob = 1.0f - std::exp(-alpha * side_len);

  vnl_vector_fixed<float, 3> intensity;
  bstm_data_base *int_base = cache->get_data_base(id, data_type);
  std::cout << "data_type: " << data_type << std::endl;
  vnl_vector_fixed<unsigned char, 8> color =
      ((vnl_vector_fixed<unsigned char, 8> *)
           int_base->data_buffer())[data_offset_t];
  if (data_type.find(bstm_data_traits<BSTM_GAUSS_RGB>::prefix()) !=
      std::string::npos) {
    intensity = expected_color(color);
  }
  rgb[0] = (unsigned char)(intensity[0] * 255.0f);
  rgb[1] = (unsigned char)(intensity[1] * 255.0f);
  rgb[2] = (unsigned char)(intensity[2] * 255.0f);

  return true;
}

vnl_vector_fixed<float, 3>
bstm_util::expected_color(vnl_vector_fixed<unsigned char, 8> mog) {
  float y = (float)mog[0] / 255.0f;
  float u = (float)mog[1] / 255.0f * 0.872f - 0.436f;
  float v = (float)mog[2] / 255.0f * 1.23f - 0.615f;

  vnl_vector_fixed<float, 3> exp_intensity;
  exp_intensity[0] = y + 1.1402508551881f * v;
  exp_intensity[1] = y - 0.39473137491174f * u - 0.5808092090311f * v;
  exp_intensity[2] = y + 2.0325203252033f * u;

  return exp_intensity;
}

bool bstm_util::query_point(bstm_scene_sptr &scene,
                            bstm_cache_sptr &cache,
                            const vgl_point_3d<double> &point,
                            unsigned time,
                            float &prob,
                            float &intensity) {
  vgl_point_3d<double> local;
  double local_t;
  bstm_block_id id;
  bool foundPtInScene = scene->contains(point, id, local, time, local_t);
  if (!foundPtInScene)
    return false;

  int index_x = (int)std::floor(local.x());
  int index_y = (int)std::floor(local.y());
  int index_z = (int)std::floor(local.z());
  bstm_block *blk = cache->get_block(id);
  bstm_block_metadata blk_data = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char, 16> treebits =
      blk->trees()(index_x, index_y, index_z);
  boct_bit_tree tree(treebits.data_block(), blk_data.max_level_);
  int bit_index = tree.traverse(local);
  int depth = tree.depth_at(bit_index);
  auto side_len =
      static_cast<float>(blk_data.sub_block_dim_.x() / ((float)(1 << depth)));
  int data_offset = tree.get_data_index(bit_index, false);

  std::cout << "Found point at bit index: " << bit_index
            << " and data offset: " << data_offset << std::endl;

  vgl_point_3d<double> localCenter = tree.cell_center(bit_index);
  vgl_point_3d<double> cellCenter(localCenter.x() + index_x,
                                  localCenter.y() + index_y,
                                  localCenter.z() + index_z);
  vgl_point_3d<double> cellCenter_global(
      float(cellCenter.x() * blk_data.sub_block_dim_.x() +
            blk_data.local_origin_.x()),
      float(cellCenter.y() * blk_data.sub_block_dim_.y() +
            blk_data.local_origin_.y()),
      float(cellCenter.z() * blk_data.sub_block_dim_.z() +
            blk_data.local_origin_.z()));

  vgl_box_3d<double> datBox(cellCenter_global,
                            side_len,
                            side_len,
                            side_len,
                            vgl_box_3d<double>::centre);
  std::cout << "Found point at box: " << datBox << ' ' << id << std::endl;

  // do the query with data_offset....
  bstm_time_block *time_blk = cache->get_time_block(id);
  vnl_vector_fixed<unsigned char, 8> time_treebits =
      time_blk->get_cell_tt(data_offset, local_t);

  bstm_time_tree time_tree(time_treebits.data_block(), blk_data.max_level_t_);
  int bit_index_t = time_tree.traverse(local_t - time_blk->tree_index(local_t));
  float min, max;
  time_tree.cell_range(bit_index_t, min, max);
  std::cout << "cell range: [" << min << ',' << max << "] ";
  int data_offset_t = time_tree.get_data_index(bit_index_t);

  bstm_data_base *alpha_base =
      cache->get_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix());

  auto *alphas = (float *)(alpha_base->data_buffer());
  float alpha = alphas[data_offset_t];

  // store cell probability
  prob = 1.0f - std::exp(-alpha * side_len);

  bstm_data_base *mog_base =
      cache->get_data_base(id, bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  intensity = ((vnl_vector_fixed<unsigned char, 8>)
                   mog_base->data_buffer()[data_offset_t])[0] /
              255.0f;

  return true;
}

bool bstm_util::verify_appearance(const std::vector<std::string> &apps,
                                  const std::vector<std::string> &valid_types,
                                  std::string &data_type,
                                  int &appTypeSize) {
  bool foundDataType = false;
  for (const auto & app : apps) {
    // look for valid types
    for (const auto & valid_type : valid_types) {
      if (app == valid_type) {
        foundDataType = true;
        data_type = app;
        appTypeSize = (int)bstm_data_info::datasize(app);
      }
    }
  }
  return foundDataType;
}

bool bstm_util::verify_appearance(const bstm_scene &scene,
                                  const std::vector<std::string> &valid_types,
                                  std::string &data_type,
                                  int &appTypeSize) {
  return verify_appearance(
      scene.appearances(), valid_types, data_type, appTypeSize);
}

// private helper method prepares an input image to be processed by update
vil_image_view_base_sptr
bstm_util::prepare_input_image(const vil_image_view_base_sptr& loaded_image,
                               bool force_grey) {
  // then it's an RGB image (assumes byte image...)
  if (loaded_image->nplanes() == 3 || loaded_image->nplanes() == 4) {
    // if not forcing RGB image to be grey
    if (!force_grey) {
      // load image from file and format it into RGBA
      vil_image_view_base_sptr n_planes =
          vil_convert_to_n_planes(4, loaded_image);
      vil_image_view_base_sptr comp_image =
          vil_convert_to_component_order(n_planes);
      auto *rgba_view =
          new vil_image_view<vil_rgba<vxl_byte> >(comp_image);

      // make sure all alpha values are set to 255 (1)
      vil_image_view<vil_rgba<vxl_byte> >::iterator iter;
      for (iter = rgba_view->begin(); iter != rgba_view->end(); ++iter) {
        (*iter) = vil_rgba<vxl_byte>(iter->R(), iter->G(), iter->B(), 255);
      }
      vil_image_view_base_sptr toReturn(rgba_view);
      return toReturn;
    } else {
      // load image from file and format it into grey
      auto *inimg =
          dynamic_cast<vil_image_view<vxl_byte> *>(loaded_image.ptr());
      vil_image_view<float> gimg(loaded_image->ni(), loaded_image->nj());
      vil_convert_planes_to_grey<vxl_byte, float>(*inimg, gimg);

      // stretch it into 0-1 range
      auto *floatimg =
          new vil_image_view<float>(loaded_image->ni(), loaded_image->nj());
      vil_convert_stretch_range_limited(
          gimg, *floatimg, 0.0f, 255.0f, 0.0f, 1.0f);
      vil_image_view_base_sptr toReturn(floatimg);
      return toReturn;
    }
  }

  // else if loaded planes is just one...
  if (loaded_image->nplanes() == 1) {
    // prepare floatimg for stretched img
    vil_image_view<float> *floatimg;
    if (auto *img_byte =
            dynamic_cast<vil_image_view<vxl_byte> *>(loaded_image.ptr())) {
      floatimg =
          new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
      vil_convert_stretch_range_limited(
          *img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    } else if (auto *img_float =
                   dynamic_cast<vil_image_view<float> *>(loaded_image.ptr())) {
      return vil_image_view_base_sptr(img_float);
    } else {
      std::cerr << "Failed to load image\n";
      return nullptr;
    }
    vil_image_view_base_sptr toReturn(floatimg);
    return toReturn;
  }

  // otherwise it's messed up, return a null pointer
  std::cerr << "Failed to recognize input image type\n";
  return nullptr;
}
