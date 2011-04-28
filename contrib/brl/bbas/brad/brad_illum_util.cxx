#include "brad_illum_util.h"
#include <vgl/vgl_vector_3d.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_brent_minimizer.h>
#include <vcl_iostream.h>
#include <vnl/vnl_cross.h>
#include <brad/brad_illum_cost_function.h>

bool brad_illum_util::
load_surface_nhbds(vcl_string const& path,
                   vcl_vector<vcl_vector<vnl_matrix<float> > >& nhds)
{
  vcl_ifstream is(path.c_str());
  if (!is.is_open())
  {
    vcl_cerr << "In brad_illum_util::load_surface_nhbds(.) - "
             <<"neigborhood path is not valid\n";
    return false;
  }

  unsigned dim, ntracks;
  vcl_string temp;
  is >> temp;
  if(temp!="dim:"){
    vcl_cerr << "In brad_illum_util::load_surface_nhbds(.) - "
             <<"file parse error\n";
    return false;
  }
  is >> dim;
  is >> temp;
  if(temp!="n_tracks:"){
    vcl_cerr << "In brad_illum_util::load_surface_nhbds(.) - "
             <<"file parse error\n";
    return false;
  }
  is >> ntracks;
  for (unsigned i = 0; i<ntracks; ++i){
    vcl_vector<vnl_matrix<float> > nbs;
    is >> temp;
    if (temp!="n_i:") {
      vcl_cerr << "In brad_illum_util::load_surface_nhbds(.) - "
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
brad_illum_util::load_illumination_dirs(vcl_string const& path,
                                        vcl_vector<vnl_double_3>& ill_dirs)
{
  vcl_ifstream is(path.c_str());
  if (!is.is_open())
  {
    vcl_cerr << "In brad_illum_util::load_illumination_dirs(.) - "
             <<"illumination dir path is not valid\n";
    return false;
  }
  ill_dirs.clear();
  vcl_string temp;
  is >> temp;
  if (temp != "n_dirs:")
  {
    vcl_cerr << "In brad_illum_util::load_illumination_dirs(.) - "
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
bool brad_illum_util::
load_norm_intensities(vcl_string const& path,
                      vcl_vector<double>& norm_ints)
{
  vcl_ifstream is(path.c_str());
  if(!is.is_open())
    {
      vcl_cerr << "In brad_illum_util::load_norm_intensities(.) - "
               <<"normlized intensity path is not valid\n";
      return false;
    }
  norm_ints.clear();
  vcl_string temp;
  is >> temp;
  if(temp != "n_ints:")
    {
      vcl_cerr << "In brad_illum_util::load_norm_intensities(.) - "
               <<"invalid file syntax\n";
      return false;
    }
  unsigned n_ints = 0;
  is >> n_ints;
  for(unsigned i = 0; i<n_ints; ++i)
    {
      double v;
      is >> v;
      norm_ints.push_back(v);
    }
  return true;
}
bool brad_illum_util::
solve_lambertian_model(vcl_vector<vnl_double_3> const& ill_dirs,
                       vcl_vector<double> const& intensities,
                       vnl_double_4& model_params,
                       vcl_vector<double>& fitting_error)
{
  // form the "A" and "b" matrices
  unsigned m = ill_dirs.size(), n = 3;
  if(m<4){
    vcl_cerr << "In brad_illum_util::solve_lambertian_model(.) - "
             <<"insufficient number of illumination dirs\n";
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
#if 1
  vcl_cout << "Singular values\n" << D << '\n';
#endif
  model_params = Ainv*b;

  // compute model fitting error

  for(unsigned j = 0; j<m; ++j)
    {
      double Im = S[j][0]*model_params[0];
      Im += S[j][1]*model_params[1]; Im += S[j][2]*model_params[2];
      Im += model_params[3];
      double Io = intensities[j];
      fitting_error.push_back((Im-Io)*(Im-Io));
    }
  return true;
}

double brad_illum_util::expected_intensity(vnl_double_3 const& illum_dir,
                                           vnl_double_4 const& model_params)
{
  double Im = 0.0;
  for (unsigned i = 0; i<3; ++i)
    Im += illum_dir[i]*model_params[i];
  Im += model_params[3];
  return Im;
}
double brad_illum_util::
nearest_ill_dir(vcl_vector<vnl_double_3> const& ill_dirs,
                vnl_double_3 const& dir)
{
  double min_ang = 1.0e10; 
  for(unsigned i = 0; i<ill_dirs.size(); ++i)
    {
      double dot = dot_product(ill_dirs[i],dir);
      double ang = 180.0*vcl_acos(dot)/3.14159;
      if(ang<min_ang)
        min_ang = ang;
    }  
  return min_ang;
}

void brad_illum_util::
solution_error(vcl_vector<vnl_double_3> const& ill_dirs,
               vcl_vector<double> const& intensities,
               vcl_vector<double>& fitting_error)
{
  unsigned m = ill_dirs.size();
  // direction to leave out
  for(unsigned j = 0; j<m; j++)
    {
      vcl_vector<vnl_double_3> ill_dir_1;
      vcl_vector<double> intens_1;
      for(unsigned i = 0; i<m; ++i)
        //if(i!=j&&i!=j+1){
        if(i!=j){
          ill_dir_1.push_back(ill_dirs[i]);
          intens_1.push_back(intensities[i]);
        }          
      //find prior fitting error 
      vcl_vector<double> fit_error;
      vnl_double_4 model_params;
      solve_lambertian_model(ill_dir_1,
                             intens_1,
                             model_params,
                             fit_error);

      double ex_int0 = brad_illum_util::expected_intensity(ill_dirs[j],model_params);
      //double ex_int1 = brad_illum_util::expected_intensity(ill_dirs[j+1],model_params);
      double er0 = vcl_fabs(ex_int0-intensities[j]);
      // double er1 = vcl_fabs(ex_int1-intensities[j+1]);
      fitting_error.push_back(er0);
      //fitting_error.push_back(er1);
    }
}
// Find the range in surface normal orientation angle for which
// the dot product with respect to illumination direction is 
// positive for all images
static void search_range(vnl_matrix<double> illum_dirs, 
                         vnl_vector<double> u, vnl_vector<double> v, 
                         double& theta_min, double& theta_max)
{
  unsigned n_dirs = illum_dirs.rows();
  double min = 0, max = 0.0; 
  for(unsigned j = 0; j<n_dirs; ++j)
    {
      vnl_vector<double> ill_dir = illum_dirs.get_row(j);
      double dpu = dot_product(u, ill_dir);
      double dpv = dot_product(v, ill_dir);
      double theta = -vcl_atan( dpu/dpv);
      double tmid = theta + vnl_math::pi/2.0;
      vnl_vector<double> n_mid = vcl_cos(tmid)*u + vcl_sin(tmid)*v;
      double dp_mid = dot_product(n_mid, ill_dir);
      double t_min = theta, t_max = theta+vnl_math::pi;
      if(dp_mid<0){
        t_min = theta-vnl_math::pi, t_max = theta;
      }
      if(j==0){min = t_min; max = t_max; continue;}
      //shrink the range
      if(t_min>min) min = t_min;
      if(t_max<max) max = t_max;
    }
  theta_min = min; theta_max = max;
}
// nearest surface normal to the z direction
//
void nearest_to_z(vnl_vector<double> u, vnl_vector<double> v,
                  vnl_vector<double>& n)
{
  double dpu = u[2], dpv = v[2];
  if(vcl_fabs(dpu)<1e-3){
    n = v;
    if(dpv<0)
      n = -n;
    return;
  }
  double theta = vcl_atan(dpv/dpu);
  n = vcl_cos(theta)*u + vcl_sin(theta)*v;
  if(n[2]<0) n = -n;
}
void brad_illum_util::solve_atmospheric_model(vnl_matrix<double> illum_dirs,
                                              vnl_matrix<double> intensities,
                                              vnl_vector<double> airlight,
                                              unsigned max_iterations,
                                              double max_fitting_error,
                                              vnl_vector<double>& scene_irrad,
                                              vnl_matrix<double>& surf_normals,
                                              vnl_vector<double>& reflectances)
                                
{
	unsigned n_images = illum_dirs.rows(), n_surfs = intensities.cols();
  vnl_matrix<double> pred_ints;
  vnl_svd<double> svd(illum_dirs);
  vnl_matrix<double> V = svd.V();
  vnl_vector<double> v2 = V.get_column(2);//the degnerate dir
  //find the smallest element of v2
  double vmin = 1e10;
  unsigned emin = 0;
  for(unsigned i =0;i<3;++i)
    if(vcl_fabs(v2[i])<vmin){
      vmin = vcl_fabs(v2[i]);
      emin = i;
    }
  //define a vector to establish axes perpendicular to the degenerate dir
  vnl_vector<double> ax(3,0);
  ax[emin] = 1.0;
  vnl_vector<double> u = vnl_cross_3d(ax, v2);
  u = u/u.magnitude();
  vnl_vector<double> v = vnl_cross_3d(u, v2);
  vcl_cout << "u " << u << " v " << v << '\n';
  //define residual coefficients that depend only on the illumination dirs,
  // a_uu, a_uv, a_vv
  vnl_matrix<double> StS = illum_dirs.transpose()*illum_dirs;
  vnl_vector<double> us=StS*u, vs=StS*v;
  double a_uu = dot_product(us, us), a_uv = dot_product(us, vs);
  double a_vv = dot_product(vs, vs);
  vcl_cout << "a_uu= " << a_uu << " a_uv= " << a_uv << " a_vv= " 
           << a_vv << '\n';  
  double theta_min = 0, theta_max = 0;
  search_range(illum_dirs, u, v, theta_min, theta_max);
  vcl_cout << "theta_min = " <<  theta_min << " theta_max = " <<  theta_max << '\n'; 
  vnl_matrix<double> an(3, n_surfs, 0.0);//initial guess at normals
  //find closest normal vector to nz
  vnl_vector<double> near_z;
  nearest_to_z(u, v, near_z);
  for(unsigned i=0; i<n_surfs; ++i)
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
  for(unsigned r = 0; r<n_images; ++r){
    double airl = airlight[r];
    for(unsigned c = 0; c<n_surfs; ++c)
      corr_intens[r][c]=(intensities[r][c]-airl);
  }
  // 2) compute irradiance x average reflectance for each image  
  //    assuming the surface normals are along z
  vnl_vector<double> irrad_avg(n_images);
  for(unsigned r = 0; r<n_images; ++r){
    double sum = 0.0;
    vnl_vector<double> temp = illum_dirs.get_row(r);
    for(unsigned c = 0; c<n_surfs; ++c){
      vnl_vector<double> norm_dir = an.get_column(c);
      double dp = dot_product(temp, norm_dir);
      sum += corr_intens[r][c]/dp; // along z
    }
    irrad_avg[r]=sum/n_surfs;
  }
  vcl_cout << "Initial Radiance \n" << irrad_avg << '\n';
  unsigned count = 0;
  vnl_matrix<double> norm_dirs(3, n_surfs);
  //debug
  for(unsigned r = 0; r<3; ++r)
    for(unsigned c = 0; c<n_surfs; ++c)
      norm_dirs[r][c]=an[r][c];
  //debug
  vnl_vector<double> reflec(n_surfs);
  while(count<max_iterations){
    // 3)normalize intensities
    vnl_matrix<double> norm_intens(n_images, n_surfs);
    for(unsigned r = 0; r<n_images; ++r){
      double iradr = 1.0/irrad_avg[r];
      for(unsigned c = 0; c<n_surfs; ++c)
        norm_intens[r][c]=iradr*corr_intens[r][c];
    }
   
    //4) Solve for reflectance
    // a) Compute the lagrange multiplier, mu.
    double sum_den = 0;
    double sum_neu = 0;
    vcl_vector<double> nbetas(n_surfs), snsj(n_surfs);
    for(unsigned i = 0; i<n_surfs; ++i){
      double sum_nsj = 0;
      double sum_nbeta = 0;
      vnl_vector<double> norm_dir = norm_dirs.get_column(i);
      for(unsigned j = 0; j<n_images; ++j){
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
    for(unsigned i = 0; i<n_surfs; ++i){
       double r = (nbetas[i]-0.5*n_images*mu)/snsj[i];
       double temp;
       if(r>0)
         temp=r;
       else
         temp=0.01;
       rsum += temp;
       reflec[i]=temp;
    }
    rsum /= n_surfs; //average
    for(unsigned i = 0; i<n_surfs; ++i)
      reflec[i]/=rsum;    
    //5) compute the surface normals
    // a) define residual coefficients that depend on the individual surface
    for(unsigned i = 0; i<n_surfs; ++i)
      {
        vnl_vector<double> Gi = norm_intens.get_column(i);
        vnl_vector<double> hi = illum_dirs.transpose()*Gi/reflec[i];
        double a_uh = dot_product(us, hi),a_vh = dot_product(vs,hi),
          a_hh = dot_product(hi,hi);
#if 0
        vcl_cout << "hi " << hi << "\na_uh= " << a_uh << " a_vh= " << a_vh
                 << " a_hh= " << a_hh << '\n';
#endif
        // b) Solve for surface normal
        brad_illum_cost_function icf(u, v, theta_min, theta_max,
                                     a_uu, a_uv, a_vv, a_uh, a_vh, a_hh);
        double ax=0, bx=0, cx=0;
        icf.determine_brackets(ax, bx, cx, 0.05);
        vnl_brent_minimizer bm(icf);
        double x = bm.minimize_given_bounds(ax, bx, cx);
        vnl_vector<double> ndir = vcl_cos(x)*u + vcl_sin(x)*v;
        norm_dirs.set_column(i, ndir);
      }
#if 1
    //6) compute next irradiance estimate
    for(unsigned r = 0; r<n_images; ++r){
      double sum = 0.0;
      vnl_vector<double> ill_dir = illum_dirs.get_row(r);
      for(unsigned c = 0; c<n_surfs; ++c){
        vnl_vector<double> norm_dir = norm_dirs.get_column(c);
        double dp = dot_product(ill_dir, norm_dir);
        sum += corr_intens[r][c]/dp;
      }
      irrad_avg[r]=sum/n_surfs;
    }
#endif 
    vcl_cout << "Iteration " << count << '\n';
    vcl_cout << "Reflectances \n" << reflec << '\n';
    vcl_cout << "Surf Norms \n" << norm_dirs << '\n';
    vnl_matrix<double> fit_errs;
    brad_illum_util::solution_error(illum_dirs, intensities, airlight,
                                    irrad_avg,
                                    norm_dirs,
                                    reflec,
                                    fit_errs,
                                    pred_ints);
#if 0
    vcl_cout << "Fit Errors\n" << fit_errs << '\n';
#endif
    double er = fit_errs.absolute_value_sum()/(n_images*n_surfs);
    vcl_cout << "Average Fit Error "<< er << '\n';
    count++;
  }
  vcl_cout << "Predicted Intensities\n" << pred_ints << '\n';
  // 7) set outputs
  scene_irrad = irrad_avg; //may change later
  surf_normals = norm_dirs;
  reflectances = reflec;
}


void brad_illum_util::solution_error(vnl_matrix<double> illum_dirs,
                                     vnl_matrix<double> intensities,
                                     vnl_vector<double> airlight,
                                     vnl_vector<double> scene_irrad,
                                     vnl_matrix<double> surf_normals,
                                     vnl_vector<double> reflectances,
                                     vnl_matrix<double>& fit_errors,
                                     vnl_matrix<double>& pred_intensities)
{
  unsigned n_images = illum_dirs.rows(), n_surfs = intensities.cols();
  // compute reflected radiance = 
  //  irrad*(illum_dir dot reflectance*unit_norm_dir)
  vnl_matrix<double> radiance(n_images, n_surfs);
  for(unsigned r = 0; r<n_images; ++r){
    vnl_vector<double> ill_dir = illum_dirs.get_row(r);
    double irad = scene_irrad[r];
    for(unsigned c = 0; c<n_surfs; ++c){
      vnl_vector<double> norm_dir = surf_normals.get_column(c);
      double dp = dot_product(ill_dir, norm_dir);
      dp *= reflectances[c];
      radiance[r][c] = dp*irad;
    }
  }
  fit_errors.set_size(n_images, n_surfs);
  pred_intensities.set_size(n_images, n_surfs);
  for(unsigned r = 0; r<n_images; ++r){
    double airl = airlight[r];
    for(unsigned c = 0; c<n_surfs; ++c){
      pred_intensities[r][c] = airl+radiance[r][c];
      fit_errors[r][c] = vcl_fabs(intensities[r][c]-airl-radiance[r][c]);
    }
  }
}

double brad_illum_util::atmos_prediction(vnl_double_3 const& ill_dir,
                                         double airlight,
                                         double scene_irrad,
                                         vnl_double_3 const& surface_norm,
                                         double reflectance
                                         )
{
  double dp = dot_product(ill_dir, surface_norm);
  double radiance = dp*reflectance*scene_irrad;
  return (radiance + airlight);
}
void brad_illum_util::solve_atmospheric_model(vnl_matrix<double> illum_dirs,
                                              vnl_matrix<double> corr_intens,
                                              unsigned max_iterations,
                                              double max_fitting_error,
                                              vnl_matrix<double>& surf_normals,
                                              vnl_vector<double>& reflectances)
{
	unsigned n_images = illum_dirs.rows(), n_surfs = corr_intens.cols();
  vnl_matrix<double> pred_ints;
  vnl_svd<double> svd(illum_dirs);
  vnl_matrix<double> V = svd.V();
  vnl_vector<double> v2 = V.get_column(2);//the degnerate dir
  //find the smallest element of v2
  double vmin = 1e10;
  unsigned emin = 0;
  for(unsigned i =0;i<3;++i)
    if(vcl_fabs(v2[i])<vmin){
      vmin = vcl_fabs(v2[i]);
      emin = i;
    }
  //define a vector to establish axes perpendicular to the degenerate dir
  vnl_vector<double> ax(3,0);
  ax[emin] = 1.0;
  vnl_vector<double> u = vnl_cross_3d(ax, v2);
  u = u/u.magnitude();
  vnl_vector<double> v = vnl_cross_3d(u, v2);
  vcl_cout << "u " << u << " v " << v << '\n';
  //define residual coefficients that depend only on the illumination dirs,
  // a_uu, a_uv, a_vv
  vnl_matrix<double> StS = illum_dirs.transpose()*illum_dirs;
  vnl_vector<double> us=StS*u, vs=StS*v;
  double a_uu = dot_product(us, us), a_uv = dot_product(us, vs);
  double a_vv = dot_product(vs, vs);
  vcl_cout << "a_uu= " << a_uu << " a_uv= " << a_uv << " a_vv= " 
           << a_vv << '\n';  
  double theta_min = 0, theta_max = 0;
  search_range(illum_dirs, u, v, theta_min, theta_max);
  vcl_cout << "theta_min = " <<  theta_min << " theta_max = " <<  theta_max << '\n'; 
  vnl_matrix<double> an(3, n_surfs, 0.0);//initial guess at normals
  //find closest normal vector to nz
  vnl_vector<double> near_z;
  nearest_to_z(u, v, near_z);
  for(unsigned i=0; i<n_surfs; ++i)
    an.set_column(i, near_z);

  unsigned count = 0;
  vnl_matrix<double> norm_dirs(3, n_surfs);
  //debug
  for(unsigned r = 0; r<3; ++r)
    for(unsigned c = 0; c<n_surfs; ++c)
      norm_dirs[r][c]=an[r][c];
  //debug
  vnl_vector<double> reflec(n_surfs);
  vnl_matrix<double> fit_errs;
  while(count<max_iterations){

    //4) Solve for reflectance
    // a) Compute the lagrange multiplier, mu.
    double sum_den = 0;
    double sum_neu = 0;
    vcl_vector<double> nbetas(n_surfs), snsj(n_surfs);
    for(unsigned i = 0; i<n_surfs; ++i){
      double sum_nsj = 0;
      double sum_nbeta = 0;
      vnl_vector<double> norm_dir = norm_dirs.get_column(i);
      for(unsigned j = 0; j<n_images; ++j){
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
    for(unsigned i = 0; i<n_surfs; ++i){
       double r = (nbetas[i]-0.5*n_images*mu)/snsj[i];
       double temp;
       if(r>0)
         temp=r;
       else
         temp=0.01;
       rsum += temp;
       reflec[i]=temp;
    }
    rsum /= n_surfs; //average
    for(unsigned i = 0; i<n_surfs; ++i)
      reflec[i]/=rsum;    
    //5) compute the surface normals
    // a) define residual coefficients that depend on the individual surface
    for(unsigned i = 0; i<n_surfs; ++i)
      {
        vnl_vector<double> Gi = corr_intens.get_column(i);
        vnl_vector<double> hi = illum_dirs.transpose()*Gi/reflec[i];
        double a_uh = dot_product(us, hi),a_vh = dot_product(vs,hi),
          a_hh = dot_product(hi,hi);
#if 0
        vcl_cout << "hi " << hi << "\na_uh= " << a_uh << " a_vh= " << a_vh
                 << " a_hh= " << a_hh << '\n';
#endif
        // b) Solve for surface normal
        brad_illum_cost_function icf(u, v, theta_min, theta_max,
                                     a_uu, a_uv, a_vv, a_uh, a_vh, a_hh);
        double ax=0, bx=0, cx=0;
        icf.determine_brackets(ax, bx, cx, 0.05);
        vnl_brent_minimizer bm(icf);
        double x = bm.minimize_given_bounds(ax, bx, cx);
        vnl_vector<double> ndir = vcl_cos(x)*u + vcl_sin(x)*v;
        norm_dirs.set_column(i, ndir);
      }
    vcl_cout << "Iteration " << count << '\n';
    vcl_cout << "Reflectances \n" << reflec << '\n';
    vcl_cout << "Surf Norms \n" << norm_dirs << '\n';
    brad_illum_util::solution_error(illum_dirs, corr_intens,
                                    norm_dirs,
                                    reflec,
                                    fit_errs);
#if 0
    vcl_cout << "Fit Errors\n" << fit_errs << '\n';
#endif
    double er = fit_errs.absolute_value_sum()/(n_images*n_surfs);
    vcl_cout << "Average Fit Error "<< er << '\n';
    count++;
  }
  vcl_cout << "FitErrors\n" << fit_errs << '\n';
  // 7) set outputs
  surf_normals = norm_dirs;
  reflectances = reflec;
}
void brad_illum_util::solution_error(vnl_matrix<double> illum_dirs,
                                     vnl_matrix<double> corr_intens,
                                     vnl_matrix<double> surf_normals,
                                     vnl_vector<double> reflectances,
                                     vnl_matrix<double>& fit_errors)

{
  unsigned n_images = illum_dirs.rows(), n_surfs = corr_intens.cols();

  fit_errors.set_size(n_images, n_surfs);
  for(unsigned r = 0; r<n_images; ++r){
	vnl_vector<double> ill_dir = illum_dirs.get_row(r);
    for(unsigned c = 0; c<n_surfs; ++c){
      vnl_vector<double> norm_dir = surf_normals.get_column(c);
      double dp = dot_product(ill_dir, norm_dir);
      dp *= reflectances[c];
      fit_errors[r][c] = corr_intens[r][c]-dp;
    }
  }
}
void   brad_illum_util::
display_illumination_space_vrml(vnl_matrix<double> illum_dirs,
                                vcl_string const& path,
                                vnl_double_3 degenerate_dir)
{
  vcl_ofstream str(path.c_str());
  if(!str.is_open())
    {
      vcl_cerr << "In brad_illum_util::display_illumination_space(.) - "
               <<"vrml file path is not valid\n";
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
  for (unsigned i =0; i<n; i++){
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
  if(degenerate_dir[0]==0&&degenerate_dir[1]==0&&degenerate_dir[2]==0){
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
