// This is mul/vil2/vil2_plane.h
#ifndef vil2_plane_h_
#define vil2_plane_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott.

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view.h>



//: Return a view of im's plane p.
//  O(1).
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil2_plane(const vil2_image_view<T> &im, unsigned p)
{
  assert(p<im.nplanes());
  return vil2_image_view<T>(im.memory_chunk(),im.top_left_ptr()+p*im.planestep(),im.ni(),im.nj(),1,
    im.istep(),im.jstep(),im.planestep());
}


//: Return a specific plane of an image.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_plane(const vil2_image_resource_sptr &src, unsigned p);


//: A generic_image adaptor that behaves like a single plane version of its input
// For implementation use only - use vil2_plane() to create one.
class vil2_plane_image_resource : public vil2_image_resource
{
  vil2_plane_image_resource(vil2_image_resource_sptr const&, unsigned p);
  friend vil2_image_resource_sptr vil2_plane(const vil2_image_resource_sptr &src, unsigned p);
 public:

  virtual unsigned nplanes() const { return 1; }
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
  virtual bool get_property(char const* tag, void* property_value = 0) const;

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 protected:
  //: Reference to underlying image source
  vil2_image_resource_sptr src_;
  //: which plane.
  unsigned plane_;
};

#endif // vil2_plane_h_
