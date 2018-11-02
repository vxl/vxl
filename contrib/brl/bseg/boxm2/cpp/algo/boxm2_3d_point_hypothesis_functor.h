#ifndef boxm2_3d_point_hypothesis_functor_h
#define boxm2_3d_point_hypothesis_functor_h
//:
// \file

#include <iostream>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//#define DEBUG 1
//#define DEBUG2 1

// compute all the running sums
class boxm2_3d_point_hypothesis_functor
{
 public:
  //: "default" constructor
  boxm2_3d_point_hypothesis_functor() = default;

  bool init_data(boxm2_data_base* alpha_data, boxm2_data_base* pts_data,
                 boxm2_data_base* sums_data,
                 vil_image_view<float> * depth_img,
                 vil_image_view<float> * var_img,
                 vpgl_perspective_camera<double> * pcam,
                 vpgl_generic_camera<double> * cam,
                 vbl_array_2d<vnl_matrix_fixed<double, 3, 3> >* Rss,
                 vil_image_view<float> * vis_img, boxm2_block_id spt_bid, int spt_data_index)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(alpha_data->data_buffer(),alpha_data->buffer_length(),alpha_data->block_id());
    pts_data_=new boxm2_data<BOXM2_POINT>(pts_data->data_buffer(),pts_data->buffer_length(),pts_data->block_id());
    sums_data_=new boxm2_data<BOXM2_FLOAT16>(sums_data->data_buffer(),sums_data->buffer_length(),sums_data->block_id());
    depth_img_=depth_img;
    var_img_=var_img;
    cam_=cam;
    Rss_ = Rss;
    C_ = vgl_point_3d<double>(pcam->camera_center());
    v_ = pcam->principal_axis();
    vis_img_ = vis_img;
    spt_bid_ = spt_bid;
    spt_data_index_ = spt_data_index;
    id_ = alpha_data->block_id();

    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth=0.0f)
  {
    float vis=(*vis_img_)(i,j);
    float md_s = (*depth_img_)(i,j);
    float var_d_s = (*var_img_)(i,j);

    vgl_ray_3d<double> ray_ij = cam_->ray(i,j);
    vgl_vector_3d<double> v_s = ray_ij.direction();
    vnl_matrix_fixed<double, 3, 3> R_s = (*Rss_)[i][j];

    // calculate point hypothesis
    boxm2_data<BOXM2_POINT>::datatype & pt=pts_data_->data()[index];
#if DEBUG
    //if (index%1000000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "index: " << index << " len: " << seg_len << std::endl;
      std::cout << "current pt: " << pt << " vis(" << i << ", " << j << "): " << vis << std::endl;
      std::cout << "cam center: " << C_.x() << " " << C_.y() << " " << C_.z() << "\n";
    }
#endif

    pt[0] += float(vis*(C_.x() + md_s*v_s.x()));
    pt[1] += float(vis*(C_.y() + md_s*v_s.y()));
    pt[2] += float(vis*(C_.z() + md_s*v_s.z()));

    //pt[0] += float(vis*(C_.x()));
    //pt[1] += float(vis*(C_.y()));
    //pt[2] += float(vis*(C_.z()));

    pt[3] += vis;


#if DEBUG
    //if (index%1000000 == 0)
    if (index == 5822246)
    //if (spt_bid_ == id_ && index == spt_data_index_)
      std::cout << "pt after update: " << pt << std::endl;
#endif

    // calculate sums
    boxm2_data<BOXM2_FLOAT16>::datatype & sums=sums_data_->data()[index];

    sums[0] += vis;

    sums[1] += (float)(vis*md_s*R_s[0][0]);  // delP1/delv1
    sums[2] += (float)(vis*md_s*R_s[0][1]);  // delP1/delv2
    sums[3] += (float)(vis*md_s*R_s[0][2]);  // delP1/delv3

    sums[4] += (float)(vis*md_s*R_s[1][0]);  // delP2/delv1
    sums[5] += (float)(vis*md_s*R_s[1][1]);  // delP2/delv2
    sums[6] += (float)(vis*md_s*R_s[1][2]);  // delP2/delv3

    sums[7] += (float)(vis*md_s*R_s[2][0]);  // delP3/delv1
    sums[8] += (float)(vis*md_s*R_s[2][1]);  // delP3/delv2
    sums[9] += (float)(vis*md_s*R_s[2][2]);  // delP3/delv3

    float delP1_delmd_s = (float)(vis*v_s.x());
    float delP2_delmd_s = (float)(vis*v_s.y());
    float delP3_delmd_s = (float)(vis*v_s.z());
    sums[10] += delP1_delmd_s*delP1_delmd_s*var_d_s;
    sums[11] += delP1_delmd_s*delP2_delmd_s*var_d_s;
    sums[12] += delP1_delmd_s*delP3_delmd_s*var_d_s;
    sums[13] += delP2_delmd_s*delP2_delmd_s*var_d_s;
    sums[14] += delP2_delmd_s*delP3_delmd_s*var_d_s;
    sums[15] += delP3_delmd_s*delP3_delmd_s*var_d_s;

#if DEBUG
    //if (index%1000000 == 0)
    if (index == 5822246)
    //if (spt_bid_ == id_ && index == spt_data_index_)
      std::cout << "sums: " << sums << std::endl;
