#ifndef boxm2_3d_point_estimator_batch_functor_h_
#define boxm2_3d_point_estimator_batch_functor_h_
//:
// \file

#include <vector>
#include <limits>
#include <iostream>
#include <cmath>
#include <boxm2/io/boxm2_stream_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_symmetric_eigensystem.h>

//#define DEBUG 1

class boxm2_3d_point_estimator_batch_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_POINT>::datatype pt_datatype;
  typedef boxm2_data_traits<BOXM2_COVARIANCE>::datatype cov_datatype;

  //: "default" constructor
  boxm2_3d_point_estimator_batch_functor() = default;

  bool init_data(boxm2_data_base *output_pts, boxm2_data_base *output_covs, boxm2_stream_cache_sptr str_cache,
    boxm2_block_id spt_bid, int spt_data_index)
  {
    pts_data_=new boxm2_data<BOXM2_POINT>(output_pts->data_buffer(),output_pts->buffer_length(),output_pts->block_id());
    covs_data_=new boxm2_data<BOXM2_COVARIANCE>(output_covs->data_buffer(),output_covs->buffer_length(),output_covs->block_id());
    str_cache_ = str_cache;

    id_ = output_pts->block_id();
    spt_bid_ = spt_bid;
    spt_data_index_ = spt_data_index;

    return true;
  }

  inline bool process_cell(int index)
  {

    pt_datatype & pt=pts_data_->data()[index];
    cov_datatype & cov=covs_data_->data()[index];

    std::vector<pt_datatype> im_pts = str_cache_->get_next<BOXM2_POINT>(id_, index);
    std::vector<cov_datatype> im_covs = str_cache_->get_next<BOXM2_COVARIANCE>(id_, index);

    //: the point is the average of all the point hypothesis from the images
    pt = pt_datatype(0.0);
#if DEBUG
    //if (index%10000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "index: " << index << std::endl;
      std::cout << "stream cache returns a vector of size: " << im_pts.size() << std::endl;
      std::cout << "pt initialized to: " << pt << std::endl;
      std::cout << "pts from stream cache:\n";
      for (unsigned j = 0; j < im_pts.size(); j++)
        std::cout << im_pts[j] << std::endl;
      std::cout << "covariances from stream cache:\n";
      for (unsigned j = 0; j < im_covs.size(); j++)
        std::cout << im_covs[j] << std::endl;
    }
#endif

    float w_sum = 0.0f;
    for (auto & im_pt : im_pts) {
      float w = im_pt[3];
      pt[0] += im_pt[0];
      pt[1] += im_pt[1];
      pt[2] += im_pt[2];
      w_sum += w;
    }
    pt[0] /= w_sum;
    pt[1] /= w_sum;
    pt[2] /= w_sum;
    pt[3] = w_sum;

    if (w_sum <= 0.0) {  // assign negative covariance to mark
      cov[0] = cov[1] = cov[2] = cov[3] = cov[4] = cov[5] = cov[6] = cov[7] = cov[8] = -1.0;
      return true;
    }

    //: now create the jacobians and sum up the covariances for each point
    vnl_matrix<double> pt_cov(3,3,0.0);
    vnl_matrix<double> jac(3,3,0.0);
    vnl_matrix<double> temp(3,3,0.0);
    vnl_matrix<double> im_cov(3,3,0.0);

    for (unsigned j = 0; j < im_pts.size(); j++) {
      jac.fill(0.0); temp.fill(0.0);

      im_cov[0][0] = im_covs[j][0]; im_cov[0][1] = im_covs[j][1]; im_cov[0][2] = im_covs[j][2];
      im_cov[1][0] = im_covs[j][3]; im_cov[1][1] = im_covs[j][4]; im_cov[1][2] = im_covs[j][5];
      im_cov[2][0] = im_covs[j][6]; im_cov[2][1] = im_covs[j][7]; im_cov[2][2] = im_covs[j][8];

      float w = im_pts[j][3];
      jac[0][0] = jac[1][1] = jac[2][2] = w/w_sum;
      // jac transpose equals jac in this case
      temp = im_cov*jac;
      pt_cov += jac*temp;
    }

