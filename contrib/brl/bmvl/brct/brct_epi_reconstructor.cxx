/////////////////////////////////////////////////////////////////////
//:
// \file

#include "brct_epi_reconstructor.h"
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sscanf()
#include <vcl_cmath.h> // for exp()
#include <vcl_cstdlib.h> // for exit()
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <mvl/FMatrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_quaternion.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <bdgl/bdgl_curve_algs.h>
#include "brct_structure_estimator.h"
#include "brct_algos.h"

const double brct_epi_reconstructor::large_num_ = 1e15;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
brct_epi_reconstructor::brct_epi_reconstructor()
{
  e_ = 0;
  cur_pos_ = 0;
  queue_size_ = 100;
  memory_size_ = 0;
  num_points_ = 0;
  debug_ = false;
}

brct_epi_reconstructor::brct_epi_reconstructor(const char* fname)
{
  // read data into the pool
  read_data(fname);
  e_ = 0;
  cur_pos_ = 0;
  queue_size_ = 100;
  memory_size_ = 0;
  num_points_ = 0;
  debug_ = false;
}

void brct_epi_reconstructor::init()
{
  //
  cur_pos_ = 0;

  //
  memory_size_ = 0;

  // initialize the default queue size
  queue_size_ = 100;
  observes_.resize(queue_size_);
  motions_.resize(queue_size_);

  init_cam_intrinsic();

  init_velocity();
  init_covariant_matrix();

  init_state_3d_estimation();
  memory_size_ = 2;

  cur_pos_ = (cur_pos_ + 1) % queue_size_;
}


brct_epi_reconstructor::~brct_epi_reconstructor()
{
  delete e_;
}

//: Define the set of 3-d points that are to be tracked across frames.
//  Use the observed curves from frame 0 and frame 1 and the initial
//  epipole to define the set of 3-d points that are to be tracked
//  across frames.  In this implementation no new 3-d points are created as
//  tracking proceeds, but the initial curve is interpolated to twice
//  its number of samples to define the tracked pointset.
//
//  The camera for frame 0 is taken as the identity camera.
//  The camera for frame 1 is defined by the initial epipole.  That is,
//  the camera projection of translation vector in 3-d is equivalent
//  to the epipole in frame 1. These two cameras are used to
//  triangulate the 3-d point coordinates are defined in
//  in the coordinate system of camera 0.
//
void brct_epi_reconstructor::init_state_3d_estimation()
{
  double dt = time_tick_[1] - time_tick_[0];
  vnl_double_3 T(X_[3]*dt,X_[4]*dt,X_[5]*dt);
  vcl_cout<<"T = "<<T<<'\n';
  debug_ = false;//JLM
  // compute camera calibration matrix
  vnl_double_3x4 E0, E1;

  //camera for frame 0
  E0[0][0] = 1;     E0[0][1] = 0;      E0[0][2] = 0;        E0[0][3] = 0;
  E0[1][0] = 0;     E0[1][1] = 1;      E0[1][2] = 0;        E0[1][3] = 0;
  E0[2][0] = 0;     E0[2][1] = 0;      E0[2][2] = 1;        E0[2][3] = 0;

  //camera for frame 1
  E1[0][0] = 1;     E1[0][1] = 0;      E1[0][2] = 0;        E1[0][3] = T[0];
  E1[1][0] = 0;     E1[1][1] = 1;      E1[1][2] = 0;        E1[1][3] = T[1];
  E1[2][0] = 0;     E1[2][1] = 0;      E1[2][2] = 1;        E1[2][3] = T[2];

  vnl_double_3x4 P0 = K_*E0, P1 = K_*E1;

  // save the motion
  motions_[0] = vnl_double_3(0.0,0.0,0.0);
  motions_[1] = T;

  // compute epipole from velocity
  vnl_double_3 e = K_*T;

  // construct fundamental matrix between the first and second views.
  vnl_double_3x3 F;
  F[0][0] = 0;     F[0][1] = -e[2];  F[0][2] = e[1];
  F[1][0] = e[2];  F[1][1] = 0;      F[1][2] = -e[0];
  F[2][0] = -e[1]; F[2][1] = e[0];   F[2][2] = 0;

  FMatrix FM(F);

  //
  // 3D estimation
  //

  // point matcher using epipolar geometry
  unsigned int c = tracks_.size();
  joe_observes_.resize(c);
  grad_angles_.resize(c);
  vnl_double_3x3 Sigma3d;
  vnl_double_2x2 Sigma2d;
  Sigma2d.set_identity();
  Sigma3d.set_identity();
  vcl_cout<<Sigma3d<<'\n';
  int k = 0;
  for (unsigned int t =0; t<c; ++t)
  {
    vcl_vector<bugl_normal_point_3d_sptr> pts_3d;
    vcl_vector<bugl_gaussian_point_2d<double> > tracked_points0;
    vcl_vector<bugl_gaussian_point_2d<double> > tracked_points1;
    vcl_vector<double> grad_angles;
    //assert(tracks_[t].size()== time_tick_.size());
    vdgl_digital_curve_sptr dc0 = tracks_[t][0];

    vdgl_digital_curve_sptr dc1 = tracks_[t][1];

    //define the initial set of points to be twice the number of
    //points in the shortest curve (between c0 and c1)
    int npts0 = dc0->n_pts();
    int npts1 = dc1->n_pts();
    int npts = 2* ((npts0 < npts1) ? npts0 : npts1);
    for (int i=0; i<npts; i++)
    {
      double index = i/double(npts);//uniformly sample the curve parameter

      //construct the corresponding point
      vgl_point_2d<double> p0(dc0->get_x(index),dc0->get_y(index));
      vgl_homg_point_2d<double> p0h(p0.x(), p0.y());
      double angle0 = dc0->get_theta(index);
      vnl_double_2x2 sigma1;
      sigma1.set_identity();
      bugl_gaussian_point_2d<double> x0(p0, sigma1);

      //the epipolar line through the point
      vgl_line_2d<double> lr(FM.image2_epipolar_line(p0h));

      //Construct the corresponding point by intersecting
      //the epipolar line with c1
      vgl_point_2d<double> p1;
      if (bdgl_curve_algs::match_intersection(dc1, lr, p0, angle0, p1))
      {
        bugl_gaussian_point_2d<double> x1(p1, Sigma2d);
        //construct the 3-d point from the two matching points
        vgl_point_3d<double> point_3d =
          brct_algos::triangulate_3d_point(x0, P0, x1, P1);
        if (debug_)
          vcl_cout << "P0[" << k << "]= (" << point_3d.x()
                   << ' ' << point_3d.y() << ' ' << point_3d.z()
                   << ")\n";
        bugl_normal_point_3d_sptr p3d_sptr =
          new bugl_normal_point_3d(point_3d, Sigma3d);
        pts_3d.push_back(p3d_sptr);
        //Joe section
        grad_angles_[t].push_back(angle0);
        tracked_points0.push_back(x0);
        tracked_points1.push_back(x1);
        observes_[0].push_back(x0);//observed points
        observes_[1].push_back(x1);
        k++;
      }
    }// end of interpolated points
    curve_3d_.add_curve(pts_3d);
    joe_observes_[t].push_back(tracked_points0);
    joe_observes_[t].push_back(tracked_points1);
  } // end of each track

  num_points_ = curve_3d_.get_num_points();
  prob_.resize(num_points_);
  for (int i=0; i<num_points_; i++)
  {
    prob_[i] = 1.0/num_points_;
  }

  X_[0] = T[0];
  X_[1] = T[1];
  X_[2] = T[2];
}

