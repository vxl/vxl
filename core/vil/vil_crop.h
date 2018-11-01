// This is core/vil/vil_crop.h
#ifndef vil_crop_h_
#define vil_crop_h_
//:
// \file
// \author Ian Scott.

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Create a view which is a cropped version of src.
// Doesn't modify underlying data. O(1).
// \relatesalso vil_image_view
// \return an n_i x n_j window of im with offset (i0,j0)
template<class T>
inline vil_image_view<T> vil_crop(const vil_image_view<T> &im, unsigned i0,
                                  unsigned n_i, unsigned j0, unsigned n_j)
{
  assert(i0<im.ni()); assert(i0+n_i<=im.ni());
  assert(j0<im.nj()); assert(j0+n_j<=im.nj());
  return vil_image_view<T>(im.memory_chunk(), im.top_left_ptr()+ i0*im.istep() + j0*im.jstep(),
                           n_i, n_j, im.nplanes(), im.istep(), im.jstep(), im.planestep());
}

//: Crop to a region of src.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_crop(const vil_image_resource_sptr &src, unsigned i0,
                                 unsigned n_i, unsigned j0, unsigned n_j);

//: A generic_image adaptor that behaves like a cropped version of its input
class vil_crop_image_resource : public vil_image_resource
{
 public:
  vil_crop_image_resource(vil_image_resource_sptr const&, unsigned i0, unsigned n_i,
                          unsigned j0, unsigned n_j);

  unsigned nplanes() const override { return src_->nplanes(); }
  unsigned ni() const override { return ni_; }
  unsigned nj() const override { return nj_; }

  enum vil_pixel_format pixel_format() const override { return src_->pixel_format(); }


  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j) const override;

  vil_image_view_base_sptr get_view(unsigned i0, unsigned n_i,
                                            unsigned j0, unsigned n_j) const override;


  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) override
  {
    return src_->put_view(im, i0+i0_, j0+j0_);
  }

  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override
  {
    return src_->get_property(tag, property_value);
  }

 protected:
  vil_image_resource_sptr src_;
  int i0_;
  int ni_;
  int j0_;
  int nj_;
};

#endif // vil_crop_h_