#if DEBUG
    //if (index%10000 == 0) {
    if (index == 5822246) {
    //if (spt_bid_ == id_ && index == spt_data_index_) {
      std::cout << "pt: " << pt << std::endl;
      std::cout << "cov: \n" << pt_cov << std::endl;
      // compute the eigenvalues
      vnl_matrix<double> V(3,3,0.0); vnl_vector<double> eigs(3);
      vnl_symmetric_eigensystem_compute(pt_cov, V, eigs);
      std::cout << "eigen values (length of axis of error ellipse) in meters: " << std::endl;
      std::cout << std::sqrt(eigs[0]) << " " << std::sqrt(eigs[1]) << " " << std::sqrt(eigs[2]) << "\n";
    }
#endif

    cov[0] = (float)pt_cov[0][0];
    cov[1] = (float)pt_cov[0][1];
    cov[2] = (float)pt_cov[0][2];

    cov[3] = (float)pt_cov[1][0];
    cov[4] = (float)pt_cov[1][1];
    cov[5] = (float)pt_cov[1][2];

    cov[6] = (float)pt_cov[2][0];
    cov[7] = (float)pt_cov[2][1];
    cov[8] = (float)pt_cov[2][2];

    return true;
  }

 private:

  boxm2_data<BOXM2_POINT>     * pts_data_;
  boxm2_data<BOXM2_COVARIANCE>     * covs_data_;

  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  boxm2_block_id spt_bid_;
  int spt_data_index_;
};

class boxm2_3d_point_estimator_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_POINT>::datatype pt_datatype;
  typedef boxm2_data_traits<BOXM2_COVARIANCE>::datatype cov_datatype;

  //: "default" constructor
  boxm2_3d_point_estimator_functor() = default;

  bool init_data(boxm2_data_base *output_pts, boxm2_data_base *output_covs, boxm2_data_base *img_pts, boxm2_data_base *img_covs)
  {
    pts_data_=new boxm2_data<BOXM2_POINT>(output_pts->data_buffer(),output_pts->buffer_length(),output_pts->block_id());
    covs_data_=new boxm2_data<BOXM2_COVARIANCE>(output_covs->data_buffer(),output_covs->buffer_length(),output_covs->block_id());
    img_pt_data_=new boxm2_data<BOXM2_POINT>(img_pts->data_buffer(),img_pts->buffer_length(),img_pts->block_id());
    img_cov_data_=new boxm2_data<BOXM2_COVARIANCE>(img_covs->data_buffer(),img_covs->buffer_length(),img_covs->block_id());

    return true;
  }

  inline bool process_cell(int index)
  {
    pt_datatype & pt=pts_data_->data()[index];
    cov_datatype & cov=covs_data_->data()[index];

    pt_datatype & im_pt = img_pt_data_->data()[index];
    cov_datatype & im_cov_data = img_cov_data_->data()[index];

#if DEBUG
    //if (index%10000 == 0) {
    if (index == 5822246) {
      std::cout << "......index: " << index << std::endl;
      std::cout << "pt read: " << pt << std::endl;
      std::cout << "cov read: " << cov << std::endl;
      std::cout << "im_pt: " << im_pt << std::endl;
      std::cout << "im_cov_data: " << im_cov_data << std::endl;
    }
#endif

    pt[0] += im_pt[0];
    pt[1] += im_pt[1];
    pt[2] += im_pt[2];
    pt[3] += im_pt[3];  // w

    //: now create the jacobians and sum up the covariances for each point
    vnl_matrix<double> pt_cov(3,3,0.0);
    vnl_matrix<double> jac(3,3,0.0);
    vnl_matrix<double> temp(3,3,0.0);
    vnl_matrix<double> im_cov(3,3,0.0);

    jac.fill(0.0); temp.fill(0.0);
    float w = im_pt[3];
    jac[0][0] = jac[1][1] = jac[2][2] = w;

    im_cov[0][0] = im_cov_data[0]; im_cov[0][1] = im_cov_data[1]; im_cov[0][2] = im_cov_data[2];
    im_cov[1][0] = im_cov_data[3]; im_cov[1][1] = im_cov_data[4]; im_cov[1][2] = im_cov_data[5];
    im_cov[2][0] = im_cov_data[6]; im_cov[2][1] = im_cov_data[7]; im_cov[2][2] = im_cov_data[8];

    // jac transpose equals jac in this case
    temp = im_cov*jac;
    pt_cov += jac*temp;

    cov[0] += (float)pt_cov[0][0];
    cov[1] += (float)pt_cov[0][1];
    cov[2] += (float)pt_cov[0][2];

    cov[3] += (float)pt_cov[1][0];
    cov[4] += (float)pt_cov[1][1];
    cov[5] += (float)pt_cov[1][2];

    cov[6] += (float)pt_cov[2][0];
    cov[7] += (float)pt_cov[2][1];
    cov[8] += (float)pt_cov[2][2];

#if DEBUG
    //if (index%10000 == 0) {
    if (index == 5822246) {
      std::cout << ".......index: " << index << std::endl;
      std::cout << "pt : " << pt << std::endl;
      std::cout << "cov : " << cov << std::endl;
      std::cout << "pt_cov: " << pt_cov << std::endl;
    }
#endif

    return true;
  }

 private:

  boxm2_data<BOXM2_POINT>     * pts_data_;
  boxm2_data<BOXM2_COVARIANCE>     * covs_data_;
  boxm2_data<BOXM2_POINT>     * img_pt_data_;
  boxm2_data<BOXM2_COVARIANCE>     * img_cov_data_;
};


