// This is mul/vil3d/vil3d_crop.h
#ifndef vil3d_crop_h_
#define vil3d_crop_h_
//:
// \file


#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vcl_cassert.h>


//: Create a view that is a window onto an existing image.
// O(1).
template <class T>
vil3d_image_view<T> vil3d_crop(const vil3d_image_view<T>& im,
                               unsigned i0, unsigned ni, unsigned j0, unsigned nj,
                               unsigned k0, unsigned nk)
{
  assert(i0+ni<=im.ni());
  assert(j0+nj<=im.nj());
  assert(k0+nk<=im.nk());

  // Have to force the cast to avoid compiler warnings about const
  return vil3d_image_view<T>(im.memory_chunk(), &im(i0,j0,k0),
                             ni, nj, nk, im.nplanes(),
                             im.istep(), im.jstep(), im.kstep(), im.planestep());
}

//: Crop to a region of src.
// \relates vil3d_image_resource
vil3d_image_resource_sptr vil3d_crop(const vil3d_image_resource_sptr &src,
                                     unsigned i0, unsigned ni, 
                                     unsigned j0, unsigned nj,
                                     unsigned k0, unsigned nk);

//: A generic_image adaptor that behaves like a cropped version of its input
class vil3d_crop_image_resource : public vil3d_image_resource
{
 public:
  vil3d_crop_image_resource(vil3d_image_resource_sptr const&, 
                            unsigned i0, unsigned ni, 
                            unsigned j0, unsigned nj,
                            unsigned k0, unsigned nk);

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return ni_; }
  virtual unsigned nj() const { return nj_; }
  virtual unsigned nk() const { return nk_; }

  virtual enum vil_pixel_format pixel_format() const { return src_->pixel_format(); }


  virtual vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i, 
                                                   unsigned j0, unsigned n_j,
                                                   unsigned k0, unsigned n_k) const
  {
    if (i0 + n_i > ni() || j0 + n_j > nj() || k0 + n_k > nk()) return 0;
    return src_->get_copy_view(i0+i0_, n_i, j0+j0_, n_j, k0+k0_, n_k);
  }

  virtual vil3d_image_view_base_sptr get_view(unsigned i0, unsigned n_i, 
                                              unsigned j0, unsigned n_j,
                                              unsigned k0, unsigned n_k) const
  {
    if (i0 + n_i > ni() || j0 + n_j > nj() || k0 + n_k > nk()) return 0;
    return src_->get_view(i0+i0_, n_i, j0+j0_, n_j, k0+k0_, n_k);
  }


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil3d_image_view_base& im, unsigned i0, unsigned j0, unsigned k0)
  {
    return src_->put_view(im, i0+i0_, j0+j0_, k0+k0_);
  }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const
  {
    return src_->get_property(tag, property_value);
  }

 protected:
  vil3d_image_resource_sptr src_;
  int i0_;
  int ni_;
  int j0_;
  int nj_;
  int k0_;
  int nk_;
};


#endif // vil3d_crop_h_
