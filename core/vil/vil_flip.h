// This is mul/vil2/vil2_flip.h
#ifndef vil2_flip_h_
#define vil2_flip_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott.

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view.h>


//: Create a reflected view in which i -> ni-1-i.
//  i.e vil2_flip_lr(view)(i,j,p) = view(ni-1-i,j,p)
//  O(1).
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil2_flip_lr(const vil2_image_view<T>& v)
{
  return vil2_image_view<T>(v.memory_chunk(),
                            v.top_left_ptr()+(v.ni()-1)*v.istep(),
                            v.ni(),v.nj(),v.nplanes(),
                            -v.istep(),v.jstep(),v.planestep());
}

//: Create a reflected view in which y -> nj-1-j.
//  i.e vil2_flip_ud(view)(i,j,p) = view(i,nj-1-j,p)
//  O(1).
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil2_flip_ud(const vil2_image_view<T>& v)
{
  return vil2_image_view<T>(v.memory_chunk(),
                            v.top_left_ptr()+(v.nj()-1)*v.jstep(),
                            v.ni(),v.nj(),v.nplanes(),
                            v.istep(),-v.jstep(),v.planestep());
}


//: Flip an image resource left to right.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_flip_lr(const vil2_image_resource_sptr &src);


//: A generic_image adaptor that behaves like a flipped left to right version of its input
class vil2_flip_lr_image_resource : public vil2_image_resource
{
  vil2_flip_lr_image_resource(vil2_image_resource_sptr const&);
  friend vil2_image_resource_sptr vil2_flip_lr(const vil2_image_resource_sptr &src);
 public:

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return src_->ni(); }
  virtual unsigned nj() const { return src_->nj(); }

  virtual enum vil2_pixel_format pixel_format() const { return src_->pixel_format(); }


  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni, 
                                                  unsigned j0, unsigned nj) const;

  virtual vil2_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                             unsigned j0, unsigned nj) const;


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0,
                        unsigned j0); 

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const {
    return src_->get_property(tag, property_value); }


  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 protected:
  //: Reference to underlying image source
  vil2_image_resource_sptr src_;
};


//: Flip an image resource left to right.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_flip_ud(const vil2_image_resource_sptr &src);


//: A generic_image adaptor that behaves like a flipped left to right version of its input
class vil2_flip_ud_image_resource : public vil2_image_resource
{
  vil2_flip_ud_image_resource(vil2_image_resource_sptr const&);
  friend vil2_image_resource_sptr vil2_flip_ud(const vil2_image_resource_sptr &src);
 public:

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return src_->ni(); }
  virtual unsigned nj() const { return src_->nj(); }

  virtual enum vil2_pixel_format pixel_format() const { return src_->pixel_format(); }


  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni, 
                                                  unsigned j0, unsigned nj) const;

  virtual vil2_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                             unsigned j0, unsigned nj) const;


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0,
                        unsigned j0); 

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const {
    return src_->get_property(tag, property_value); }


  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 protected:
  //: Reference to underlying image source
  vil2_image_resource_sptr src_;
};

#endif // vil2_flip_h_
