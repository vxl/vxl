// bcal_zhang_linear_calibrate.cpp: implementation of the bcal_zhang_linear_calibrate class.
//
//////////////////////////////////////////////////////////////////////
#include "bcal_zhang_linear_calibrate.h"
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_3.h> // for vnl_cross_3d
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vnl/algo/vnl_svd.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bcal_zhang_linear_calibrate::bcal_zhang_linear_calibrate()
{
  cam_graph_ptr_ = 0;
}

bcal_zhang_linear_calibrate::~bcal_zhang_linear_calibrate()
{
}


void bcal_zhang_linear_calibrate::
setCameraGraph(bcal_camera_graph<bcal_calibrate_plane, bcal_zhang_camera_node, bcal_euclidean_transformation> *pG)
{
  cam_graph_ptr_ = pG;
  initialize();
}


int bcal_zhang_linear_calibrate::compute_homography()
{
  if (!cam_graph_ptr_){
    vcl_cerr<<"empty graphy, need to set graph first\n";
    return 1;
  }

  vgl_h_matrix_2d_compute_linear hmcl;

  int size = cam_graph_ptr_->num_vertice();
  h_matrice_.resize(size);

  vcl_vector<vgl_homg_point_2d<double> > &p0 = cam_graph_ptr_->get_source()->get_points();


  for (int i=0; i<size; i++) {// for each camera
    bcal_zhang_camera_node *cam = cam_graph_ptr_->get_vertex_from_pos(i);
    assert(cam);
    // compute homography
    int nViews = cam->num_views();
    for (int j=0; j<nViews; j++){ // for each view
      vcl_vector<vgl_homg_point_2d<double> > &p1 = cam->getPoints(j);
      h_matrice_[i][j] = hmcl.compute(p0, p1);
    }
  }
  return 0;
}

int bcal_zhang_linear_calibrate::initialize()
{
  // resize h_matrice_
  int num_camera = cam_graph_ptr_->num_vertice();
  h_matrice_.resize(num_camera);
  num_views_.resize(num_camera);

  // allocate vgl_h_matrix_2d<double> for each views
  for (int i=0; i<num_camera; i++){ // from camera 1 to camera n
    bcal_zhang_camera_node *cam = cam_graph_ptr_->get_vertex_from_pos(i);
    assert(cam);

    int nViews = cam->num_views();
    num_views_[i] = nViews;
    for (int j=0; j<nViews; j++)
      h_matrice_[i] = new vgl_h_matrix_2d<double> [nViews];
  }

  return 0;
}

int bcal_zhang_linear_calibrate::clear()
{
  int size = h_matrice_.size();

  // delete all the data
  for (int i=0; i<size; i++){
    delete [] h_matrice_[i];
  }
  return 0;
}


vnl_vector_fixed<double, 6> bcal_zhang_linear_calibrate::homg_constrain(const vgl_h_matrix_2d<double> &hm, int i, int j)
{
  vnl_vector_fixed<double, 6> v;

  assert(i>=0 && i<=2);
  assert(j>=0 && j<=2);

  v[0] = hm.get(0,i) * hm.get(0,j);
  v[1] = hm.get(0,i) * hm.get(1,j) + hm.get(1,i) * hm.get(0,j);
  v[2] = hm.get(1,i) * hm.get(1,j);
  v[3] = hm.get(2,i) * hm.get(0,j) + hm.get(0,i) * hm.get(2,j);
  v[4] = hm.get(2,i) * hm.get(1,j) + hm.get(1,i) * hm.get(2,j);
  v[5] = hm.get(2,i) * hm.get(2,j);

  return v;
}

int bcal_zhang_linear_calibrate::calibrate()
{
  this->clear();
  this->initialize();

  // get homographies
  compute_homography();

  // calibrate cameras
  calibrate_intrinsic();
  calibrate_extrinsic();

  return 0;
}

vnl_double_3x3 bcal_zhang_linear_calibrate::compute_intrinsic(vgl_h_matrix_2d<double> *hm_list, int n_hm)
{
  assert(n_hm > 3);

  // 1) construct V matrix
  vnl_matrix<double> v(n_hm*2, 6);

  vnl_vector_fixed<double, 6> v11, v12, v22, v11_v22;
  for (int i=0; i<n_hm; i++){
    v11 = homg_constrain(hm_list[i], 0, 0);
    v12 = homg_constrain(hm_list[i], 0, 1);
    v22 = homg_constrain(hm_list[i], 1, 1);
    v11_v22 = v11 - v22;

    for (int j=0; j<6; j++){
      v[i*2][j] = v12[j];
      v[i*2+1][j] = v11_v22[j];
    }
  }

  // 2) now solve for b
  vnl_svd<double> svd(v);
  vnl_vector_fixed<double, 6> b;
  for (int i=0; i<6; i++)
    b[i] = svd.V(i, 5);

  double sv4 = svd.W(4);
  double sv5 = svd.W(5);

  if (sv5){ /* error check */
    double ratio = sv4/sv5;

    if (vcl_fabs(ratio) < 200){
      vcl_cerr << "Warning after comparing the singular values\n"
               << "It may be that the system of homographies is underconstrained:\n"
               << sv4 <<  ' ' << sv5 << '\n';
    }
  }

  // 3) get intrinsinsic parameter
  double B11,B12,B22,B13,B23,B33;

  B11 = b[0];
  B12 = b[1];
  B22 = b[2];
  B13 = b[3];
  B23 = b[4];
  B33 = b[5];

  double v0 = (B12*B13 - B11*B23)/(B11*B22 - B12*B12);
  double lamda = B33 - (B13*B13 + v0*(B12*B13-B11*B23))/B11;
  double alpha = vcl_sqrt(vcl_fabs(lamda/B11));
  double beta = vcl_sqrt(vcl_fabs(lamda * B11 /(B11*B22 - B12*B12)));
  double gamma = 0.0-B12*alpha*alpha*beta/lamda;
  double u0 = gamma*v0/beta - B13*alpha*alpha/lamda;

  vnl_double_3x3 k(0.0);
  k[0][0] = alpha;
  k[0][1] = gamma;
  k[0][2] = u0;
  k[1][1] = beta;
  k[1][2] = v0;
  k[2][2] = 1;

  return k;
}

