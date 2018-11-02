#include <iostream>
#include <fstream>
#include "bvxm_illum_util.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/algo/vnl_svd.h>
bool bvxm_illum_util::
load_surface_nhbds(std::string const& path,
                   std::vector<std::vector<vnl_matrix<float> > >& nhds)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
  {
    std::cerr << "In bvxm_illum_util::load_surface_nhbds(.) - "
             <<"neighborhood path is not valid\n";
    return false;
  }

  unsigned dim, ntracks;
  std::string temp;
  is >> temp;
  if (temp!="dim:"){
    std::cerr << "In bvxm_illum_util::load_surface_nhbds(.) - "
             <<"file parse error\n";
    return false;
  }
  is >> dim;
  is >> temp;
  if (temp!="n_tracks:"){
    std::cerr << "In bvxm_illum_util::load_surface_nhbds(.) - "
             <<"file parse error\n";
    return false;
  }
  is >> ntracks;
  for (unsigned i = 0; i<ntracks; ++i){
    std::vector<vnl_matrix<float> > nbs;
    is >> temp;
    if (temp!="n_i:") {
      std::cerr << "In bvxm_illum_util::load_surface_nhbds(.) - "
               <<"file parse error\n";
      return false;
    }
    unsigned ni;
    is >> ni;
    for (unsigned i = 0; i<ni; ++i)
    {
      vnl_matrix<float> m(dim, dim);
      is >> m;
      nbs.push_back(m);
    }
    nhds.push_back(nbs);
  }
  return true;
}

bool
bvxm_illum_util::load_illumination_dirs(std::string const& path,
                                        std::vector<vnl_double_3>& ill_dirs)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
  {
    std::cerr << "In bvxm_illum_util::load_illumination_dirs(.) - "
             <<"illumination dir path is not valid\n";
    return false;
  }
  ill_dirs.clear();
  std::string temp;
  is >> temp;
  if (temp != "n_dirs:")
  {
    std::cerr << "In bvxm_illum_util::load_illumination_dirs(.) - "
             <<"invalid file syntax\n";
    return false;
  }
  unsigned n_dirs = 0;
  is >> n_dirs;
  for (unsigned i = 0; i<n_dirs; ++i)
  {
    vnl_double_3 v;
    is >> v;
    ill_dirs.push_back(v);
  }
  return true;
}

bool bvxm_illum_util::
solve_lambertian_model(std::vector<vnl_double_3> const& ill_dirs,
                       std::vector<double> const& intensities,
                       vnl_double_4& model_params,
                       double& fitting_error)
{
  // form the "A" and "b" matrices
  unsigned m = ill_dirs.size(), n = 3;
  if (m<4) {
    std::cerr << "In bvxm_illum_util::solve_lambertian_model(.) - "
             <<"insufficient number of illumination dirs\n";
    return false;
  }
  vnl_matrix<double> S(m,n), A3x3(3,3), A(4,4);
  for (unsigned j = 0; j<m; ++j)
    for (unsigned i = 0; i<3; ++i)
      S[j][i]=ill_dirs[j][i];
  A3x3 = S.transpose()*S;
  double s0=0.0, s1=0.0, s2=0.0;
  double b0=0.0, b1=0.0, b2=0.0, b3=0.0;
  for (unsigned j=0; j<m; ++j){
    s0 += S[j][0]; s1 += S[j][1]; s2 += S[j][2];
    b0 += S[j][0]*intensities[j];
    b1 += S[j][1]*intensities[j];
    b2 += S[j][2]*intensities[j];
    b3 += intensities[j];
  }
  vnl_vector<double> b(4);
  b[0]=b0;   b[1]=b1;   b[2]=b2;   b[3]=b3;
  b /= m;
  for (unsigned j =0; j<3; ++j)
    for (unsigned i =0; i<3; ++i)
      A[j][i]=A3x3[j][i];
  A[3][0]=s0; A[0][3]=s0;
  A[3][1]=s1; A[1][3]=s1;
  A[3][2]=s2; A[2][3]=s2; A[3][3]=m;
  A /= m;

  //Solve for the model
  vnl_svd<double> svd(A);
  vnl_matrix<double> Ainv = svd.inverse();

  vnl_diag_matrix<double> D = svd.W();
  std::cout << "Singular values\n" << D << '\n';

  model_params = Ainv*b;

  // compute model fitting error
  double var = 0.0;
  for (unsigned j = 0; j<m; ++j)
  {
    double Im = S[j][0]*model_params[0];
    Im += S[j][1]*model_params[1]; Im += S[j][2]*model_params[2];
    Im += model_params[3];
    double Io = intensities[j];
    var += (Im-Io)*(Im-Io);
  }
  fitting_error = std::sqrt(var/m);
  return true;
}

double bvxm_illum_util::expected_intensity(vnl_double_3 const& illum_dir,
                                           vnl_double_4 const& model_params)
{
  double Im = 0.0;
  for (unsigned i = 0; i<3; ++i)
    Im += illum_dir[i]*model_params[i];
  Im += model_params[3];
  return Im;
}
