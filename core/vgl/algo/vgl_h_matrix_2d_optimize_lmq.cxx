// This is core/vgl/algo/vgl_h_matrix_2d_optimize_lmq.cxx
#include "vgl_h_matrix_2d_optimize_lmq.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>

//: Construct a vgl_h_matrix_2d_optimize_lmq object.
vgl_h_matrix_2d_optimize_lmq::
vgl_h_matrix_2d_optimize_lmq(vgl_h_matrix_2d<double> const& initial_h)
  : vgl_h_matrix_2d_optimize(initial_h)
{
}
//: optimize the normalized projection problem problem
bool vgl_h_matrix_2d_optimize_lmq::
optimize_h(vcl_vector<vgl_homg_point_2d<double> > const& points1,
           vcl_vector<vgl_homg_point_2d<double> > const& points2,
           vgl_h_matrix_2d<double> const& h_initial, 
           vgl_h_matrix_2d<double>& h_optimized)
{
  projection_lsqf lsq(points1, points2);
  vnl_vector<double> hv(9);
  vnl_matrix_fixed<double,3,3> m =  h_initial.get_matrix();
  unsigned i = 0;
  for(unsigned r = 0; r<3; ++r)
    for(unsigned c = 0; c<3; ++c, ++i)
      hv[i] = m[r][c];
  vnl_levenberg_marquardt lm(lsq);
  lm.set_verbose(verbose_);
  lm.set_trace(trace_);
  lm.set_x_tolerance(htol_);
  lm.set_f_tolerance(ftol_);
  lm.set_g_tolerance(gtol_);
  bool success = lm.minimize(hv);
  lm.diagnose_outcome(vcl_cout);

  if(success)
    h_optimized.set(hv.data_block());
  else
    h_optimized = h_initial;
  return success;
}

bool vgl_h_matrix_2d_optimize_lmq::
optimize_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
          vcl_vector<vgl_homg_point_2d<double> > const& points2,
          vgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int n = points1.size();
  assert(n>4);

  //compute the normalizing transforms
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points(points1))
    return false;
  if (!tr2.compute_from_points(points2))
    return false;
  //normalize the input point sets
  vcl_vector<vgl_homg_point_2d<double> > tpoints1, tpoints2;
  for (int i = 0; i<n; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  //Transform the initial homography into the normalized coordinate frame
  //  p1' = tr1 p1 , p2' = tr2 p2
  // p2 = initial_h_(p1)
  // (tr2^-1)p2' = initial_h_((tr1^-1)p1')
  // p2' = (tr2*initial_h_*(tr1^-1))p1'
  // thus initial_h_' = tr2*initial_h_*(tr1^-1)

  vgl_h_matrix_2d<double> tr1_inv = tr1.get_inverse();
  vgl_h_matrix_2d<double> initial_h_norm = tr2*initial_h_*tr1_inv;

  //The Levenberg-Marquardt algorithm can now be applied
  vgl_h_matrix_2d<double> hopt;
  if(!optimize_h(tpoints1, tpoints2, initial_h_norm, hopt))
    return false;

  // hopt has to be transformed back to the coordinate system of
  // the original point sets, i.e.,
  //  p1' = tr1 p1 , p2' = tr2 p2
  // hopt was determined from the transform relation
  //  p2' = hopt p1', thus
  // (tr2 p2) = hopt (tr1 p1)
  //  p2 = (tr2^-1 hopt tr1) p1 = H p1
  vgl_h_matrix_2d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hopt*tr1;
  return true;
}

