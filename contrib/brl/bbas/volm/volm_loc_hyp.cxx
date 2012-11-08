#include "volm_loc_hyp.h"
//:
// \file
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_polygon.h>
#include <bkml/bkml_write.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vcl_cassert.h>

//: construct using a single dem file
volm_loc_hyp::volm_loc_hyp(vgl_polygon<double>& poly, vil_image_view<float>& dem, vpgl_geo_camera* geocam, int int_i, int int_j) : current_(0)  
{
  cnt_ = 0;
  for (unsigned i = 0; i < dem.ni(); i+=int_i)
    for (unsigned j = 0; j < dem.nj(); j+=int_j) {
      double lon, lat;
      geocam->img_to_global(i,j,lon,lat);
      if (dem(i,j) <= 0)
        continue;
      if (!poly.contains(lon, lat))
        continue;
      vcl_map<float, vcl_map<float, float> >::iterator iter = locs_.find((float)lat);
      if (iter != locs_.end()) { // lat exists, there is only one lat, lon for now so just add to the map of this lat
        vcl_map<float, float>& tmp = iter->second;
        tmp[(float)lon] = dem(i,j);
        cnt_++;
      } else {  // add this lat
        vcl_map<float, float> tmp;
        tmp[(float)lon] = dem(i,j);
        locs_[(float)lat] = tmp;
        cnt_++;
      }
    }
}

//: add the location to the set
bool volm_loc_hyp::add(float lat, float lon, float elev)
{
  vcl_map<float, vcl_map<float, float> >::iterator iter = locs_.find((float)lat);
  if (iter != locs_.end()) { // lat exists, there is only one lat, lon for now so just add to the map of this lat
   vcl_map<float, float>& tmp = iter->second;
   tmp[(float)lon] = elev;  // replaces an existing loc if any
   cnt_++;
  } else {  // add this lat
   vcl_map<float, float> tmp;
   tmp[(float)lon] = elev;
   locs_[(float)lat] = tmp;
   cnt_++;
  }
  return true;
}
  

//: construct by reading from a binary file
volm_loc_hyp::volm_loc_hyp(vcl_string bin_file) : current_(0)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    vcl_cerr << "In boxm2_volm_loc_hypotheses::boxm2_volm_loc_hypotheses() -- cannot open: " << bin_file << vcl_endl;
    return;
  }
  this->b_read(is);
  is.close();
}

bool volm_loc_hyp::write_hypotheses(vcl_string out_file)
{
  vsl_b_ofstream os(out_file.c_str());
  if (!os)
    return false;
  this->b_write(os);
  os.close(); 
  return true;
}

bool volm_loc_hyp::get_next(vgl_point_3d<float>& h) {
  if (!current_) {
    current_lat_iter_ = locs_.begin();
    current_lon_iter_ = (current_lat_iter_->second).begin();
  }

  if (current_lon_iter_ != (current_lat_iter_->second).end()) {
    h.set(current_lon_iter_->first,    // longitude is x (east)
          current_lat_iter_->first,    // latitude is y (north)
          current_lon_iter_->second);  // elev is z
    current_++;
    current_lon_iter_++;
    return true;
  } else {
    current_lat_iter_++;
    if (current_lat_iter_ != locs_.end()) {
      current_lon_iter_ = (current_lat_iter_->second).begin(); 
      h.set(current_lon_iter_->first,    // longitude is x (east)
            current_lat_iter_->first,    // latitude is y (north)
            current_lon_iter_->second);  // elev is z
      current_++;
      current_lon_iter_++;
      return true;
    } else
      return false;
  }
}

//: return false when all hyps are returned, return in a sequence which started from start and incremented by skip, e.g. i+=skip
bool volm_loc_hyp::get_next(unsigned start, unsigned skip, vgl_point_3d<float>& h)
{
  assert(skip > 0);
  if (current_ >= cnt_)
    return false;
  while (current_ < start)
    if (!get_next(h))
      return false;
  if (!get_next(h))  // this one retrieves the current needed
    return false;
  // now skip
  vgl_point_3d<float> dummy;
  unsigned end = current_+skip-1;  // the previous get_next already incremented by one
  while (current_ < end)
    if (!get_next(dummy))
      break;
  return true;
}
  

//: Binary save self to stream.
void volm_loc_hyp::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, cnt_);
  vsl_b_write(os, locs_.size());
  for (vcl_map<float, vcl_map<float, float> >::const_iterator iter = locs_.begin(); iter != locs_.end(); iter++) {
    vsl_b_write(os, iter->first);
    vsl_b_write(os, iter->second.size());
    for (vcl_map<float, float>::const_iterator it = iter->second.begin(); it != iter->second.end(); it++) {
      vsl_b_write(os, it->first);
      vsl_b_write(os, it->second);
    }
  }
}

//: Binary load self from stream.
void volm_loc_hyp::b_read(vsl_b_istream &is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1: {
     vsl_b_read(is, cnt_);
     unsigned read = 0;
     unsigned lat_cnt = 0;
     vsl_b_read(is, lat_cnt);
     for (unsigned i = 0; i < lat_cnt; i++) {
       float lat;
       vsl_b_read(is, lat);
       vcl_map<float, float> tmp;
       unsigned lon_cnt = 0;
       vsl_b_read(is, lon_cnt);
       for (unsigned j = 0; j < lon_cnt; j++) {
         float lon, height;
         vsl_b_read(is, lon);
         vsl_b_read(is, height);
         tmp[lon] = height;
         read++;
       }
       locs_[lat] = tmp;
     }
     if (read != cnt_) {
       vcl_cerr << "I/O ERROR: boxm2_volm_loc_hypotheses::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
       is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
     }
     break; }
   default:
    vcl_cerr << "I/O ERROR: boxm2_volm_loc_hypotheses::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

  