//: Initialize the covariance matrix of the state vector to the identity.
//  Initialize covariance matrix of the 2-d projection to have
//  larger variance in the motion direction
//
void brct_epi_reconstructor::init_covariant_matrix()
{
  // initialize P
  Q_.set_identity();

  // initialize Q0_ to let the variance on
  // velocity direction bigger
  vnl_double_3x3 Sigma;

  Sigma = 0; Sigma[0][0] = 1;
  Sigma[1][1] = 0.25; Sigma[2][2] = 0.25;

  vnl_double_3 xaxis(1, 0, 0);
  vnl_double_3 T(X_[0], X_[1], X_[2]);
  vnl_double_3 axis = vnl_cross_3d(xaxis, T);
  axis /= axis.magnitude();
  double theta = angle(T, xaxis);

  vnl_quaternion<double> q(axis, theta);
  vnl_double_3x3 RT = q.rotation_matrix_transpose();
  Sigma = RT.transpose()*Sigma*RT;

  Q0_ = 0.0;
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      Q0_[i][j] = Sigma[i][j];

  // initialize R
  for (int i=0; i<2; i++)
    for (int j=0; j<2; j++)
      R_[i][j] = 0;

  for (int i=0; i<2; i++)
    R_[i][i] = 0.25;
}

#if 0
void brct_epi_reconstructor::init_cam_intrinsic()
{
  // set up the intrinsic matrix of the camera
  K_[0][0] = 841.3804; K_[0][1] = 0;        K_[0][2] = 331.0916;
  K_[1][0] = 0;        K_[1][1] = 832.7951; K_[1][2] = 221.5451;
  K_[2][0] = 0;        K_[2][1] = 0;        K_[2][2] = 1;
}
#endif

void brct_epi_reconstructor::init_cam_intrinsic()
{
  // set up the intrinsic matrix of the camera
  K_[0][0] = 2100; K_[0][1] = 0;        K_[0][2] = 400;
  K_[1][0] = 0;        K_[1][1] = 2100; K_[1][2] = 384;
  K_[2][0] = 0;        K_[2][1] = 0;    K_[2][2] = 1;
}


vnl_double_3x4 brct_epi_reconstructor::get_projective_matrix(const vnl_double_3& v ) const
{
  vnl_double_3x4 M_ex;

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      M_ex[i][j] = 0;

  for (int i=0; i<3; i++)
    M_ex[i][i] = 1;

  for (int i=0; i<3; i++)
    M_ex[i][3] = v[i];

  return K_*M_ex;
}


vnl_matrix_fixed<double, 2, 6> brct_epi_reconstructor::get_H_matrix(vnl_double_3x4 &P, vnl_double_3 &X)
{
  vnl_matrix_fixed<double, 2, 6> H;

  // compute \sum {P_{4k} X_k } + P_{44}
  double temp = P[2][3];
  for (int k = 0; k<3; k++)
    temp += P[2][k]*X[k];

  for (int i=0; i<2; i++)
  {
    // \sum{P_{ik} X_4} + P_{i4}
    double t = 0;
    for (int k=0; k<3; k++)
      t += P[i][k] * X[k];
    t += P[i][3];

    for (int j=0; j<3; j++)
      H[i][j] = K_[i][j] / temp - t * K_[2][j] / (temp*temp);

    for (int j=3; j<6; j++)
      H[i][j] = 0;
  }

  return H;
}


