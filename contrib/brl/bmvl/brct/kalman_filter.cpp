// kalman_filter.cpp: implementation of the kalman_filter class.
//
//////////////////////////////////////////////////////////////////////

#include "kalman_filter.h"
#include <vcl_list.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <mvl/FMatrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_edgel.h>
//#include <vdgl/vdgl_edgel_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include "brct_algos.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

kalman_filter::kalman_filter()
{
  // initialize the transit matrix
  dt_ = 1.0/28;
  init_transit_matrix();

  init_cam_intrinsic();
  // initialize the observe matrix
  //init_observes();
  init_state_vector();

  // init covariant matrix P_
  init_covariant_matrix();
}

kalman_filter::~kalman_filter()
{

}

kalman_filter::init_transit_matrix()
{
  for(int i=0; i<6; i++)
    for(int j=0; j<6; j++)
      A_[i][j] = 0.0;

  for(int i = 0; i<6; i++)
    A_[i][i] = 1;

  for(int i=0; i<3; i++)
    A_[i][i+3] = dt_;

  for(int i=0; i<6; i++){
    for(int j=0; j<6; j++)
      vcl_cout<<A_[i][j]<<' ';
    vcl_cout<<'\n';
  }
}


kalman_filter::init_state_vector()
{

  assert(observes_.size()>2);
  vgl_homg_point_2d<double> mp0;
  vgl_homg_point_2d<double> mp1;
 
  vcl_list<vgl_homg_line_2d<double> > lines;

  vnl_matrix<double> &img0 = observes_[0], &img1 = observes_[1];
  for(int i=0; i< num_points_; i++){

    vgl_homg_line_2d<double> l(vgl_homg_point_2d<double>(img0[0][i],img0[1][i]), \
                              vgl_homg_point_2d<double>(img1[0][i],img1[1][i]));
 
    lines.push_back(l);
  }

  vgl_homg_point_2d<double> epipole = vgl_homg_operators_2d<double>::lines_to_point(lines);
  vnl_vector<double> e(3);
  e[0] = epipole.x(); e[1] = epipole.y(); e[2] = epipole.w();

  vnl_matrix<double> F(3, 3);
  F[0][0] = 0;     F[0][1] = -e[2];  F[0][2] = e[1];
  F[1][0] = e[2]; F[1][1] = 0;     F[1][2] = -e[0];
  F[2][0] = -e[1];  F[2][1] = e[0]; F[2][2] = 0;

  FMatrix Fmat = F;

  init_cam_intrinsic();

  // get tanslation
  double trans_dist = 1; // 105mm
  vnl_double_3 T = vnl_matrix_inverse<double>(M_in_) * e;
  T /= sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T *= trans_dist;

  vcl_cout<<T;
  
  // setting up the external parameters
  vnl_double_3x4 E1, E2;
  E1[0][0] = 1;       E1[0][1] = 0;        E1[0][2] = 0;          E1[0][3] = 0;
  E1[1][0] = 0;       E1[1][1] = 1;        E1[1][2] = 0;          E1[1][3] = 0;
  E1[2][0] = 0;       E1[2][1] = 0;        E1[2][2] = 1;          E1[2][3] = 0;                         
  
  E2[0][0] = 1;       E2[0][1] = 0;        E2[0][2] = 0;          E2[0][3] = T[0];
  E2[1][0] = 0;       E2[1][1] = 1;        E2[1][2] = 0;          E2[1][3] = T[1];
  E2[2][0] = 0;       E2[2][1] = 0;        E2[2][2] = 1;          E2[2][3] = T[2];
  
  vnl_double_3x4 P1 = M_in_*E1, P2 = M_in_*E2;
  
  vcl_vector<vgl_point_3d<double> > pts_3d;
  
  
  for(int i=0; i<num_points_; i++){
        pts_3d.push_back(brct_algos::triangulate_3d_point(vgl_point_2d<double> (img0[0][i],img0[1][i]), \
                         P1, vgl_point_2d<double> (img1[0][i],img1[1][i]), P2));    
  }


  //get center of the point
  double xc=0, yc=0, zc=0;
  for(int i=0; i<num_points_; i++){
    xc += pts_3d[i].x();
    yc += pts_3d[i].y();
    zc += pts_3d[i].z();
  }

  xc /= num_points_;
  yc /= num_points_;
  zc /= num_points_;

  for(int i=0; i<num_points_; i++){
    Xl_[i][0] = pts_3d[i].x() - xc;
    Xl_[i][1] = pts_3d[i].y() - yc;
    Xl_[i][2] = pts_3d[i].z() - zc;
  }
  
  X_[0] = xc;
  X_[1] = yc;
  X_[2] = zc;

  // initialize the velocity
  X_[3] = 0;
  X_[4] = 0;
  X_[5] = 0;
}

