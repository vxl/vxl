//:
// \file
// \brief implementation of the brct_algos class.
//
//////////////////////////////////////////////////////////////////////

#include "brct_algos.h"
#include <vcl_algorithm.h>//for sort
#include <vcl_cmath.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_2x3.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bbas/bdgl/bdgl_curve_algs.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vcl_cassert.h>

// Construction/Destruction

brct_algos::brct_algos()
{
}

brct_algos::~brct_algos()
{
}

vgl_point_3d<double> brct_algos::triangulate_3d_point(const vgl_point_2d<double>& x1, const vnl_double_3x4& P1,
                                                      const vgl_point_2d<double>& x2, const vnl_double_3x4& P2)
{
  vnl_double_4x4 A;

  for (int i=0; i<4; i++){
    A[0][i] = x1.x()*P1[2][i] - P1[0][i];
    A[1][i] = x1.y()*P1[2][i] - P1[1][i];
    A[2][i] = x2.x()*P2[2][i] - P2[0][i];
    A[3][i] = x2.y()*P2[2][i] - P2[1][i];
  }

  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}

vgl_point_2d<double> brct_algos::projection_3d_point(const vgl_point_3d<double> & x, const vnl_double_3x4& P)
{
  vnl_double_4 X(x.x(),x.y(),x.z(),1.0);
  vnl_double_3 t = P*X;
  return vgl_homg_point_2d<double>(t[0],t[1],t[2]);
}

//: project a 3d gaussian distribution into a 2d gaussian
bugl_gaussian_point_2d<double> brct_algos::project_3d_point(const vnl_double_3x4 &P,
                                                            const bugl_gaussian_point_3d<double> & X)
{
  vnl_double_4 Y(X.x(),X.y(),X.z(),1.0);
  vnl_double_3 u = P*Y;

  // compute weak prospect matrix, i.e., the car is far away from the camera
  vnl_double_2x3 H;

  // sum_{k=1}^{3} {P_{3k}Y_k + P_{34}
  double t = P[2][3];
  for (int k=0; k<3; k++)
    t += P[2][k]*Y[k];

  //
  for (int i=0; i<2; i++){
    double t1 = P[i][3];
    for (int k = 0; k<3; k++)
      t1 += P[i][k]*Y[k];

    for (int j=0; j<3; j++)
      H[i][j] = P[i][j] / t - P[2][j]* t1 / t/t;
  }

  vnl_matrix_fixed<double, 3, 3> Sigma3d = X.get_covariant_matrix();
  vnl_matrix_fixed<double, 2, 2> Sigma2d = H*Sigma3d*H.transpose();
#ifdef DEBUG
  vcl_cout << "Sigma2d\n" << Sigma2d << vcl_endl;
#endif
  vgl_homg_point_2d<double> hp2d(u[0], u[1], u[2]);
  vgl_point_2d<double> p2d(hp2d);
  return bugl_gaussian_point_2d<double>(p2d, Sigma2d);
}
//:  Given a set of perspective views of a point reconstruct the point according to least squares (SVD).
//   The cameras for each view are given.
//
vgl_point_3d<double> brct_algos::bundle_reconstruct_3d_point(vcl_vector<vnl_double_2> &pts, vcl_vector<vnl_double_3x4> &Ps)
{
  assert(pts.size() == Ps.size());
  unsigned int nviews = pts.size();

  vnl_matrix<double> A(2*nviews, 4, 0.0);

  for (unsigned int v = 0; v<nviews; v++)
    for (unsigned int i=0; i<4; i++) {
      A[2*v  ][i] = pts[v][0]*Ps[v][2][i] - Ps[v][0][i];
      A[2*v+1][i] = pts[v][1]*Ps[v][2][i] - Ps[v][1][i];
    }
  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}

vsol_box_3d_sptr brct_algos::get_bounding_box(vcl_vector<vgl_point_3d<double> > &pts_3d)
{
  vsol_box_3d_sptr box = new vsol_box_3d;

  unsigned int size = pts_3d.size();

  for (unsigned int i=0; i<size; i++) {
    vgl_point_3d<double> &pt = pts_3d[i];
    box->add_point(pt.x(), pt.y(), pt.z());
  }

  return box;
}

void brct_algos::add_box_vrml(double /*xmin*/, double /*ymin*/, double /*zmin*/, double /*xmax*/, double /*ymax*/, double /*zmax*/)
{
  vcl_cerr << "brct_algos::add_box_vrml() NYI\n"; // TODO
}

vgl_point_2d<double> brct_algos::closest_point(vdgl_digital_curve_sptr dc, vgl_point_2d<double> pt)
{
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = interp->get_edgel_chain();

  int i = bdgl_curve_algs::closest_point(ec, pt.x(), pt.y());

  return (*ec)[i].get_pt();
}

