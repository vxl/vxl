// This is mul/vil2/file_formats/vil2_pnm.h
#ifndef vil2_pnm_file_format_h_
#define vil2_pnm_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
// \verbatim
// Modifications
// 7 June 2001 - Peter Vanroose - made pbm (magic P4) working
// 3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
// 20 Sep 2002  Ian Scott  - Coverted to vil2.
//\endverbatim

#include <vil/file_formats/vil_pnm.h>
#include <vil2/vil2_image_data.h>


class vil2_image_view_base;

//: Generic image implementation for PNM files
class vil2_pnm_generic_image : public vil2_image_data
{
  vil_stream* vs_;
  int magic_;
  int width_;
  int height_;
  unsigned long int maxval_;

  int start_of_data_;
  int components_;
  int bits_per_component_;

  bool read_header();
  bool write_header();

  friend class vil2_pnm_file_format;
 public:

  vil2_pnm_generic_image(vil_stream* is);
  vil2_pnm_generic_image(vil_stream* is, int planes,
                        int width,
                        int height,
                        int components,
                        int bits_per_component,
                        vil_component_format format);
  ~vil2_pnm_generic_image();

  //: Dimensions:  planes x width x height x components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Create a new view which is compatible with the underlying image type.
  // \param planewise, Do you want the data provided in planes (or components)?
  virtual vil2_image_view_base* new_view(bool planewise=true);

  //: Create a read/write view of the data.
  // Modifying this view might modify the actual data.
  // If you want to modify this data in place, call put_view after you done, and 
  // it should work efficiently.
  virtual bool get_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned width, unsigned height) const = 0;

  //: Create a read/write view of the data.
  // Modifying this view might modify the actual data.
  // If you want to modify this data in place, call put_view after you done, and 
  // it should work efficiently.
  virtual bool get_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned plane0, unsigned width, unsigned height, unsigned nplanes) const = 0;

  //: Create a read/write view of a copy of this data.
  virtual bool get_copy_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned width, unsigned height) const = 0;

  //: Create a read/write view of a copy of this data.
  virtual bool get_copy_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned plane0, unsigned width, unsigned height, unsigned nplanes) const = 0;

  //: Put the data in this view back into the image source.
  virtual bool put_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned plane0 = 0) = 0;

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil2_pnm_file_format_h_
