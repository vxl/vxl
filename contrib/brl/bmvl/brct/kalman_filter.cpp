// kalman_filter.cpp: implementation of the kalman_filter class.
//
//////////////////////////////////////////////////////////////////////

#include "kalman_filter.h"
#include <vcl_list.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sscanf()
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <mvl/FMatrix.h>
#include <vnl/vnl_math.h> // for pi
#include <vnl/vnl_inverse.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <bbas/bdgl/bdgl_curve_algs.h>
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

  // initialize the default queue size
  queue_size_ = 10;
  observes_.resize(queue_size_);
  
  init_transit_matrix();

  init_cam_intrinsic();
  // initialize the observe matrix
  //init_observes();
  init_state_vector();
  // init covariant matrix P_
  init_covariant_matrix();

  // increase the frame number
  cur_pos_ = (cur_pos_+1) % queue_size_;
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

#ifdef DEBUG
  for (int i=0; i<6; i++) {
    for (int j=0; j<6; j++)
      vcl_cout<<A_[i][j]<<' ';
    vcl_cout<<'\n';
  }
#endif // DEBUG
}


void kalman_filter::init_state_vector()
{
  // initialize the velocity direction
  init_velocity();
  vnl_double_3 T;
  T[0] = X_[3];
  T[1] = X_[4];
  T[2] = X_[5];

  // compute camera calibration matrix
  vnl_double_3x4 E1, E2;
  E1[0][0] = 1;       E1[0][1] = 0;        E1[0][2] = 0;          E1[0][3] = 0;
  E1[1][0] = 0;       E1[1][1] = 1;        E1[1][2] = 0;          E1[1][3] = 0;
  E1[2][0] = 0;       E1[2][1] = 0;        E1[2][2] = 1;          E1[2][3] = 0;

  E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] = T[0];
  E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] = T[1];
  E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] = T[2];

  vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2;


  // compute epipole from velocity
  vnl_double_3 e = M_in_*T;

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
  vcl_vector<vgl_point_2d<double> > c0; // matched point for the first view
  vcl_vector<vgl_point_2d<double> > c1;
  
  for (int i=0; i<npts; i++)
  {
    double index = i/double(npts);
    vgl_homg_point_2d<double> p1(dc0->get_x(index),dc0->get_y(index));
    vgl_point_2d<double> x1(p1);
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

        int x1_index = bdgl_curve_algs:: closest_point(ec1, pts[j].x(), pts[j].y());
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
       vgl_point_2d<double> x2(p2);
       vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x1, P1, x2, P2);
        pts_3d.push_back(point_3d);
        c0.push_back(x1);
        c1.push_back(x2);
      }
    }
  }

  num_points_ = pts_3d.size();

  // get observes
  vnl_matrix<double> t0(2, num_points_);
  vnl_matrix<double> t1(2, num_points_);
  for(int i=0; i<num_points_; i++){
    t0[0][i] = c0[i].x();
    t0[1][i] = c0[i].y(); 
    t1[0][i] = c1[i].x();
    t1[1][i] = c1[i].y(); 
  }
  
  observes_[0] = t0;
  observes_[1] = t1;

  //get local coordinates
  double xc=0, yc=0, zc=0;
  for (int i=0; i<num_points_; i++) {
    xc += pts_3d[i].x();
    yc += pts_3d[i].y();
    zc += pts_3d[i].z();
  }

  xc /= num_points_;
  yc /= num_points_;
  zc /= num_points_;

  Xl_.resize(num_points_);
  prob_.resize(num_points_);

  for (int i=0; i<num_points_; i++) {
    Xl_[i][0] = pts_3d[i].x() - xc;
    Xl_[i][1] = pts_3d[i].y() - yc;
    Xl_[i][2] = pts_3d[i].z() - zc;
    prob_[i] = 1.0/num_points_;
  }

  X_[0] = xc;
  X_[1] = yc;
  X_[2] = zc;
}


void kalman_filter::init_observes(vcl_vector<vnl_matrix<double> > &input)
{
  cur_pos_ = 0;
  queue_size_ = 10;
  memory_size_ = 2; // but can grow up to queue size

  observes_.resize(queue_size_);
  assert(input.size() >= (unsigned int)queue_size_);

  curves_.resize(queue_size_);
  motions_.resize(queue_size_);

  for (int i=0; i<queue_size_; ++i) {
    assert(input[i].rows() >= 2 && input[i].cols() >= (unsigned int)queue_size_);
    observes_[i] = input[i];
  }
}


