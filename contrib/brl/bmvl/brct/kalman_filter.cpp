/////////////////////////////////////////////////////////////////////

#include "kalman_filter.h"
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sscanf()
#include <vcl_cmath.h> // for exp()
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <mvl/FMatrix.h>
#include <vnl/vnl_math.h> // for pi
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_quaternion.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bbas/bdgl/bdgl_curve_algs.h>
#include "brct_structure_estimator.h"
#include "brct_algos.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
kalman_filter::kalman_filter()
{
}

kalman_filter::kalman_filter(char* fname)
{
  // read data into the pool
  read_data(fname);
  init();
}

void kalman_filter::init()
{
  //
  cur_pos_ = 0;

  // initialize the transit matrix
  dt_ = 1.0;

  //
  memory_size_ = 0;

  // initialize the default queue size
  queue_size_ = 100;
  observes_.resize(queue_size_);
  motions_.resize(queue_size_);

  init_transit_matrix();

  init_cam_intrinsic();
  
  init_velocity();
  init_covariant_matrix();  

  init_state_vector();
  memory_size_ = 2;


  cur_pos_ = (cur_pos_ + 1) % queue_size_;
}

kalman_filter::~kalman_filter()
{
}

void kalman_filter::init_transit_matrix()
{
  for (int i=0; i<6; i++)
    for (int j=0; j<6; j++)
      A_[i][j] = 0.0;

  for (int i = 0; i<6; i++)
    A_[i][i] = 1;

  for (int i=0; i<3; i++)
    A_[i][i+3] = dt_;
}

void kalman_filter::init_state_vector()
{
  vnl_double_3 T(X_[3],X_[4],X_[5]);

  // compute camera calibration matrix
  vnl_double_3x4 E1, E2;
  E1[0][0] = 1;       E1[0][1] = 0;        E1[0][2] = 0;          E1[0][3] = 0;
  E1[1][0] = 0;       E1[1][1] = 1;        E1[1][2] = 0;          E1[1][3] = 0;
  E1[2][0] = 0;       E1[2][1] = 0;        E1[2][2] = 1;          E1[2][3] = 0;

  E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] = T[0];
  E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] = T[1];
  E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] = T[2];

  vnl_double_3x4 P1 = K_*E1, P2 = K_*E2;

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

  // point matcher using epipolar geometry
  assert(curves_.size()>=2);
  vdgl_digital_curve_sptr dc0 = curves_[0];
  vdgl_interpolator_sptr interp0 = dc0->get_interpolator();
  vdgl_edgel_chain_sptr  ec0 = interp0->get_edgel_chain();

  vdgl_digital_curve_sptr dc1 = curves_[1];
  vdgl_interpolator_sptr interp1 = dc1->get_interpolator();
  vdgl_edgel_chain_sptr  ec1 = interp1->get_edgel_chain();

  int size0 = ec0->size();
  int size1 = ec1->size();
  int npts = 2* ((size0 < size1) ? size0 : size1); // interpolate 2 times more

  vcl_vector<vgl_point_3d<double> > pts_3d;

  vnl_double_3x3 Sigma3d;

  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++)
      Sigma3d[i][j] = Q0_[i][j];
  }

  for (int i=0; i<npts; i++)
  {
    double index = i/double(npts);
    vgl_homg_point_2d<double> p1(dc0->get_x(index),dc0->get_y(index));
    vgl_point_2d<double> temp_p(p1);
    vnl_double_2x2 sigma1;
    sigma1.set_identity();
    bugl_gaussian_point_2d<double> x1(temp_p, sigma1);
    int x0_index = bdgl_curve_algs:: closest_point(ec0, x1.x(), x1.y());
    double angle0 = (*ec0)[x0_index].get_theta();

    vgl_line_2d<double> lr(FM.image2_epipolar_line(p1));

    // get rid of any point whose gradient is perpendicule to the epipole line
    double nx = lr.a(), ny = lr.b();
    nx = nx / vcl_sqrt(nx*nx + ny*ny);
    ny = ny / vcl_sqrt(nx*nx + ny*ny);
    if (vcl_fabs( nx*vcl_cos(angle0*vnl_math::pi/180) + ny*vcl_sin(angle0*vnl_math::pi/180) )< 0.95)
    {
      // getting the intersection point
      vgl_point_2d<double> p2;
      vcl_vector<vgl_point_2d<double> > pts;
      bdgl_curve_algs::intersect_line(dc1, lr, pts);

      // find the correspoinding point
      double dist = 1e10; // big number
      bool flag = false;
      for (unsigned int j=0; j<pts.size(); j++)
      {
        vgl_homg_point_2d<double> temp(pts[j].x(), pts[j].y());

        int x1_index = bdgl_curve_algs::closest_point(ec1, pts[j].x(), pts[j].y());
        double angle1 = (*ec1)[x1_index].get_theta();

        double dist_p1p2 = vgl_homg_operators_2d<double>::distance_squared(p1, temp);
        if (vcl_fabs(angle1-angle0)<90 && dist > dist_p1p2 &&
            vcl_fabs(nx*vcl_cos(angle1*vnl_math::pi/180) + ny*vcl_sin(angle1*vnl_math::pi/180))<0.95 )
        { // make sure it filters out lines parallel to epipole lines.
          p2 = temp;
          flag = true;
          dist = dist_p1p2;
        }
        else
          continue;
      }

      if (flag) { // if have corresponding
        vnl_double_2x2 Sigma2d;
        Sigma2d.set_identity();
        bugl_gaussian_point_2d<double> x2(p2, Sigma2d);
        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x1, P1, x2, P2);
        pts_3d.push_back(point_3d);
        observes_[0].push_back(x1);
        observes_[1].push_back(x2);
      }
    }
  }


  num_points_ = pts_3d.size();

  curve_3d_.resize(num_points_);
  prob_.resize(num_points_);
  for (int i=0; i<num_points_; i++)
  {
    bugl_gaussian_point_3d<double> p3d(pts_3d[i].x(), pts_3d[i].y(), pts_3d[i].z(), Sigma3d);
    curve_3d_[i] = p3d;

    prob_[i] = 1.0/num_points_;
  }

  X_[0] = T[0];
  X_[1] = T[1];
  X_[2] = T[2];

}

