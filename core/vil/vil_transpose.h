// This is mul/vil2/vil2_transpose.h
#ifndef vil2_transpose_h_
#define vil2_transpose_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott.

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view.h>


//: Create a view which appears as the transpose of this view.
//  i.e. transpose(i,j,p) = view(j,i,p). O(1).
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil2_transpose(const vil2_image_view<T>& v)
{
  // Create view with i and j switched
  return vil2_image_view<T>(v.memory_chunk(),v.top_left_ptr(),
                            v.nj(),v.ni(),v.nplanes(),
                            v.jstep(),v.istep(),v.planestep());
}


//: Transpose an image.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_transpose(const vil2_image_resource_sptr &src);


//: A generic_image adaptor that behaves like a transposed version of its input
class vil2_transpose_image_resource : public vil2_image_resource
{
  //: Reference to underlying image source
  vil2_image_resource_sptr src_;
  //: You can't construct one of these directly, use vil2_transpose() instead.
  vil2_transpose_image_resource(vil2_image_resource_sptr const&);
  friend vil2_image_resource_sptr vil2_transpose(const vil2_image_resource_sptr &src);

 public:
  inline unsigned nplanes() const { return src_->nplanes(); }
  inline unsigned ni() const { return src_->nj(); }
  inline unsigned nj() const { return src_->ni(); }

  inline enum vil2_pixel_format pixel_format() const { return src_->pixel_format(); }

  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const;

  virtual vil2_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                             unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0);

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const {
    return src_->get_property(tag, property_value); }
};

#endif // vil2_transpose_h_
