#include <limits>
#include <iostream>
#include <fstream>
#include "volm_loc_hyp.h"
//:
// \file
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_polygon.h>
#include <bkml/bkml_write.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <cassert>
#include <vgl/vgl_distance.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: construct using a single dem file
void volm_loc_hyp::add(vgl_polygon<double>& poly, vil_image_view<float>& dem, vpgl_geo_camera* geocam, int inc_i, int inc_j, bool adjust_cam, char hemi, char dir)
{
  for (unsigned i = 0; i < dem.ni(); i+=inc_i)
    for (unsigned j = 0; j < dem.nj(); j+=inc_j) {
      double lon, lat;
      geocam->img_to_global(i,j,lon,lat);
      if (adjust_cam) {
        if (hemi == 'S') lat = -lat;
        if (dir == 'W') lon = -lon;
      }
      if (dem(i,j) <= 0)
        continue;
      if (!poly.contains(lon, lat))
        continue;
      // create the point such that lon is x and lat is y
      vgl_point_3d<double> pt(lon, lat, dem(i,j));
      locs_.push_back(pt);
    }
}

//: add the location to the set, WARNING: does not check if the hyp already exists!!
bool volm_loc_hyp::add(double lat, double lon, double elev)
{
  vgl_point_3d<double> pt(lon, lat, elev);
  locs_.push_back(pt);
  return true;
}


//: construct by reading from a binary file
volm_loc_hyp::volm_loc_hyp(const std::string& bin_file) : current_(0)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    std::cerr << "In boxm2_volm_loc_hypotheses::boxm2_volm_loc_hypotheses() -- cannot open: " << bin_file << std::endl;
    return;
  }
  this->b_read(is);
  is.close();
}

bool volm_loc_hyp::write_hypotheses(const std::string& out_file)
{
  vsl_b_ofstream os(out_file.c_str());
  if (!os)
    return false;
  this->b_write(os);
  os.close();
  return true;
}

bool volm_loc_hyp::get_next(vgl_point_3d<double>& h)
{
  if (current_ < locs_.size()) {
    h = locs_[current_];
    current_++;
    return true;
  }
  else
    return false;
}

//: return false when all hyps are returned, return in a sequence which started from start and incremented by skip, e.g. i+=skip
bool volm_loc_hyp::get_next(unsigned start, unsigned skip, vgl_point_3d<double>& h)
{
  if (current_ < start) {
    h = locs_[start];
    current_ = start+skip;
    return true;
  }
  if (current_ < locs_.size()) {
    h = locs_[current_];
    current_ += skip;
    return true;
  }
  return false;
}

//: get the hypothesis closest to the given and its id if get_next method were to be used,
double volm_loc_hyp::get_closest(double lat, double lon, vgl_point_3d<double>& h, unsigned& hyp_id)
{
  double min_dist = std::numeric_limits<double>::max();
  current_ = 0;
  for (unsigned i = 0; i < locs_.size(); ++i)
  {
    vgl_point_3d<double> query_pt(lon, lat, locs_[i].z());  // make the z's equal cause we don't care about elev
    double dist = vgl_distance(query_pt, locs_[i]);
    if (dist < min_dist) {
      min_dist = dist;
      h = locs_[i];
      hyp_id = i;
    }
  }
  return min_dist;
}

//: check if there exists a hyp closer than the given size and return its id if there is
//  warning: assumes that size is small enough so that euclidean distance approximates geodesic distance well
bool volm_loc_hyp::exist(double lat, double lon, double size, unsigned& id)
{
  vgl_point_2d<double> query_pt(lon, lat);  // make the z's equal cause we don't care about elev
  for (unsigned i = 0; i < locs_.size(); ++i)
  {
    vgl_point_2d<double> h_pt(locs_[i].x(), locs_[i].y());  // make the z's equal cause we don't care about elev
    double dist = vgl_distance(query_pt, h_pt);
    if (dist < size) {
      id = i;
      return true;
    }
  }
  return false;
}

//: Binary save self to stream.
void volm_loc_hyp::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, locs_.size());
  for (const auto & loc : locs_) {
    vsl_b_write(os, loc.x());
    vsl_b_write(os, loc.y());
    vsl_b_write(os, loc.z());
  }
}

//: Binary load self from stream.
void volm_loc_hyp::b_read(vsl_b_istream &is)
{
  current_ = 0;
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1: {
     std::cerr << "I/O ERROR: boxm2_volm_loc_hypotheses::b_read(vsl_b_istream&)\n"
              << "           Version 1 is not supported anymore!\n";
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
           }
   case 2: {
     unsigned cnt;
     vsl_b_read(is, cnt);
     for (unsigned i = 0; i < cnt; ++i) {
       double x, y, z;
       vsl_b_read(is, x);
       vsl_b_read(is, y);
       vsl_b_read(is, z);
       vgl_point_3d<double> pt(x,y,z);
       locs_.push_back(pt);
     }
     break;
     }
   default:
    std::cerr << "I/O ERROR: boxm2_volm_loc_hypotheses::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
  return;
}


//: create a kml file, size is in seconds, e.g. 0.001
void volm_loc_hyp::write_to_kml(const std::string& out_file, double size, bool const& write_as_dot)
{
  std::ofstream ofs(out_file.c_str());
  bkml_write::open_document(ofs);

  vnl_double_2 ul, ll, lr, ur;
  //double arcsecond = (n/2.0) * (1.0/3600.0);

  for (unsigned i = 0; i < locs_.size(); ++i) {

   double lon = locs_[i].x();
   double lat = locs_[i].y();
   double elev = locs_[i].z();
   std::stringstream str; str << i << '_' << lat << '_' << lon << '_' << elev;
   if (write_as_dot) {
     vgl_point_2d<double> pt(lon, lat);
     bkml_write::write_location(ofs, pt, str.str(), "location", 0.4);
   }
   else {
     ll[0] = lat; ll[1] = lon;
     ul[0] = lat+size; ul[1] = lon;
     lr[0] = lat; lr[1] = lon+size;
     ur[0] = lat+size; ur[1] = lon+size;
     bkml_write::write_box(ofs, str.str(), "location", ul, ur, ll, lr);
   }
  }

  bkml_write::close_document(ofs);
}
