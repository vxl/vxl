// This is core/vil/vil_decimate.h
#ifndef vil_decimate_h_
#define vil_decimate_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott.

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>


//: Create a view which is a decimated version of src.
// Doesn't modify underlying data. O(1).
// \relates vil_image_view
// The factor describes the number of input rows (or columns)
// that are equivalent to one output.
// If you don't specify the j_factor, it will be set equal to i_factor.
template<class T>
inline vil_image_view<T> vil_decimate(const vil_image_view<T> &im, unsigned i_factor,
                                      unsigned j_factor=0)
{
  if (j_factor==0) j_factor=i_factor;
  return vil_image_view<T>(im.memory_chunk(), im.top_left_ptr(),
                           im.ni()/i_factor, im.nj()/j_factor, im.nplanes(),
                           im.istep()*i_factor, im.jstep()*j_factor, im.planestep());
}

vil_image_view_base_sptr vil_decimate(const vil_image_view_base_sptr im, unsigned i_factor,
                                      unsigned j_factor=0);

//: decimate to a region of src.
// \relates vil_image_resource
vil_image_resource_sptr vil_decimate(const vil_image_resource_sptr &src,
                                     unsigned i_factor, unsigned j_factor=0);

//: A generic_image adaptor that behaves like a decimated version of its input
class vil_decimate_image_resource : public vil_image_resource
{
 public:
  vil_decimate_image_resource(vil_image_resource_sptr const&,
                              unsigned i_factor, unsigned j_factor);

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return src_->ni() / i_factor_; }
  virtual unsigned nj() const { return src_->nj() / j_factor_; }

  virtual enum vil_pixel_format pixel_format() const { return src_->pixel_format(); }


  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j) const;

  virtual vil_image_view_base_sptr get_view(unsigned i0, unsigned n_i,
                                            unsigned j0, unsigned n_j) const;


  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const
  {
    return src_->get_property(tag, property_value);
  }

 protected:
  vil_image_resource_sptr src_;
  unsigned i_factor_;
  unsigned j_factor_;
};

#endif // vil_decimate_h_
