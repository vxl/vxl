#include "acal_tile_data_manager.h"
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>


static bool
extract_tile_index(std::string const& tile_name, size_t& tidx)
{
  // tile_xx
  std::stringstream ss;
  std::size_t idx = tile_name.find("_");
  if (idx==std::string::npos)
    return false;
  size_t n = tile_name.size();
  idx++;
  for( ; idx<n; ++idx)
    ss << tile_name[idx];
  ss >> tidx;
  return true;
}


bool
acal_tile_data_manager::init_tile_paths(
    std::string const& tile_dir,
    std::string metadata_dir, bool add_crop)
{
  if(! vul_file::is_directory(tile_dir)){
    std::cout << tile_dir << " is not a directory" << std::endl;
    return false;
  }
  if(! vul_file::is_directory(metadata_dir)){
    std::cout << metadata_dir << " is not a directory" << std::endl;
    return false;
  }
  metadata_dir_ = metadata_dir;
  std::string image_metadata_path = metadata_dir + image_metadata_path_;

  Json::Value root;
  std::ifstream istr(image_metadata_path.c_str());
  if(istr){
    Json::Reader reader;
    bool good = reader.parse(istr, root);
    if(!good){
    std::string error_msg = reader.getFormattedErrorMessages();
    std::cout << "error in parsing file\n"<< error_msg << std::endl;
    root = Json::nullValue;
    return false;
    }
  }else{
    std::cout << "couldn't open " << image_metadata_path << " to read metadata" << std::endl;
    return false;
  }
  meta_.deserialize_image_meta(root);
  size_t n = meta_.img_meta_.size();
  for (size_t i = 0; i < n; ++i) {
    std::string iname = meta_.img_meta_[i].image_name_;
    if (add_crop)
      iname += "_crop";
    meta_img_names_to_id_[iname] = i;
  }

  tile_dir_ = tile_dir;
  std::string glb = tile_dir + "*";
  for (vul_file_iterator fn=glb; fn; ++fn) {
    std::string dir = fn();
    if( vul_file::is_directory(dir)){
      std::string temp = vul_file::strip_directory(dir);
      if(temp == "." || temp == "..")
        continue;
      size_t tile_idx = -1;
      if(!extract_tile_index(temp, tile_idx)){
        std::cout << "Bad syntax for tile directory " << temp << std::endl;
        return false;
      }
      tile_dirs_[tile_idx] = dir + "/";
    }
  }

  size_t n_dirs = tile_dirs_.size();
  if( n_dirs ==  0){
    tile_dirs_[0] = tile_dir_; // only one tile
  }
  // extract image names
  for(std::map<size_t, std::string>::iterator dit = tile_dirs_.begin();
      dit != tile_dirs_.end(); ++dit){
    size_t didx = dit->first;
    std::string tile_glb = dit->second + "/*" + image_format_ext_;
    std::map<size_t, std::string> image_names;
    for (vul_file_iterator fn=tile_glb; fn; ++fn) {
      std::string temp = fn();
      std::string temp2 = vul_file::strip_directory(temp);
      std::string name = vul_file::strip_extension(temp2);
      if(name == ""){
        std::cout << "null image name -- fatal" << std::endl;
        return false;
      }
      std::string match_mtx_name = vul_file::strip_extension(match_matrix_fname_);
      if (name == match_mtx_name)
        continue;
      size_t id = meta_img_names_to_id_[name];
      image_names[id] = name;
    }
    if (image_names.size() == 0) {
      std::cout << "Tile[" << didx << "] has no images with extension " << image_format_ext_ << std::endl;
      return false;
    }
    tile_image_names_[didx] = image_names;
  }
  std::string tile_metadata_path =  metadata_dir + tile_metadata_path_;

  Json::Value tile_root;
  std::ifstream tistr(tile_metadata_path.c_str());
  if(tistr){
    Json::Reader reader;
    bool good = reader.parse(tistr, tile_root);
    if(!good){
    std::string error_msg = reader.getFormattedErrorMessages();
    std::cout << "error in parsing file\n"<< error_msg << std::endl;
    root = Json::nullValue;
    return false;
    }
  }else{
    std::cout << "couldn't open " << tile_metadata_path << " to read metadata" << std::endl;
    return false;
  }
  meta_.deserialize_tile_meta(tile_root);
  size_t ntm = meta_.tile_meta_.size();
  if(ntm != tile_dirs_.size()){
    std::cout << "inconsisent number of tiles in image and tile metadata files" << std::endl;
    return false;
  }
  for(size_t i = 0; i<ntm; ++i)
    tile_bounds_[i]=meta_.tile_meta_[i].local_bb_;

  return true;
}


std::map<size_t, std::string>
acal_tile_data_manager::image_paths(size_t tile_id)
{
  std::map<size_t, std::string> ret;
  if(tile_image_names_.count(tile_id) == 0){
    std::cout << "tile id " << tile_id << " not in dataset - return empty map" << std::endl;
    return ret;
  }
  const std::map<size_t, std::string>&  image_names = tile_image_names_[tile_id];
  for(std::map<size_t, std::string>::const_iterator iit = image_names.begin();
      iit != image_names.end(); ++iit){
    std::string ipath = tile_dirs_[tile_id] + iit->second + image_format_ext_;
    ret[iit->first] = ipath;
  }
  return ret;
}


std::map<size_t, std::string>
acal_tile_data_manager::camera_paths(size_t tile_id)
{
  std::map<size_t, std::string> ret;
  if(tile_image_names_.count(tile_id) == 0){
    std::cout << "tile id " << tile_id << " not in dataset - return empty map" << std::endl;
    return ret;
  }
  const std::map<size_t, std::string>&  image_names = tile_image_names_[tile_id];
  for(std::map<size_t, std::string>::const_iterator iit = image_names.begin();
      iit != image_names.end(); ++iit){
    std::string cpath = tile_dirs_[tile_id] + iit->second + camera_format_ext_;
    ret[iit->first] = cpath;
  }
  return ret;
}
