// This is core/vil/file_formats/vil_viff.h
#ifndef vil_viff_file_format_h_
#define vil_viff_file_format_h_
//:
// \file
// \brief  Loader for Khoros 1.0 images
// \author Peter Vanroose, ESAT, KULeuven.
// \date 17 Feb 2000
// VIFF is the "Visualization Image File Format" used by Khoros 1.0.
//
// \verbatim
//  Modifications
//    3 Oct  2001 Peter Vanroose - Implemented get_property("top_row_first")
//   21 Febr 2002 Maarten Vergauwen - Added access functions for [fi]spare[12]
//    5 June 2003 Peter Vanroose - bug fix for 64-bit platforms: data is 32-bit
//   14 June 2003 Peter Vanroose - ported from vil1
// \endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>
#include "vil_viffheader.h"
#include <vxl_config.h> // for vxl_uint_32

//: Loader for VIFF files, i.e., 1-banded Khoros 1.0 images
// This supports char, short, int, float and double pixel cell types.
// Adapted from the version by Reinhard Koch, Nov. 19, 1996.
class vil_viff_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned int ni, unsigned int nj, unsigned int nplanes,
                                                    vil_pixel_format format);
};

//: Generic image implementation for VIFF (Khoros) files
class vil_viff_image : public vil_image_resource
{
  vil_stream* is_;
  unsigned int ni_;
  unsigned int nj_;
  unsigned int nplanes_;

  int start_of_data_;
  enum vil_pixel_format format_;

  bool endian_consistent_;
  vil_viff_xvimage header_;

  bool read_header();
  bool write_header();
  bool check_endian();

  friend class vil_viff_file_format;

 public:
  vil_viff_image(vil_stream* is);
  vil_viff_image(vil_stream* is,
                 unsigned int ni, unsigned int nj, unsigned int nplanes,
                 vil_pixel_format format);
  ~vil_viff_image();

  //: Dimensions.  Planes x W x H
  virtual unsigned int ni() const { return ni_; }
  virtual unsigned int nj() const { return nj_; }
  virtual unsigned int nplanes() const { return nplanes_; }

  virtual enum vil_pixel_format pixel_format() const { return format_; }

  //: Return part of this as buffer
  virtual vil_image_view_base_sptr get_copy_view(unsigned int x0, unsigned int ni, unsigned int y0, unsigned int nj) const;
  //: Write buf into this at position (x0,y0)
  virtual bool put_view(vil_image_view_base const& buf, unsigned int x0, unsigned int y0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

  //: User defined spare values in header
  vxl_uint_32 ispare1() const { return header_.ispare1;}
  vxl_uint_32 ispare2() const { return header_.ispare2;}
  float fspare1() const { return header_.fspare1;}
  float fspare2() const { return header_.fspare2;}
  void set_ispare1(vxl_uint_32 ispare1);
  void set_ispare2(vxl_uint_32 ispare2);
  void set_fspare1(float fspare1);
  void set_fspare2(float fspare2);
};

#endif // vil_viff_file_format_h_