bool vgl_h_matrix_2d_optimize_lmq::
optimize_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
          vcl_vector<vgl_homg_line_2d<double> > const& lines2,
          vgl_h_matrix_2d<double>& H)
{
  //number of lines must be the same
  int n = lines1.size();
  assert(n == lines2.size());
  assert(n>4);
  //compute the normalizing transforms. By convention, these are point
  //transformations that act properly if the input is a line,
  // i.e., linei_norm = trx^-t(linei).
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_lines(lines1))
    return false;
  if (!tr2.compute_from_lines(lines2))
    return false;
  vcl_vector<vgl_homg_point_2d<double> > tlines1, tlines2;
  for (vcl_vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines1.begin(); lit != lines1.end(); lit++)
  {
    // transform lines1 according to the normalizing transform
    vgl_homg_line_2d<double> l = tr1(*lit);
    // convert the line to a point to use the same linear code
    vgl_homg_point_2d<double> p(l.a(), l.b(), l.c());
    tlines1.push_back(p);
  }
  for (vcl_vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines2.begin(); lit != lines2.end(); lit++)
  {
    // transform lines2 according to the normalizing transform
    vgl_homg_line_2d<double> l = tr2(*lit);
    // convert the line to a point to use the same linear code
    vgl_homg_point_2d<double> p(l.a(), l.b(), l.c());
    tlines2.push_back(p);
  }
  //At this step, we have two line sets normalized as a set of points.
  //The input to the point optimizer method must be a line transform,
  //so the inital homography, which is by convention a point transform
  //must be converted as h_initial_line = h_initial_^-t

  // normalize the initial guess
  vgl_h_matrix_2d<double> h_initial_line, h_line_opt, initial_h_norm;
  vgl_h_matrix_2d<double> tr1_inv = tr1.get_inverse();
   initial_h_norm = tr2*initial_h_*tr1_inv;
  // convert to line form
  vnl_matrix_fixed<double, 3, 3> const &  Mp_init =
    initial_h_norm.get_matrix();
  vnl_matrix_fixed<double, 3, 3> Ml_init = vnl_inverse_transpose(Mp_init);
  h_initial_line.set(Ml_init);

  //run the optimization to refine the line transform
  if(!this->optimize_h(tlines1, tlines2, h_initial_line, h_line_opt))
    return false;

  // Convert the optimized line transform back to point form.
  vgl_h_matrix_2d<double> h_point_opt;
  vnl_matrix_fixed<double, 3, 3> const &  Ml_opt = h_line_opt.get_matrix();
  vnl_matrix_fixed<double, 3, 3> Mp_opt = vnl_inverse_transpose(Ml_opt);
  h_point_opt.set(Mp_opt);

  // Finally, h_point_opt has to be transformed back to the coordinate 
  // system of the original lines, i.e.,
  //  l1' = tr1 l1 , l2' = tr2 l2
  // h_point_opt was determined from the transform relation
  //  l2' = h_point_opt l1', thus
  // (tr2 l2) = hh (tr1 l1)
  //  p2 = (tr2^-1 hh tr1) p1 = H p1
   vgl_h_matrix_2d<double> tr2inv = tr2.get_inverse();
  H = tr2inv*h_point_opt*tr1;
  return true;
}

bool vgl_h_matrix_2d_optimize_lmq::
optimize_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
           vcl_vector<vgl_homg_point_2d<double> > const& points2,
           vcl_vector<vgl_homg_line_2d<double> > const& lines1,
           vcl_vector<vgl_homg_line_2d<double> > const& lines2,
           vgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int np = points1.size();
  //number of lines must be the same
  assert(lines1.size() == lines2.size());
  int nl = lines1.size();
  // Must have enough equations 
  assert((np+nl)>4);
  //compute the normalizing transforms
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points_and_lines(points1,lines1))
    return false;
  if (!tr2.compute_from_points_and_lines(points2,lines2))
    return false;
  vcl_vector<vgl_homg_point_2d<double> > tpoints1, tpoints2;
  for (int i = 0; i<np; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  for (int i = 0; i<nl; i++)
  {
    double a=lines1[i].a(), b=lines1[i].b(), c=lines1[i].c(), d=vcl_sqrt(a*a+b*b);
    tpoints1.push_back(tr1(vgl_homg_point_2d<double>(-a*c,-b*c,d)));
    a=lines2[i].a(), b=lines2[i].b(), c=lines2[i].c(), d = vcl_sqrt(a*a+b*b);
    tpoints2.push_back(tr2(vgl_homg_point_2d<double>(-a*c,-b*c,d)));
  }

  vgl_h_matrix_2d<double> tr1_inv = tr1.get_inverse();
  vgl_h_matrix_2d<double> initial_h_norm = tr2*initial_h_*tr1_inv;

  //The Levenberg-Marquardt algorithm can now be applied
  vgl_h_matrix_2d<double> hopt;
  if(!optimize_h(tpoints1, tpoints2, initial_h_norm, hopt))
    return false;

  // hopt has to be transformed back to the coordinate system of
  // the original point sets, i.e.,
  //  p1' = tr1 p1 , p2' = tr2 p2
  // hopt was determined from the transform relation
  //  p2' = hopt p1', thus
  // (tr2 p2) = hopt (tr1 p1)
  //  p2 = (tr2^-1 hopt tr1) p1 = H p1

  vgl_h_matrix_2d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hopt*tr1;
  return true;
}
