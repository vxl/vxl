#ifndef mfpf_profile_pdf_h_
#define mfpf_profile_pdf_h_

//:
// \file
// \brief Searches along a profile using a statistical model
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <mbl/mbl_cloneable_ptr.h>

//: Searches along a profile using a statistical model.
class mfpf_profile_pdf : public mfpf_point_finder
{
private:
  //: Size of step between sample points
  double step_size_;

  //: Kernel mask is [ilo_,ihi_]
  int ilo_;
  //: Kernel mask is [ilo_,ihi_]
  int ihi_;

  //: PDf for profile vector
  mbl_cloneable_ptr<vpdfl_pdf_base> pdf_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Define default values
  void set_defaults();
public:

    //: Dflt ctor
  mfpf_profile_pdf();

    //: Destructor
  virtual ~mfpf_profile_pdf();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Define size and PDF (clone taken)
  void set(int ilo, int ihi, const vpdfl_pdf_base& pdf);

  //: Define search size
  virtual void set_search_area(unsigned ni, unsigned nj);

  //: Kernel mask is [ilo_,ihi_]
  int ilo() const { return ilo_; }

  //: Kernel mask is [ilo_,ihi_]
  int ihi() const { return ihi_; }

  int search_ni() const { return search_ni_; }

  //: PDf for profile vector
  const vpdfl_pdf_base& pdf() const { return pdf_; }

   //: Evaluate match at p, using u to define scale and orientation 
   // Returns -1*logp(vector) at p along direction u
  virtual double evaluate(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u);

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
                        vimt_image_2d_of<double>& response);

   //: Search given image around p, using u to define scale and orientation 
   //  On exit, new_p and new_u define position, scale and orientation of 
   //  the best nearby match.  Returns a qualtity of fit measure at that
   //  point (the smaller the better).
  virtual double search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u);

  //: Initialise from a string stream
  virtual bool set_from_stream(vcl_istream &is);

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder* clone() const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Test equality
  bool operator==(const mfpf_profile_pdf& nc) const;
};

#endif


