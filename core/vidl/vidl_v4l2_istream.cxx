// This is core/vidl/vidl_v4l2_istream.cxx
#include <iostream>
#include "vidl_v4l2_istream.h"
//:
// \file
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//   30 Apr 2008 Created (A. Garrido)
//\endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vidl_frame.h"
#include "vidl_v4l2_pixel_format.h"


void vidl_v4l2_istream::update_frame()
{
  frame_number_=0;
#if 0
  if (!dev) return;
  std::cout << "Nbuf: " << dev->get_number_of_buffers() << std::endl;
  if (dev->get_number_of_buffers()!=1 && dev->is_capturing()){
    dev->stop_capturing();
    if (!dev->set_number_of_buffers(1)) // right now, only 1
      std::cout << "No puedo!" << std::endl;
    dev->start_capturing();
  }
  else if (!dev->set_number_of_buffers(1)) // right now, only 1
    std::cout << "No!" << std::endl;
#endif // 0
  dev.set_number_of_buffers(1);
  if (dev.ibuffer(0)) {
    cur_frame_ = new vidl_shared_frame(
        dev.ibuffer(0), dev.get_width(), dev.get_height(), v4l2_to_vidl(dev.get_v4l2_format()));
  }
  else {
    std::cout << "No Buf!" << std::endl;
    cur_frame_=0;
  }
}

vidl_v4l2_istream::vidl_v4l2_istream(vidl_v4l2_device& device): dev(device)
{
  update_frame();
}

bool vidl_v4l2_istream::advance()
{
   if (!dev.is_capturing())
    if (!dev.start_capturing())
       return false;
   if (cur_frame_!=0 && dev.read_frame()) {
     frame_number_++;
     return true;
   }
   else return false;
}

vidl_frame_sptr vidl_v4l2_istream::read_frame()
{
  advance();
  return cur_frame_;
}