vnl_double_2 brct_epi_reconstructor::projection(const vnl_double_3x4 &P, const vnl_double_3 &X)
{
  vnl_double_2 z;
  double t1 = 0;
  for (int k=0; k<3; k++)
    t1 += P[2][k]*X[k];
  t1 += P[2][3];

  for (int i=0; i<2; i++)
  {
    double t0 =0;
    for (int k=0; k<3; k++)
      t0 += P[i][k]*X[k];
    t0 += P[i][3];

    z[i] = t0/t1;
  }

  return z;
}


void brct_epi_reconstructor::update_observes(const vnl_double_3x4 &P, int iframe)
{
  observes_[iframe%queue_size_].resize(num_points_);
  vnl_double_2x2 sigma;
  sigma.set_identity();

  unsigned int c = tracks_.size();
  for (unsigned int t=0; t<c; ++t)
  {
    int frag_size = curve_3d_.get_fragment_size(t);
    for (int i=0; i<frag_size; i++)
    {
      int pos = curve_3d_.get_global_pos(t, i);
      bugl_gaussian_point_2d<double> x = brct_algos::project_3d_point(P, *curve_3d_.get_point(pos));
      vgl_point_2d<double> u = brct_algos::most_possible_point(tracks_[t][iframe], x);
      // set point
      observes_[iframe%queue_size_][pos].set_point(u);
      observes_[iframe%queue_size_][pos].set_covariant_matrix(sigma);
    }
  }
}

bool brct_epi_reconstructor::match_point(vdgl_digital_curve_sptr const& dc,
                                         bugl_gaussian_point_2d<double>& p0,
                                         double grad_angle,
                                         bugl_gaussian_point_2d<double>& p)
{
  vnl_double_2x2 sigma;
  sigma.set_identity();
  //construct the epipolar line through p0;
  vgl_point_2d<double> e((*e_)[0], (*e_)[1]), temp;
  vgl_line_2d<double> el(e, p0);
  if (!bdgl_curve_algs::match_intersection(dc, el, p0, grad_angle, temp))
    return false;
  p = bugl_gaussian_point_2d<double>(temp, sigma);
  return true;
}

//Joe version using epipolar line
void brct_epi_reconstructor::update_observes_joe(int iframe)
{
  unsigned int c = tracks_.size();
  for (unsigned int t=0; t<c; ++t)
  {
    //get the tracked curve in the current frame
    vdgl_digital_curve_sptr dci = tracks_[t][iframe];
    //get the corresponding points from frame 0
    vcl_vector<bugl_gaussian_point_2d<double> > pts0 =
      joe_observes_[t][0];
    unsigned int npts = pts0.size();
    //vector for current track and frame
    vcl_vector<bugl_gaussian_point_2d<double> > cur_frame(npts);
    for (unsigned int i = 0; i<npts; ++i)
    {
#if 0 //JLM Debug
      if (i>0)
      {
        vgl_line_2d<double> l(pts0[i-1], pts0[i]);
        double ang = l.slope_degrees();
        if (ang<0)
          ang +=180;
        if (debug_)
          vcl_cout << "\nTrack Tangent:" << ang << '\n';
      }
#endif // 0
      //default constructor creates a point will zero probability
      bugl_gaussian_point_2d<double> p;
      if (!match_point(dci, pts0[i], grad_angles_[t][i], p))
        vcl_cout << "Match failed for (" << pts0[i].x() << ' '
                 << pts0[i].y() << ")\n";
      cur_frame[i]=p;
    }
    joe_observes_[t].push_back(cur_frame);
  }
}

vcl_vector<bugl_gaussian_point_2d<double> >
brct_epi_reconstructor::get_cur_joe_observes(int frame)
{
  vcl_vector<bugl_gaussian_point_2d<double> > pts(num_points_);
  unsigned int c = tracks_.size();
  unsigned int ip = 0;
  for (unsigned int t = 0; t<c; ++t)
    for (unsigned int i = 0; i<joe_observes_[t][frame].size(); ++i, ++ip)
      pts[ip] = joe_observes_[t][frame][i];
  return pts;
}

void brct_epi_reconstructor::write_results(const char* fname)
{
  vcl_ofstream out(fname);
  int num_total_points = curve_3d_.get_num_points();
  out<<"[ point3d "
     <<num_total_points<<" ]\n";

  for (int i=0; i<num_total_points; i++)
  {
    out<<curve_3d_.get_point(i)->x()<<' '
       <<curve_3d_.get_point(i)->y()<<' '
       <<curve_3d_.get_point(i)->z()<<' '
       <<curve_3d_.get_point(i)->exists()<<'\n';
  }

  // only write frame 0
  int frame = 0;
  unsigned int c = tracks_.size();
  for (unsigned int t = 0; t<c; ++t)
  {
    out<<"[ curve "<< t <<' '<< joe_observes_[t][frame].size()<<" ]\n";
    for (unsigned int i = 0; i<joe_observes_[t][frame].size(); ++i)
    {
      out<< joe_observes_[t][frame][i].x()<< ' '
         << joe_observes_[t][frame][i].y()<< ' '
         << joe_observes_[t][frame][i].exists()<<'\n';
    }
  }
  return;
}

