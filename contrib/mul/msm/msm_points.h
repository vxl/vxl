#ifndef msm_points_h_
#define msm_points_h_
//:
// \file
// \brief Set of 2D points, stored in a vnl_vector (x0,y0,x1,y1...)
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <string>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vimt/vimt_transform_2d.h>

//: Set of 2D points, stored in a vnl_vector (x0,y0,x1,y1...)
//  Get at raw vector with vector()
class msm_points
{
 private:
  vnl_vector<double> v_;
 public:

  // Dflt ctor
  msm_points();

  // Set to hold n points (initially all zero)
  msm_points(unsigned n);

  // Destructor
  ~msm_points();

  //: Vector storing point ordinates as (x0,y0,x1,y1...)
  const vnl_vector<double>& vector() const { return v_; }

  //: Vector storing point ordinates as (x0,y0,x1,y1...)
  // Non-const access - use with care
  vnl_vector<double>& vector() { return v_; }

  //: Number of points
  unsigned size() const { return v_.size()/2; }

  //: Set to hold n points (initially all (x,y))
  //  If n==size() then does not change anything.
  void set_size(unsigned n, double x=0, double y=0);

  //: Set point i
  void set_point(unsigned i, double x, double y)
  {
    assert(i<size()); v_[2*i]=x; v_[2*i+1]=y;
  }

  //: Set point i
  void set_point(unsigned i, const vgl_point_2d<double>& p)
  {
    assert(i<size()); v_[2*i]=p.x(); v_[2*i+1]=p.y();
  }

  //: Return i-th point
  vgl_point_2d<double> operator[](unsigned i) const
  {
    assert(i<size());
    return {v_[2*i],v_[2*i+1]};
  }

  //: Set this to be equal to supplied points
  void set_points(const std::vector<vgl_point_2d<double> >& pts);

  //: Copy points into pts
  void get_points(std::vector<vgl_point_2d<double> >& pts) const;

  //: Return centre of gravity of points
  vgl_point_2d<double> cog() const;

  //: Return RMS of distance of points to CoG.
  double scale() const;

  //: Compute centre of gravity and RMS distance to CoG
  void get_cog_and_scale(vgl_point_2d<double>& cog, double& scale) const;

  //: Scale current points by s about the origin
  void scale_by(double s);

  //: Translate current points by (tx,ty)
  void translate_by(double tx, double ty);

  //: Transform current points with similarity transform
  //  (x,y) -> (ax-by+tx, bx+ay+ty);
  void transform_by(double a, double b, double tx, double ty);

  //: Transform current points with t
  void transform_by(const vimt_transform_2d& t);

  //: Bounding box of points
  void get_bounds(vgl_point_2d<double>& b_lo,
                  vgl_point_2d<double>& b_hi) const;

  //: Return bounding box of points
  vgl_box_2d<double> bounds() const;

  //: Write out points to named text file
  //  Returns true if successful.
  bool write_text_file(const std::string& path) const;

  //: Read in points from named text file
  //  Returns true if successful.
  bool read_text_file(const std::string& path);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: Equality test
  bool operator==(const msm_points& points) const;
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_points& pts);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_points& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_points& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_points& pts);

#endif // msm_points_h_