kalman_filter::init_observes(vcl_vector<vnl_matrix<double> > &input)
{
  cur_pos_ = 0;
  queue_size_ = 10;
  memory_size_ = 2;
  observes_.resize(queue_size_);
  curves_.resize(queue_size_);
  motions_.resize(queue_size_);

  for(int i=0; i<queue_size_; i++)
    observes_[i] = input[i];
}

kalman_filter::init_covariant_matrix()
{
  // initialize P
  for(int i=0; i<6; i++)
    for(int j=0; j<6; j++)
      P_[i][j] = 0.0;

  for(int i=3; i<6; i++)
    P_[i][i] = 1;

  // initialize Q
  for(int i=0; i<6; i++)
    for(int j=0; j<6; j++)
      Q_[i][j] = 0.0;

  for(int i=3; i<6; i++)
    Q_[i][i] = 1;

  // initialize R
  for(int i=0; i<2; i++)
    for(int j=0; j<2; j++)
      R_[i][j] = 0;
  
  for(int i=0; i<2; i++)
    R_[i][i] = 1;
}

kalman_filter::init_cam_intrinsic()
{
  // set up the intrinsic matrix of the camera
  M_in_[0][0] = 841.3804; M_in_[0][1] = 0;        M_in_[0][2] = 331.0916;
  M_in_[1][0] = 0;        M_in_[1][1] = 832.7951; M_in_[1][2] = 221.5451;
  M_in_[2][0] = 0;        M_in_[2][1] = 0;        M_in_[2][2] = 1;
}

kalman_filter::prediction()
{


}

vnl_double_3x4 kalman_filter::get_projective_matrix()
{
  vnl_double_3x4 M_ex;

  for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      M_ex[i][j] = 0;

  for(int i=0; i<3; i++)
    M_ex[i][i] = 1;


  for(int i=0; i<3; i++)
    M_ex[i][3] = X_pred_[i];
 
  return M_in_*M_ex;
}

kalman_filter::set_H_matrix(vnl_double_3x4 &P, vnl_vector_fixed<double, 3> &X)
{
  // compute \sum {P_{4k} X_k } + P_{44}
  double temp = 0;
  for(int k = 0; k<3; k++)
    temp += P[3][k]*X[k];
  temp += P[3][3];

  for(int i=0; i<2; i++){

    // \sum{P_{ik} X_4} + P_{i4}
    double t = 0;
    for(int k=0; k<3; k++)
      t += P[i][k] * X[k];
    t += P[i][3];
      
    for(int j=0; j<3; j++)
      H_[i][j] = P[i][j] / temp - t * P[3][j] / (temp*temp);
      
    for(int j=3; j<6; j++)
        H_[i][j] = 0;
  }
  
}

vnl_vector_fixed<double, 2> kalman_filter::projection(vnl_double_3x4 &P, vnl_vector_fixed<double, 3> &X)
{
    vnl_vector_fixed<double, 2> z;
    for(int i=0; i<2; i++){
      double t0 =0;  
      for(int k=0; k<3; k++)
        t0 += P[i][k]*X[k];
      t0 += P[i][3];

      double t1 = 0;
      for(int k=0; k<3; k++)
        t1 += P[3][k]*X[k];
      t1 += P[3][3];

      z[i] = t0/t1;
    }

    return z;
}

kalman_filter::update_covariant()
{
  P_ = A_*P_*A_.transpose() + Q_;
  K_ = P_*H_.transpose()*(H_*P_*H_.transpose()+R_);
}

