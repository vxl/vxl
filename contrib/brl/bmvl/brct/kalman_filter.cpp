/////////////////////////////////////////////////////////////////////

#include "kalman_filter.h"
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

const double kalman_filter::large_num_ = 1e15;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
kalman_filter::kalman_filter()
{
}

kalman_filter::kalman_filter(const char* fname)
{
  // read data into the pool
  read_data(fname);
  e_ = 0;
}


void kalman_filter::init()
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


kalman_filter::~kalman_filter()
{
  delete e_;
}


void kalman_filter::init_state_3d_estimation()
{
  double dt = time_tick_[1] - time_tick_[0];
  vnl_double_3 T(X_[3]*dt,X_[4]*dt,X_[5]*dt);
  vcl_cout<<"T = "<<T<<'\n';

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

  //
  // 3D estimation
  //

  // point matcher using epipolar geometry
  int c = trackers_.size();

  vnl_double_3x3 Sigma3d;
  vnl_double_2x2 Sigma2d;
  Sigma2d.set_identity();
  Sigma3d.set_identity();
  vcl_cout<<Sigma3d<<'\n';

  for (int t =0; t<c; t++){
    vcl_vector<bugl_normal_point_3d_sptr> pts_3d;

    //assert(trackers_[t].size()== time_tick_.size());
    vdgl_digital_curve_sptr dc0 = trackers_[t][0];
    vdgl_interpolator_sptr interp0 = dc0->get_interpolator();
    vdgl_edgel_chain_sptr  ec0 = interp0->get_edgel_chain();

    vdgl_digital_curve_sptr dc1 = trackers_[t][1];
    vdgl_interpolator_sptr interp1 = dc1->get_interpolator();
    vdgl_edgel_chain_sptr  ec1 = interp1->get_edgel_chain();

    int size0 = ec0->size();
    int size1 = ec1->size();
    int npts = 2* ((size0 < size1) ? size0 : size1); // interpolate 2 times more

#if 0
    for (int i=0; i<3; i++)
      for (int j=0; j<3; j++)
        Sigma3d[i][j] = Q0_[i][j];
#endif

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

        // get rid of any matching candidate point whose gradient is far away original gradient
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
          if (vcl_fabs(angle1-angle0)<12.5 && dist > dist_p1p2)
          { // make sure it filters out lines parallel to epipole lines.
            p2 = temp;
            flag = true;
            dist = dist_p1p2;
          }
          else
            continue;
        }

        if (flag) { // if have corresponding
          bugl_gaussian_point_2d<double> x2(p2, Sigma2d);
          vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x1, P1, x2, P2);
          bugl_normal_point_3d_sptr p3d_sptr = new bugl_normal_point_3d(point_3d, Sigma3d);
          pts_3d.push_back(p3d_sptr);
          observes_[0].push_back(x1);
          observes_[1].push_back(x2);
        }
      }// end of point

      curve_3d_.add_curve(pts_3d);
  } // end of each tracker

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


void kalman_filter::init_cam_intrinsic()
{
  // set up the intrinsic matrix of the camera
  K_[0][0] = 841.3804; K_[0][1] = 0;        K_[0][2] = 331.0916;
  K_[1][0] = 0;        K_[1][1] = 832.7951; K_[1][2] = 221.5451;
  K_[2][0] = 0;        K_[2][1] = 0;        K_[2][2] = 1;
}

vnl_double_3x4 kalman_filter::get_projective_matrix(const vnl_double_3& v ) const
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
  vnl_double_2x2 sigma;
  sigma.set_identity();

  int c = trackers_.size();
  for (int t=0; t<c; t++){
    int frag_size = curve_3d_.get_fragment_size(t);
    for (int i=0; i<frag_size; i++)
    {
      int pos = curve_3d_.get_global_pos(t, i);
      bugl_gaussian_point_2d<double> x = brct_algos::project_3d_point(P, *curve_3d_.get_point(pos));
      vgl_point_2d<double> u = brct_algos::most_possible_point(trackers_[t][iframe], x);
      // set point
      observes_[iframe%queue_size_][pos].set_point(u);
      observes_[iframe%queue_size_][pos].set_covariant_matrix(sigma);
    }
  }
}


