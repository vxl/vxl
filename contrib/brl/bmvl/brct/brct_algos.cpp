//:
// \file
// \brief implementation of the brct_algos class.
//
//////////////////////////////////////////////////////////////////////

#include "brct_algos.h"
#include <vcl_algorithm.h>//for sort
#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_2x3.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d_optimize_lmq.h>
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <rrel/rrel_homography2d_est.h>
#include <rrel/rrel_ransac_obj.h>
#include <rrel/rrel_ran_sam_search.h>
#include <rrel/rrel_muset_obj.h>
#include <brip/brip_vil_float_ops.h>
#include <bdgl/bdgl_curve_algs.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <brct/brct_corr.h>
#include <brct/brct_corr_sptr.h>


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

//: NYI
void brct_algos::print_motion_array(vnl_matrix<double>& H)
{
  vcl_cerr << "brct_algos::print_motion_array(H) NYI\nH=\n" << H;
}

void brct_algos::write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
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

void brct_algos::write_vrml_trailer(vcl_ofstream&)
{
  vcl_cerr << "brct_algos::write_vrml_trailer(std::ofstream&) NYI (empty trailer)\n";
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
  str << "   ]\n  }\n }\n}\n";
}

void
brct_algos::write_vrml_points(vcl_ofstream& str,
                              vcl_vector<vgl_point_3d<double> > const& pts3d,
                              vcl_vector<vgl_point_3d<float> > const& color
                              )
{
  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "       color[\n";
  int n = color.size();
  for (int i =0; i<n; i++)
    str << color[i].x() << ' '
        << color[i].y() << ' '
        << color[i].z() << '\n';
  str << "   ]\n  }\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  n = pts3d.size();
  for (int i =0; i<n; i++)
    str << -pts3d[i].x() << ' ' << pts3d[i].y() << ' ' << pts3d[i].z() << '\n';
  str << "   ]\n  }\n }\n}\n";
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

void brct_algos::write_vrml_sphere(vcl_ofstream& str, vgl_sphere_3d<double> const& sphere,
                                const float r, const float g, const float b,
                                const float transparency)
{
  double x0 = sphere.centre().x(), y0 = sphere.centre().y(), z0 = sphere.centre().z();
  double rad = sphere.radius();
  str << "Transform {\n"
      << "translation " << -x0 << ' ' << y0 << ' '
      << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << rad << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

void brct_algos::write_vrml_polyline(vcl_ofstream& str,
                                vcl_vector<vgl_point_3d<double> > const& vts,
                                const float r,
                                const float g,
                                const float b)
{
  str << "Transform {\n"
      << "translation " << 0 << ' ' << 0 << ' '
      << ' ' << 0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      emissiveColor " << r << ' ' << g << ' ' << b << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedLineSet\n"
      <<   "{\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  int n = vts.size();
  for (int i =0; i<n; i++)
    str << -vts[i].x() << ' ' << vts[i].y() << ' ' << vts[i].z() << '\n';
  str << "   ]\n\n }"
      << "   coordIndex [\n";
  for (int i =0; i<n; i++)
    str << i << ',';
  str << -1 << ','
      << "   ]\n  }\n}"
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

bool brct_algos::
solve_p_matrix(vcl_vector<vgl_homg_point_2d<double> >const& image_points,
               vcl_vector<vgl_homg_point_3d<double> >const& world_points,
               vnl_double_3x4& P)
{
  //set up linear regression matrix
  //ingnore ideal points for now
  int ni = image_points.size();
  int nw = world_points.size();
  vgl_norm_trans_2d<double> Hi;
  vgl_norm_trans_3d<double> Hw;

  if (!Hi.compute_from_points(image_points))
    return false;
  if (!Hw.compute_from_points(world_points))
    return false;

  if (!ni || !nw || ni != nw)
    return false;
  vnl_matrix<double> M(2*ni, 12);
  int k = 0;
  vcl_vector<vgl_homg_point_2d<double> > norm_image_points;
  vcl_vector<vgl_homg_point_3d<double> > norm_world_points;

  for (int i = 0; i<ni; i++, k+=2)
  {
    vgl_homg_point_2d<double> const & pi = image_points[i];
    vgl_homg_point_3d<double> const & pw = world_points[i];
    //    vcl_cout << "pw[" << i << "]= " << pw << "pi[" << i << "]= " << pi << '\n';
    if (pi.ideal(1e-06)||pw.ideal(1e-06))
      return false;
    vgl_homg_point_2d<double> norm_pi = Hi(pi);
    vgl_homg_point_3d<double> norm_pw = Hw(pw);

    norm_image_points.push_back(norm_pi);
    norm_world_points.push_back(norm_pw);
    double u = norm_pi.x()/norm_pi.w(), v = norm_pi.y()/norm_pi.w();
    double X = norm_pw.x()/norm_pw.w();
    double Y = norm_pw.y()/norm_pw.w();
    double Z = norm_pw.z()/norm_pw.w();

    M[k][0] = X;  M[k][1] = Y;  M[k][2] = Z;  M[k][3] = 1;
    M[k][4] = 0;  M[k][5] = 0;  M[k][6] = 0;  M[k][7] = 0;
    M[k][8] = -u*X; M[k][9] = -u*Y; M[k][10] = -u*Z; M[k][11] = -u;
    M[k+1][0] = 0;  M[k+1][1] = 0;  M[k+1][2] = 0;  M[k+1][3] = 0;
    M[k+1][4] = X;  M[k+1][5] = Y;  M[k+1][6] = Z;  M[k+1][7] = 1;
    M[k+1][8] = -v*X; M[k+1][9] = -v*Y; M[k+1][10] = -v*Z; M[k+1][11] = -v;
  }
  M.normalize_rows();
  vnl_svd<double> svd(M);
  vnl_double_3x4 P_prime;
  P_prime.set(svd.nullvector().data_block());
  // Next, P' has to be transformed back to the coordinate system of
  // the original point sets where
  // Hi(pi) = P' * Hw(pw)
  // So pi = [Hi^-1*P'*Hw] * pw
  vnl_matrix_fixed<double ,3,3> Mi = Hi.get_matrix();
  vnl_matrix_fixed<double ,3,3> Mi_inv = vnl_inverse(Mi);
  vnl_matrix_fixed<double ,4,4> Mw = Hw.get_matrix();
  P = Mi_inv*P_prime*Mw;
  return true;
}

bool brct_algos::
compute_euclidean_camera(vcl_vector<vgl_point_2d<double> > const& image_points,
                         vcl_vector<vgl_point_3d<double> > const& world_points,
                         vnl_double_3x3 const & K,
                         vnl_double_3x4& P)
{
  //convert the input image points to homogeneous points
  vcl_vector<vgl_homg_point_2d<double> > h_image_points;
  vnl_double_3x3 Kinv = vnl_inverse(K);
  for (vcl_vector<vgl_point_2d<double> >::const_iterator pit = image_points.begin();
       pit != image_points.end(); pit++)
  {
    vnl_double_3 p((*pit).x(), (*pit).y(), 1.0), pnorm = Kinv*p;
    h_image_points.push_back(vgl_homg_point_2d<double>(pnorm[0], pnorm[1], pnorm[2]));
  }
  //convert the input world points to normalized homogeneous points
  vcl_vector<vgl_homg_point_3d<double> > h_world_points;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
    h_world_points.push_back(vgl_homg_point_3d<double>((*pit).x(),
                                                       (*pit).y(),
                                                       (*pit).z()));
  vnl_double_3x4 Pn;
  if (!brct_algos::solve_p_matrix(h_image_points, h_world_points, Pn))
    return false;

  //transform back to original image coordinates
  P = K*Pn;
  return true;
}

//: Assume that the world points have all z ==0. Otherwise a preprocessing
// step to project points onto the best fitting plane.
bool brct_algos::homography(vcl_vector<vgl_point_3d<double> > const& world_points,
                            vcl_vector<vgl_point_2d<double> > const& image_points,
                            vgl_h_matrix_2d<double> & H,
                            bool optimize)
{
  //convert points to homogeneous points
  vcl_vector<vgl_homg_point_2d<double> > h_image_points;
  for (vcl_vector<vgl_point_2d<double> >::const_iterator pit = image_points.begin();
       pit != image_points.end(); pit++)
    h_image_points.push_back(vgl_homg_point_2d<double>((*pit).x(), (*pit).y()));

  vcl_vector<vgl_homg_point_2d<double> > h_world_points;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
    h_world_points.push_back(vgl_homg_point_2d<double>((*pit).x(),(*pit).y()));
  vgl_h_matrix_2d_compute_linear hc;
  hc.verbose(true);
  bool success = hc.compute(h_world_points, h_image_points, H);
  if (!success)
    return false;
  if (!optimize)
    return true;
  // optimize the homography using Levenberg-Marquardt
  vgl_h_matrix_2d_optimize_lmq lm(H);
  return lm.optimize(h_world_points, h_image_points, H);
}

bool brct_algos::
homography_ransac(vcl_vector<vgl_point_3d<double> > const& world_points,
                  vcl_vector<vgl_point_2d<double> > const& image_points,
                  vgl_h_matrix_2d<double> & H, bool optimize)
{
  //convert points to homogeneous points
  vcl_vector<vgl_homg_point_2d<double> > h_image_points;
  for (vcl_vector<vgl_point_2d<double> >::const_iterator pit = image_points.begin();
       pit != image_points.end(); pit++)
    h_image_points.push_back(vgl_homg_point_2d<double>((*pit).x(), (*pit).y()));

  vcl_vector<vgl_homg_point_2d<double> > h_world_points;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
    h_world_points.push_back(vgl_homg_point_2d<double>((*pit).x(),(*pit).y()));

  int trace_level = 0;
  rrel_homography2d_est hg( h_world_points, h_image_points);
  rrel_ransac_obj* ransac = new rrel_ransac_obj();
  hg.set_prior_scale(1.0);
  rrel_ran_sam_search* ransam = new rrel_ran_sam_search;
  ransam->set_trace_level(trace_level);
#if 0
  double max_outlier_frac = 0.5;
  double desired_prob_good = 0.99;
  int max_pops = 1;
  ransam->set_sampling_params( max_outlier_frac, desired_prob_good, max_pops);
#endif // 0
  ransam->set_gen_all_samples();
  if ( !ransam->estimate( &hg, ransac ) )
  {vcl_cout << "RANSAC failed!!\n";
  return false;}
    else {
      vcl_cout << "RANSAC succeeded.\n"
               << "estimate = " << ransam->params() << vcl_endl
               << "scale = " << ransam->scale() << vcl_endl;
    }
  H.set((ransam->params()).data_block());
  delete ransac;
  delete ransam;
  if (!optimize)
    return true;

  // optimize the homography using Levenberg-Marquardt
  vgl_h_matrix_2d_optimize_lmq lm(H);
  return lm.optimize(h_world_points, h_image_points, H);
}

bool brct_algos::
homography_muse(vcl_vector<vgl_point_3d<double> > const& world_points,
                vcl_vector<vgl_point_2d<double> > const& image_points,
                vgl_h_matrix_2d<double> & H, bool optimize)
{
  //convert points to homogeneous points
  vcl_vector<vgl_homg_point_2d<double> > h_image_points;
  for (vcl_vector<vgl_point_2d<double> >::const_iterator pit = image_points.begin();
       pit != image_points.end(); pit++)
    h_image_points.push_back(vgl_homg_point_2d<double>((*pit).x(), (*pit).y()));

  vcl_vector<vgl_homg_point_2d<double> > h_world_points;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
    h_world_points.push_back(vgl_homg_point_2d<double>((*pit).x(),(*pit).y()));

  int trace_level = 0;
  rrel_homography2d_est hg( h_world_points, h_image_points);
  hg.set_no_prior_scale();
  rrel_muset_obj* muset = new rrel_muset_obj( world_points.size()+1 );
  rrel_ran_sam_search* ransam = new rrel_ran_sam_search;
  ransam->set_trace_level(trace_level);
  ransam->set_gen_all_samples();
  if ( !ransam->estimate( &hg, muset ) )
  {vcl_cout << "MUSE failed!!\n";
  return false;}
    else {
      vcl_cout << "MUSE succeeded.\n"
               << "estimate = " << ransam->params() << vcl_endl
               << "scale = " << ransam->scale() << vcl_endl;
    }

  H.set((ransam->params()).data_block());
  delete muset;
  delete ransam;
  if (!optimize)
    return true;

  // optimize the homography using Levenberg-Marquardt
  vgl_h_matrix_2d_optimize_lmq lm(H);
  return lm.optimize(h_world_points, h_image_points, H);
}

void brct_algos:: project(vcl_vector<vgl_point_3d<double> > const& world_points,
                          vgl_h_matrix_2d<double> const& H,
                          vcl_vector<vgl_point_2d<double> > & image_points)
{
  image_points.clear();
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
  {
    vgl_homg_point_2d<double> pw((*pit).x(),(*pit).y());
    vgl_homg_point_2d<double> pi = H(pw);
    image_points.push_back(vgl_point_2d<double>(pi));
  }
}

 //: project world points into an image using a projection matrix
void brct_algos::project(vcl_vector<vgl_point_3d<double> > const& world_points,
                     vgl_p_matrix<double> const& P,
                     vcl_vector<vgl_point_2d<double> > & image_points)
{
  image_points.clear();
  image_points.resize(0);
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
  {
    vgl_homg_point_3d<double> pw((*pit).x(),(*pit).y(), (*pit).z());
    vgl_homg_point_2d<double> pi = P(pw);
    image_points.push_back(vgl_point_2d<double>(pi));
    vcl_cout << "W:" << *pit << " I:" << vgl_point_2d<double>(pi) << '\n';
  }
}

// project a 3-d polygon with a planar homography (z is considered zero);
vsol_polygon_2d_sptr
brct_algos::project(vsol_polygon_3d_sptr const& world_poly,
                    vgl_h_matrix_2d<double> const& H)
{
  vcl_vector<vsol_point_2d_sptr > image_verts;
  unsigned nv = world_poly->size();
  for (unsigned i = 0; i<nv; ++i)
  {
    vsol_point_3d_sptr v3d = world_poly->vertex(i);
    vgl_homg_point_2d<double> hv(v3d->x(), v3d->y());
    vgl_homg_point_2d<double> phv = H(hv);
    vgl_point_2d<double> v2d(phv);
    vsol_point_2d_sptr p = new vsol_point_2d(v2d.x(), v2d.y());
    image_verts.push_back(p);
  }
  return new vsol_polygon_2d(image_verts);
}

void brct_algos::project(vcl_vector<vsol_polygon_3d_sptr> const& world_polys,
                         vgl_h_matrix_2d<double> const& H,
                         vcl_vector<vsol_polygon_2d_sptr > & image_polys)
{
  image_polys.clear();
  for (vcl_vector<vsol_polygon_3d_sptr>::const_iterator wpi=world_polys.begin();
      wpi != world_polys.end(); ++wpi)
    image_polys.push_back(brct_algos::project(*wpi, H));
}

vsol_polygon_2d_sptr
brct_algos::project(vsol_polygon_3d_sptr const& world_poly,
                    vgl_p_matrix<double> const& P)
{
  vcl_vector<vsol_point_2d_sptr > image_verts;
  unsigned nv = world_poly->size();
  for (unsigned i = 0; i<nv; ++i)
  {
    vsol_point_3d_sptr v3d = world_poly->vertex(i);
    vgl_homg_point_3d<double> hv(v3d->x(), v3d->y(), v3d->z());
    vgl_homg_point_2d<double> phv = P(hv);
    vgl_point_2d<double> v2d(phv);
    vsol_point_2d_sptr p = new vsol_point_2d(v2d.x(), v2d.y());
    image_verts.push_back(p);
  }
  return new vsol_polygon_2d(image_verts);
}

void brct_algos::project(vcl_vector<vsol_polygon_3d_sptr> const& world_polys,
                         vgl_p_matrix<double> const& P,
                         vcl_vector<vsol_polygon_2d_sptr > & image_polys)
{
  image_polys.clear();
  for (vcl_vector<vsol_polygon_3d_sptr>::const_iterator wpi=world_polys.begin();
      wpi != world_polys.end(); ++wpi)
    image_polys.push_back(brct_algos::project(*wpi, P));
}

vsol_polygon_3d_sptr
brct_algos::back_project(vsol_polygon_2d_sptr const& image_poly,
                         vgl_h_matrix_2d<double> const& H)
{
  vcl_vector<vsol_point_3d_sptr > world_verts;
  unsigned nv = image_poly->size();
  for (unsigned i = 0; i<nv; ++i)
  {
    vsol_point_2d_sptr v2d = image_poly->vertex(i);
    vgl_homg_point_2d<double> hv2d(v2d->x(), v2d->y());
    vgl_homg_point_2d<double> hv3d = H.preimage(hv2d);
    vgl_point_2d<double> v3d(hv3d);
    vsol_point_3d_sptr p = new vsol_point_3d(v3d.x(), v3d.y(), 0);
    world_verts.push_back(p);
  }
  return new vsol_polygon_3d(world_verts);
}

vsol_polygon_3d_sptr
brct_algos::back_project(vsol_polygon_2d_sptr const& image_poly,
                         vgl_p_matrix<double> const& P,
                         const double height)
{
  // construct the homography for a plane parallel to the x-y plane at
  // a given height
  vnl_double_3x3 M;
  vnl_double_3x4 Mp;
  Mp = P.get_matrix();
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<2; ++c)
      M[r][c]=Mp[r][c];
  for (unsigned r = 0; r<3; ++r)
    M[r][2] = Mp[r][2]*height + Mp[r][3];
  vgl_h_matrix_2d<double> H(M);
  vsol_polygon_3d_sptr temp = brct_algos::back_project(image_poly, H);
  vcl_vector<vsol_point_3d_sptr> verts;
  for (unsigned i = 0; i<image_poly->size(); ++i)
  {
    vsol_point_3d_sptr p = new vsol_point_3d(*temp->vertex(i));
    p->set_z(height);
    verts.push_back(p);
  }
  return new vsol_polygon_3d(verts);
}

void brct_algos::
scale_and_translate_world(vcl_vector<vgl_point_3d<double> > const& world_points,
                          const double magnification,
                          vgl_h_matrix_2d<double> & H)
{
  double xmin = vnl_numeric_traits<double>::maxval;
  double ymin = xmin;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = world_points.begin();
       pit != world_points.end(); pit++)
  {
    xmin = vnl_math_min(xmin, (*pit).x());
    ymin = vnl_math_min(ymin, (*pit).y());
  }
  brct_algos::scale_and_translate_world(xmin, ymin, magnification, H);
}

void brct_algos::scale_and_translate_world( const double world_x_min,
                                            const double world_y_min,
                                            const double magnification,
                                            vgl_h_matrix_2d<double> & H)
{
  vnl_double_3x3 M, Mt, Ms;
  Ms[0][0]=magnification;   Ms[0][1]=0;   Ms[0][2]=0;
  Ms[1][0]=0;   Ms[1][1]=magnification;   Ms[1][2]=0;
  Ms[2][0]=0;   Ms[2][1]=0;   Ms[2][2]=1.0;

  Mt[0][0]=1.0;   Mt[0][1]=0;   Mt[0][2]=-world_x_min;
  Mt[1][0]=0;     Mt[1][1]=1.0; Mt[1][2]=-world_y_min;
  Mt[2][0]=0;     Mt[2][1]=0;   Mt[2][2]=1.0;
  M = Ms*Mt;
  H = vgl_h_matrix_2d<double>(M);
}

vnl_double_4x4 brct_algos::convert_to_target(vnl_double_3x4 const& P)
{
  vnl_double_4x4 T;
  for (int r = 0; r<2; r++)
    for (int c = 0; c<4; c++)
      T[r][c]=P[r][c];

  for (int c = 0; c<3; c++)
    T[2][c] = 0;

  T[2][3] = 1.0;

  for (int c = 0; c<4; c++)
    T[3][c] = P[2][c];

  return T.transpose();
}

bool brct_algos::read_world_points(vcl_ifstream& str,
                                   vcl_vector<vgl_point_3d<double> >& world_points)
{
  world_points.clear();
  vcl_string temp;
  str >> temp;
  if (temp != "NUMPOINTS:")
    return false;
  int n_pts;
  str >> n_pts;
  for (int i = 0; i<n_pts; i++)
  {
    str >> temp;
    if (temp != "POINT3D:")
      return false;
    vgl_point_3d<double> world_point;
    str >> world_point;
    vcl_cout << "W " << world_point << '\n';
    world_points.push_back(world_point);
  }
  return true;
}

bool brct_algos::read_world(vcl_ifstream& str,
                            vcl_vector<vgl_point_3d<double> >& world_points,
                            vcl_vector<vsol_polygon_3d_sptr>& polys,
                            vcl_vector<vcl_vector<unsigned> >& indexed_face_set)
{
  if (!read_world_points(str, world_points))
    return false;
  unsigned npts = world_points.size();
  //see if there are any polygons in the world
 if (str.eof())
    return true;
  vcl_string temp;
  str >> temp;
  if (temp == "")
     return true;
  //there are polygons
  if (temp != "NPOLYS:")
    return false;
  unsigned npolys, nverts;
  str >> npolys;
  for (unsigned ip = 0; ip<npolys; ++ip)
  {
    str >> temp;
    if (temp != "NVERTS:")
      return false;
    str >> nverts;
    vcl_vector<vsol_point_3d_sptr> verts;
    vcl_vector<unsigned> vert_indices;
    for (unsigned i=0; i< nverts; ++i)
    {
      unsigned vert_index;
      str >> vert_index;
      if (vert_index >= npts)
        return false;
      vert_indices.push_back(vert_index);
      vgl_point_3d<double> v = world_points[vert_index];
      vsol_point_3d_sptr sv = new vsol_point_3d(v);
      verts.push_back(sv);
    }
    indexed_face_set.push_back(vert_indices);
    polys.push_back(new vsol_polygon_3d(verts));
  }
  return true;
}

void brct_algos::
write_world_points(vcl_ofstream& str,
                   vcl_vector<vgl_point_3d<double> >const& world_points)
{
  unsigned npts = world_points.size();
  str << "NUMPOINTS: " << npts << '\n';
  for (unsigned i = 0; i<npts; i++)
  {
    str << "POINT3D: ";
    vgl_point_3d<double> p3d = world_points[i];
    str << p3d.x() << ' ' << p3d.y() << ' ' << p3d.z() << '\n';
  }
}

void brct_algos::
write_world(vcl_ofstream& str,
            vcl_vector<vgl_point_3d<double> > const& world_points,
            vcl_vector<vcl_vector<unsigned> > const& polys)
{
  brct_algos::write_world_points(str, world_points);
  unsigned npolys = polys.size();

  str << "NPOLYS: " << npolys << '\n';
  for (unsigned ip = 0; ip<npolys; ++ip)
  {
    unsigned nverts = polys[ip].size();
    str << "NVERTS: " << nverts << '\n';
    for (unsigned iv = 0; iv<nverts; ++iv)
      str << polys[ip][iv] << ' ';
    str << '\n';
  }
}

void brct_algos::
write_world_ply2(vcl_ofstream& str,
                 vcl_vector<vgl_point_3d<double> > const& world_points,
                 vcl_vector<vcl_vector<unsigned> > const& polys)
{
  unsigned npts = world_points.size();
  unsigned nfaces = polys.size();
  str << npts << '\n'
      << nfaces << '\n';
  for (unsigned iv = 0; iv<npts; ++iv)
  {
    vgl_point_3d<double> p = world_points[iv];
    str << p.x() << ' ' << p.y() << ' ' << p.z() << '\n';
  }
  for (unsigned fi = 0; fi<nfaces; ++fi)
  {
    unsigned nverts = polys[fi].size();
    str << nverts << ' ';
    for (unsigned vi = 0; vi<nverts; ++vi)
      str << polys[fi][vi] << ' ';
    str << '\n';
  }
}

bool brct_algos::
read_world_ply2(vcl_ifstream& str,
                vcl_vector<vgl_point_3d<double> >& world_points,
                vcl_vector<vsol_polygon_3d_sptr>& polys,
                vcl_vector<vcl_vector<unsigned> >& indexed_face_set)
{
  world_points.clear();
  polys.clear();
  indexed_face_set.clear();
  unsigned npts=0, nfaces=0;
  str >> npts >> nfaces;
  for (unsigned i = 0; i<npts; ++i)
  {
    double x=0, y=0, z=0;
    str >> x >> y >> z;
    vgl_point_3d<double> p(x, y, z);
    world_points.push_back(p);
  }
  unsigned nverts = 0;
  for (unsigned ip = 0; ip<nfaces; ++ip)
  {
    vcl_vector<vsol_point_3d_sptr> verts;
    vcl_vector<unsigned> vert_indices;
    str >> nverts;
    for (unsigned i=0; i< nverts; ++i)
    {
      unsigned vert_index;
      str >> vert_index;
      if (vert_index >= npts)
        return false;
      vert_indices.push_back(vert_index);
      vgl_point_3d<double> v = world_points[vert_index];
      vsol_point_3d_sptr sv = new vsol_point_3d(v);
      verts.push_back(sv);
    }
    indexed_face_set.push_back(vert_indices);
    polys.push_back(new vsol_polygon_3d(verts));
  }
  return true;
}

bool brct_algos::
read_target_corrs(vcl_ifstream& str,
                  vcl_vector<bool>& valid,
                  vcl_vector<vgl_point_2d<double> >& image_points,
                  vcl_vector<vgl_point_3d<double> >& world_points)
{
  vcl_string temp;
  str >> temp;
  if (temp != "NUMPOINTS:")
    return false;
  int n_corrs;
  str >> n_corrs;
  for (int i = 0; i<n_corrs; i++)
  {
    str >> temp;
    if (temp != "CORRESP:")
      return false;
    bool val;
    int junk;
    vgl_point_2d<double> image_point;
    vgl_point_3d<double> world_point;
    str >> val >> junk
        >> world_point >> image_point;
    vcl_cout << "W " << world_point << "  I " << image_point << '\n';
    valid.push_back(val);
    image_points.push_back(image_point);
    world_points.push_back(world_point);
  }
  return true;
}

bool brct_algos::write_corrs(vcl_ofstream& str,
                             vcl_vector<bool>& valid,
                             vcl_vector<vgl_point_2d<double> >& image_points,
                             vcl_vector<vgl_point_3d<double> >& world_points)
{
  if (!str)
    return false;
  str << "NUMPOINTS:";
  int n_corrs = world_points.size();
  str << ' ' << n_corrs << '\n';
  for (int i = 0; i<n_corrs; i++)
  {
    vgl_point_3d<double> P = world_points[i];
    vgl_point_2d<double> p = image_points[i];
    str << "CORRESP:"
        << ' ' << (int)valid[i]
        << ' ' << i
        << ' ' << P.x() << ' ' << P.y() << ' ' << P.z()
        << ' ' << p.x() << ' ' << p.y() << '\n';
  }
  return true;
}

bool brct_algos::write_brct_corrs(vcl_ofstream& str,
                                  vcl_vector<brct_corr_sptr> const& corrs)
{
  unsigned n = corrs.size();
  if (!n)
    return false;
  brct_corr_sptr c0 = corrs[0];
  int ncams = c0->n_cams();
  str << "NCAMS:" << ' ' << ncams << '\n'
      << "NCORRS:" << ' ' << n << '\n';
  for (unsigned i = 0; i<n; ++i)
    for (int c = 0; c<ncams; ++c)
    {
      bool v = corrs[i]->valid(c);
      if (v) {
        str << "VALID:" << ' ' << 1 << '\n'
            << "X:" << ' ' << (corrs[i]->match(c)).x() << '\n'
            << "Y:" << ' ' << (corrs[i]->match(c)).y() << '\n';
      } else {
        str << "VALID:" << ' ' << 0 << '\n'
            << "X:" << ' ' << -1 << '\n'
            << "Y:" << ' ' << -1 << '\n';
      }
    }
  return true;
}

bool brct_algos::read_brct_corrs(vcl_ifstream& str,
                                 vcl_vector<brct_corr_sptr>& corrs)
{
  corrs.clear();
  vcl_string temp;
  str >> temp;
  if (temp != "NCAMS:")
    return false;
  int ncams;
  str >> ncams;
  str >> temp;
  if (temp != "NCORRS:")
    return false;
  unsigned ncorrs;
  str >> ncorrs;
  for (unsigned i = 0; i<ncorrs; ++i)
  {
    brct_corr_sptr bc = new brct_corr(ncams);
    for (int c = 0; c<ncams; ++c)
    {
      str >> temp;
      if (temp != "VALID:")
        return false;
      bool v;
      str >> v;
      if (v) {
        str >> temp;
        if (temp != "X:")
          return false;
        double x, y;
        str >> x;
        str >> temp;
        if (temp != "Y:")
          return false;
        str >> y;
        bc->set_match(c, x, y);
      }
    }
    corrs.push_back(bc);
  }
  return true;
}

void brct_algos::reconstruct_corrs(vcl_vector<brct_corr_sptr> const& image_corrs,
                       vpgl_proj_camera<double> const& cam0,
                       vpgl_proj_camera<double> const& cam1,
                       vcl_vector<vgl_point_3d<double> >& world_points)
{
  world_points.clear();
  for(unsigned i = 0; i<image_corrs.size(); ++i)
    {
      brct_corr_sptr cr = image_corrs[i];
      if(cr->n_cams()!=2)
        continue;
      vgl_homg_point_2d<double> hp0 = cr->match(0);
      vgl_homg_point_2d<double> hp1 = cr->match(1);
      vgl_point_2d<double> x0(hp0), x1(hp1);
      vgl_point_3d<double> Xw = brct_algos::triangulate_3d_point(x0,cam0.get_matrix(), x1, cam1.get_matrix());
      world_points.push_back(Xw);
    }
}

void brct_algos::write_target_camera(vcl_ofstream& str, vnl_double_3x4 const& P)
{
  str << "TRANSFORM:\n"
      << 0 << '\n' << 0 << '\n' << 0 <<'\n';
  vnl_double_4x4 T = brct_algos::convert_to_target(P);
  vcl_cout << "T =\n" << T << '\n';
  str << T << '\n';
  str.close();
}

vnl_double_2 brct_algos::target_project(vnl_double_4x4 const& T,
                                        vnl_double_3 const& v)
{
  const int axis = 2;
  const int dim = 3;
  vnl_double_2 proj;
  proj.fill(0.0);
  int i, j;
  double h = T[3][3];            // homogeneous component
  for (i = 0; i < dim; i++)      // from last column of t
    h += v[i] * T[i][3];

  for (j = 0; j < dim-1; j++) {    // first n-1 components
    int tj = (axis + j + 1) % dim; // column index in transform
    double pj = T[3][tj];          // from n-1 columns of t
    for (i = 0; i < dim; i++)      // CCW after axis column
      pj += v[i] * T[i][tj];
    proj[j] = pj / h;
  }
  return proj;
}

//------------------------------------------------------------
//: form a p_matrix from a homography, assumes the world plane is X-Y.
//
// Given H = [h0|h1|h2], then P = [h0|h1|0|h2]
//
vgl_p_matrix<double> brct_algos::p_from_h(vgl_h_matrix_2d<double> const& H)
{
  vnl_double_3x3 M = H.get_matrix();
  vnl_double_3x4 Mp;
  for (int r = 0; r<3; r++)
  {
    Mp[r][0] = M[r][0];
    Mp[r][1] = M[r][1];
    Mp[r][2] = 0;
    Mp[r][3] = M[r][2];
  }
  vgl_p_matrix<double> P(Mp);
  return P;
}

//------------------------------------------------------------
//: form a p_matrix from a homography and Z-y pairs, assumes the world plane is X-Y.
//  hi are columns from H
// Given H = [h0|h1|h2], then P = [h0|h1|h'|h2], where h' is of the form
// \verbatim
// _   _
// | 0 |
// | v |
// | 0 |
// -   -
// \endverbatim
//  Given a y-Z pair
// \verbatim
//         h01 X + h11 Y  +  v Z + h21
//   y =   -------------------------------
//             h02 X + h12 Y  + h22
// \endverbatim
//  Thus  $ v =  (y(h_{02}X + h_{12}Y  + h_{22}) - h_{01}X - h_{11}Y - h_{21}) / Z $
//
vgl_p_matrix<double> brct_algos::
p_from_h(vgl_h_matrix_2d<double> const& H, vcl_vector<double> const& image_y,
         vcl_vector<vgl_point_3d<double> > const& world_p)
{
  unsigned n = world_p.size();
  if (!n)
    return brct_algos::p_from_h(H);
  vnl_double_3x3 h = H.get_matrix();
  double vsum = 0;
  for (unsigned i = 0; i<n; ++i)
  {
    vgl_point_3d<double> pw = world_p[i];
    if (vcl_fabs(pw.z()) < 0.01)
      continue;
    //note h has row vectors not column vectors as in comments
    //so transpose indices
    double temp = image_y[i]*(h[2][0]*pw.x() + h[2][1]*pw.y() + h[2][2]);
    temp -= h[1][0]*pw.x();
    temp -= h[1][1]*pw.y();
    temp -= h[1][2];
    temp /= pw.z();
    vsum += temp;
  }
  vsum /= n;
  if (vcl_fabs(vsum) < 1e-6)
    return brct_algos::p_from_h(H);
  vgl_p_matrix<double> P = brct_algos::p_from_h(H);
  vnl_double_3x4 p = P.get_matrix();
  p[1][2] = vsum;
  P.set(p);
  return P;
}

vgl_h_matrix_2d<double> brct_algos::h_from_p(vgl_p_matrix<double> const& P)
{
  vnl_double_3x4 M = P.get_matrix();
  vnl_double_3x3 Mh;
  for (int r = 0; r<3; r++)
  {
    Mh[r][0] = M[r][0];
    Mh[r][1] = M[r][1];
    Mh[r][2] = M[r][3];
  }
  vgl_h_matrix_2d<double> H(Mh);
  return H;
}

//Map an image onto a world plane.
vil_image_resource_sptr brct_algos::
map_image_to_world(vil_image_resource_sptr const& image,
                   vgl_p_matrix<double> const& cam,
                   const double world_units_per_pixel)
{
  (void) world_units_per_pixel;//scaling not implemented
  if (!image)
    return 0;
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(image);
  //for now only planar mapping is supported (i.e. no ray backprojection onto a 3-d surface)
  //extract a planar homography from the perspective camera
  vnl_double_3x4 M = cam.get_matrix();
  vnl_double_3x3 Mh, Mhinv; //the planar homography
  //copy first two columns
  for (unsigned c = 0; c<2; ++c)
    for (unsigned r = 0; r<3; ++r)
      Mh[r][c] = M[r][c];
  //copy last column
  Mh[0][2]=M[0][3]; Mh[1][2]=M[1][3]; Mh[2][2]=M[2][3];
  //invert to get image-to-world homography
  vnl_svd<double> svd(Mh);
  Mhinv = svd.inverse();
  vgl_h_matrix_2d<double> H(Mhinv);
  vil_image_view<float> mapped_image, temp;
  if (!brip_vil_float_ops::homography(fimage, H, temp))
    return 0;
  mapped_image = vil_flip_ud(temp);

  if (image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char> cimage =
      brip_vil_float_ops::convert_to_byte(mapped_image);
    return vil_new_image_resource_of_view(cimage);
  }
  if (image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<unsigned short> simage =
      brip_vil_float_ops::convert_to_short(mapped_image, 0, 65536);
    return vil_new_image_resource_of_view(simage);
  }
  return 0;
}

bool brct_algos::
save_constraint_file(vcl_vector<vgl_point_2d<double> > const& image_pts,
                     vcl_vector<bool> const& valid,
                     vcl_vector<vgl_point_3d<double> > const& world_pts,
                     vcl_vector<vgl_line_segment_2d<double> > const& vertls,
                     vcl_ofstream& str)
{
  unsigned npts = image_pts.size();
  if (npts!=world_pts.size())
    return false;
  str << "dbrcl camera constraint file\n\n"
      << "frame\n"
      << 0 << "\n\n";
  for (unsigned i = 0; i<npts; ++i)
  {
    if (!valid[i])
      continue;
    str << "world image correspondence\n"
        << world_pts[i].x()<< ' ' << world_pts[i].y() << ' ' << world_pts[i].z() << '\n'
        << image_pts[i].x()<< ' ' << image_pts[i].y() << "\n\n";
  }
  for (vcl_vector<vgl_line_segment_2d<double> >::const_iterator lit = vertls.begin(); lit != vertls.end(); ++lit)
  {
    str << "height\n"
        << (*lit).point1().x() << ' ' << (*lit).point1().y() << '\n'
        << (*lit).point2().x() << ' ' << (*lit).point2().y() << "\n\n";
  }
  return true;
}

//Create a 3-d box as an indexed face set. Specified by face corners on
// top face. The top face is parallel to the x-y plane
//
//               c1        c2
//                0--------0
//                |        |
//                |        |  arbitrary orientation
//             c0 0--------0
//
void brct_algos::box_3d(vgl_point_3d<double> const& c0,
                        vgl_point_3d<double> const& c1,
                        vgl_point_3d<double> const& c2,
                        vcl_vector<vgl_point_3d<double> >& world_points,
                        vcl_vector<vsol_polygon_3d_sptr>& polys,
                        vcl_vector<vcl_vector<unsigned> >& indexed_face_set)
{
  world_points.clear();
  polys.clear();
  indexed_face_set.clear();
  vgl_point_3d<double> p[8];
  p[0].set(c0.x(), c0.y(), c0.z());
  p[1].set(c1.x(), c1.y(), c1.z());
  p[2].set(c2.x(), c2.y(), c2.z());
  p[3].set(c0.x()+c2.x()-c1.x(), c0.y()+c2.y()-c1.y(), c0.z());
  for (unsigned i = 4; i<8; ++i)
    p[i].set(p[i-4].x(), p[i-4].y(), 0.0);

  for (unsigned i = 0; i<8; ++i)
  {
    world_points.push_back(p[i]);
    vcl_cout << "p[" << i << "]= " << p[i] << '\n';
  }

  vcl_vector<unsigned> v(4);
  v[0]=0;   v[1]=3;   v[2]=2; v[3]=1;
  indexed_face_set.push_back(v);
  v[0]=2;   v[1]=3;   v[2]=7; v[3]=6;
  indexed_face_set.push_back(v);
  v[0]=0;   v[1]=4;   v[2]=7; v[3]=3;
  indexed_face_set.push_back(v);
  v[0]=4;   v[1]=5;   v[2]=6; v[3]=7;
  indexed_face_set.push_back(v);
  v[0]=0;   v[1]=1;   v[2]=5; v[3]=4;
  indexed_face_set.push_back(v);
  v[0]=1;   v[1]=2;   v[2]=6; v[3]=5;
  indexed_face_set.push_back(v);
  for (unsigned ip = 0; ip<6; ++ip)
  {
    vcl_vector<vsol_point_3d_sptr> verts;
    for (unsigned iv = 0; iv<4; ++iv)
    {
      vsol_point_3d_sptr pv =
        new vsol_point_3d(p[indexed_face_set[ip][iv]]);
      verts.push_back(pv);
    }
    polys.push_back(new vsol_polygon_3d(verts));
  }
}

bool brct_algos::
write_ifs_box(vcl_ofstream& ostr,
              vcl_vector<vgl_point_3d<double> > const& verts,
              vcl_vector<vcl_vector<unsigned> > const& faces,
              const float r, const float g, const float b)
{
  if (verts.size()!=8 ||faces.size()!=6)
    return false;
  ostr << " Shape {\n"
       << "appearance Appearance {\n"
       <<"   material Material\n"
       <<    "{  diffuseColor " << r << ' ' << g << ' ' << b << " }\n"
       << "}\n"
       <<   "geometry IndexedFaceSet\n"
       << "{\n"
       << "coordIndex [\n";
  for (unsigned indx = 0; indx<6; ++indx)
  {
    for (unsigned vi =0; vi<4; ++vi)
      ostr << faces[indx][vi] << ", ";
    ostr << -1 ;
    if (indx<5)
      ostr << ", ";
  }
  ostr << "]\n"
       <<"coord Coordinate{\n"
       << "point [\n";
  for (unsigned ip = 0; ip<8; ++ip)
  {
    ostr << verts[ip].x() << ' ' << verts[ip].y()<< ' ' << verts[ip].z() ;
    if (ip<7)
      ostr << ", ";
  }
  ostr <<"]}\n"
       << "}\n"
       << "}\n";
  return true;
}

bool brct_algos::
write_ifs(vcl_ofstream& ostr,
              vcl_vector<vgl_point_3d<double> > const& verts,
              vcl_vector<vcl_vector<unsigned> > const& faces,
              const float r, const float g, const float b)
{
  if(!ostr.is_open())
    return false;
  ostr << " Shape {\n"
       << "appearance Appearance {\n"
       <<"   material Material\n"
       <<    "{  diffuseColor " << r << ' ' << g << ' ' << b << " }\n"
       << "}\n"
       <<   "geometry IndexedFaceSet\n"
       << "{\n"
       << "coordIndex [\n";
  unsigned nf = faces.size();
  for (unsigned indx = 0; indx<nf; ++indx)
  {
    unsigned ni = faces[indx].size();
    for (unsigned vi =0; vi<ni; ++vi)
      ostr << faces[indx][vi] << ", ";
    ostr << -1 ;
    if (indx<nf-1)
      ostr << ", ";
  }
  ostr << "]\n"
       <<"coord Coordinate{\n"
       << "point [\n";
  unsigned nv = verts.size();
  for (unsigned ip = 0; ip<nv; ++ip)
  {
    ostr << verts[ip].x() << ' ' << verts[ip].y()<< ' ' << verts[ip].z() ;
    if (ip<nv-1)
      ostr << ", ";
  }
  ostr <<"]}\n"
       << "}\n"
       << "}\n";
  return true;
}

//extract boxes from merged ply2 file and write as individual indexed face sets
//in vrml 2.0 format. Assumes the ply2 file contains only rectangular boxes
bool brct_algos::translate_ply2_to_vrml(vcl_ifstream& istr, vcl_ofstream& ostr,
                                        const float r, const float g, const float b)
{
  vcl_vector<vgl_point_3d<double> > pts;
  vcl_vector<vsol_polygon_3d_sptr> polys;
  vcl_vector<vcl_vector<unsigned> > ifs;
  if (!brct_algos::read_world_ply2(istr, pts, polys, ifs))
    return false;
  brct_algos::write_vrml_header(ostr);
  if (!brct_algos::write_ifs(ostr, pts, ifs, r, g, b))
      return false;
  return true;
}
