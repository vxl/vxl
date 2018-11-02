// This is brl/bbas/bsta/algo/bsta_display_vrml.h
#ifndef bsta_display_vrml_h_
#define bsta_display_vrml_h_
//:
// \file
// \brief  Utility functions for displaying distributions
// \author J. L. Mundy
// \date   June 7, 2013
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <iostream>
#include <bsta/bsta_gaussian_full.h>
#include <bsta/bsta_mixture.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_svd_fixed.h>
#include <vgl/algo/vgl_rotation_3d.h>
//#define VRML_DEBUG
//: display bsta_gaussian_full to the stream as vrml. If single = true
// then a vmrl header is written to form a complete display string.
template<class T>
bool bsta_display_vrml(std::ostream& os, bsta_gaussian_full<T, 3> const& dist,
                       bool single = true, T r = T(0), T g = T(1), T b = T(0)){
  if(!os)
    {
      std::cout << "out stream is not open\n";
      return false;
    }
  const vnl_vector_fixed<T, 3> mean = dist.mean();
  const vnl_matrix_fixed<T, 3, 3> covar = dist.covar();
  //: Find the eigen system
  vnl_matrix<T> V; //eigenvectors
  vnl_vector<T> D; //eigenvalues
#ifdef VRML_DEBUG
  bool good =
#endif
 vnl_symmetric_eigensystem_compute<T>(covar, V, D);
#ifdef VRML_DEBUG
  assert(good);
  std::cout << "V \n" << V << '\n';
  std::cout << "V^t \n" << V.transpose() << '\n';
  std::cout << "D \n" << D << '\n';
  std::cout << "V * D * Vt \n" << V*vnl_diag_matrix<float>(D)*V.transpose() << '\n';
#endif
  vnl_matrix_fixed<T,3,3> R(V);
  vgl_rotation_3d<T> rot(R);
  vnl_vector_fixed<T, 3> axis = rot.axis();
  double ang = rot.angle();
  if(std::fabs(ang) < 1.0e-3){
    // covar is already diagonal
    D[0]= covar[0][0];    D[1]= covar[1][1]; D[2]= covar[2][2];
  }
  vnl_vector_fixed<T,3> temp=axis*T(ang);
  vgl_rotation_3d<T> rot_temp(temp);
#ifdef VRML_DEBUG
  std::cout << "recon rot \n" << rot_temp.as_matrix() << '\n';
  std::cout << "Axis, angle " << axis << ' ' << ang << '\n';
#endif
  if(single)
      os << "#VRML V2.0 utf8\n";
  os << "Transform {\n";
  os << " translation " << mean[0] << ' ' << mean[1] << ' ' << mean[2] << '\n';
  os << " children [\n";
  os << "  Transform{\n";
  os << "   rotation " << axis[0] << ' ' << axis[1] << ' '
     << axis[2] << ' ' << ang << '\n';
  os << "   children [\n";
  os << "    Transform{\n";
  os << "     scale " << std::sqrt(D[0]) << ' ' << std::sqrt(D[1]) << ' '
     << std::sqrt(D[2]) << '\n';
  os << "     children [ \n";
  os << "      Shape {\n";
  os << "       appearance Appearance {\n";
  os << "        material Material {diffuseColor "<< r << ' ' << g << ' '
     << b << " }\n";
  os << "      }\n";
  os << "       geometry Sphere {\n";
  os << "       radius 1.0\n";
  os << "       }\n";
  os << "      }\n";
  os << "     ]\n";
  os << "    }\n";
  os << "   ]\n";
  os << "  }\n";
  os << " ]\n";
  os << "}\n";
  return true;
}

template<class T>
bool bsta_display_vrml(std::ostream& os, bsta_mixture< bsta_gaussian_full<T, 3> > const& mix, T r = T(0), T g = T(1), T b = T(0)){
  if(!os)
    {
      std::cout << "out stream is not open\n";
      return false;
    }
  os << "#VRML V2.0 utf8\n";
  unsigned n_comp = mix.num_components();
  for(unsigned i = 0; i<n_comp; ++i){
    const bsta_gaussian_full<T, 3>& comp = mix.distribution(i);
    T w = mix.weight(i);
    bool good = bsta_display_vrml(os, comp, false, w*r, w*g, w*b);
    if(!good){
      std::cout << "display component failed\n";
      return false;
    }
  }
  return true;
}
#endif //bsta_display_vrml_h_
