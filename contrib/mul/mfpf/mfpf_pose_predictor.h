#ifndef mfpf_pose_predictor_h_
#define mfpf_pose_predictor_h_
//:
// \file
// \brief Uses regression to predict new pose from current sample
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_pose.h>
#include <mbl/mbl_chord.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Types of 2D transformation
// translation=(tx,ty), rigid=(tx,ty,A),
// zoom=(tx,ty,s), similarity = (tx,ty,scosA,ssinA)
enum mfpf_pose_type { translation,rigid,zoom,similarity };

std::ostream& operator<<(std::ostream&,const mfpf_pose_type&);

//: Uses regression to predict new pose from current sample.
//  Samples intensities in a region of interest, defined by the set of mbl_chords roi_.
//  These are in the bounding box [0,roi_ni_)x[0,roi_nj_).
//  Uses a pre-trained regression function to predict a
//  better position based on this sample.
class mfpf_pose_predictor
{
 private:
  //: Size of step between sample points
  double step_size_;

  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_x_;
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_y_;

  //: Chords defining the region of interest
  std::vector<mbl_chord> roi_;

  //: Size of bounding box of region of interest
  unsigned roi_ni_;
  //: Size of bounding box of region of interest
  unsigned roi_nj_;

  //: Number of pixels in region
  unsigned n_pixels_;

  //: Which sort of transformation to use
  mfpf_pose_type pose_type_;

  //: Which normalisation to use (0=none, 1=linear)
  short norm_method_;

  //: Lower bound on variance used in normalisation
  double var_min_;

  //: Matrix used to predict update dp=Rv+dp0
  vnl_matrix<double> R_;

  //: Offset in regression dp=Rv+dp0
  vnl_vector<double> dp0_;

  //: Define default values
  void set_defaults();

 public:

  // Dflt ctor
  mfpf_pose_predictor();

  // Destructor
  virtual ~mfpf_pose_predictor();

  //: Size of step between sample points
  void set_step_size(double);

  //: Size of step between sample points
  double step_size() const { return step_size_; }

  //: Which sort of transformation to use
  const mfpf_pose_type& pose_type() const
  { return pose_type_; }

  //: Which sort of transformation to use
  void set_pose_type(const mfpf_pose_type&);

  //: Define region to be used
  void set(const std::vector<mbl_chord>& roi,
           double ref_x, double ref_y,
           short norm_method=1);

  //: Initialise as a rectangle with ref. in centre.
  void set_as_box(unsigned ni, unsigned nj, short norm_method=1);

  //: Define model region as an ellipse with radii ri, rj
  //  Ref. point in centre.
  void set_as_ellipse(double ri, double rj,
                      short norm_method=1);

  //: Set regression matrices
  void set_predictor(const vnl_matrix<double>& R,
                     const vnl_vector<double>& dp0);

  //: Minimum variance used when normalising patch
  void set_var_min(double var_min) {var_min_=var_min;}

  //: Minimum variance used when normalising patch
  double var_min() const {return var_min_;}

  //: Radius of circle containing modelled region
  virtual double radius() const;

  unsigned n_pixels() const { return n_pixels_; }

  //: Get sample of region around specified point in image
  virtual void get_sample_vector(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 vnl_vector<double>& v);

  //: Sample at pose0 and predict a better pose (new_pose)
  virtual void new_pose(const vimt_image_2d_of<float>& image,
                          const mfpf_pose& pose0,
                          mfpf_pose& new_pose);


  //: Generate points in ref frame that represent boundary
  //  Points of a contour around the shape.
  //  Used for display purposes.
  virtual void get_outline(std::vector<vgl_point_2d<double> >& pts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_pose_predictor* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Prints ASCII representation of shape to os
  void print_shape(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Test equality
  bool operator==(const mfpf_pose_predictor& nc) const;
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_pose_predictor& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_pose_predictor& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mfpf_pose_predictor& b);


#endif
