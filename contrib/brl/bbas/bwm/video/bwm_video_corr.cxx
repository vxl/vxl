#include "bwm_video_corr.h"
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
  if(iter != matches_.end())
    return false;
  matches_[frame]=pt;
  return true;
}

//: the lowest frame number for which there is a match
unsigned bwm_video_corr::min_frame()
{
  unsigned min = vnl_numeric_traits<unsigned>::maxval;
  vcl_map<unsigned, vgl_point_2d<double> >::iterator mit = matches_.begin();
  for(; mit != matches_.end(); ++mit)
    if((*mit).first < min)
      min = (*mit).first;
  return min;
}

//: the highest frame number for which there is a match
unsigned bwm_video_corr::max_frame()
{
  unsigned max = 0;
  vcl_map<unsigned, vgl_point_2d<double> >::iterator mit = matches_.begin();
  for(; mit != matches_.end(); ++mit)
    if((*mit).first > max)
      max = (*mit).first;
  return max;

}

bool bwm_video_corr::nearest_frame(unsigned frame, unsigned& near_frame)
{
  if(!matches_.size()) return false;
  bool found = false;
  unsigned closest = vnl_numeric_traits<unsigned>::maxval;
  vcl_map<unsigned, vgl_point_2d<double> >::iterator mit = matches_.begin();
  for(; mit != matches_.end(); ++mit)
    {
      unsigned fr = (*mit).first;
      int dif = frame - fr;
      if(dif<0) dif = -dif;
      unsigned d = static_cast<unsigned>(dif);
      if(d<closest){
        closest = d;
        near_frame = fr;
        found = true;
      }
  }
  return found;
}