void kalman_filter::init_covariant_matrix()
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
  vnl_double_3 axis = cross_3d(xaxis, T);
  axis /= axis.magnitude(); 
  double theta = angle(T, xaxis);

  vnl_quaternion<double> q(axis, theta);
  vnl_double_3x3 RT = q.rotation_matrix_transpose();
  Sigma = RT.transpose()*Sigma*RT;
  
  Q0_ = 0.0;
  for (int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      Q0_[i][j] = Sigma[i][j];

  // initialize R
  for (int i=0; i<2; i++)
    for (int j=0; j<2; j++)
      R_[i][j] = 0;

  for (int i=0; i<2; i++)
    R_[i][i] = 0.25;
}

void kalman_filter::init_cam_intrinsic()
{
  // set up the intrinsic matrix of the camera
  K_[0][0] = 841.3804; K_[0][1] = 0;        K_[0][2] = 331.0916;
  K_[1][0] = 0;        K_[1][1] = 832.7951; K_[1][2] = 221.5451;
  K_[2][0] = 0;        K_[2][1] = 0;        K_[2][2] = 1;
}

void kalman_filter::prediction()
{
  // TODO
}

vnl_double_3x4 kalman_filter::get_projective_matrix(vnl_double_3& v )
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

vnl_matrix_fixed<double, 2, 6> kalman_filter::get_H_matrix(vnl_double_3x4 &P, vnl_double_3 &X)
{
  vnl_matrix_fixed<double, 2, 6> H;

  // compute \sum {P_{4k} X_k } + P_{44}
  double temp = 0;
  for (int k = 0; k<3; k++)
    temp += P[2][k]*X[k];
  temp += P[2][3];

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

vnl_double_2 kalman_filter::projection(const vnl_double_3x4 &P, const vnl_double_3 &X)
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


void kalman_filter::update_observes(const vnl_double_3x4 &P, int iframe)
{
  observes_[iframe%queue_size_].resize(num_points_);

  for (int i=0; i<num_points_; i++)
  {
    //vgl_point_3d<double> X(curve_3d_[i].x(), curve_3d_[i].y(), curve_3d_[i].z());
    bugl_gaussian_point_2d<double> x = brct_algos::project_3d_point(P, curve_3d_[i]);
    //vgl_point_2d<double> u = brct_algos::closest_point(curves_[iframe], x);
    vgl_point_2d<double> u = brct_algos::most_possible_point(curves_[iframe], x);
    observes_[iframe%queue_size_][i].set_point(u);
    vnl_double_2x2 sigma;
    sigma.set_identity();
    observes_[iframe%queue_size_][i].set_covariant_matrix(sigma);
  }

}

void kalman_filter::update_confidence()
{
  vcl_vector<vnl_double_3x4> cams(cur_pos_+1); //cur_pos_ is 0 based
  for (int i = 0; i < cur_pos_; i++)
    cams[i] = get_projective_matrix(motions_[i]);

  double normalization_factor = 0;
  for (int i=0; i<num_points_; i++)
  {
    double dist2 = 0; // square distance

    for (int f = 0; f<cur_pos_; ++f)
    {
      vgl_point_3d<double> X(curve_3d_[i].x(), curve_3d_[i].y(), curve_3d_[i].z());
      vgl_point_2d<double> x = brct_algos::projection_3d_point(X, cams[f]);
      vgl_point_2d<double> u = brct_algos::closest_point(curves_[f], x);

      double dx = x.x() - u.x();
      double dy = x.y() - u.y();

      dist2 += (dx*dx + dy*dy);
    }

    prob_[i] = vcl_exp(-dist2);
    normalization_factor += prob_[i];
  }

  vcl_cout<<"normalization_factor = "<<normalization_factor;
  // normalize the probability weight across all the points
  for (int i=0; i<num_points_; i++)
    prob_[i] /= normalization_factor;
}

void kalman_filter::inc()
{
  if ((unsigned)(cur_pos_+1) >= curves_.size()){ // end of the data
    vcl_cout<<"\n at the end of last curve\n";
    return;
  }

  cur_pos_ ++;

  //
  // prediction step:
  //
  vnl_vector_fixed<double, 6> Xpred = A_*X_;
  vnl_double_3 camCenter(Xpred[0],Xpred[1],Xpred[2]);
  vnl_double_3x4 P = get_projective_matrix(camCenter);
  update_observes(P, cur_pos_);

  // adjustion
  vcl_vector<bugl_gaussian_point_2d<double> > & cur_measures = observes_[cur_pos_%queue_size_];

  for (int i=0; i<num_points_; i++)
  {
    vnl_double_3 X;

    X[0] = curve_3d_[i].x();
    X[1] = curve_3d_[i].y();
    X[2] = curve_3d_[i].z();

    vnl_matrix_fixed<double, 2, 6> H = get_H_matrix(P, X);

    vnl_matrix_fixed<double, 6, 6> Qpred = A_*Q_*A_.transpose() + Q0_;

#ifdef DEBUG
    vcl_cout<<"P is\n";
    for (int l=0; l<6; l++) {
      for (int j=0; j<6; j++)
        vcl_cout<<' '<<Qpred[l][j];
      vcl_cout<<'\n';
    }
#endif

    G_ = Qpred*H.transpose()*vnl_inverse(H*Qpred*H.transpose()+R_);
#ifdef DEBUG
    vcl_cout<<"K is\n";
    for (int l=0; l<6; l++) {
      for (int j=0; j<2; j++)
        vcl_cout<<' '<<G[l][j];
      vcl_cout<<'\n';
    }
#endif

    vnl_double_2 z(cur_measures[i].x(), cur_measures[i].y());

    vnl_double_2 z_pred = projection(P,X);

    //
    // go to the correction step
    //
    Xpred = Xpred +  G_*(z - z_pred)*prob_[i];

    vnl_matrix_fixed<double, 6, 6> I;
    I.set_identity();
    Q_ = (I - G_*H)*Qpred;
  }

  Xpred[3] = Xpred[0] - X_[0];
  Xpred[4] = Xpred[1] - X_[1];
  Xpred[5] = Xpred[2] - X_[2];

  vnl_double_3 xNew(Xpred[0], Xpred[1], Xpred[2]);

  motions_[cur_pos_%queue_size_] = xNew;
  P = get_projective_matrix(motions_[cur_pos_%queue_size_] );
  update_observes(P, cur_pos_);

  // store the history
  X_ = Xpred;

  if (memory_size_ < queue_size_)
    ++memory_size_;

  // update 3d reconstruction results
  vcl_vector<vnl_double_3x4> Ps(memory_size_);
  vcl_vector<vnl_double_2> pts(memory_size_);

  for (int i=0; i<num_points_; i++)
  {
    for (int j=0; j<memory_size_; j++)
    {
      pts[j] = vnl_double_2(observes_[j][i].x(), observes_[j][i].y());
      Ps[j] = get_projective_matrix(motions_[j]);
    }
    
    vgl_point_3d<double> X3d = brct_algos::bundle_reconstruct_3d_point(pts, Ps);
    curve_3d_[i].set_point(X3d);
#if 0
    brct_structure_estimator se(Ps[cur_pos_%queue_size_]); 
    bugl_gaussian_point_3d<double> X = se.forward(curve_3d_[i], observes_[cur_pos_%queue_size_][i]);
    curve_3d_[i] = se.forward(curve_3d_[i], observes_[cur_pos_%queue_size_][i]);
#endif
    
  }

  // update confidence level for each points
  update_confidence();
}

void kalman_filter::read_data(char *fname)
{
  vcl_ifstream fp(fname);

  char buffer[1000];
  int MAX_LEN=1000;
  int numEdges;

  double x,y, dir , conf;
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
        e.set_theta(0);
        e.set_grad(0);
        ec->add_edgel(e);
        //add this edge to the current contour
      }

      curves_.push_back(new vdgl_digital_curve(new vdgl_interpolator_linear(ec)));
      //read in the end of contour block
      fp.getline(buffer,MAX_LEN);

      //make sure that this is the end marker
      assert(!vcl_strncmp(buffer, "[END CONTOUR]", sizeof("[END CONTOUR]")-1));

      continue;
    }
  }
}