vgl_point_2d<double> brct_algos::most_possible_point(vdgl_digital_curve_sptr dc, bugl_gaussian_point_2d<double> &pt)
{
  vdgl_interpolator_sptr interp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = interp->get_edgel_chain();

  if (!ec)
  {
    vcl_cout<<"In brct_algos::most_possible_point(...) - warning, null chain\n";
    return vgl_point_2d<double>();
  }

  double maxd = -100;
  int N = ec->size();
  vgl_point_2d<double> pmax;

  for (int i=0; i<N; i++)
  {
    vgl_point_2d<double> p = (*ec)[i].get_pt();
    double d = pt.prob_at(p);
    if (d>maxd)
    {
      maxd = d;
      pmax = p;
    }
  }

  return pmax;
}

vnl_double_2 brct_algos::projection_3d_point(const vnl_double_3x4 &P, const vnl_double_3 &X)
{
  double t1 = P[2][3];
  for (int k=0; k<3; k++)
    t1 += P[2][k]*X[k];

  vnl_double_2 z;
  for (int i=0; i<2; i++)
  {
    double t0 = P[i][3];
    for (int k=0; k<3; k++)
      t0 += P[i][k]*X[k];

    z[i] = t0/t1;
  }

  return z;
}
//Gives a sort on mutual information decreasing order
static bool error_compare(brct_error_index* const e1,
                          brct_error_index* const e2)


{
  if (!e1||!e2)
    return false;
  return e1->error() > e2->error();
}

//:filter correspondences to remove outliers.
//  fraction is the portion of correspondences to be removed,
//  i.e. fraction = 0.1 takes out the points with top 10% projection error.
//
void brct_algos::filter_outliers(const vnl_double_3x3& K,
                                 const vnl_double_3& trans,
                                 vcl_vector<vnl_double_2> & pts_2d,
                                 vcl_vector<vnl_double_3> & pts_3d,
                                 double fraction)
{
  // compute camera calibration matrix
  vnl_double_3x4 Pe;
  Pe[0][0] = 1;    Pe[0][1] = 0;     Pe[0][2] = 0;      Pe[0][3] = trans[0];
  Pe[1][0] = 0;    Pe[1][1] = 1;     Pe[1][2] = 0;      Pe[1][3] = trans[1];
  Pe[2][0] = 0;    Pe[2][1] = 0;     Pe[2][2] = 1;      Pe[2][3] = trans[2];
  vnl_double_3x4 P = K*Pe;
  vcl_vector<brct_error_index*> errors;
  int Np = pts_2d.size();
  for (int i = 0; i<Np; i++)
  {
    //project the 3-d points
    vnl_double_4 X(pts_3d[i][0], pts_3d[i][1], pts_3d[i][2], 1.0);
    vnl_double_3 p = P*X;
    double x_proj = p[0]/p[2], y_proj = p[1]/p[2];
    double x = pts_2d[i][0], y = pts_2d[i][1];
    double error = vcl_sqrt((x-x_proj)*(x-x_proj)+(y-y_proj)*(y-y_proj));
    errors.push_back(new brct_error_index(i,error));
  }
  //sort the errors, largest first
  vcl_sort(errors.begin(), errors.end(), error_compare);
  int n_errors = errors.size();
  //remove the top fraction of correspondences
  int n_remove = (int)(fraction*n_errors);
  vcl_vector<vnl_double_2> filt_pts_2d;
  vcl_vector<vnl_double_3> filt_pts_3d;
  for (int i = 0; i<Np; i++)
  {
    bool erase = false;
    int k = 0;
    for (; k<n_remove&&!erase; k++)
      if (i==errors[k]->i())
        erase=true;
    if (!erase)
    {
      filt_pts_2d.push_back(pts_2d[i]);
      filt_pts_3d.push_back(pts_3d[i]);
    }
#ifdef DEBUG
    else
      vcl_cout << "Error(" << pts_2d[i][0] << ' ' << pts_2d[i][1]
               << ")/("<< pts_3d[i][0] << ' ' << pts_3d[i][0]
               << ' ' << pts_3d[i][2] << ")= " << errors[k]->error()
               << vcl_endl;
#endif
  }
  for (int k = 0; k<n_errors; k++)
    delete errors[k];
  //replace the point sets
  pts_2d = filt_pts_2d;
  pts_3d = filt_pts_3d;
 }

