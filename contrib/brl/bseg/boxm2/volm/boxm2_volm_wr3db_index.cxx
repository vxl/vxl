#include "boxm2_volm_wr3db_index.h"
//:
// \file
#include <bbas/volm/volm_spherical_container.h>
#include <boxm2/volm/boxm2_volm_locations.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_algorithm.h>

bool boxm2_volm_wr3db_index_params::write_params_file(vcl_string index_file_name)
{
  vcl_string index_params_file = vul_file::strip_extension(index_file_name) + ".params";
  vcl_ofstream ofs(index_params_file.c_str());
  if (!ofs.is_open())
    return false;
  ofs << "start " << start << '\n'
      << "skip " << skip << '\n'
      << "vmin " << vmin << '\n'
      << "dmax " << dmax << '\n'
      << "solid_angle " << solid_angle << '\n'
#if 0
      << "cap_angle " << cap_angle << '\n'
      << "point_angle " << point_angle << '\n'
      << "top_angle " << top_angle << '\n'
      << "bottom_angle " << bottom_angle << '\n'
#endif
      << "layer_size " << layer_size << vcl_endl;
  ofs.close();
  return true;
}

bool boxm2_volm_wr3db_index_params::read_params_file(vcl_string index_file_name)
{
  vcl_string index_params_file = vul_file::strip_extension(index_file_name) + ".params";
  vcl_ifstream ifs(index_params_file.c_str());
  if (!ifs.is_open())
    return false;
  vcl_string tmp;
  ifs >> tmp >> start;
  ifs >> tmp >> skip;
  ifs >> tmp >> vmin;
  ifs >> tmp >> dmax;
  ifs >> tmp >> solid_angle;
#if 0
  ifs >> tmp >> cap_angle;
  ifs >> tmp >> point_angle;
  ifs >> tmp >> top_angle;
  ifs >> tmp >> bottom_angle;
#endif
  ifs >> tmp >> layer_size;
  ifs.close();
  return true;
}

bool boxm2_volm_wr3db_index_params::query_params_equal(boxm2_volm_wr3db_index_params& p1, boxm2_volm_wr3db_index_params& p2)
{
  return //p1.bottom_angle == p2.bottom_angle &&
         //p1.cap_angle == p2.cap_angle &&
         p1.dmax == p2.dmax &&
         //p1.point_angle == p2.point_angle &&
         p1.solid_angle == p2.solid_angle &&
         //p1.top_angle == p2.top_angle &&
         p1.vmin == p2.vmin &&
         p1.layer_size == p2.layer_size;
}


bool boxm2_volm_wr3db_index_params::write_size_file(vcl_string index_file_name, unsigned long indexed_cnt)
{
  vcl_string index_size_file = vul_file::strip_extension(index_file_name) + ".txt";
  vcl_ofstream ofs(index_size_file.c_str());
  if (!ofs.is_open())
    return false;
  ofs << indexed_cnt << '\n';
  ofs.close();
  return true;
}

bool boxm2_volm_wr3db_index_params::read_size_file(vcl_string index_file_name, unsigned long& size)
{
  vcl_string index_size_file = vul_file::strip_extension(index_file_name) + ".txt";
  vcl_ifstream ifs(index_size_file.c_str());
  if (!ifs.is_open())
    return false;
  ifs >> size;
  ifs.close();
  return true;
}


boxm2_volm_wr3db_index::boxm2_volm_wr3db_index(unsigned layer_size, float buffer_capacity) :
layer_size_(layer_size), buffer_size_(0), current_id_(0), current_global_id_(0), m_(NOT_INITIALIZED), file_name_(""), active_buffer_(0)
{
  buffer_size_ = (unsigned int)vcl_floor((buffer_capacity*1024*1024*1024)/(2.0f*layer_size));
  active_buffer_ = new uchar[buffer_size_*layer_size_];
}

boxm2_volm_wr3db_index::~boxm2_volm_wr3db_index()
{
  finalize();

  if (active_buffer_)
    delete [] active_buffer_;
}

bool boxm2_volm_wr3db_index::initialize_write(vcl_string file_name)
{
  if (m_ == READ)
    this->finalize();
  m_ = WRITE;
  file_name_ = file_name;
  //f_obj_.open(file_name.c_str(), vcl_ios::app | vcl_ios::binary);
  of_obj_.open(file_name.c_str(), vcl_ios::binary);
  if (!of_obj_.good())
    return false;
  current_id_ = 0;
  current_global_id_ = 0;
  return true;
}

