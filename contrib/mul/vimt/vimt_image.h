// This is mul/vimt/vimt_image.h
#ifndef vimt_image_h_
#define vimt_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class for images of any dimension and type
// \author Tim Cootes

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>

//: A base class for images of any dimension and type
//  Derived classes tend to have world to image transformations
//  attached to them, and to be able to act as `views' of
//  external data.
class vimt_image
{
  //: Shallow equality tester.
  //  The parameter must be identical type to this.
  virtual bool equals(const vimt_image &) const =0;

 public:
    //: Dflt ctor
  vimt_image() {}

    //: Destructor
  virtual ~vimt_image() {}

    //: Return dimensionality of image
  virtual unsigned n_dims() const = 0;

    //: Return vector indicating size of image in pixels
    //  2D image is v[0] x v[1],  3D image is v[0] x v[1] x v[2]
    //  Somewhat inefficient: Only use when you absolutely have to.
    //  Usually one only needs to know the size once one knows the exact type.
  virtual vcl_vector<unsigned> image_size() const = 0;

    //: Return vectors defining bounding box containing image in world co-ords
  virtual void world_bounds(vcl_vector<double>& b_lo,
                            vcl_vector<double>& b_hi) const = 0;

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const = 0;

    //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const&) const = 0;

    //: Create a copy on the heap and return base class pointer
  virtual vimt_image* clone() const = 0;

    //: Shallow equality.
    // tests if the two images are the same type, have equal transforms, and point
    // to the same image data with equal step sizes, etc.
  bool operator==(const vimt_image &) const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

    //: Print whole image to os
  virtual void print_all(vcl_ostream& os) const = 0;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const vimt_image& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const vimt_image& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, vimt_image& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const vimt_image& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const vimt_image* b);

//: Print class to os
void vsl_print_summary(vcl_ostream& os, const vimt_image& im);

#endif // vimt_image_h_