//: Determine the camera translation from a set of matched points.
//  The calibration matrix K is given along with a set of correspondences.
//  Form the design matrix, i.e. two equations from each image coordinate,
//  (ui vi).
//
// [wi ui]      [1  0  0  tx][Xi]
// [wi vi] = [K][0  1  0  ty][Yi]
// [  wi ]      [0  0  1  tz][Zi]
//                           [1 ]
// Solve for tx, ty, tz
//
bool brct_algos::camera_translation(const vnl_double_3x3& K,
                                    vcl_vector<vnl_double_2> & pts_2d,
                                    vcl_vector<vnl_double_3> & pts_3d,
                                    vnl_double_3& trans)
{
  int Np = pts_2d.size();
  vnl_matrix<double> A(2*Np, 4, 0.0);
  for (int ip = 0; ip<Np; ip++)
  {
    double sum0 = 0;
    double sum1 = 0;
    for (int j = 0; j<3; j++)
    {
      sum0 += K[0][j]*pts_3d[ip][j];
      sum1 += K[1][j]*pts_3d[ip][j];
      A[2*ip  ][j] = K[0][j];
      A[2*ip+1][j] = K[1][j];
    }
    sum0 -= pts_2d[ip][0]*pts_3d[ip][2];
    sum1 -= pts_2d[ip][1]*pts_3d[ip][2];
    A[2*ip  ][2]-=pts_2d[ip][0];
    A[2*ip+1][2]-=pts_2d[ip][1];
    A[2*ip  ][3] = sum0;
    A[2*ip+1][3] = sum1;
  }
  vnl_svd<double> svd_solver(A);
  vnl_double_4 t = svd_solver.nullvector();
  vnl_matrix<double> sings = svd_solver.W();
#ifdef DEBUG
  vcl_cout << "W:\n" << sings << vcl_endl;
#endif
  if (vcl_fabs(t[3])<1e-06)
  {
    vcl_cout << "In brct_algos::camera_translation(...) -"
             << " singular matrix - returning 0\n";
    trans[0] = 0.0; trans[1] = 0.0; trans[2] = 0.0;
    return false;
  }
  trans[0]=t[0]/t[3];   trans[1]=t[1]/t[3];   trans[2]=t[2]/t[3];
  return true;
}

void brct_algos::
robust_camera_translation(const vnl_double_3x3& K,
                          vcl_vector<bugl_gaussian_point_2d<double> > & pts_2d,
                          vcl_vector<vgl_point_3d<double> > & pts_3d,
                          vnl_double_3& trans)
{
  //filter out the uncorresponded 2d points
  vcl_vector<vnl_double_2> p2d;
  vcl_vector<vnl_double_3> p3d;
  int npts_2d = pts_2d.size();
  for (int i = 0; i<npts_2d; i++)
    if (pts_2d[i].exists())
    {
      p2d.push_back(vnl_double_2(pts_2d[i].x(),pts_2d[i].y()));
      p3d.push_back(vnl_double_3(pts_3d[i].x(),pts_3d[i].y(), pts_3d[i].z()));
    }
  //first solution
  if (!camera_translation(K, p2d, p3d, trans))
    return;
#ifdef DEBUG
  vcl_cout << "Initial Trans (" << trans[0] << ' ' << trans[1]
           << ' ' << trans[2] << ')' << vcl_endl;
#endif
    brct_algos::filter_outliers(K, trans, p2d, p3d);//JLM
  //recompute camera translation
  camera_translation(K, p2d, p3d, trans);
   vcl_cout << "Trans after filtering (" << trans[0] << ' ' << trans[1]
            << ' ' << trans[2] << ')' << vcl_endl;
  //just to print new errors
  brct_algos::filter_outliers(K, trans, p2d, p3d);
}
//
//:solve for the world translation given three points on a projective line.
// epi corresponds to -infinity, p0 == 0, p1 == 1 in world coordinates
// image epipolar location is taken to be zero on the epipolar line
//
static double line_trans(const double p0,
                         const double p1, const double p)
{
  if (p0==p1)
  {
    vcl_cout << "In line_trans(..) - infinite translation\n";
    return 0;
  }
  //from the cross ratio
  double pw = (p1*(p0-p))/(p*(p0-p1));
  return pw;
}
//: get the distance along a line from point0  to p
static double line_distance(vgl_line_segment_2d<double> const& seg,
                            vgl_point_2d<double> p)
{
  vgl_point_2d<double> p0 = seg.point1();
  vgl_vector_2d<double> v0(p0.x(), p0.y());
  vgl_vector_2d<double> v(p.x(), p.y());
  //get the length of p projected on  seg
  vgl_vector_2d<double> dir = seg.direction();
  vgl_vector_2d<double> dv = v-v0;
  double length = dot_product(dv,dir);
  return length;
}
//
//: compute camera translation using two corresponding points on a bundle of epipolar lines passing through the epipole.
//  The epipole, points_0 and points_1 define the projective transform
//  between the image and the world. We assume the translation magnitude
//  between points_0 and points_1 is unity.  The output will be the
//  world translation between points_0 and points.
void
brct_algos::camera_translation(vnl_double_3x3 const & K,
                               vnl_double_2 const& image_epipole,
                               vcl_vector<vnl_double_2> const& points_0,
                               vcl_vector<vnl_double_2> const& points_1,
                               vcl_vector<vnl_double_2> const& points,
                               vnl_double_3& T)
{
  unsigned int n = points_0.size();
  if (!n)
    return;
  assert(n==points_0.size());
  assert(n==points_1.size());
  vgl_point_2d<double> E(image_epipole[0], image_epipole[1]);
  double sumt = 0;
  for (unsigned int i =0; i<n; i++)
  {
    vgl_point_2d<double> P0(points_0[i][0],points_0[i][1]);
    //construct an image epipolar line through the epipole and p0
    vgl_line_segment_2d<double> seg(E, P0);
    //get the appropiate distances
    double p0 = line_distance(seg, P0);
    vgl_point_2d<double> P1(points_1[i][0],points_1[i][1]);
    double p1 = line_distance(seg, P1);
    vgl_point_2d<double> P(points[i][0],points[i][1]);
    double p = line_distance(seg, P);
    //get the line translation distance
    double t = line_trans(p0, p1, p);
    sumt += t;
  }
  sumt/=n;//the average epipolar translation magnitude
  vnl_double_3 e(E.x(), E.y(), 1);
  T = vnl_inverse(K) * e;
  T /= vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  if (T[2]<0)
    T *= -sumt;
  else
    T *= sumt;
  vcl_cout << "Camera Trans " << T << vcl_endl;
}
double brct_algos::motion_constant(vnl_double_2 const& image_epipole,
                                   int i,
                                   vnl_double_2& p_i,
                                   vnl_double_2& p_i1)
{
  vgl_point_2d<double> E(image_epipole[0], image_epipole[1]);
  vgl_point_2d<double> Pi(p_i[0],p_i[1]);
  //construct an image epipolar line through the epipole and p_i
  vgl_line_segment_2d<double> seg(E, Pi);
  //get the appropiate distances
  double k_i = line_distance(seg, Pi);
  vgl_point_2d<double> Pi1(p_i1[0],p_i1[1]);
  double k_i1 = line_distance(seg, Pi1);
  double dk = k_i1-k_i;
  double temp = (i+1)*k_i1 - i*k_i;
  if (!temp)
    return 0;
  double gamma = dk/temp;

  return gamma;
}