kalman_filter::inc()
{
  //
  // prediction step:
  // 
  X_pred_ = A_*X_;

  // adjustion
  vnl_matrix<double> & cur_measures = observes_[cur_pos_];

  for(int i=0; i<num_points_; i++){
    vnl_double_3x4 P = get_projective_matrix();

    vnl_vector_fixed<double, 3> X;
    
    for(int j=0; j<3; j++)
      X[j] = Xl_[j][i] + X_[j];

    set_H_matrix(P, X);

    update_covariant();

    vnl_vector_fixed<double, 2> z;
    
    for(int j=0; i<2; i++)
      z[j] = cur_measures[j][i];

    adjust_state_vector(projection(P, X), z);

  }


  cur_pos_ = (cur_pos_ ++) % queue_size_;

  // store the history
  motions_[cur_pos_] = get_projective_matrix();
  X_ = X_pred_;

  if(memory_size_ < queue_size_)
    memory_size_++;

  // update local coordinates
  vcl_vector<vnl_double_3x4 > Ps;
  vcl_vector<vnl_vector_fixed<double, 2> > pts;
  
  double xc=0, yc=0, zc=0;
  for(int i=0; i<num_points_; i++){
    Ps.clear();
    pts.clear();

    for(int j=0; j<memory_size_; j++){
      vnl_vector_fixed<double, 2> pt;
      pt[0] = observes_[j][0][j];
      pt[1] = observes_[j][1][j];
      pts.push_back(pt);
      Ps.push_back(motions_[i]);
    }

    vnl_vector_fixed<double, 3> X3d = brct_algos::bundle_reconstruct_3d_point(pts, Ps);
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

  for(int i=0; i<num_points_; i++){
    Xl_[i][0] -= xc;
    Xl_[i][1] -= yc;
    Xl_[i][2] -= zc;
  }  
  

}

kalman_filter::adjust_state_vector(vnl_vector_fixed<double, 2> &pred, vnl_vector_fixed<double, 2> &meas)
{
  X_pred_ += K_*(meas - pred); 
}

void kalman_filter::read_data(char *fname)
{
  vcl_ifstream fp("temp.txt");
  
  
  char buffer[1000];
  int MAX_LEN=1000;
  int numEdges;
  
  double x,y, dir , conf;
  while (fp.getline(buffer,MAX_LEN)) 
  {
    
    
    
    //ignore comment lines and empty lines
    if (strlen(buffer)<2 || buffer[0]=='#')
      continue;
    //read the line with the contour count info
    
    //read the beginning of a contour block
    
    if (!strncmp(buffer, "[BEGIN CONTOUR]", sizeof("[BEGIN CONTOUR]")-1)){
      
      //read in the next line to get the number of edges in this contour
      fp.getline(buffer,MAX_LEN);
      
      sscanf(buffer,"EDGE_COUNT=%d",&(numEdges));
      
      //instantiate a new contour structure here
      vdgl_edgel_chain_sptr ec=new vdgl_edgel_chain;
      
      for (int j=0; j<numEdges;j++){
        //read in all the edge information
        fp.getline(buffer,MAX_LEN);
        sscanf(buffer," [%lf, %lf]   %lf %lf  ", &(x), &(y), &(dir), &(conf));
        vdgl_edgel e;
        vcl_cout<<"\n " <<x;
        e.set_x(x);
        e.set_y(y);
        e.set_theta(0);
        e.set_grad(0);
        ec->add_edgel(e);
        //add this edge to the current contour
        
      }
      vdgl_interpolator_sptr intp=new vdgl_interpolator_linear(ec);
      vdgl_digital_curve_sptr curve= new vdgl_digital_curve(intp);
      
      vcl_cout<<"\n the length of the curve is : :"<<curve->length();	
      curves_.push_back(curve);
      //read in the end of contour block
      fp.getline(buffer,MAX_LEN);
      
      //make sure that this is the end marker
      if (strncmp(buffer, "[END CONTOUR]", sizeof("[END CONTOUR]")-1))
      {	
        assert(false);
        
      }
      
      continue;
    }
  }          
  
  
}
