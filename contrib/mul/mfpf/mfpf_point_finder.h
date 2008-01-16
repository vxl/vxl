#ifndef  mfpf_point_finder_h_
#define mfpf_point_finder_h_

//:
// \file
// \brief Base for classes which locate feature points
// \author Tim Cootes

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_cassert.h>
#include <vnl/vnl_vector.h>
#include <vcl_memory.h>

#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_vector_2d.h>

//: Base for classes which locate feature points
class mfpf_point_finder
{
protected:
public:

    //: Dflt ctor
  mfpf_point_finder();

    //: Destructor
  virtual ~mfpf_point_finder();

  //: Size of step between sample points
  virtual void set_step_size(double)=0;

  //: Define search region size
  //  During search, samples at points on grid [-ni,ni]x[-nj,nj], 
  //  with axes defined by u.
  virtual void set_search_area(unsigned ni, unsigned nj)=0;

   //: Evaluate match at p, using u to define scale and orientation 
   // Returns a qualtity of fit measure at the point (the smaller the better).
  virtual double evaluate(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)=0;

   //: Evaluate match at in a region around p 
   // Returns a qualtity of fit at a set of positions.
   // response image (whose size and transform is set inside the
   // function), indicates the points at which the function was
   // evaluated.  response(i,j) is the fit at the point
   // response.world2im().inverse()(i,j).  The world2im() transformation
   // may be affine.
  virtual void evaluate_region(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)=0;

   //: Search given image around p, using u to define scale and orientation 
   //  On exit, new_p and new_u define position, scale and orientation of 
   //  the best nearby match.  Returns a qualtity of fit measure at that
   //  point (the smaller the better).
  virtual double search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u)=0;

  //: Initialise from a string stream 
  virtual bool set_from_stream(vcl_istream &is);

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder* clone() const = 0;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const =0;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;

//: Create a concrete object, from a text specification.
  static vcl_auto_ptr<mfpf_point_finder> create_from_stream(vcl_istream &is);

};

  //: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_point_finder& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_point_finder& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_point_finder& b);

  //: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder& b);

  //: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder* b);

#endif


