// This is gel/vifa/vifa_image_histogram.h
#ifndef VIFA_IMAGE_HISTOGRAM_H
#define VIFA_IMAGE_HISTOGRAM_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Histogram of an image's overall intensity values.
//
// The vifa_image_histogram class is used to construct intensity histograms of
// the pixels in an image.
//
// \author Jim Farley, Constantinos Marinos
//
// \verbatim
//  Modifications:
//   MPP Jun 2003, Ported to VXL from TargetJr
// \endverbatim
//-----------------------------------------------------------------------------

#include <vil/vil_image_view_base.h>
#include <vifa/vifa_histogram.h>


class vifa_image_histogram : public vifa_histogram
{
 protected:
  enum vil_pixel_format  form_;
  unsigned int      byte_depth_;

 public:
  vifa_image_histogram(const vil_image_view_base_sptr&  image,
                       double                    percentage = 100.0
                      );

 protected:
  void init(void);
  void fill_histogram(const vil_image_view_base_sptr&  src_buf,
                      double                    percentage = 100.0
                     );
};


#endif  // VIFA_IMAGE_HISTOGRAM_H
