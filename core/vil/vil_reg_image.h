// This is mul/vil2/vil2_reg_image.h
#ifndef vil2_reg_image_h_
#define vil2_reg_image_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class for handling image dimensionality and world to image transforms
// \author Ian Scott - Manchester
// There should be very litle interface to this class.


//: An abstract base class for handling image dimensionality and world to image transforms.

class vil2_reg_image
{
    //: Return class name
  virtual vcl_string is_a() const;

    //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;
};


#endif // vil2_reg_image_h_