void kalman_filter::update_confidence()
{
#if 1
  vcl_vector<vnl_double_3x4> cams(memory_size_); //cur_pos_ is 0 based
  for (int i = 0; i < memory_size_; i++)
    cams[i] = get_projective_matrix(motions_[(cur_pos_-i)%memory_size_]);

  double normalization_factor = 0;
  int c = trackers_.size();
  for (int t=0; t<c; t++){
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
        vgl_point_2d<double> u = brct_algos::most_possible_point(trackers_[t][cur_pos_ - f], x);
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


void kalman_filter::inc()
{
  if ((unsigned)(cur_pos_+1) >= trackers_[0].size()){ // end of the data
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
  vnl_vector_fixed<double, 6> Xpred = A*X_;
  vnl_double_3 camCenter(Xpred[0],Xpred[1],Xpred[2]);
  vnl_double_3x4 P = get_projective_matrix(camCenter);
  update_observes(P, cur_pos_);

  // adjustion
  vcl_vector<bugl_gaussian_point_2d<double> > & cur_measures = observes_[cur_pos_%queue_size_];

  //int c = trackers_.size();
  //for (int t = 0; t < c; t ++)

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
  vcl_vector<vnl_double_3x4> Ps(memory_size_);
  vcl_vector<vnl_double_2> pts(memory_size_);

  for (int i=0; i<num_points_; i++)
  {
    for (int j=0; j<memory_size_; j++)
    {
      if (observes_[j][i].x() != this->large_num_ ||
          observes_[j][i].y() != this->large_num_){
        pts[j] = vnl_double_2(observes_[j][i].x(), observes_[j][i].y());
        Ps[j] = get_projective_matrix(motions_[j]);
      }
    }

    vgl_point_3d<double> X3d = brct_algos::bundle_reconstruct_3d_point(pts, Ps);
    // update covariant matrix
    bugl_normal_point_3d_sptr p3d_sptr = curve_3d_.get_point(i);
    vnl_double_3 dX(X3d.x() - p3d_sptr->x(), X3d.y() - p3d_sptr->y(), X3d.z() - p3d_sptr->z());
    vnl_double_3x3 Sigma3d = p3d_sptr->get_covariant_matrix();

    Sigma3d = Sigma3d*(cur_pos_-1.0)/(double)cur_pos_;

    for (int m = 0; m<3; m++)
      for (int n = 0; n<3; n++)
        Sigma3d[m][n] += dX[m]*dX[n] /(cur_pos_);

    p3d_sptr->set_point(X3d);
    p3d_sptr->set_covariant_matrix(Sigma3d);
  }

  // update confidence level for each points
  update_confidence();
}


void kalman_filter::read_data(const char *fname)
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

    // push a tracker into the memory
    trackers_.push_back(read_tracker_file(buff));
  }
}

vcl_vector<double> kalman_filter::read_timestamp_file(char *fname)
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

vcl_vector<vdgl_digital_curve_sptr> kalman_filter::read_tracker_file(char *fname)
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

void kalman_filter::init_velocity()
{
  vcl_vector<vgl_homg_line_2d<double> > lines;

  if (!e_) //if epipole is not initialized
    vcl_cerr<<"epipole is not initialized\n";

  vnl_double_3 e((*e_)[0],(*e_)[1],1.0);
  init_cam_intrinsic();

  // get translation
  double trans_dist = 1.0; // 105mm
  vnl_double_3 T = vnl_inverse(K_) * e;
  T /= vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  if (T[2]<0)
    T *= trans_dist;
  else
    T *= -trans_dist;

  //initialize the state vector
  X_[0] = T[0];
  X_[1] = T[1];
  X_[2] = T[2];

  double dt = time_tick_[1]-time_tick_[0];
  X_[3] = -T[0] /dt;
  X_[4] = -T[1] /dt;
  X_[5] = -T[2] /dt;
}

bugl_curve_3d kalman_filter::get_curve_3d()
{
  return curve_3d_;
}

vcl_vector<vgl_point_3d<double> > kalman_filter::get_local_pts()
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


vcl_vector<vgl_point_2d<double> > kalman_filter::get_next_observes()
{
  vcl_vector<vgl_point_2d<double> > pts(num_points_);

  int c = trackers_.size();
  for (int t=0; t<c; t++){
    vdgl_digital_curve_sptr dc = trackers_[t][(cur_pos_+1)%queue_size_];
    vdgl_interpolator_sptr interp = dc->get_interpolator();
    vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
    int size = ec->size();
    for (int i=0; i<size; i++)
    {
      double s = double(i) / double(size);
      pts.push_back(vgl_point_2d<double> (dc->get_x(s), dc->get_y(s)));
    }
  }
  return pts;
}


vcl_vector<vgl_point_2d<double> > kalman_filter::get_cur_observes()
{
  vcl_vector<vgl_point_2d<double> > pts;

  int c = trackers_.size();

  for (int t=0; t<c; t++){
    vdgl_digital_curve_sptr dc = trackers_[t][cur_pos_];
    vdgl_interpolator_sptr interp = dc->get_interpolator();
    vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
    int size = ec->size();
    for (int i=0; i<size; i++)
    {
      double s = double(i) / double(size);
      pts.push_back(vgl_point_2d<double>(dc->get_x(s), dc->get_y(s)));
    }
  }
  return pts;
}


vcl_vector<vgl_point_2d<double> > kalman_filter::get_pre_observes()
{
  assert(cur_pos_ > 0);
  vcl_vector<vgl_point_2d<double> > pts;

  int c = trackers_.size();

  for (int t = 0; t<c; t++){
    vdgl_digital_curve_sptr dc = trackers_[t][(cur_pos_-1)%queue_size_];
    vdgl_interpolator_sptr interp = dc->get_interpolator();
    vdgl_edgel_chain_sptr  ec = interp->get_edgel_chain();
    int size = ec->size();
    for (int i=0; i<size; i++)
    {
      double s = double(i) / double(size);
      pts.push_back(vgl_point_2d<double> (dc->get_x(s), dc->get_y(s)));
    }
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


vcl_vector<vnl_matrix<double> > kalman_filter::get_back_projection() const
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


vgl_point_2d<double> kalman_filter::get_cur_epipole() const
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


void kalman_filter::init_epipole(double x, double y)
{
  if (!e_)
    e_ = new vnl_double_2;

  (*e_)[0] = x;
  (*e_)[1] = y;
}


double kalman_filter::matched_point_prob(vnl_double_2& z, vnl_double_2& z_pred)
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


vnl_matrix_fixed<double, 6, 6> kalman_filter::get_transit_matrix(int i, int j)
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

