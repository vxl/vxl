// This is core/vil/file_formats/vil_tiff.h
#ifndef vil_tiff_file_format_h_
#define vil_tiff_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00
//
//\verbatim
//  Modifications
//  3 Oct 2001 Peter Vanroose - Implemented get_property and set_property
//  5 Jan 2002 Ian Scott      - Converted to vil
//  9 Dec 2003 Peter Vanroose - Added support for 1-bit pixel (bitmapped) images
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>

//: Loader for tiff files
class vil_tiff_file_format : public vil_file_format
{
 public:
  virtual char const *tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream *vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format);
};

struct vil_tiff_structures;


//: Generic image interface for TIFF files
class vil_tiff_image : public vil_image_resource
{
  vil_tiff_structures* p;

  unsigned int width_;
  unsigned int height_;

  unsigned int components_;
  unsigned int bits_per_component_;

  bool read_header();
  bool write_header();

  friend class vil_tiff_file_format;
 public:

  vil_tiff_image(vil_stream *is);
  vil_tiff_image (vil_stream* is, unsigned ni,
                  unsigned nj, unsigned nplanes, vil_pixel_format format);
  ~vil_tiff_image();

  //: TIFF specific fields relating to scanned images
  void get_resolution(float& x_res, float& y_res, unsigned short& units) const;
  void set_resolution(float x_res, float y_res, unsigned short units);


  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return components_; }
  virtual unsigned ni() const { return width_; }
  virtual unsigned nj() const { return height_; }

  virtual enum vil_pixel_format pixel_format() const;

  //: returns "tiff"
  char const *file_format() const;


  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  //: Return true if the property given in the first argument has been set.
  //  Currently, only "quantisation_depth" returns true;
  //  if the second argument is given in that case, and if it is a valid
  //  "unsigned int" pointer, the actual quantisation depth (number of relevant
  //  bits per pixel) is written into that memory location.
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil_tiff_file_format_h_