void brct_epi_reconstructor::print_motion_array()
{
  unsigned int n_frames = tracks_[0].size();
  if (!n_frames||!num_points_)
    return;
  for (unsigned int i = 2; i+1<n_frames; ++i)
    update_observes_joe(i);
  vnl_matrix<double> H(n_frames, num_points_);
  vcl_vector<bugl_gaussian_point_2d<double> > temp_0, temp_i, temp_i1;
  temp_0 = get_cur_joe_observes(0);
  for (unsigned int i = 0; i+1<n_frames; ++i)
  {
    temp_i = get_cur_joe_observes(i);
    temp_i1 = get_cur_joe_observes(i+1);
    for (unsigned int j = 0; j<temp_0.size(); ++j)
    {
      bugl_gaussian_point_2d<double>& gp_0 = temp_0[j], gp_i = temp_i[j],
        gp_i1 = temp_i1[j];
      if (!gp_0.exists()||!gp_i.exists()||!gp_i1.exists())
        continue;
      vnl_double_2 p_i(gp_i.x(), gp_i.y());
      vnl_double_2 p_i1(gp_i1.x(), gp_i1.y());
      double gamma = brct_algos::motion_constant(*e_, i, p_i, p_i1);
      vcl_cout << "gamma[" << i << "][" << j << "]("
               << gp_0.x() << ' ' << gp_0.y() << ") = " << gamma << '\n';
      H.put(i, j, gamma);
    }
    brct_algos::print_motion_array(H);
  }
}

//: estimate the probability of each 3-d point
//
void brct_epi_reconstructor::update_confidence()
{
#if 1
  vcl_vector<vnl_double_3x4> cams(memory_size_); //cur_pos_ is 0 based
  for (int i = 0; i < memory_size_; i++)
    cams[i] = get_projective_matrix(motions_[(cur_pos_-i)%memory_size_]);

  double normalization_factor = 0;
  unsigned int c = tracks_.size();
  for (unsigned int t=0; t<c; ++t)
  {
    int frag_size = curve_3d_.get_fragment_size(t);
    for (int i=0; i<frag_size; i++)
    {
      double prob = 1; // square distance
      int pos = curve_3d_.get_global_pos(t, i);

      for (int f = 0; f<memory_size_; ++f)
      {
        bugl_gaussian_point_2d<double> x = brct_algos::project_3d_point(cams[f], *curve_3d_.get_point(pos));
        // find most possible point across all the trackers
        double probability = 0;
        vgl_point_2d<double> u = brct_algos::most_possible_point(tracks_[t][cur_pos_ - f], x);
        vnl_double_2 z1(x.x(), x.y()), z2(u.x(), u.y());
        if (probability<matched_point_prob(z1,z2)){
          probability = matched_point_prob(z1,z2);
        }

        prob *= probability;
      }

      prob_[pos] = prob;
      normalization_factor += prob_[pos];
    }
  }

  vcl_cout<<"normalization_factor = "<<normalization_factor;
  // normalize the probability weight across all the points

  if (normalization_factor == 0)
    return ;

  for (int i=0; i<num_points_; i++)
    prob_[i] /= normalization_factor;
#endif

#if 0
  for (int i=0; i<num_points_; i++)
    prob_[i] = 1.0/vnl_det(curve_3d_.get_point(i)->get_covariant_matrix());
#endif
}

#if 0 //original inc()
void brct_epi_reconstructor::inc()
{
  if ((unsigned)(cur_pos_+1) >= tracks_[0].size()){ // end of the data
    vcl_cout<<"\n at the end of last curve\n";
    return;
  }

  cur_pos_ ++;

  //
  // prediction step:
  //
  vnl_matrix_fixed<double, 6, 6> A = get_transit_matrix(cur_pos_-1, cur_pos_);
  vcl_cout<<A<<'\n'
          <<X_<<'\n';
  //update state vector
  vnl_vector_fixed<double, 6> Xpred = A*X_;
  //construct the camera for this time step
  vnl_double_3 camCenter(Xpred[0],Xpred[1],Xpred[2]);
  vnl_double_3x4 P = get_projective_matrix(camCenter);

  //project the 3-d uncertain point set into the image as a set of
  //2-d uncertain points and find the closest edgel observations
  update_observes(P, cur_pos_);

  // get these best matching edgels
  vcl_vector<bugl_gaussian_point_2d<double> > & cur_measures = observes_[cur_pos_%queue_size_];

  //unsigned int c = tracks_.size();
  //for (unsigned int t = 0; t < c; ++t)
  //compare the matched points with the projected 3-d points
  //in order to correct the Kalman parameters
  for (int i=0; i<num_points_; i++)
  {
    vnl_double_3 X;

    X[0] = curve_3d_.get_point(i)->x();
    X[1] = curve_3d_.get_point(i)->y();
    X[2] = curve_3d_.get_point(i)->z();

    vnl_matrix_fixed<double, 2, 6> H = get_H_matrix(P, X);

    vnl_matrix_fixed<double, 6, 6> Qpred = A*Q_*A.transpose() + Q0_;

    G_ = Qpred*H.transpose()*vnl_inverse(H*Qpred*H.transpose()+R_);

    vnl_double_2 z(cur_measures[i].x(), cur_measures[i].y());

    vnl_double_2 z_pred = projection(P,X);
    if (matched_point_prob(z, z_pred) >= 0) // if not a outlier
    {
      // go to the correction step
      //
      Xpred = Xpred +  G_*(z - z_pred)*prob_[i];

      vnl_matrix_fixed<double, 6, 6> I;
      I.set_identity();
      Q_ = (I - G_*H)*Qpred;
    }
  }

  //Correct the velocity
  double dt = time_tick_[cur_pos_] - time_tick_[cur_pos_-1];
  Xpred[3] = (Xpred[0] - X_[0])/dt;
  Xpred[4] = (Xpred[1] - X_[1])/dt;
  Xpred[5] = (Xpred[2] - X_[2])/dt;

  vnl_double_3 xNew(Xpred[0], Xpred[1], Xpred[2]);

  motions_[cur_pos_%queue_size_] = xNew;
  P = get_projective_matrix(motions_[cur_pos_%queue_size_] );
  update_observes(P, cur_pos_);

  // store the history
  X_ = Xpred;
  vcl_cout<<"X_ = "<< X_;

  if (memory_size_ < queue_size_)
    ++memory_size_;

  // update 3d reconstruction results
  //step through the set of edgel observations for each view
  //and compute the least-squares 3-d reconstruction of the corresponding
  //point
  vcl_vector<vnl_double_3x4> Ps(memory_size_);
  vcl_vector<vnl_double_2> pts(memory_size_);
  for (int i=0; i<num_points_; i++)
  {
    // assemble the observations and cameras for point(i)
    for (int j=0; j<memory_size_; j++)
    {
      if (observes_[j][i].x() != this->large_num_ ||
          observes_[j][i].y() != this->large_num_){
        pts[j] = vnl_double_2(observes_[j][i].x(), observes_[j][i].y());
        Ps[j] = get_projective_matrix(motions_[j]);
      }
    }

    //least squares reconstruction of the point using SVD
    vgl_point_3d<double> X3d = brct_algos::bundle_reconstruct_3d_point(pts, Ps);
//     vcl_cout << "Pi[" << i << "]= (" << X3d.x()
//              << ' ' << X3d.y() << ' ' << X3d.z()
//              << ")\n";

    //Get the displacement between the current 3d point and the
    //least-squares reconstruction
    bugl_normal_point_3d_sptr p3d_sptr = curve_3d_.get_point(i);
    vnl_double_3 dX(X3d.x() - p3d_sptr->x(), X3d.y() - p3d_sptr->y(), X3d.z() - p3d_sptr->z());

    //incrementally update the covariance matrix
    vnl_double_3x3 Sigma3d = p3d_sptr->get_covariant_matrix();
    Sigma3d = Sigma3d*(cur_pos_-1.0)/(double)cur_pos_;
    for (int m = 0; m<3; m++)
      for (int n = 0; n<3; n++)
        Sigma3d[m][n] += dX[m]*dX[n] /(cur_pos_);
    //replace the point with the least-squares estimate
    p3d_sptr->set_point(X3d);
    p3d_sptr->set_covariant_matrix(Sigma3d);
  }

  // update confidence level for each points
  update_confidence();
}
#else //end of original inc()

