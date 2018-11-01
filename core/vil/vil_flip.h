// This is core/vil/vil_flip.h
#ifndef vil_flip_h_
#define vil_flip_h_
//:
// \file
// \author Ian Scott.

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>


//: Create a reflected view in which i -> ni-1-i.
//  i.e. vil_flip_lr(view)(i,j,p) = view(ni-1-i,j,p)
//  O(1).
// \relatesalso vil_image_view
template<class T>
inline vil_image_view<T> vil_flip_lr(const vil_image_view<T>& v)
{
  return vil_image_view<T>(v.memory_chunk(),
                           v.top_left_ptr()+(v.ni()-1)*v.istep(),
                           v.ni(),v.nj(),v.nplanes(),
                           -v.istep(),v.jstep(),v.planestep());
}

//: Create a reflected view in which y -> nj-1-j.
//  i.e. vil_flip_ud(view)(i,j,p) = view(i,nj-1-j,p)
//  O(1).
// \relatesalso vil_image_view
template<class T>
inline vil_image_view<T> vil_flip_ud(const vil_image_view<T>& v)
{
  return vil_image_view<T>(v.memory_chunk(),
                           v.top_left_ptr()+(v.nj()-1)*v.jstep(),
                           v.ni(),v.nj(),v.nplanes(),
                           v.istep(),-v.jstep(),v.planestep());
}


//: Flip an image resource left to right.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_flip_lr(const vil_image_resource_sptr &src);


//: A generic_image adaptor that behaves like a flipped left to right version of its input
class vil_flip_lr_image_resource : public vil_image_resource
{
  vil_flip_lr_image_resource(vil_image_resource_sptr const&);
  friend vil_image_resource_sptr vil_flip_lr(const vil_image_resource_sptr &src);
 public:

  unsigned nplanes() const override { return src_->nplanes(); }
  unsigned ni() const override { return src_->ni(); }
  unsigned nj() const override { return src_->nj(); }

  enum vil_pixel_format pixel_format() const override { return src_->pixel_format(); }


  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const override;


  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0,
                        unsigned j0) override;

  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override {
    return src_->get_property(tag, property_value); }


 protected:
  //: Reference to underlying image source
  vil_image_resource_sptr src_;
};


//: Flip an image resource left to right.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_flip_ud(const vil_image_resource_sptr &src);


//: A generic_image adaptor that behaves like a flipped left to right version of its input
class vil_flip_ud_image_resource : public vil_image_resource
{
  vil_flip_ud_image_resource(vil_image_resource_sptr const&);
  friend vil_image_resource_sptr vil_flip_ud(const vil_image_resource_sptr &src);
 public:

  unsigned nplanes() const override { return src_->nplanes(); }
  unsigned ni() const override { return src_->ni(); }
  unsigned nj() const override { return src_->nj(); }

  enum vil_pixel_format pixel_format() const override { return src_->pixel_format(); }


  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const override;


  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0,
                        unsigned j0) override;

  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override {
    return src_->get_property(tag, property_value); }

 protected:
  //: Reference to underlying image source
  vil_image_resource_sptr src_;
};

#endif // vil_flip_h_