vgl_h_matrix_3d<double> bcal_zhang_linear_calibrate::
compute_extrinsic(vgl_h_matrix_2d<double> const &H, vnl_double_3x3 const &A)
{
  // let A = the intrinsic parameters;
  // let the Extrinsic parameters = (R | t) where R is a rotion matrix
  // and t is a translation matrix.
  // let R = (r1,r2,r3) where ri is the ith column vector
  //  let H = the homogrhaphy = (h1,h2,h3)
  // let l = 1/||A^-1 h1||
  // it turns out that:
  // r1 = l A^-1 h1
  // r2 = l A^-1 h2
  // r3 = r1 x r2
  // t = l A^-1 h3
  // Due to noise r1,r2,r3 might not be a pure rotation matrix.
  // In this case we will have to find the closest pure rotation matrix


  // compute A_inv which is the inverse of the intrinsic parameters
  vnl_double_3x3 A_inv = vnl_inverse(A);

  // get h1 h2 h3
  vnl_double_3 h1, h2, h3;
  for (int i=0;i<3;i++){
    h1[i] = H.get(i,0);
    h2[i] = H.get(i,1);
    h3[i] = H.get(i,2);
  }

  // compute l = 1.0 / ||(A_inv * h1)||
  vnl_double_3 hold = A_inv * h1;
  double mag = hold.two_norm();
  double l = mag ? 1.0/mag : 1.0;

  // calcuate r1 = l A_inv h1
  vnl_double_3 r1 = l * A_inv * h1;

  // calcuate r2 = l A_inv h2
  vnl_double_3 r2 = l * A_inv * h2;

  // note that although r1 will have a unit normal,
  // r2 is not guaranteed to have a unit normal due
  // to noise.

  // caluculate r3 = r1 x r2
  vnl_double_3 r3 = vnl_cross_3d(r1,r2);

  vnl_double_3x3 Q;
  for (int i=0; i<3; i++){
    Q[i][0] = r1[i];
    Q[i][1] = r2[i];
    Q[i][2] = r3[i];
  }

  // since due to noise r2 and r3 might not be unit vecotors
  // we must find the closest valid rotation matrix
  vnl_double_3x3 R = get_closest_rotation(Q);

  // calculate t = l A_inv h3
  vnl_double_3 t = l * A_inv * h3;

  // make a transformation matrix to return
  return vgl_h_matrix_3d<double>(R, t);
}

vnl_double_3x3 bcal_zhang_linear_calibrate::get_closest_rotation(const vnl_double_3x3 &Q)
{
  // let Q = U S V^T
  // it turns out that R = UV^T is the closest valid rotation matrix;
  // to Q.
  vnl_svd<double> svd(Q);

  vnl_double_3x3 R = svd.U() * svd.V().transpose();

  return R;
}

void bcal_zhang_linear_calibrate::calibrate_intrinsic()
{
  int num_camera = cam_graph_ptr_->num_vertice();
  for (int i= 0; i<num_camera; i++){
    bcal_zhang_camera_node *cam = cam_graph_ptr_->get_vertex_from_pos(i);
    assert(cam);
    vnl_double_3x3 K = compute_intrinsic(h_matrice_[i], num_views_[i]);
    cam->set_intrinsic(K);
    vcl_cerr<<"intrinsic parameters K is:\n"<<cam->get_intrinsic()<<'\n';
  }
}

int bcal_zhang_linear_calibrate::calibrate_extrinsic()
{
  int num_camera = cam_graph_ptr_->num_vertice();

  for (int i= 0; i<num_camera; i++){// for each camera
    // get edge
    bcal_zhang_camera_node *cam = cam_graph_ptr_->get_vertex_from_pos(i);
    assert(cam);
    int source_id = cam_graph_ptr_->get_source_id();
    int vertex_id = cam_graph_ptr_->get_vertex_id(i);
    bcal_euclidean_transformation *e = cam_graph_ptr_->get_edge(source_id, vertex_id);
    assert(e != 0) ;

    // compute and set extrinsic parameter
    vnl_double_3x3 K = cam->get_intrinsic();
    int num_views = cam->num_views();

    vcl_vector<vgl_h_matrix_3d<double> > trans_list(num_views);
    for (int j=0; j< num_views; j++){ // for each view
      trans_list[j] = compute_extrinsic(h_matrice_[i][j], K);
    }

    // store it into edge
    e->set_transformations(trans_list);
  }

  return 0;
}