//start of joe version of inc()
void brct_epi_reconstructor::inc()
{
  if ((unsigned)(cur_pos_+1) >= tracks_[0].size()){ // end of the data
    vcl_cout<<"\n at the end of last curve\n";
    return;
  }

  cur_pos_ ++;
  update_observes_joe(cur_pos_);

  //get the camera for the current step using the previous 3-d points
  //get the corresponding points from current obs
  vcl_vector<bugl_gaussian_point_2d<double> >
    temp0 = get_cur_joe_observes(0),
    temp1 = get_cur_joe_observes(1),
    temp  = get_cur_joe_observes(cur_pos_);

  vcl_vector<vnl_double_2> points_0, points_1, points;
  int ip = 0;
  for (vcl_vector<bugl_gaussian_point_2d<double> >::iterator pit = temp0.begin(); pit != temp0.end(); pit++, ip++)
  {
    bugl_gaussian_point_2d<double>& gp1 = temp1[ip], gp = temp[ip];
    if (!(*pit).exists()||!gp1.exists()||!gp.exists())
    continue;
    vnl_double_2 p0((*pit).x(), (*pit).y());
    vnl_double_2 p1(gp1.x(), gp1.y());
    vnl_double_2 p(gp.x(), gp.y());
    points_0.push_back(p0);
    points_1.push_back(p1);
    points.push_back(p);
  }
  vnl_double_3 T;
  brct_algos::camera_translation(K_, *e_, points_0, points_1, points, T);

  vnl_double_3 Tk;
  //
  vcl_cout << "X prior to update " << X_ << '\n';
  //update state vector
#if 0
  vcl_cout << "Camera translation " << T << '\n';
  vnl_matrix_fixed<double, 6, 6> A = get_transit_matrix(cur_pos_-1, cur_pos_);
  //update state vector
  vnl_vector_fixed<double, 6> Xpred = A*X_;
#endif
#if 1
  //update state vector
  vnl_vector_fixed<double, 6> Xpred=X_;
  Xpred[0]=T[0];   Xpred[1]=T[1];   Xpred[2]=T[2];
#endif
  vcl_cout << Xpred<< vcl_flush <<'\n';
  Tk[0]=Xpred[0];  Tk[1]=Xpred[1];   Tk[2]=Xpred[2];
  motions_[cur_pos_%queue_size_] = Tk;

  // store the history
  X_ = Xpred;

  if (memory_size_ < queue_size_)
    ++memory_size_;

  // update 3d reconstruction results
  //step through the set of edgel observations for each view
  //and compute the least-squares 3-d reconstruction of the corresponding
  //point using the predicted camera for the current view
  vcl_vector<vnl_double_3x4> Ps(memory_size_);
  vcl_vector<vnl_double_2> pts(memory_size_);
  unsigned int c = tracks_.size();
  int ipt = 0;
  for (unsigned int t=0; t<c; ++t)
  {
    unsigned int n = joe_observes_[t][0].size();
    for (unsigned int ip = 0; ip<n; ++ip, ++ipt)
    {
      for (int j =0; j<memory_size_; j++)
        if (joe_observes_[t][j][ip].exists()){
          pts[j] =
            vnl_double_2(joe_observes_[t][j][ip].x(),
                         joe_observes_[t][j][ip].y());
          Ps[j] = get_projective_matrix(motions_[j]);
        }
      //least squares reconstruction of the point using SVD
      vgl_point_3d<double> X3d =
        brct_algos::bundle_reconstruct_3d_point(pts, Ps);
      if (debug_)
        vcl_cout << "Pi[" << ipt << "]= (" << X3d.x()
                 << ' ' << X3d.y() << ' ' << X3d.z()
                 << ")\n";
      //Get the displacement between the current 3d point and the
      //least-squares reconstruction
      bugl_normal_point_3d_sptr p3d_sptr = curve_3d_.get_point(ipt);
      vnl_double_3 dX(X3d.x() - p3d_sptr->x(),
                      X3d.y() - p3d_sptr->y(),
                      X3d.z() - p3d_sptr->z());
      if (debug_)
        vcl_cout << "3dERROR " << dX.magnitude() << '\n';
      if (p3d_sptr->exists()&&dX.magnitude()>3)//JLM
      {
        if (debug_)
          vcl_cout << "Killing [" << ipt
                   << "]= (" << X3d.x()
                   << ' ' << X3d.y() << ' ' << X3d.z()
                   << ")\n";
        p3d_sptr->set_point(); // no point is set
        continue;
      }
#ifdef DEBUG
      vcl_cout << "dX( " << dX[0] << ' ' << dX[1] << ' ' << dX[2] // << ")\n";
#endif

      //incrementally update the covariance matrix
      vnl_double_3x3 Sigma3d = p3d_sptr->get_covariant_matrix();
      Sigma3d = Sigma3d*(cur_pos_-1.0)/(double)cur_pos_;
      for (int m = 0; m<3; m++)
        for (int n = 0; n<3; n++)
          Sigma3d[m][n] += dX[m]*dX[n] /(cur_pos_);
      //replace the point with the least-squares estimate
      p3d_sptr->set_point(X3d);
      p3d_sptr->set_covariant_matrix(Sigma3d);
    }
  }
}
#endif //end of joe version of inc()

