// This is mul/vil2/vil2_crop.h
#ifndef vil2_crop_h_
#define vil2_crop_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott.

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view.h>
#include <vcl_cassert.h>


//: Create a view which is a cropped version of src.
// Doesn't modify underlying data. O(1).
// \relates vil2_image_view
// \return an ni x nj window of im with offset (x0,y0)
template<class T>
inline vil2_image_view<T> vil2_crop(const vil2_image_view<T> &im, unsigned i0,
                                    unsigned ni, unsigned j0, unsigned nj)
{
  assert(i0<im.ni()); assert(i0+ni<=im.ni());
  assert(j0<im.nj()); assert(j0+nj<=im.nj());
  return vil2_image_view<T>(im.memory_chunk(), im.top_left_ptr()+ i0*im.istep() + j0*im.jstep(),
    ni, nj, im.nplanes(), im.istep(), im.jstep(), im.planestep());
}


//: Crop to a region of src.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_crop(const vil2_image_resource_sptr &src, unsigned i0,
                                   unsigned ni, unsigned j0, unsigned nj);


//: A generic_image adaptor that behaves like a cropped version of its input
class vil2_crop_image_resource : public vil2_image_resource
{
 public:
  vil2_crop_image_resource(vil2_image_resource_sptr const&, unsigned i0, unsigned ni,
                           unsigned j0, unsigned nj);

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return ni_; }
  virtual unsigned nj() const { return nj_; }

  virtual enum vil2_pixel_format pixel_format() const { return src_->pixel_format(); }


  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni, 
                                                  unsigned j0, unsigned nj) const {
    if (i0 + ni > ni_ || j0 + nj > nj_) return 0;
    return src_->get_copy_view(i0+i0_, ni, j0+j0_, nj);
  }

  virtual vil2_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                             unsigned j0, unsigned nj) const {
    if (i0 + ni > ni_ || j0 + nj > nj_) return 0;
    return src_->get_view(j0+j0_, ni, j0+j0_, nj);
  }


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0,
                        unsigned j0) {
    return src_->put_view(im, i0+i0_, j0+j0_);
  }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const {
    return src_->get_property(tag, property_value);
  }

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 protected:
  vil2_image_resource_sptr src_;
  int i0_;
  int ni_;
  int j0_;
  int nj_;
};

#endif // vil2_crop_h_
