//:\brief a double type spesification of bgul_guassian_oint_3d
// \author Kongbin Kang
// 

#ifndef bugl_normal_point_3d_h_
#define bugl_normal_point_3d_h_

#include <bugl/bugl_gaussian_point_3d.h>
#include <vbl/vbl_ref_count.h>

class bugl_normal_point_3d : public vbl_ref_count
{
  private:
  bugl_gaussian_point_3d<double> p_;

  public:
  bugl_normal_point_3d() {}
  bugl_normal_point_3d(vgl_point_3d<double> &p, vnl_matrix_fixed<double, 3, 3> &s): p_(p, s) {}
  bugl_normal_point_3d(double x, double y, double z, vnl_matrix_fixed<double, 3, 3> & s) : p_(x, y, z, s) {}

  virtual ~bugl_normal_point_3d() {}

  vnl_matrix_fixed<double, 3, 3> get_covariant_matrix() const { return p_.get_covariant_matrix(); }
  void set_point(vgl_point_3d<double> &p) { p_.set_point(p);}
  void set_covariant_matrix(vnl_matrix_fixed<double, 3, 3> & sigma){ p_.set_covariant_matrix(sigma);}

  double prob_at(vgl_point_3d<double> &p) {return p_.prob_at(p);}
};


#endif