void brct_epi_reconstructor::read_data(const char *fname)
{
  vcl_ifstream fin(fname);

  if (!fin){
    vcl_cerr<<"cannot open the file - "<<fname<<'\n';
    vcl_exit(2);
  }

  char buff[255];
  fin >> buff;

  time_tick_ = read_timestamp_file(buff);

  while (fin>>buff){ // for each line
    if (vcl_strlen(buff)<2 || buff[0]=='#')
      continue;

    // push a track into the vector
    this->add_track(read_track_file(buff));
  }
}

vcl_vector<double> brct_epi_reconstructor::read_timestamp_file(char *fname)
{
  vcl_ifstream fin(fname);

  if (!fin){
    vcl_cerr<<"cannot open the file - "<<fname<<'\n';
    vcl_exit(2);
  }

  int nviews =0, junk=0;
  fin >> nviews;
  vcl_vector<double> times(nviews);

  for (int i=0; i<nviews; i++){
    fin>> junk >> times[i];
  }

  return times;
}

void
brct_epi_reconstructor::add_track(vcl_vector<vdgl_digital_curve_sptr> const& track)
{
  tracks_.push_back(track);
}

//----------------------------------------------------------------------
//: read a set of curves corresponding to tracked edge contours.
//  Each "contour" is the curve in a given image frame.
//  The file format is:
// \verbatim
// CURVE
// [BEGIN CONTOUR]
// EDGE_COUNT=150
// [202, 298.257]   -68.673 13.1904 ([edgel position]  angle   gradient mag)
//   ...
//   ...
//   ...
// [END CONTOUR]
// [BEGIN CONTOUR]
// EDGE_COUNT=153
// [218.086, 295.532]   -104.187 50.5706
// ...
// [END CONTOUR]
// END CURVE
// \endverbatim
vcl_vector<vdgl_digital_curve_sptr> brct_epi_reconstructor::read_track_file(char *fname)
{
  vcl_ifstream fp(fname);

  if (!fp){
    vcl_cerr<<" cannot open the file - "<<fname<<'\n';
    vcl_exit(2);
  }

  char buffer[1000];
  int MAX_LEN=1000;
  int numEdges;

  vcl_vector<vdgl_digital_curve_sptr > tracker;

  double x, y, dir, conf;
  while (fp.getline(buffer,MAX_LEN))
  {
    //ignore comment lines and empty lines
    if (vcl_strlen(buffer)<2 || buffer[0]=='#')
      continue;
    //read the line with the contour count info

    //read the beginning of a contour block

    if (!vcl_strncmp(buffer, "[BEGIN CONTOUR]", sizeof("[BEGIN CONTOUR]")-1))
    {
      //read in the next line to get the number of edges in this contour
      fp.getline(buffer,MAX_LEN);

      vcl_sscanf(buffer,"EDGE_COUNT=%d",&(numEdges));

      //instantiate a new contour structure here
      vdgl_edgel_chain_sptr ec=new vdgl_edgel_chain;

      for (int j=0; j<numEdges;j++)
      {
        //read in all the edge information
        fp.getline(buffer,MAX_LEN);
        vcl_sscanf(buffer," [%lf, %lf]   %lf %lf  ", &(x), &(y), &(dir), &(conf));
        vdgl_edgel e;
        e.set_x(x);
        e.set_y(y);
        e.set_theta(dir);
        e.set_grad(conf);
        ec->add_edgel(e);
        //add this edge to the current contour
      }

      tracker.push_back(new vdgl_digital_curve(new vdgl_interpolator_linear(ec)));
      //read in the end of contour block
      fp.getline(buffer,MAX_LEN);

      //make sure that this is the end marker
      assert(!vcl_strncmp(buffer, "[END CONTOUR]", sizeof("[END CONTOUR]")-1));

      continue;
    }
  }

  return tracker;
}
//---------------------------------------------------------------
//: Use the initial epipole to specify the velocity at the first time step.
//  The initial projection matrix is assumed to be the identity camera.
//  The camera at the first time step is deterimined from the relation,
// \verbatim
//           _    _        _          _   _  _
//          | w ex |      | 1  0  0  0 | | Tx |
//          | w ey | = [K]| 0  1  0  0 | | Ty |
//          |  w   |      | 0  0  1  0 | | Tz |
//           -    -        -           - | 1  |
//                                        -  -
// \endverbatim
void brct_epi_reconstructor::init_velocity()
{
  vcl_vector<vgl_homg_line_2d<double> > lines;

  if (!e_) //if epipole is not initialized
  {
    vcl_cerr<<"epipole is not initialized\n";
    return;
  }
  vnl_double_3 e((*e_)[0],(*e_)[1],1.0);//projective point.
  init_cam_intrinsic();

  // get translation
  double trans_dist = 1.0; // 105mm
  vnl_double_3 T = vnl_inverse(K_) * e;
  // normalize
  T /= vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);

  // Flip sign if necessary (?Not clear why?)
  if (T[2]<0)
    T *= trans_dist;
  else
    T *= -trans_dist;

  //initialize the state vector
  X_[0] = T[0];
  X_[1] = T[1];
  X_[2] = T[2];

  double dt = time_tick_[1]-time_tick_[0];
  //the camera motion is opposite in sign to the object motion
  //we imagine the camera is moving to the left as the vehicle is
  //moving to the right.
  X_[3] = -T[0] /dt;
  X_[4] = -T[1] /dt;
  X_[5] = -T[2] /dt;
}