void kalman_filter::init_velocity()
{
  vcl_vector<vgl_homg_line_2d<double> > lines;

  //
  // This is a temporary solution
  //
  vgl_homg_point_2d<double> mp00(179, 253), mp01(364, 254), mp02(463, 306), mp03(416, 202), mp04(402, 192);
  vgl_homg_point_2d<double> mp10(235, 267), mp11(450, 271), mp12(566, 331), mp13(505, 212), mp14(488, 202);

  vgl_homg_line_2d<double> l1(mp00, mp10), l2(mp01, mp11), l3(mp02, mp12), l4(mp03, mp13), l5(mp04, mp14);
  lines.push_back(l1);
  lines.push_back(l2);
  lines.push_back(l3);
  lines.push_back(l4);
  lines.push_back(l5);
  /////////////////////////////////////////////////////////////////////////////////////////////////////


  vgl_homg_point_2d<double> epipole = vgl_homg_operators_2d<double>::lines_to_point(lines);
  vnl_double_3 e(epipole.x(),epipole.y(),epipole.w());
#if 0 // F is not used
  vnl_double_3x3 F;
  F[0][0] =  0;    F[0][1] = -e[2]; F[0][2] =  e[1];
  F[1][0] =  e[2]; F[1][1] =  0;    F[1][2] = -e[0];
  F[2][0] = -e[1]; F[2][1] =  e[0]; F[2][2] =  0;

  FMatrix Fmat(F);
#endif // 0
  init_cam_intrinsic();

  // get translation
  double trans_dist = 1.0; // 105mm
  vnl_double_3 T = vnl_inverse(K_) * e;
  T /= vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T *= trans_dist;

  //initialize the state vector
  X_[0] = X_[3] = T[0];
  X_[1] = X_[4] = T[1];
  X_[2] = X_[5] = T[2];
}