#endif

    // update vis
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float temp=std::exp(-seg_len*alpha);
    vis*=temp;
    (*vis_img_)(i,j)=vis;
    return true;
  }
 private:
   boxm2_data<BOXM2_ALPHA>     * alpha_data_;
   boxm2_data<BOXM2_POINT> * pts_data_;
   boxm2_data<BOXM2_FLOAT16> * sums_data_;
   vil_image_view<float> * depth_img_;
   vil_image_view<float> * var_img_;
   vpgl_generic_camera<double>* cam_;
   vbl_array_2d<vnl_matrix_fixed<double, 3, 3> >* Rss_;
   vgl_point_3d<double> C_;
   vgl_vector_3d<double> v_;
   vil_image_view<float>* vis_img_;
   boxm2_block_id spt_bid_;
   int spt_data_index_;
   boxm2_block_id id_;
};

// compute the covariance of the point hypothesis for each image
class boxm2_3d_point_hypothesis_cov_functor
{
 public:
  //: "default" constructor
  boxm2_3d_point_hypothesis_cov_functor() = default;

  bool init_data(boxm2_data_base* covs_data,
                 boxm2_data_base* sums_data,
                 vnl_matrix_fixed<double, 3, 3> cov_C,
                 vnl_matrix_fixed<double, 3, 3> cov_v, boxm2_block_id spt_bid, int spt_data_index)
  {
    covs_data_=new boxm2_data<BOXM2_COVARIANCE>(covs_data->data_buffer(),covs_data->buffer_length(),covs_data->block_id());
    sums_data_=new boxm2_data<BOXM2_FLOAT16>(sums_data->data_buffer(),sums_data->buffer_length(),sums_data->block_id());
    cov_C_ = cov_C;
    cov_v_ = cov_v;
    spt_bid_ = spt_bid;
    spt_data_index_ = spt_data_index;
    id_ = covs_data->block_id();
    return true;
  }

  //inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth=0.0f)
  inline bool process_cell(int index)
  {
    boxm2_data<BOXM2_COVARIANCE>::datatype & cov=covs_data_->data()[index];
    boxm2_data<BOXM2_FLOAT16>::datatype & sums=sums_data_->data()[index];

#if DEBUG2
    //if (index%1000000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "--------------------------------------------------\n";
      std::cout << "index: " << index << "\n";// len: " << seg_len << "\n";// ray i: " << i << " j: "<< j << std::endl;
      std::cout << "cov matrix: \n" << cov_C_ << "\n";
      std::cout << "cov matrix: \n" << cov_v_ << "\n";
    }
#endif

    vnl_matrix_fixed<double, 3, 3> out_cov(0.0), temp(0.0);

    //: build the camera center Jacobian
    vnl_matrix_fixed<double, 3, 3> jac(0.0);
    jac[0][0] = jac[1][1] = jac[2][2] = sums[0];  // weight of the hypothesis

    //: the transpose of the jacobian is the same in this case
    temp = cov_C_*jac;
    out_cov = jac*temp;

#if DEBUG2
    //if (index%1000000 == 0) {
    if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "portion of cov from cam center variation: " << std::endl;
      std::cout << out_cov << std::endl;
    }
#endif

    //: build the orientation vector Jacobian
    jac.fill(0.0);
    jac[0][0] = sums[1];
    jac[0][1] = sums[2];
    jac[0][2] = sums[3];

    jac[1][0] = sums[4];
    jac[1][1] = sums[5];
    jac[1][2] = sums[6];

    jac[2][0] = sums[7];
    jac[2][1] = sums[8];
    jac[2][2] = sums[9];

    temp = cov_v_*jac.transpose();
    out_cov += jac*temp;

#if DEBUG2
    //if (index%1000000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "portion of cov from cam orientation variation: " << std::endl;
      std::cout << jac*temp << std::endl;
    }
#endif

    //: add the portion due to depth variation
    temp.fill(0.0);
    temp[0][0] = sums[10]; temp[0][1] = sums[11]; temp[0][2] = sums[12];
    temp[1][0] = sums[11]; temp[1][1] = sums[13]; temp[1][2] = sums[14];
    temp[2][0] = sums[12]; temp[2][1] = sums[14]; temp[2][2] = sums[15];

#if DEBUG2
    //if (index%1000000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "sums: " << sums << std::endl;
      std::cout << "portion of cov from depth variation: " << std::endl;
      std::cout << temp << std::endl;
    }
#endif

    out_cov += temp;

    cov[0] = (float)out_cov[0][0];
    cov[1] = (float)out_cov[0][1];
    cov[2] = (float)out_cov[0][2];

    cov[3] = (float)out_cov[1][0];
    cov[4] = (float)out_cov[1][1];
    cov[5] = (float)out_cov[1][2];

    cov[6] = (float)out_cov[2][0];
    cov[7] = (float)out_cov[2][1];
    cov[8] = (float)out_cov[2][2];

#if DEBUG2
    //if (index%1000000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "cov: " << std::endl;
      std::cout << out_cov << std::endl;
    }
#endif

    return true;
  }
 private:
   boxm2_data<BOXM2_COVARIANCE> * covs_data_;
   boxm2_data<BOXM2_FLOAT16> * sums_data_;
   vnl_matrix_fixed<double, 3, 3> cov_C_;
   vnl_matrix_fixed<double, 3, 3> cov_v_;
   boxm2_block_id spt_bid_;
   int spt_data_index_;
   boxm2_block_id id_;
};



#endif
