// This is brl/bseg/bmrf/bmrf_epipole.h
#ifndef bmrf_epipole_h_
#define bmrf_epipole_h_
//:
// \file
// \brief A representation of an epipole
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 2/26/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_2d.h> 

//: A representation of an epipole
// Contains member function to convert between image
// and epipolar coordinates
class bmrf_epipole
{
 public:
  //: Constructor
  bmrf_epipole();
  bmrf_epipole(double u, double v);
  bmrf_epipole(const vgl_point_2d<double>& epipole);

  //: Conversion to epipolar coordinates
  vgl_point_2d<double> to_epi_coords(const vgl_point_2d<double>& u_v) const;
  void to_epi_coords(double u, double v, double &s, double &a) const;

  //: Conversion to image coordinates
  vgl_point_2d<double> to_img_coords(const vgl_point_2d<double>& s_a) const;
  void to_img_coords(double s, double a, double &u, double &v) const;

  const vgl_point_2d<double>& location() const { return epi_; } 

 private:
  //:the epipole location in image coordinates
  vgl_point_2d<double> epi_;

};

#endif // bmrf_epipole_h_