void kalman_filter::init_covariant_matrix()
{
  // initialize P
  for (int i=0; i<6; i++)
    for (int j=0; j<6; j++)
      P_[i][j] = 0.0;

  for (int i=3; i<6; i++)
    P_[i][i] = 1;

  // initialize Q
  for (int i=0; i<6; i++)
    for (int j=0; j<6; j++)
      Q_[i][j] = 0.0;

  for (int i=3; i<6; i++)
    Q_[i][i] = 1;

  // initialize R
  for (int i=0; i<2; i++)
    for (int j=0; j<2; j++)
      R_[i][j] = 0;

  for (int i=0; i<2; i++)
    R_[i][i] = 1;
}

void kalman_filter::init_cam_intrinsic()
{
  // set up the intrinsic matrix of the camera
  M_in_[0][0] = 841.3804; M_in_[0][1] = 0;        M_in_[0][2] = 331.0916;
  M_in_[1][0] = 0;        M_in_[1][1] = 832.7951; M_in_[1][2] = 221.5451;
  M_in_[2][0] = 0;        M_in_[2][1] = 0;        M_in_[2][2] = 1;
}

void kalman_filter::prediction()
{
  // TODO
}

vnl_double_3x4 kalman_filter::get_projective_matrix(vnl_vector_fixed<double,6>& v )
{
  vnl_double_3x4 M_ex;

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      M_ex[i][j] = 0;

  for (int i=0; i<3; i++)
    M_ex[i][i] = 1;

  for (int i=0; i<3; i++)
    M_ex[i][3] = v[i];

  return M_in_*M_ex;
}

void kalman_filter::set_H_matrix(vnl_double_3x4 &P, vnl_double_3 &X)
{
  // compute \sum {P_{4k} X_k } + P_{44}
  double temp = 0;
  for (int k = 0; k<3; k++)
    temp += P[3][k]*X[k];
  temp += P[3][3];

  for (int i=0; i<2; i++)
  {
    // \sum{P_{ik} X_4} + P_{i4}
    double t = 0;
    for (int k=0; k<3; k++)
      t += P[i][k] * X[k];
    t += P[i][3];

    for (int j=0; j<3; j++)
      H_[i][j] = P[i][j] / temp - t * P[3][j] / (temp*temp);

    for (int j=3; j<6; j++)
        H_[i][j] = 0;
  }
}

vnl_double_2 kalman_filter::projection(const vnl_double_3x4 &P, const vnl_double_3 &X)
{
    vnl_double_2 z;
    for (int i=0; i<2; i++) {
      double t0 =0;
      for (int k=0; k<3; k++)
        t0 += P[i][k]*X[k];
      t0 += P[i][3];

      double t1 = 0;
      for (int k=0; k<3; k++)
        t1 += P[3][k]*X[k];
      t1 += P[3][3];

      z[i] = t0/t1;
    }

    return z;
}


void kalman_filter::update_observes(const vnl_double_3x4 &P)
{
  vnl_matrix<double> t(2, num_points_);
  for (int i=0; i<num_points_; i++){
   vgl_point_3d<double> X(Xl_[0][i], Xl_[1][i], Xl_[2][i]);
   vgl_point_2d<double> x = brct_algos::projection_3d_point(X, P);
   vgl_point_2d<double> u = brct_algos::closest_point(curves_[cur_pos_], x);

   t[0][i] = u.x();
   t[1][i] = u.y();
  }

  observes_[cur_pos_] = t;
}

void kalman_filter::update_covariant()
{
  P_ = A_*P_*A_.transpose() + Q_;
  K_ = P_*H_.transpose()*(H_*P_*H_.transpose()+R_);
}