class boxm2_3d_point_estimator_functor2
{
 public:
  typedef boxm2_data_traits<BOXM2_POINT>::datatype pt_datatype;
  typedef boxm2_data_traits<BOXM2_COVARIANCE>::datatype cov_datatype;

  //: "default" constructor
  boxm2_3d_point_estimator_functor2() = default;

  bool init_data(boxm2_data_base *output_pts, boxm2_data_base *output_covs)
  {
    pts_data_=new boxm2_data<BOXM2_POINT>(output_pts->data_buffer(),output_pts->buffer_length(),output_pts->block_id());
    covs_data_=new boxm2_data<BOXM2_COVARIANCE>(output_covs->data_buffer(),output_covs->buffer_length(),output_covs->block_id());

    return true;
  }

  inline bool process_cell(int index)
  {

    pt_datatype & pt=pts_data_->data()[index];
    cov_datatype & cov=covs_data_->data()[index];

    if (index == 5822246) {
      std::cout << "in normalization.......index: " << index << std::endl;
      std::cout << "pt : " << pt << std::endl;
      std::cout << "cov : " << cov << std::endl;
    }

    //: the point is the average of all the point hypothesis from the images
    float w_sum = pt[3];
    float w_sum_sq = w_sum*w_sum;
    pt[0] /= w_sum;
    pt[1] /= w_sum;
    pt[2] /= w_sum;

#if DEBUG
    if (index == 5822246) {
      std::cout << "in normalization.......index: " << index << std::endl;
      std::cout << "pt : " << pt << std::endl;
    }
#endif

    if (w_sum <= 0.0) {  // assign negative covariance to mark
      cov[0] = cov[1] = cov[2] = cov[3] = cov[4] = cov[5] = cov[6] = cov[7] = cov[8] = -1.0;
      return true;
    }

    cov[0] /= w_sum_sq;
    cov[1] /= w_sum_sq;
    cov[2] /= w_sum_sq;

    cov[3] /= w_sum_sq;
    cov[4] /= w_sum_sq;
    cov[5] /= w_sum_sq;

    cov[6] /= w_sum_sq;
    cov[7] /= w_sum_sq;
    cov[8] /= w_sum_sq;

#if DEBUG
    if (index == 5822246) {
      std::cout << "in normalization.......index: " << index << std::endl;
      std::cout << "cov : " << cov << std::endl;
    }
#endif

    return true;
  }

 private:

  boxm2_data<BOXM2_POINT>     * pts_data_;
  boxm2_data<BOXM2_COVARIANCE>     * covs_data_;
};



#endif // boxm2_3d_point_estimator_batch_functor_h_