bugl_curve_3d brct_epi_reconstructor::get_curve_3d()
{
  return curve_3d_;
}

vcl_vector<vgl_point_3d<double> > brct_epi_reconstructor::get_local_pts()
{
  vcl_vector<vgl_point_3d<double> > pts;

  double xc=0, yc=0, zc=0;

  for (int i=0; i<num_points_; i++)
  {
    bugl_normal_point_3d_sptr pt3d_sptr = curve_3d_.get_point(i);
    xc += pt3d_sptr->x();
    yc += pt3d_sptr->y();
    zc += pt3d_sptr->z();
  }

  xc /= num_points_;
  yc /= num_points_;
  zc /= num_points_;

  for (int i=0; i<num_points_; i++)
    if (prob_[i] > 0) {
      bugl_normal_point_3d_sptr pt3d_sptr = curve_3d_.get_point(i);
      vgl_point_3d<double> pt(pt3d_sptr->x()-xc, pt3d_sptr->y()-yc, pt3d_sptr->z()-zc);
      pts.push_back(pt);
    }

  return pts;
}


vcl_vector<vgl_point_2d<double> > brct_epi_reconstructor::get_next_observes()
{
  vcl_vector<vgl_point_2d<double> > pts(num_points_);

  unsigned int c = tracks_.size();
  for (unsigned int t=0; t<c; ++t)
  {
    vdgl_digital_curve_sptr dc = tracks_[t][(cur_pos_+1)%queue_size_];
    vdgl_interpolator_sptr interp = dc->get_interpolator();
    vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
    unsigned int size = ec->size();
    for (unsigned int i=0; i<size; ++i)
    {
      double s = double(i) / double(size);
      pts.push_back(vgl_point_2d<double> (dc->get_x(s), dc->get_y(s)));
    }
  }
  return pts;
}


vcl_vector<vgl_point_2d<double> > brct_epi_reconstructor::get_cur_observes()
{
  vcl_vector<vgl_point_2d<double> > pts;

  unsigned int c = tracks_.size();

  for (unsigned int t=0; t<c; ++t)
  {
    vdgl_digital_curve_sptr dc = tracks_[t][cur_pos_];
    vdgl_interpolator_sptr interp = dc->get_interpolator();
    vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
    unsigned int size = ec->size();
    for (unsigned int i=0; i<size; ++i)
    {
      double s = double(i) / double(size);
      pts.push_back(vgl_point_2d<double>(dc->get_x(s), dc->get_y(s)));
    }
  }
  return pts;
}

vcl_vector<vgl_point_2d<double> > brct_epi_reconstructor::get_joe_cur_observes()
{
  vcl_vector<bugl_gaussian_point_2d<double> > pts =
    this->get_cur_joe_observes(cur_pos_);

  unsigned int Np = pts.size();
  vcl_vector<vgl_point_2d<double> > res;
  for (unsigned int i=0; i<Np; ++i)
    res.push_back(vgl_point_2d<double>(pts[i].x(), pts[i].y()));
  return res;
}


vcl_vector<vgl_point_2d<double> > brct_epi_reconstructor::get_joe_pre_observes()
{
  assert(cur_pos_ > 0);
  vcl_vector<bugl_gaussian_point_2d<double> > pts =
    this->get_cur_joe_observes(cur_pos_-1);

  unsigned int Np = pts.size();
  vcl_vector<vgl_point_2d<double> > res;
  for (unsigned int i=0; i<Np; ++i)
    res.push_back(vgl_point_2d<double>(pts[i].x(), pts[i].y()));
  return res;
}