void kalman_filter::inc()
{
  //
  // prediction step:
  //
  X_pred_ = A_*X_;

  vnl_double_3x4 P = get_projective_matrix(X_pred_);
  update_observes(P);

  // adjustion
  vnl_matrix<double> & cur_measures = observes_[cur_pos_];

  for (int i=0; i<num_points_; i++)
  {
    vnl_double_3 X;

    for (int j=0; j<3; j++)
      X[j] = Xl_[j][i] + X_[j];

    set_H_matrix(P, X);

    update_covariant();

    vnl_double_2 z;

    for (int j=0; i<2; i++)
      z[j] = cur_measures[j][i];

    vnl_double_2 z_pred = projection(P,X);

    adjust_state_vector(z_pred, z);
  }

  cur_pos_ = (cur_pos_+1) % queue_size_;

  // store the history
  X_ = X_pred_;

  motions_[cur_pos_] = get_projective_matrix(X_);

  if (memory_size_ < queue_size_)
    memory_size_++;

  // update local coordinates
  vcl_vector<vnl_double_3x4 > Ps;
  vcl_vector<vnl_double_2 > pts;

  double xc=0, yc=0, zc=0;
  for (int i=0; i<num_points_; i++)
  {
    Ps.clear();
    pts.clear();

    for (int j=0; j<memory_size_; j++)
    {
      vnl_double_2 pt;
      pt[0] = observes_[j][0][j];
      pt[1] = observes_[j][1][j];
      pts.push_back(pt);
      Ps.push_back(motions_[j]);
    }

    vnl_double_3 X3d = brct_algos::bundle_reconstruct_3d_point(pts, Ps);
    Xl_[i][0] = X3d[0];
    Xl_[i][1] = X3d[1];
    Xl_[i][2] = X3d[2];

    xc += X3d[0];
    yc += X3d[1];
    zc += X3d[2];
  }

  //get center of the point

  xc /= num_points_;
  yc /= num_points_;
  zc /= num_points_;

  for (int i=0; i<num_points_; i++)
  {
    Xl_[i][0] -= xc;
    Xl_[i][1] -= yc;
    Xl_[i][2] -= zc;
  }
}

void kalman_filter::adjust_state_vector(vnl_double_2 const& pred, vnl_double_2 const& meas)
{
  X_pred_ += K_*(meas - pred);
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
  vcl_list<vgl_homg_line_2d<double> > lines;

#if 0
  vnl_matrix<double> &img0 = observes_[0], &img1 = observes_[1];


  for (int i=0; i< num_points_; i++)
  {
    vgl_homg_line_2d<double> l(vgl_homg_point_2d<double>(img0[0][i],img0[1][i]),
                               vgl_homg_point_2d<double>(img1[0][i],img1[1][i]));

    lines.push_back(l);
  }
#endif // 0

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
  vnl_double_3 e; e[0] = epipole.x(); e[1] = epipole.y(); e[2] = epipole.w();
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
  vnl_double_3 T = vnl_inverse(M_in_) * e;
  T /= vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T *= trans_dist;

  vcl_cout<<T;

  //setting velocity part of the state vector
  X_[3] = T[0];
  X_[4] = T[1];
  X_[5] = T[2];
}

vcl_vector<vgl_point_3d<double> > kalman_filter::get_local_pts()
{
  vcl_vector<vgl_point_3d<double> > pts;
  pts.resize(num_points_);

  for (int i=0; i<num_points_; i++) {
    pts[i].set(Xl_[i][0], Xl_[i][1], Xl_[i][2]);
  }
  return pts;
}

vcl_vector<vgl_point_2d<double> > kalman_filter::get_cur_observes()
{
  vcl_vector<vgl_point_2d<double> > pts;
  pts.resize(num_points_);

  vdgl_digital_curve_sptr dc = curves_[cur_pos_];
  for (int i=0; i<num_points_; i++) {
    double s = double(i) / double(num_points_);
    pts[i].set(dc->get_x(s), dc->get_y(s));
  }
  return pts;
}

vcl_vector<vgl_point_2d<double> > kalman_filter::get_pre_observes()
{
  vcl_vector<vgl_point_2d<double> > pts;
  pts.resize(num_points_);

  assert(cur_pos_ > 0);
  vdgl_digital_curve_sptr dc = curves_[cur_pos_-1];
  for (int i=0; i<num_points_; i++) {
    double s = double(i) / double(num_points_);
    pts[i].set(dc->get_x(s), dc->get_y(s));
  }
  return pts;
}