vcl_vector<vgl_point_3d<double> > kalman_filter::get_local_pts()
{
  vcl_vector<vgl_point_3d<double> > pts(num_points_);

  double xc=0, yc=0, zc=0;

  for (int i=0; i<num_points_; i++)
  {
    xc += curve_3d_[i].x();
    yc += curve_3d_[i].y();
    zc += curve_3d_[i].z();
  }

  xc /= num_points_;
  yc /= num_points_;
  zc /= num_points_;

  for (int i=0; i<num_points_; i++)
    pts[i].set(curve_3d_[i].x()-xc, curve_3d_[i].y()-yc, curve_3d_[i].z()-zc);

  return pts;
}

vcl_vector<vgl_point_2d<double> > kalman_filter::get_next_observes()
{
  vcl_vector<vgl_point_2d<double> > pts(num_points_);

  vdgl_digital_curve_sptr dc = curves_[(cur_pos_+1)%queue_size_];
  for (int i=0; i<num_points_; i++)
  {
    double s = double(i) / double(num_points_);
    pts[i].set(dc->get_x(s), dc->get_y(s));
  }
  return pts;
}

vcl_vector<vgl_point_2d<double> > kalman_filter::get_cur_observes()
{
  vcl_vector<vgl_point_2d<double> > pts(num_points_);

  vdgl_digital_curve_sptr dc = curves_[cur_pos_];
  for (int i=0; i<num_points_; i++)
  {
    double s = double(i) / double(num_points_);
    pts[i].set(dc->get_x(s), dc->get_y(s));
  }
  return pts;
}