void brct_algos::print_motion_array(vnl_matrix<double>& H)
{
}

void brct_algos::write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 1 1 1 ]\n"
      << "  groundColor [ 1 1 1 ]\n"
      << "}\n"
      << "PointLight {\n"
      << "  on FALSE\n"
      << "  intensity 1\n"
      << "ambientIntensity 0\n"
      << "color 1 1 1\n"
      << "location 0 0 0\n"
      << "attenuation 1 0 0\n"
      << "radius 100\n"
      << "}\n";
}
void brct_algos::write_vrml_trailer(vcl_ofstream& str)
{
}
void brct_algos::write_vrml_points(vcl_ofstream& str,
                                   vcl_vector<vsol_point_3d_sptr> const& pts3d)
{
  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color NULL\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  int n = pts3d.size();
  for (int i =0; i<n; i++)
    str << -pts3d[i]->x() << ' ' << pts3d[i]->y() << ' ' << pts3d[i]->z() << '\n';
  str << "   ]\n"
      << "  }\n"
      << " }\n"
      << "}\n";
}
void brct_algos::write_vrml_box(vcl_ofstream& str, vsol_box_3d_sptr const& box,
                                const float r, const float g, const float b,
                                const float transparency)
{
  if (!box)
    return;
  double x0 = box->get_min_x(), y0 = box->get_min_y(), z0 = box->get_min_z();
  double width = box->width(), height = box->height(), depth = box->depth();
  double hw=width/2, hh=height/2, hd=depth/2;
  str << "Transform {\n"
      << "translation " << -(hw + x0) << ' ' << (hh + y0) << ' '
      << ' ' << (hd + z0) << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Box\n"
      <<   "{\n"
      << "  size " << width << ' ' << height << ' ' <<  depth << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

void brct_algos::read_vrml_points(vcl_ifstream& str,
                                  vcl_vector<vsol_point_3d_sptr>& pts3d)
{
  pts3d.clear();
  bool point_set_found = false;
  vcl_string s, ps = "point[";
  while (!point_set_found&&!str.eof())
  {
    str >> s;
    point_set_found = (s==ps);
  }
  if (!point_set_found)
    return;

  //we found the beginning of the points
  vcl_string np0 = "#", np1 = "npoints";
  str >> s;
  if (s!=np0)
    return;
  str >> s;
  if (s!=np1)
    return;
  int npts = 0;
  str >> npts;
  for (int i = 0; i<npts; i++)
  {
    double x=0, y=0, z=0;
    str >> x >> y >> z;
    vsol_point_3d_sptr p = new vsol_point_3d(x, y, z);
    pts3d.push_back(p);
  }
}