bool boxm2_volm_wr3db_index::initialize_read(vcl_string file_name)
{
  if (m_ == WRITE)
    this->finalize();
  m_ = READ;
  file_name_ = file_name;
  if_obj_.open(file_name.c_str(), vcl_ios::in | vcl_ios::binary);
  if (!if_obj_.good())
    return false;
  current_id_ = 0;
  current_global_id_ = 0;
  file_size_ = vul_file::size(file_name);
  read_so_far_ = 0;
  return true;
}

bool boxm2_volm_wr3db_index::finalize()
{
  if (m_ == WRITE && current_id_ != 0) { // write whatever is on the cache
    of_obj_.write((char*)active_buffer_, (long)(current_id_*layer_size_));
    of_obj_.close();
  }
  if (m_ == READ)
    if_obj_.close();

  m_ = NOT_INITIALIZED;
  return true;
}


//: just appends to the end of the current active buffer, nothing about which location hypothesis these values correspond is known.
//  caller is responsible to keep the ordering consistent with the hypotheses ordering
bool boxm2_volm_wr3db_index::add_to_index(vcl_vector<uchar>& values)
{
  if (m_ == READ) {
    vcl_cout << "index object is in READ mode! cannot add to index!\n";
    return false;
  }
  if (values.size() != layer_size_) {
    vcl_cout << "In boxm2_volm_wr3db_index::add_to_index() -- size of value array is not consistent with layer size of this index object!\n";
    return false;
  }
  if (current_id_ == buffer_size_) {  // write the current cache
    // initiate a write
    of_obj_.write((char*)active_buffer_, (long)(buffer_size_*layer_size_));
    current_id_ = 0;
  }

  for (unsigned i = 0; i < layer_size_; i++)
    active_buffer_[current_id_*layer_size_ + i] = values[i];
  current_id_++;
  current_global_id_++;

  return true;
}

//: caller is responsible to pass a valid array of size layer_size
bool boxm2_volm_wr3db_index::add_to_index(uchar* values)
{
  if (m_ == READ) {
    vcl_cout << "index object is in READ mode! cannot add to index!\n";
    return false;
  }
  if (current_id_ == buffer_size_) {  // write the current cache
    // initiate a write
    of_obj_.write((char*)active_buffer_, (long)(buffer_size_*layer_size_));
    current_id_ = 0;
  }

  for (unsigned i = 0; i < layer_size_; i++)
    active_buffer_[current_id_*layer_size_ + i] = values[i];
  current_id_++;
  current_global_id_++;
  return true;
}

unsigned int boxm2_volm_wr3db_index::read_to_buffer(uchar* buf)
{
  long remaining = file_size_-read_so_far_;
  if ((long)(buffer_size_*layer_size_) <= remaining) {
    if_obj_.read((char*)buf, (long)(buffer_size_*layer_size_));
    read_so_far_ += (long)(buffer_size_*layer_size_);
    return buffer_size_;
  }
  else if (remaining > 0) {
    if_obj_.read((char*)buf, remaining);
    read_so_far_ += remaining;
    return remaining/layer_size_;
  }
  else
    return 0;
}

//: retrieve the next index, use the active_cache, if all on the active_cache has been retrieved, read from disc, values array is resized to layer_size
bool boxm2_volm_wr3db_index::get_next(vcl_vector<uchar>& values)
{
  if (m_ == WRITE) {
    vcl_cout << "index object is in WRITE mode! cannot read from index!\n";
    return false;
  }

  if (!current_global_id_ || current_id_ == active_buffer_size_) { // never read before or the cache is exhausted, get a chunk from the disc
    active_buffer_size_ = read_to_buffer(active_buffer_);
    if (!active_buffer_size_)
      return false;
    current_id_ = 0;
  }
  values.resize(layer_size_,0);
  for (unsigned i = 0; i < layer_size_; i++)
    values[i] = active_buffer_[current_id_*layer_size_ + i];
  current_global_id_++;
  current_id_++;
  return true;
}

//: caller is responsible to pass a valid array of size at least layer_size, if size>layer_size, fill the rest with zeros
bool boxm2_volm_wr3db_index::get_next(uchar* values, unsigned size)
{
  if (m_ == WRITE) {
    vcl_cout << "index object is in WRITE mode! cannot read from index!\n";
    return false;
  }

  if (!current_global_id_ || current_id_ == active_buffer_size_) { // never read before or the cache is exhausted, get a chunk from the disc
    active_buffer_size_ = read_to_buffer(active_buffer_);
    if (!active_buffer_size_)
      return false;
    current_id_ = 0;
  }
  for (unsigned i = 0; i < layer_size_; i++)
    values[i] = active_buffer_[current_id_*layer_size_ + i];
  //unsigned char val = (unsigned char)0;
  //for (unsigned i = layer_size_; i < size; i++)
  //  values[i] = val;
  vcl_fill(values+layer_size_, values+size, (unsigned char)0);
  current_global_id_++;
  current_id_++;
  return true;
}