vcl_vector<vgl_point_2d<double> > kalman_filter::get_pre_observes()
{
  assert(cur_pos_ > 0);
  vcl_vector<vgl_point_2d<double> > pts(num_points_);

  vdgl_digital_curve_sptr dc = curves_[(cur_pos_-1)%queue_size_];
  for (int i=0; i<num_points_; i++)
  {
    double s = double(i) / double(num_points_);
    pts[i].set(dc->get_x(s), dc->get_y(s));
  }
  return pts;
}

vnl_double_3 kalman_filter::get_next_motion(vnl_double_3 v)
{
  return motions_[cur_pos_]+v;
}


vnl_matrix<double> kalman_filter::get_predicted_curve()
{
  //
  // prediction step:
  //
  vnl_vector_fixed<double, 6> Xpred = A_*X_;
  vnl_double_3 camCenter(Xpred[0],Xpred[1],Xpred[2]);
  vnl_double_3x4 P = get_projective_matrix(camCenter);

  vnl_matrix<double> t(2, num_points_);
  for (int i=0; i<num_points_; i++)
  {
    vgl_point_2d<double> x = brct_algos::projection_3d_point(curve_3d_[i], P);

    t[0][i] = x.x();
    t[1][i] = x.y();
  }

  return t;
}

vcl_vector<vnl_matrix<double> > kalman_filter::get_back_projection()
{
 vcl_vector<vnl_matrix<double> > res(memory_size_);
 for(int f=0; f<memory_size_; f++)
  {
    vnl_double_3x4 P = get_projective_matrix(motions_[f]);
    
    vnl_matrix<double> t(2, num_points_);

    for (int i=0; i<num_points_; i++)
    {
      vgl_point_2d<double> p = brct_algos::projection_3d_point(curve_3d_[i], P);
      t[0][i] = p.x();
      t[1][i] = p.y();
    }

    res[f] = t;
  }

  return res;
}
