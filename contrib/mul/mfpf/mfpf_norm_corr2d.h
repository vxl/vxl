#ifndef mfpf_norm_corr2d_h_
#define mfpf_norm_corr2d_h_

//:
// \file
// \brief Searches over a grid using normalised correllation
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>

//: Searches over a grid using normalised correllation.
class mfpf_norm_corr2d : public mfpf_point_finder
{
private:
  //: Size of step between sample points
  double step_size_;

  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_x_;
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_y_;

  //: Filter kernel to search with
  vil_image_view<double> kernel_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Number of points either side of centre to search
  int search_nj_;

  //: Define default values
  void set_defaults();
public:

    //: Dflt ctor
  mfpf_norm_corr2d();

    //: Destructor
  virtual ~mfpf_norm_corr2d();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Define filter kernel to search with.
  //  Reference point set to the centre
  void set(const vil_image_view<double>& k);

  //: Define filter kernel to search with
  void set(const vil_image_view<double>& k, 
           double ref_x, double ref_y);

  //: Define search size
  virtual void set_search_area(unsigned ni, unsigned nj);

  int search_ni() const { return search_ni_; }
  int search_nj() const { return search_nj_; }

  //: Filter kernel to search with
  const vil_image_view<double>& kernel() const { return kernel_; }

   //: Evaluate match at p, using u to define scale and orientation 
   // Returns -1*edge strength at p along direction u
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
  bool operator==(const mfpf_norm_corr2d& nc) const;
};

#endif