//: inflate the index for ith location and return a vector of char values where last bit is visibility and second to last is prob (occupied or not)
bool boxm2_volm_wr3db_index::inflate_index_vis_and_prob(vcl_vector<uchar>& values,
                                                        volm_spherical_container_sptr cont,
                                                        vcl_vector<char>& vis_prob)
{
  // get the voxel on the indexed layer for a given voxel
  vcl_vector<volm_voxel>& voxels = cont->get_voxels();

  unsigned int offset, end_offset; double depth;
  cont->first_res(cont->min_voxel_res()*2, offset, end_offset, depth);

  vcl_map<double, unsigned int>& depth_offset_map = cont->get_depth_offset_map();
  vcl_map<double, unsigned int>::iterator iter = depth_offset_map.begin();
  vgl_point_3d<double> origin(0,0,0);
  unsigned char current_depth_interval = 0; // to count the depth intervals

  while (iter != depth_offset_map.end())
  {
    unsigned int begin = iter->second;
    iter++;
    unsigned int end;
    if (iter == depth_offset_map.end())
      end = (unsigned int)voxels.size();
    else
      end = iter->second;

    // if at the indexed layer use itself to compute vis and prob
    if (begin == offset) {
      for (unsigned ii = begin; ii < end; ii++) {
        unsigned char observed_depth_interval = values[ii-begin]; // depth of the voxel as observed at this voxel during indexing
        if (observed_depth_interval < current_depth_interval)
          vis_prob[ii] = (unsigned char)NONVIS_UNKNOWN;   // not visible
        else if (observed_depth_interval == current_depth_interval) // same depth interval
          vis_prob[ii] = (unsigned char)VIS_OCC;
        else   // visible but not yet occupied
          vis_prob[ii] = (unsigned char)VIS_UNOCC;
      }
    }
    else {
      for (unsigned ii = begin; ii < end; ii++)
      {
        // first find the voxel at the indexed layer that is closest
        unsigned int closest = 0;
        double theta_closest = vnl_math::pi;
        double dist_closest = 1000000.0;

        vgl_vector_3d<double> dir2 = voxels[ii].center_-origin;
        vgl_vector_3d<double> dir2n = dir2;
        normalize(dir2n);
        for (unsigned jj = offset; jj < end_offset; jj++) {
          vgl_vector_3d<double> dir1 = voxels[jj].center_-origin;
          vgl_vector_3d<double> dif = dir1-dir2;
          double dist = dif.length();
          normalize(dir1);
          double theta = vcl_acos(dot_product(dir1, dir2n));
          if (theta < theta_closest && dist < dist_closest) {
            closest = jj;
            theta_closest = theta;
            dist_closest = dist;
          }
        }

        unsigned char observed_depth_interval = values[closest-offset]; // depth of the voxel as observed at this voxel during indexing
        if (observed_depth_interval < current_depth_interval)
          vis_prob[ii] = NONVIS_UNKNOWN;   // not visible
        else if (observed_depth_interval == current_depth_interval) // same depth interval
          vis_prob[ii] = VIS_OCC;
        else   // visible but not yet occupied
          vis_prob[ii] = VIS_UNOCC;
      }
    }
    current_depth_interval++;
  }

  return true;
}

#if 0
bool boxm2_volm_wr3db_index::write_index(vcl_string out_file)
{
  vsl_b_ofstream os(out_file.c_str());
  if (!os)
    return false;
  if (!index_.size()) {
    vcl_cerr << "In boxm2_volm_wr3db_index::write_index() -- index size is zero, cannot write!\n";
    return false;
  }
  vsl_b_write(os, index_.size());
  vsl_b_write(os, index_[0].size());
  for (unsigned i = 0; i < index_.size(); i++) {
    for (unsigned k = 0; k < index_[i].size(); k++) {
      vsl_b_write(os, index_[i][k]);
    }
  }
  os.close();
  return true;
}

bool boxm2_volm_wr3db_index::read_index(vcl_string in_file)
{
  vsl_b_ifstream ifs(in_file.c_str());
  if (!ifs)
    return false;

  unsigned int size;
  unsigned int layer_size;
  vsl_b_read(ifs, size);
  vsl_b_read(ifs, layer_size);
  index_.resize(size);
  for (unsigned i = 0; i < size; i++) {
    index_[i].resize(layer_size);
    for (unsigned k = 0; k < layer_size; k++) {
      uchar val;
      vsl_b_read(ifs, val);
      index_[i][k] = val;
    }
  }
  ifs.close();
  return true;
}
#endif