vcl_vector<vgl_point_2d<double> > brct_epi_reconstructor::get_joe_next_observes()
{
  vcl_vector<bugl_gaussian_point_2d<double> > pts =
    this->get_cur_joe_observes(cur_pos_+1);

  unsigned int Np = pts.size();
  vcl_vector<vgl_point_2d<double> > res;
  for (unsigned int i=0; i<Np; ++i)
    res.push_back(vgl_point_2d<double>(pts[i].x(), pts[i].y()));
  return res;
}


vcl_vector<vgl_point_2d<double> > brct_epi_reconstructor::get_pre_observes()
{
  assert(cur_pos_ > 0);
  vcl_vector<vgl_point_2d<double> > pts;

  unsigned int c = tracks_.size();

  for (unsigned int t = 0; t<c; ++t)
  {
    vdgl_digital_curve_sptr dc = tracks_[t][(cur_pos_-1)%queue_size_];
    vdgl_interpolator_sptr interp = dc->get_interpolator();
    vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
    unsigned int size = ec->size();
    for (unsigned int i=0; i<size; ++i)
    {
      double s = double(i) / double(size);
      pts.push_back(vgl_point_2d<double> (dc->get_x(s), dc->get_y(s)));
    }
  }
  return pts;
}

vnl_double_3 brct_epi_reconstructor::get_next_motion(vnl_double_3 v)
{
  return motions_[cur_pos_]+v;
}


vnl_matrix<double> brct_epi_reconstructor::get_predicted_curve()
{
  //
  // prediction step:
  //
  vnl_matrix_fixed<double, 6, 6> A = get_transit_matrix(cur_pos_, cur_pos_+1);
  vnl_vector_fixed<double, 6> Xpred = A*X_;
  vnl_double_3 camCenter(Xpred[0],Xpred[1],Xpred[2]);
  vnl_double_3x4 P = get_projective_matrix(camCenter);

  vnl_matrix<double> t(2, num_points_);
  for (int i=0; i<num_points_; i++)
  {
    vgl_point_2d<double> x = brct_algos::projection_3d_point(*curve_3d_.get_point(i), P);

    t[0][i] = x.x();
    t[1][i] = x.y();
  }

  return t;
}


vcl_vector<vnl_matrix<double> > brct_epi_reconstructor::get_back_projection() const
{
  vcl_vector<vnl_matrix<double> > res(memory_size_);
  for (int f=0; f<memory_size_; f++)
  {
    vnl_double_3x4 P = get_projective_matrix(motions_[f]);

    vnl_matrix<double> t(2, num_points_);

    for (int i=0; i<num_points_; i++)
    {
      vgl_point_2d<double> p = brct_algos::projection_3d_point(* curve_3d_.get_point(i), P);
      t[0][i] = p.x();
      t[1][i] = p.y();
    }

    res[f] = t;
  }

  return res;
}

//original code
vgl_point_2d<double> brct_epi_reconstructor::get_cur_epipole() const
{
  vnl_double_3 T(motions_[cur_pos_%queue_size_]);

  // compute camera calibration matrix
  vnl_double_3x4 E1, E2;
  E1[0][0] = 1;       E1[0][1] = 0;        E1[0][2] = 0;          E1[0][3] = 0;
  E1[1][0] = 0;       E1[1][1] = 1;        E1[1][2] = 0;          E1[1][3] = 0;
  E1[2][0] = 0;       E1[2][1] = 0;        E1[2][2] = 1;          E1[2][3] = 0;

  E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] = T[0];
  E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] = T[1];
  E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] = T[2];

  vnl_double_3x4 P1 = K_*E1, P2 = K_*E2;

  // compute epipole from velocity
  vnl_double_3 e = K_*T;
  vgl_homg_point_2d<double> res(e[0], e[1], e[2]);
  return res;
}

void brct_epi_reconstructor::init_epipole(double x, double y)
{
  if (!e_)
    e_ = new vnl_double_2;

  (*e_)[0] = x;
  (*e_)[1] = y;
}


double brct_epi_reconstructor::matched_point_prob(vnl_double_2& z, vnl_double_2& z_pred)
{
  // a brutal-force implementation
  // a more sophisticated one will be implemented in the next phase.
  // a truncated Gaussian distribution is used.

  vnl_double_2 dz = z - z_pred;
  double d2 = dz[0]*dz[0] + dz[1]*dz[1];

  if (d2 > 5)
    return 0;
  else
    return vcl_exp(-d2/2);
}

//: The state transition matrix
vnl_matrix_fixed<double, 6, 6> brct_epi_reconstructor::get_transit_matrix(int i, int j)
{
  assert(i>=0 && j>=0 && j>=i);
  vnl_matrix_fixed<double, 6, 6> A;

  double dt = time_tick_[j] - time_tick_[i];

  // fix the current problem of time stamp
  for (int i=0; i<6; i++)
    for (int j=0; j<6; j++)
      A[i][j] = 0.0;

  for (int i = 0; i<6; i++)
    A[i][i] = 1;

  for (int i=0; i<3; i++)
    A[i][i+3] = dt;

  return A;
}

void brct_epi_reconstructor::print_track(const int track_index, const int frame)
{
  if ((unsigned int)track_index>=tracks_.size())
    return;
  vcl_vector<vdgl_digital_curve_sptr> track = tracks_[track_index];
  if ((unsigned int)frame>=track.size())
    return;
  for (vcl_vector<vdgl_digital_curve_sptr>::iterator cit = track.begin();
       cit != track.end(); cit++)
  {
    vdgl_interpolator_sptr intp = (*cit)->get_interpolator();
    vdgl_edgel_chain_sptr chain = intp->get_edgel_chain();
    vcl_cout << *chain << '\n';
  }
}
