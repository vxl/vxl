//==========
// Kitware (c) 2010
//
// Restrictions applicable to use by the US Government:
//
// UNLIMITED RIGHTS
//
// Restrictions applicable for all other users:
//
// This software and all information and expression are the property of Kitware, Inc. All Rights Reserved.
//==========
//:
// \file
// \brief Image I/O for JPEG2000 imagery using OpenJPEG
// \author Chuck Atkins

#ifndef vil_openjpeg_h_
#define vil_openjpeg_h_

#include <vil/vil_fwd.h>
#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>

//: OpenJPEG Codec
enum vil_openjpeg_format
{
  VIL_OPENJPEG_JP2 = 1,
  VIL_OPENJPEG_JPT = 2,
  VIL_OPENJPEG_J2K = 3
};


//: Derived class for JPEG2000 imagery using OpenJPEG
class vil_openjpeg_file_format
{
 public:
  vil_image_resource_sptr make_input_image(vil_stream* vs,
                                           vil_openjpeg_format opjfmt);
  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                            unsigned int ni,
                                            unsigned int nj,
                                            unsigned int nplanes,
                                            vil_pixel_format format,
                                            vil_openjpeg_format opjfmt);
};

class vil_openjpeg_jp2_file_format : public vil_file_format,
                                     public vil_openjpeg_file_format
{
 public:
  const char * tag() const override { return "jp2"; }

  vil_image_resource_sptr make_input_image(vil_stream* vs) override
  {
    return static_cast<vil_openjpeg_file_format*>(this)->
           make_input_image(vs, VIL_OPENJPEG_JP2);
  }

  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned int ni,
                                                    unsigned int nj,
                                                    unsigned int nplanes,
                                                    vil_pixel_format format) override
  {
    return static_cast<vil_openjpeg_file_format*>(this)->
           make_output_image(vs, ni, nj, nplanes, format, VIL_OPENJPEG_JP2);
  }
};

#if 0
class vil_openjpeg_jpt_file_format : public vil_file_format,
                                     public vil_openjpeg_file_format
{
 public:
  virtual const char * tag() const { return "jpt"; }

  virtual vil_image_resource_sptr make_input_image(vil_stream* vs)
  {
    return static_cast<vil_openjpeg_file_format*>(this)->
           make_input_image(vs, VIL_OPENJPEG_JPT);
  }

  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned int ni,
                                                    unsigned int nj,
                                                    unsigned int nplanes,
                                                    vil_pixel_format format)
  {
    return static_cast<vil_openjpeg_file_format*>(this)->
           make_output_image(vs, ni, nj, nplanes, format, VIL_OPENJPEG_JPT);
  }
};
#endif // 0

class vil_openjpeg_j2k_file_format : public vil_file_format,
                                     public vil_openjpeg_file_format
{
 public:
  const char * tag() const override { return "j2k"; }

  vil_image_resource_sptr make_input_image(vil_stream* vs) override
  {
    return static_cast<vil_openjpeg_file_format*>(this)->
           make_input_image(vs, VIL_OPENJPEG_J2K);
  }

  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned int ni,
                                                    unsigned int nj,
                                                    unsigned int nplanes,
                                                    vil_pixel_format format) override
  {
    return static_cast<vil_openjpeg_file_format*>(this)->
           make_output_image(vs, ni, nj, nplanes, format, VIL_OPENJPEG_J2K);
  }
};

struct vil_openjpeg_image_impl;

//: Derived image resource for JPEG2000 imagery using OpenJPEG
class vil_openjpeg_image : public vil_image_resource
{
 public:
  vil_openjpeg_image (vil_stream* is,
                      unsigned int ni, unsigned int nj, unsigned int nplanes,
                      vil_pixel_format format, vil_openjpeg_format opjfmt);
  vil_openjpeg_image(vil_stream* is, vil_openjpeg_format opjfmt);
  ~vil_openjpeg_image(void) override;

  bool is_valid(void) const;

  // Inherit the documentation from vil_image_resource

  unsigned int nplanes() const override;
  unsigned int ni() const override;
  unsigned int nj() const override;
  enum vil_pixel_format pixel_format() const override;
  const char * file_format() const override;

  //: Reductions.
  // An image may supply lower resolutions.
  // This will return the number of reductions available.
  // 0 means only the full size image is available.
  virtual unsigned int nreductions() const;

  vil_image_view_base_sptr get_copy_view(
    unsigned int i0, unsigned int ni, unsigned int j0, unsigned int nj) const override;

  //: Create a read/write view of a copy of this data.
  // This is similar to get_copy_view, except that a reduction level may
  // be specified. On success, the returned data is reduced by 2^reduction.
  // Coordinates should be specified relative to the full-sized image.
  // \return 0 if the reduction is not available, or for any reason that
  // get_copy_view would return 0.
  virtual vil_image_view_base_sptr get_copy_view_reduced(
    unsigned i0, unsigned ni, unsigned j0, unsigned nj,
    unsigned reduction) const;

  bool put_view(const vil_image_view_base& im,
                        unsigned int i0, unsigned int j0) override;

  bool get_property(char const* tag, void* property_value = nullptr) const override;

 private:
  bool validate_format();

  int maxbpp(void) const;

  template<typename PIXEL_TYPE>
  vil_image_view_base_sptr opj2vil(
    void *opj_view,
    unsigned int i0, unsigned int ni, unsigned int j0, unsigned int nji) const;
//    vil_memory_chunk_sptr image_mem_ptr);

  vil_openjpeg_image_impl *impl_;
};

#endif
