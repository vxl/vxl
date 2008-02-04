#include "bwm_video_corr.h"
//:
// \file
#include "bwm_video_site_io_defs.h"
#include <vsl/vsl_basic_xml_element.h>
#include <vnl/vnl_numeric_traits.h>
unsigned bwm_video_corr::unique_id_ = 0;

bool bwm_video_corr::match(unsigned frame, vgl_point_2d<double> &pt)
{
  vcl_map<unsigned, vgl_point_2d<double> >::iterator iter = matches_.begin();

  iter = matches_.find(frame);
  if (iter != matches_.end()) {
    pt = iter->second;
    return true;
  }
  return false;
}

bool bwm_video_corr::match(unsigned frame)
{
  vcl_map<unsigned, vgl_point_2d<double> >::iterator iter = matches_.begin();
  iter = matches_.find(frame);
  return  iter != matches_.end();
}

bool bwm_video_corr::add(unsigned frame, vgl_point_2d<double> const& pt)
{
  //check if the match already exists
  vcl_map<unsigned, vgl_point_2d<double> >::iterator iter = matches_.begin();
  iter = matches_.find(frame);
  //if so return false
  if (iter != matches_.end())
    return false;
  matches_[frame]=pt;
  return true;
}

//: the lowest frame number for which there is a match
unsigned bwm_video_corr::min_frame()
{
  unsigned min = vnl_numeric_traits<unsigned>::maxval;
  vcl_map<unsigned, vgl_point_2d<double> >::iterator mit = matches_.begin();
  for (; mit != matches_.end(); ++mit)
    if ((*mit).first < min)
      min = (*mit).first;
  return min;
}

//: the highest frame number for which there is a match
unsigned bwm_video_corr::max_frame()
{
  unsigned max = 0;
  vcl_map<unsigned, vgl_point_2d<double> >::iterator mit = matches_.begin();
  for (; mit != matches_.end(); ++mit)
    if ((*mit).first > max)
      max = (*mit).first;
  return max;
}

bool bwm_video_corr::nearest_frame(unsigned frame, unsigned& near_frame)
{
  if (!matches_.size()) return false;
  bool found = false;
  unsigned closest = vnl_numeric_traits<unsigned>::maxval;
  vcl_map<unsigned, vgl_point_2d<double> >::iterator mit = matches_.begin();
  for (; mit != matches_.end(); ++mit)
  {
    unsigned fr = (*mit).first;
    int dif = frame - fr;
    if (dif<0) dif = -dif;
    unsigned d = static_cast<unsigned>(dif);
    if (d<closest){
      closest = d;
      near_frame = fr;
      found = true;
    }
  }
  return found;
}

void bwm_video_corr::x_write(vcl_ostream &os)
{
   vsl_basic_xml_element corr(CORRESP_TAG);
   corr.x_write_open(os);

   if (world_pt_valid_) {
    vsl_basic_xml_element xml_element(CORRESP_WORLD_PT_TAG);
    xml_element.add_attribute("X", world_pt().x());
    xml_element.add_attribute("Y", world_pt().y());
    xml_element.add_attribute("Z", world_pt().z());
    xml_element.x_write(os);
   }

   vcl_map<unsigned, vgl_point_2d<double> >::iterator
     iter = matches_.begin();
   for (; iter != matches_.end(); ++iter)
   {
     vgl_point_2d<double>& pt = (*iter).second;
     vsl_basic_xml_element ce(CORR_ELE);
     ce.add_attribute("fr", static_cast<int>((*iter).first));
     ce.add_attribute("u", pt.x());
     ce.add_attribute("v", pt.y());
     ce.x_write(os);
   }
   corr.x_write_close(os);
}
