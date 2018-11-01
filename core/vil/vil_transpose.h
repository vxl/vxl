// This is core/vil/vil_transpose.h
#ifndef vil_transpose_h_
#define vil_transpose_h_
//:
// \file
// \author Ian Scott.

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>


//: Create a view which appears as the transpose of this view.
//  i.e. transpose(i,j,p) = view(j,i,p). O(1).
// \relatesalso vil_image_view
template<class T>
inline vil_image_view<T> vil_transpose(const vil_image_view<T>& v)
{
  // Create view with i and j switched
  return vil_image_view<T>(v.memory_chunk(),v.top_left_ptr(),
                           v.nj(),v.ni(),v.nplanes(),
                           v.jstep(),v.istep(),v.planestep());
}


//: Transpose an image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_transpose(const vil_image_resource_sptr &src);


//: A generic_image adaptor that behaves like a transposed version of its input
class vil_transpose_image_resource : public vil_image_resource
{
  //: Reference to underlying image source
  vil_image_resource_sptr src_;
  //: You can't construct one of these directly, use vil_transpose() instead.
  vil_transpose_image_resource(vil_image_resource_sptr const&);
  friend vil_image_resource_sptr vil_transpose(const vil_image_resource_sptr &src);

 public:
  inline unsigned nplanes() const override { return src_->nplanes(); }
  inline unsigned ni() const override { return src_->nj(); }
  inline unsigned nj() const override { return src_->ni(); }

  inline enum vil_pixel_format pixel_format() const override { return src_->pixel_format(); }

  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const override;

  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) override;

  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override {
    return src_->get_property(tag, property_value); }
};

#endif // vil_transpose_h_
