// This is mul/vil2/vil2_reg_image_2d_h_.h
#ifndef vil2_reg_image_2d_h_
#define vil2_reg_image_2d_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A class for handling images and world to image transforms
// \author Ian Scott - Manchester
// There should be very litle interface to this class.


//: This class links an image view and a transform.
template <class I, class T>
class vil2_reg_image_2d<I,T> : public vil2_reg_image
{
public:
  //: An image. Should be a vil2_image_view<T> or 3d version.
  I im;
  //: A world to image transform.
  T world2im;

    //: Return class name
  virtual vcl_string is_a() const;

    //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;
};


#endif // vil2_reg_image_2d_h_
