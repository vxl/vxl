#include <fstream>
#include <iostream>
#include <cmath>
#include "brad_illum_util.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_brent_minimizer.h>
#include <vnl/vnl_cross.h>
#include <brad/brad_illum_cost_function.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

bool brad_load_surface_nhbds(std::string const& path,
                             std::vector<std::vector<vnl_matrix<float> > >& nhds)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
  {
    std::cerr << "In load_surface_nhbds(.) - neighborhood path is not valid\n";
    return false;
  }

  unsigned dim, ntracks;
  std::string temp;
  is >> temp;
  if (temp!="dim:") {
    std::cerr << "In load_surface_nhbds(.) - file parse error\n";
    return false;
  }
  is >> dim;
  is >> temp;
  if (temp!="n_tracks:") {
    std::cerr << "In load_surface_nhbds(.) - file parse error\n";
    return false;
  }
  is >> ntracks;
  for (unsigned i = 0; i<ntracks; ++i) {
    std::vector<vnl_matrix<float> > nbs;
    is >> temp;
    if (temp!="n_i:") {
      std::cerr << "In load_surface_nhbds(.) - file parse error\n";
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

bool brad_load_illumination_dirs(std::string const& path,
                                 std::vector<vnl_double_3>& ill_dirs)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
  {
    std::cerr << "In load_illumination_dirs(.) - illumination dir path is not valid\n";
    return false;
  }
  ill_dirs.clear();
  std::string temp;
  is >> temp;
  if (temp != "n_dirs:")
  {
    std::cerr << "In load_illumination_dirs(.) - invalid file syntax\n";
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

bool brad_load_norm_intensities(std::string const& path,
                                std::vector<double>& norm_ints)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
  {
    std::cerr << "In load_norm_intensities(.) - normlized intensity path is not valid\n";
    return false;
  }
  norm_ints.clear();
  std::string temp;
  is >> temp;
  if (temp != "n_ints:")
  {
    std::cerr << "In load_norm_intensities(.) - invalid file syntax\n";
    return false;
  }
  unsigned n_ints = 0;
  is >> n_ints;
  for (unsigned i = 0; i<n_ints; ++i)
  {
    double v;
    is >> v;
    norm_ints.push_back(v);
  }
  return true;
}

bool brad_solve_lambertian_model(std::vector<vnl_double_3> const& ill_dirs,
                                 std::vector<double> const& intensities,
                                 vnl_double_4& model_params,
                                 std::vector<double>& fitting_error)
{
  // form the "A" and "b" matrices
  unsigned m = ill_dirs.size(), n = 3;
  if (m<4) {
    std::cerr << "In solve_lambertian_model(.) - insufficient number of illumination dirs\n";
    return false;
  }
  fitting_error.clear();
  vnl_matrix<double> S(m,n), A3x3(3,3), A(4,4);
  for (unsigned j = 0; j<m; ++j)
    for (unsigned i = 0; i<3; ++i)
      S[j][i]=ill_dirs[j][i];
  A3x3 = S.transpose()*S;
  double s0=0.0, s1=0.0, s2=0.0;
  double b0=0.0, b1=0.0, b2=0.0, b3=0.0;
  for (unsigned j=0; j<m; ++j) {
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
#if 1
  std::cout << "Singular values\n" << D << '\n';
#endif
  model_params = Ainv*b;

  // compute model fitting error

  for (unsigned j = 0; j<m; ++j)
  {
    double Im = S[j][0]*model_params[0];
    Im += S[j][1]*model_params[1]; Im += S[j][2]*model_params[2];
    Im += model_params[3];
    double Io = intensities[j];
    fitting_error.push_back((Im-Io)*(Im-Io));
  }
  return true;
}

double brad_expected_intensity(vnl_double_3 const& illum_dir,
                               vnl_double_4 const& model_params)
{
  double Im = 0.0;
  for (unsigned i = 0; i<3; ++i)
    Im += illum_dir[i]*model_params[i];
  Im += model_params[3];
  return Im;
}

double brad_nearest_ill_dir(std::vector<vnl_double_3> const& ill_dirs,
                            vnl_double_3 const& dir)
{
  double min_ang = 1.0e10;
  for (const auto & ill_dir : ill_dirs)
  {
    double dot = dot_product(ill_dir,dir);
    double ang = std::acos(dot)*vnl_math::deg_per_rad;
    if (ang<min_ang)
      min_ang = ang;
  }
  return min_ang;
}

void brad_solution_error(std::vector<vnl_double_3> const& ill_dirs,
                         std::vector<double> const& intensities,
                         std::vector<double>& fitting_error)
{
  unsigned m = ill_dirs.size();
  // direction to leave out
  for (unsigned j = 0; j<m; j++)
  {
    std::vector<vnl_double_3> ill_dir_1;
    std::vector<double> intens_1;
    for (unsigned i = 0; i<m; ++i)
      //if (i!=j&&i!=j+1) {
      if (i!=j) {
        ill_dir_1.push_back(ill_dirs[i]);
        intens_1.push_back(intensities[i]);
      }
    //find prior fitting error
    std::vector<double> fit_error;
    vnl_double_4 model_params;
    brad_solve_lambertian_model(ill_dir_1,
                                intens_1,
                                model_params,
                                fit_error);

    double ex_int0 = brad_expected_intensity(ill_dirs[j],model_params);
    //double ex_int1 = expected_intensity(ill_dirs[j+1],model_params);
    double er0 = std::fabs(ex_int0-intensities[j]);
    // double er1 = std::fabs(ex_int1-intensities[j+1]);
    fitting_error.push_back(er0);
    //fitting_error.push_back(er1);
  }
}

// Find the range in surface normal orientation angle for which
// the dot product with respect to illumination direction is
// positive for all images
static void brad_search_range(const vnl_matrix<double>& illum_dirs,
                              const vnl_vector<double>& u, const vnl_vector<double>& v,
                              double& theta_min, double& theta_max)
{
  unsigned n_dirs = illum_dirs.rows();
  double min = 0, max = 0.0;
  for (unsigned j = 0; j<n_dirs; ++j)
  {
    vnl_vector<double> ill_dir = illum_dirs.get_row(j);
    double dpu = dot_product(u, ill_dir);
    double dpv = dot_product(v, ill_dir);
    double theta = -std::atan( dpu/dpv);
    double tmid = theta + vnl_math::pi/2.0;
    vnl_vector<double> n_mid = std::cos(tmid)*u + std::sin(tmid)*v;
    double dp_mid = dot_product(n_mid, ill_dir);
    double t_min = theta, t_max = theta+vnl_math::pi;
    if (dp_mid<0) {
      t_min = theta-vnl_math::pi, t_max = theta;
    }
    if (j==0) {min = t_min; max = t_max; continue;}
    //shrink the range
    if (t_min>min) min = t_min;
    if (t_max<max) max = t_max;
  }
  theta_min = min; theta_max = max;
}

// nearest surface normal to the z direction
//
static void nearest_to_z(vnl_vector<double> u, vnl_vector<double> v,
                         vnl_vector<double>& n)
{
  double dpu = u[2], dpv = v[2];
  if (std::fabs(dpu)<1e-3) {
    n = v;
    if (dpv<0)
      n = -n;
    return;
  }
  double theta = std::atan(dpv/dpu);
  n = std::cos(theta)*u + std::sin(theta)*v;
  if (n[2]<0) n = -n;
}

void brad_solve_atmospheric_model(const vnl_matrix<double>& illum_dirs,
                                  vnl_matrix<double> intensities,
                                  vnl_vector<double> airlight,
                                  unsigned max_iterations,
                                  double  /*max_fitting_error*/,
                                  vnl_vector<double>& scene_irrad,
                                  vnl_matrix<double>& surf_normals,
                                  vnl_vector<double>& reflectances)
{
  unsigned n_images = illum_dirs.rows(), n_surfs = intensities.cols();
  vnl_matrix<double> pred_ints;
  vnl_svd<double> svd(illum_dirs);
  vnl_matrix<double> V = svd.V();
  vnl_vector<double> v2 = V.get_column(2);//the degenerate dir
  //find the smallest element of v2
  double vmin = 1e10;
  unsigned emin = 0;
  for (unsigned i =0;i<3;++i)
    if (std::fabs(v2[i])<vmin) {
      vmin = std::fabs(v2[i]);
      emin = i;
    }
  //define a vector to establish axes perpendicular to the degenerate dir
  vnl_vector<double> ax(3,0);
  ax[emin] = 1.0;
  vnl_vector<double> u = vnl_cross_3d(ax, v2);
  u = u/u.magnitude();
  vnl_vector<double> v = vnl_cross_3d(u, v2);
  std::cout << "u " << u << " v " << v << '\n';
  //define residual coefficients that depend only on the illumination dirs,
  // a_uu, a_uv, a_vv
  vnl_matrix<double> StS = illum_dirs.transpose()*illum_dirs;
  vnl_vector<double> us=StS*u, vs=StS*v;
  double a_uu = dot_product(us, us), a_uv = dot_product(us, vs);
  double a_vv = dot_product(vs, vs);
  std::cout << "a_uu= " << a_uu << " a_uv= " << a_uv << " a_vv= "
           << a_vv << '\n';
  double theta_min = 0, theta_max = 0;
  brad_search_range(illum_dirs, u, v, theta_min, theta_max);
  std::cout << "theta_min = " <<  theta_min << " theta_max = " <<  theta_max << '\n';
  vnl_matrix<double> an(3, n_surfs, 0.0);//initial guess at normals
  //find closest normal vector to nz
  vnl_vector<double> near_z;
  nearest_to_z(u, v, near_z);
  for (unsigned i=0; i<n_surfs; ++i)
    an.set_column(i, near_z);
#if 0
  an[2][0]=0.97;   an[2][1]=0.97;   an[2][2]=0.97;   an[2][3]=0.97;
  an[2][4] = 0.97;
  an[0][0]=0.22;   an[1][1]=0.22;   an[1][2]=0.22;   an[1][3]=0.22;

  an[0][4] = 0.22;
#endif

  // compute initial guess for scene_irradiance
  // 1) correct scene intensities to remove airlight
  //   a) subtract airlight

  vnl_matrix<double> corr_intens(n_images, n_surfs);
  for (unsigned r = 0; r<n_images; ++r) {
    double airl = airlight[r];
    for (unsigned c = 0; c<n_surfs; ++c)
      corr_intens[r][c]=(intensities[r][c]-airl);
  }
  // 2) compute irradiance x average reflectance for each image
  //    assuming the surface normals are along z
  vnl_vector<double> irrad_avg(n_images);
  for (unsigned r = 0; r<n_images; ++r) {
    double sum = 0.0;
    vnl_vector<double> temp = illum_dirs.get_row(r);
    for (unsigned c = 0; c<n_surfs; ++c) {
      vnl_vector<double> norm_dir = an.get_column(c);
      double dp = dot_product(temp, norm_dir);
      sum += corr_intens[r][c]/dp; // along z
    }
    irrad_avg[r]=sum/n_surfs;
  }
  std::cout << "Initial Radiance\n" << irrad_avg << '\n';
  unsigned count = 0;
  vnl_matrix<double> norm_dirs(3, n_surfs);
  //debug
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<n_surfs; ++c)
      norm_dirs[r][c]=an[r][c];
  //debug
  vnl_vector<double> reflec(n_surfs);
  while (count<max_iterations) {
    // 3)normalize intensities
    vnl_matrix<double> norm_intens(n_images, n_surfs);
    for (unsigned r = 0; r<n_images; ++r) {
      double iradr = 1.0/irrad_avg[r];
      for (unsigned c = 0; c<n_surfs; ++c)
        norm_intens[r][c]=iradr*corr_intens[r][c];
    }

    //4) Solve for reflectance
    // a) Compute the lagrange multiplier, mu.
    double sum_den = 0;
    double sum_neu = 0;
    std::vector<double> nbetas(n_surfs), snsj(n_surfs);
    for (unsigned i = 0; i<n_surfs; ++i) {
      double sum_nsj = 0;
      double sum_nbeta = 0;
      vnl_vector<double> norm_dir = norm_dirs.get_column(i);
      for (unsigned j = 0; j<n_images; ++j) {
        vnl_vector<double> ill_dir = illum_dirs.get_row(j);
        double Ij_betaj = norm_intens[j][i];
        double dp = dot_product(ill_dir, norm_dir);
        sum_nsj += dp*dp;
        sum_nbeta += dp*Ij_betaj;
      }
      snsj[i] = sum_nsj;
      nbetas[i]=sum_nbeta;
      sum_den += 1/sum_nsj;
      sum_neu += sum_nbeta/sum_nsj;
    }
    sum_den/=n_surfs;
    sum_neu/=n_surfs;
    double mu = (2.0/n_images)*(sum_neu -1)/sum_den;
    // b)Solve for the reflectances
    double rsum = 0.0;
    for (unsigned i = 0; i<n_surfs; ++i) {
      double r = (nbetas[i]-0.5*n_images*mu)/snsj[i];
      double temp;
      if (r>0)
        temp=r;
      else
        temp=0.01;
      rsum += temp;
      reflec[i]=temp;
    }
    rsum /= n_surfs; //average
    for (unsigned i = 0; i<n_surfs; ++i)
      reflec[i]/=rsum;
    //5) compute the surface normals
    // a) define residual coefficients that depend on the individual surface
    for (unsigned i = 0; i<n_surfs; ++i)
    {
      vnl_vector<double> Gi = norm_intens.get_column(i);
      vnl_vector<double> hi = illum_dirs.transpose()*Gi/reflec[i];
      double a_uh = dot_product(us, hi),
             a_vh = dot_product(vs,hi),
             a_hh = dot_product(hi,hi);
#ifdef DEBUG
      std::cout << "hi " << hi << "\na_uh= " << a_uh << " a_vh= " << a_vh
               << " a_hh= " << a_hh << '\n';
#endif
      // b) Solve for surface normal
      brad_illum_cost_function icf(u, v, theta_min, theta_max,
                                   a_uu, a_uv, a_vv, a_uh, a_vh, a_hh);
      double ax=0, bx=0, cx=0;
      icf.determine_brackets(ax, bx, cx, 0.05);
      vnl_brent_minimizer bm(icf);
      double x = bm.minimize_given_bounds(ax, bx, cx);
      vnl_vector<double> ndir = std::cos(x)*u + std::sin(x)*v;
      norm_dirs.set_column(i, ndir);
    }
#if 1
    //6) compute next irradiance estimate
    for (unsigned r = 0; r<n_images; ++r) {
      double sum = 0.0;
      vnl_vector<double> ill_dir = illum_dirs.get_row(r);
      for (unsigned c = 0; c<n_surfs; ++c) {
        vnl_vector<double> norm_dir = norm_dirs.get_column(c);
        double dp = dot_product(ill_dir, norm_dir);
        sum += corr_intens[r][c]/dp;
      }
      irrad_avg[r]=sum/n_surfs;
    }
#endif
    std::cout << "Iteration " << count << '\n'
             << "Reflectances\n" << reflec << '\n'
             << "Surf Norms\n" << norm_dirs << '\n';
    vnl_matrix<double> fit_errs;
    brad_solution_error(illum_dirs, intensities, airlight,
                        irrad_avg,
                        norm_dirs,
                        reflec,
                        fit_errs,
                        pred_ints);
#ifdef DEBUG
    std::cout << "Fit Errors\n" << fit_errs << '\n';
#endif
    double er = fit_errs.absolute_value_sum()/(n_images*n_surfs);
    std::cout << "Average Fit Error "<< er << '\n';
    count++;
  }
  std::cout << "Predicted Intensities\n" << pred_ints << '\n';
  // 7) set outputs
  scene_irrad = irrad_avg; //may change later
  surf_normals = norm_dirs;
  reflectances = reflec;
}


void brad_solution_error(const vnl_matrix<double>& illum_dirs,
                         vnl_matrix<double> intensities,
                         vnl_vector<double> airlight,
                         vnl_vector<double> scene_irrad,
                         const vnl_matrix<double>& surf_normals,
                         vnl_vector<double> reflectances,
                         vnl_matrix<double>& fit_errors,
                         vnl_matrix<double>& pred_intensities)
{
  unsigned n_images = illum_dirs.rows(), n_surfs = intensities.cols();
  // compute reflected radiance =
  //  irrad*(illum_dir dot reflectance*unit_norm_dir)
  vnl_matrix<double> radiance(n_images, n_surfs);
  for (unsigned r = 0; r<n_images; ++r) {
    vnl_vector<double> ill_dir = illum_dirs.get_row(r);
    double irad = scene_irrad[r];
    for (unsigned c = 0; c<n_surfs; ++c) {
      vnl_vector<double> norm_dir = surf_normals.get_column(c);
      double dp = dot_product(ill_dir, norm_dir);
      dp *= reflectances[c];
      radiance[r][c] = dp*irad;
    }
  }
  fit_errors.set_size(n_images, n_surfs);
  pred_intensities.set_size(n_images, n_surfs);
  for (unsigned r = 0; r<n_images; ++r) {
    double airl = airlight[r];
    for (unsigned c = 0; c<n_surfs; ++c) {
      pred_intensities[r][c] = airl+radiance[r][c];
      fit_errors[r][c] = std::fabs(intensities[r][c]-airl-radiance[r][c]);
    }
  }
}

double brad_atmos_prediction(vnl_double_3 const& ill_dir,
                             double airlight,
                             double scene_irrad,
                             vnl_double_3 const& surface_norm,
                             double reflectance
                            )
{
  double dp = dot_product(ill_dir, surface_norm);
  double radiance = dp*reflectance*scene_irrad;
  return radiance + airlight;
}

void brad_solve_atmospheric_model(const vnl_matrix<double>& illum_dirs,
                                  vnl_matrix<double> corr_intens,
                                  unsigned max_iterations,
                                  double  /*max_fitting_error*/,
                                  vnl_matrix<double>& surf_normals,
                                  vnl_vector<double>& reflectances)
{
  unsigned n_images = illum_dirs.rows(), n_surfs = corr_intens.cols();
  vnl_matrix<double> pred_ints;
  vnl_svd<double> svd(illum_dirs);
  vnl_matrix<double> V = svd.V();
  vnl_vector<double> v2 = V.get_column(2);//the degenerate dir
  //find the smallest element of v2
  double vmin = 1e10;
  unsigned emin = 0;
  for (unsigned i =0;i<3;++i)
    if (std::fabs(v2[i])<vmin) {
      vmin = std::fabs(v2[i]);
      emin = i;
    }
  //define a vector to establish axes perpendicular to the degenerate dir
  vnl_vector<double> ax(3,0);
  ax[emin] = 1.0;
  vnl_vector<double> u = vnl_cross_3d(ax, v2);
  u = u/u.magnitude();
  vnl_vector<double> v = vnl_cross_3d(u, v2);
  std::cout << "u " << u << " v " << v << '\n';
  //define residual coefficients that depend only on the illumination dirs,
  // a_uu, a_uv, a_vv
  vnl_matrix<double> StS = illum_dirs.transpose()*illum_dirs;
  vnl_vector<double> us=StS*u, vs=StS*v;
  double a_uu = dot_product(us, us), a_uv = dot_product(us, vs);
  double a_vv = dot_product(vs, vs);
  std::cout << "a_uu= " << a_uu << " a_uv= " << a_uv << " a_vv= "
           << a_vv << '\n';
  double theta_min = 0, theta_max = 0;
  brad_search_range(illum_dirs, u, v, theta_min, theta_max);
  std::cout << "theta_min = " <<  theta_min << " theta_max = " <<  theta_max << '\n';
  vnl_matrix<double> an(3, n_surfs, 0.0);//initial guess at normals
  //find closest normal vector to nz
  vnl_vector<double> near_z;
  nearest_to_z(u, v, near_z);
  for (unsigned i=0; i<n_surfs; ++i)
    an.set_column(i, near_z);

  unsigned count = 0;
  vnl_matrix<double> norm_dirs(3, n_surfs);
  //debug
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<n_surfs; ++c)
      norm_dirs[r][c]=an[r][c];
  //debug
  vnl_vector<double> reflec(n_surfs);
  vnl_matrix<double> fit_errs;
  while (count<max_iterations)
  {
    //4) Solve for reflectance
    // a) Compute the lagrange multiplier, mu.
    double sum_den = 0;
    double sum_neu = 0;
    std::vector<double> nbetas(n_surfs), snsj(n_surfs);
    for (unsigned i = 0; i<n_surfs; ++i) {
      double sum_nsj = 0;
      double sum_nbeta = 0;
      vnl_vector<double> norm_dir = norm_dirs.get_column(i);
      for (unsigned j = 0; j<n_images; ++j) {
        vnl_vector<double> ill_dir = illum_dirs.get_row(j);
        double Ij_betaj = corr_intens[j][i];
        double dp = dot_product(ill_dir, norm_dir);
        sum_nsj += dp*dp;
        sum_nbeta += dp*Ij_betaj;
      }
      snsj[i] = sum_nsj;
      nbetas[i]=sum_nbeta;
      sum_den += 1/sum_nsj;
      sum_neu += sum_nbeta/sum_nsj;
    }
    sum_den/=n_surfs;
    sum_neu/=n_surfs;
    double mu = (2.0/n_images)*(sum_neu -1)/sum_den;
    // b)Solve for the reflectances
    double rsum = 0.0;
    for (unsigned i = 0; i<n_surfs; ++i) {
      double r = (nbetas[i]-0.5*n_images*mu)/snsj[i];
      double temp;
      if (r>0)
        temp=r;
      else
        temp=0.01;
      rsum += temp;
      reflec[i]=temp;
    }
    rsum /= n_surfs; //average
    for (unsigned i = 0; i<n_surfs; ++i)
      reflec[i]/=rsum;
    //5) compute the surface normals
    // a) define residual coefficients that depend on the individual surface
    for (unsigned i = 0; i<n_surfs; ++i)
    {
      vnl_vector<double> Gi = corr_intens.get_column(i);
      vnl_vector<double> hi = illum_dirs.transpose()*Gi/reflec[i];
      double a_uh = dot_product(us, hi),
             a_vh = dot_product(vs,hi),
             a_hh = dot_product(hi,hi);
#ifdef DEBUG
      std::cout << "hi " << hi << "\na_uh= " << a_uh << " a_vh= " << a_vh
               << " a_hh= " << a_hh << '\n';
#endif
      // b) Solve for surface normal
      brad_illum_cost_function icf(u, v, theta_min, theta_max,
                                   a_uu, a_uv, a_vv, a_uh, a_vh, a_hh);
      double ax=0, bx=0, cx=0;
      icf.determine_brackets(ax, bx, cx, 0.05);
      vnl_brent_minimizer bm(icf);
      double x = bm.minimize_given_bounds(ax, bx, cx);
      vnl_vector<double> ndir = std::cos(x)*u + std::sin(x)*v;
      norm_dirs.set_column(i, ndir);
    }
    std::cout << "Iteration " << count << '\n'
             << "Reflectances\n" << reflec << '\n'
             << "Surf Norms\n" << norm_dirs << '\n';
    brad_solution_error(illum_dirs, corr_intens,
                        norm_dirs,
                        reflec,
                        fit_errs);
#ifdef DEBUG
    std::cout << "Fit Errors\n" << fit_errs << '\n';
#endif
    double er = fit_errs.absolute_value_sum()/(n_images*n_surfs);
    std::cout << "Average Fit Error "<< er << '\n';
    count++;
  }
  std::cout << "FitErrors\n" << fit_errs << '\n';
  // 7) set outputs
  surf_normals = norm_dirs;
  reflectances = reflec;
}

void brad_solution_error(const vnl_matrix<double>& illum_dirs,
                         vnl_matrix<double> corr_intens,
                         const vnl_matrix<double>& surf_normals,
                         vnl_vector<double> reflectances,
                         vnl_matrix<double>& fit_errors)
{
  unsigned n_images = illum_dirs.rows(), n_surfs = corr_intens.cols();

  fit_errors.set_size(n_images, n_surfs);
  for (unsigned r = 0; r<n_images; ++r) {
    vnl_vector<double> ill_dir = illum_dirs.get_row(r);
    for (unsigned c = 0; c<n_surfs; ++c) {
      vnl_vector<double> norm_dir = surf_normals.get_column(c);
      double dp = dot_product(ill_dir, norm_dir);
      dp *= reflectances[c];
      fit_errors[r][c] = corr_intens[r][c]-dp;
    }
  }
}

void  brad_display_illumination_space_vrml(vnl_matrix<double> illum_dirs,
                                           std::string const& path,
                                           vnl_double_3 degenerate_dir)
{
  std::ofstream str(path.c_str());
  if (!str.is_open())
  {
    std::cerr << "In brad_display_illumination_space_vrml() - vrml file path is not valid\n";
    return;
  }
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
  double rad = 15.0;
  unsigned n = illum_dirs.rows();
  vgl_vector_3d<double> cent(0,0,0);
  for (unsigned i =0; i<n; i++) {
    vgl_vector_3d<double> r(illum_dirs[i][0], illum_dirs[i][1],
                            illum_dirs[i][2]);
    vnl_double_3 yaxis(0.0, 1.0, 0.0), pvec(r.x(), r.y(), r.z());
    vgl_rotation_3d<double> rot(yaxis, pvec);
    vnl_quaternion<double> q = rot.as_quaternion();
    vnl_double_3 axis = q.axis();
    double ang = q.angle();
    str <<  "Transform {\n"
        << " translation " << cent.x()+ rad*r.x() << ' ' << cent.y()+rad*r.y()
        << ' ' << cent.z()+rad*r.z() << '\n'
        << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
        << "children [\n"
        << " Shape {\n"
        << " appearance Appearance{\n"
        << "  material Material\n"
        << "   {\n"
        << "     diffuseColor 1 0 0\n"
        << "     transparency 0\n"
        << "    }\n"
        << "  }\n"
        << " geometry Cylinder\n"
        << "{\n"
        << " radius 0.250000\n"
        << " height " << 2*rad << '\n'
        << " }\n"
        << " }\n"
        << "]\n"
        << "}\n";
  }
  if (degenerate_dir[0]==0&&degenerate_dir[1]==0&&degenerate_dir[2]==0) {
    str.close();
    return;
  }
  vgl_vector_3d<double> r(degenerate_dir[0], degenerate_dir[1],
                          degenerate_dir[2]);
  vnl_double_3 yaxis(0.0, 1.0, 0.0), pvec(r.x(), r.y(), r.z());
  vgl_rotation_3d<double> rot(yaxis, pvec);
  vnl_quaternion<double> q = rot.as_quaternion();
  vnl_double_3 axis = q.axis();
  double ang = q.angle();
  str <<  "Transform {\n"
      << " translation " << cent.x()+ rad*r.x() << ' ' << cent.y()+rad*r.y()
      << ' ' << cent.z()+rad*r.z() << '\n'
      << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
      << "children [\n"
      << " Shape {\n"
      << " appearance Appearance{\n"
      << "  material Material\n"
      << "   {\n"
      << "     diffuseColor 0 1 0\n"
      << "     transparency 0\n"
      << "    }\n"
      << "  }\n"
      << " geometry Cylinder\n"
      << "{\n"
      << " radius 0.25000\n"
      << " height " << 2*rad << '\n'
      << " }\n"
      << " }\n"
      << "]\n"
      << "}\n";
  str.close();
}


double brad_expected_radiance_chavez(double reflectance,
                                     vgl_vector_3d<double> const& normal,
                                     brad_image_metadata const& md,
                                     brad_atmospheric_parameters const& atm)
{
   double deg2rad = vnl_math::pi_over_180;
   double sun_az = md.sun_azimuth_ * deg2rad;
   double sun_el = md.sun_elevation_ * deg2rad;
   double view_az = md.view_azimuth_ * deg2rad;
   double view_el = md.view_elevation_ * deg2rad;
   vgl_vector_3d<double> view_dir(std::sin(view_az)*std::cos(view_el),
                                  std::cos(view_az)*std::cos(view_el),
                                  std::sin(view_el));
   double view_dot_norm = dot_product(view_dir,normal);
   if (view_dot_norm <= 0) {
      // surface is not visible from this viewpoint
      return 0.0;
   }
   vgl_vector_3d<double> sun_dir(std::sin(sun_az)*std::cos(sun_el),
                                 std::cos(sun_az)*std::cos(sun_el),
                                 std::sin(sun_el));

   double T_sun = std::exp(-atm.optical_depth_ / sun_dir.z());
   double T_view = std::exp(-atm.optical_depth_ / std::sin(view_el));

   return brad_expected_radiance_chavez(reflectance, normal, sun_dir, T_sun, T_view, md.sun_irradiance_, atm.skylight_, atm.airlight_);
}

double brad_expected_radiance_chavez(double reflectance,
                                     vgl_vector_3d<double> const& normal,
                                     vgl_vector_3d<double> const& sun_dir,
                                     double T_sun,
                                     double T_view,
                                     double solar_irradiance,
                                     double skylight,
                                     double airlight)
{
   double sun_dot_norm = dot_product(sun_dir, normal);
   if (sun_dot_norm < 0) {
      sun_dot_norm = 0;
   }
   // compute shape factor
   double F = 1.0 - 0.5*std::sqrt(1.0 - normal.z()*normal.z());
   return reflectance * T_view * (solar_irradiance* sun_dot_norm * T_sun + F*skylight) / vnl_math::pi  + airlight;
}

double brad_expected_reflectance_chavez(double toa_radiance,
                                        vgl_vector_3d<double> const& normal,
                                        brad_image_metadata const& md,
                                        brad_atmospheric_parameters const& atm)
{
   double deg2rad = vnl_math::pi_over_180;
   double sun_az = md.sun_azimuth_ * deg2rad;
   double sun_el = md.sun_elevation_ * deg2rad;
   double view_el = md.view_elevation_ * deg2rad;
   vgl_vector_3d<double> sun_dir(std::sin(sun_az)*std::cos(sun_el),
                                 std::cos(sun_az)*std::cos(sun_el),
                                 std::sin(sun_el));

   double T_sun = std::exp(-atm.optical_depth_ / sun_dir.z());
   double T_view = std::exp(-atm.optical_depth_ / std::sin(view_el));
   double sun_irradiance = md.sun_irradiance_;
   return brad_expected_reflectance_chavez(toa_radiance, normal, sun_dir, T_sun, T_view, sun_irradiance, atm.skylight_, atm.airlight_);
}

double brad_expected_reflectance_chavez(double toa_radiance,
                                        vgl_vector_3d<double> const& normal,
                                        vgl_vector_3d<double> const& sun_dir,
                                        double T_sun,
                                        double T_view,
                                        double solar_irradiance,
                                        double skylight,
                                        double airlight)
{
   double sun_dot_norm = dot_product(sun_dir, normal);
   if (sun_dot_norm < 0) {
      sun_dot_norm = 0;
   }
   // compute shape factor
   double F = 1.0 - 0.5*std::sqrt(1.0 - normal.z()*normal.z());
   double denom = T_view * (solar_irradiance * sun_dot_norm * T_sun + F*skylight);
   if (denom < 1e-6) {
      // reflectance is undefined, set to zero
      return 0;
   }
   double reflectance = vnl_math::pi * (toa_radiance - airlight) / denom;
   // some algorithms depend on "invalid" reflectances being returned in order to compute
   // constants A,B for reflectance = A*radiance + B  -DEC 7 Feb 2012
#if 0
   if (reflectance > 1.0) {
      reflectance = 1.0;
   }
   if (reflectance < 0.0) {
      reflectance = 0.0;
   }
#endif
   return reflectance;
}

double brad_radiance_variance_chavez(double reflectance,
                                     vgl_vector_3d<double> const& normal,
                                     brad_image_metadata const& md,
                                     brad_atmospheric_parameters const& atm,
                                     double reflectance_var,
                                     double optical_depth_var,
                                     double skylight_var,
                                     double airlight_var)
{
   double deg2rad = vnl_math::pi_over_180;
   double sun_az = md.sun_azimuth_ * deg2rad;
   double sun_el = md.sun_elevation_ * deg2rad;
   double view_az = md.view_azimuth_ * deg2rad;
   double view_el = md.view_elevation_ * deg2rad;
   vgl_vector_3d<double> view_dir(std::sin(view_az)*std::cos(view_el),
                                 std::cos(view_az)*std::cos(view_el),
                                 std::sin(view_el));
   double view_dot_norm = dot_product(view_dir, normal);
   if (view_dot_norm <= 0) {
      // surface is not visible from this viewpoint
      return 0.0;
   }
   vgl_vector_3d<double> sun_dir(std::sin(sun_az)*std::cos(sun_el),
                                 std::cos(sun_az)*std::cos(sun_el),
                                 std::sin(sun_el));

   double T_sun = std::exp(-atm.optical_depth_ / sun_dir.z());
   double T_view = std::exp(-atm.optical_depth_ / std::sin(view_el));

   double sun_dot_norm = dot_product(sun_dir, normal);
   if (sun_dot_norm < 0)
      sun_dot_norm = 0.0;

   // compute shape factor for surface
   double F = 1.0 - 0.5*std::sqrt(1.0 - normal.z()*normal.z());
   double dL_dskylight = F*reflectance*T_view/vnl_math::pi;
   double dL_dairlight = 1.0;
   double sun_irradiance = md.sun_irradiance_;
   double dL_doptical_depth = sun_irradiance*reflectance*sun_dot_norm*T_sun*T_view/(vnl_math::pi * sun_dir.z());
   dL_doptical_depth += (sun_irradiance*sun_dot_norm*T_sun + atm.skylight_*F) * reflectance * T_view / (vnl_math::pi * std::sin(view_el));

   double dL_dreflectance = (sun_irradiance * sun_dot_norm * T_sun + atm.skylight_ * F * T_view) / vnl_math::pi;

   double radiance_var = dL_dskylight*dL_dskylight*skylight_var;
   radiance_var += dL_dairlight*dL_dairlight*airlight_var;
   radiance_var += dL_doptical_depth*dL_doptical_depth*optical_depth_var;
   radiance_var += dL_dreflectance*dL_dreflectance*reflectance_var;

   return radiance_var;
}
