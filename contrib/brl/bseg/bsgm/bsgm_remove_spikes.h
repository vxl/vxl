#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <vgl/vgl_point_3d.h>
#include <vil/vil_image_view.h>

//:
// \file
// \brief A class to remove local extreme z values in a dsm
// \author J.L. Mundy
// \date Oct 3, 2019
//
// \verbatim
//  Modifications
//   none
// \endverbatim
//

struct remove_spikes_params {
  remove_spikes_params()
    : nbrhd_radius_(3), max_k_(5), cluster_tol_(1.0f) {}

  //: public members
  //: maximum number of cluster centers
  size_t max_k_;
  //: radius of k-means neighborhood
  float nbrhd_radius_;
  //: tolerance in z to belong to a cluster
  float cluster_tol_;
};

class clust_greater {
 public:
  clust_greater() {}
  bool operator () (std::pair<size_t, float> const& a, std::pair<size_t, float> const& b)
  {
    return a.first > b.first;
  }
};

class bsgm_remove_spikes {
 public:
  bsgm_remove_spikes(vil_image_view<float> const& input_img)
    : input_img_(input_img), invalid_z_(0.0f), prt_(false), id_(0), jd_(0) {}

  void set_params(remove_spikes_params const& params) {params_ = params;}

  //: a finite float value considered to be invalid data. Needed if NAN is not available in image formats
  void set_invalid_z(float z) {invalid_z_ = z;}
  float invalid_z() {return invalid_z_;}

  //: apply k-means to z values in a 2r+1 x 2r+1 neigborhood (r = nbrhd_radius)
  //  return number of values assocated with each cluster mean z
  std::vector<std::pair<size_t, float> > cluster_centers(std::vector<float> zvals, size_t max_k);

  // the main process method
  // find k-means cluster with largest population
  //: two cases:
  ///  1) smooth = false
  //     if the neigborhood center is within cluster_tol of the cluster mean just keep the original center z value
  //     otherwise there is a spike and replace the neigborhood center with the cluster mean
  //   2) smooth = true
  //     always replace the original center z with the cluster mean
  //
  bool replace_spikes_with_local_z(bool smooth);

  //: return the filtered dem image
  vil_image_view<float> filtered_img() const {return filtered_img_;}

  //: return an image of step edge locations (useful for registration)
  vil_image_view<vxl_byte> edge_img() const {return edge_img_;}

    //:debug functions
  //: return an image of number of clusters
  vil_image_view<float> k_means_img() const { return n_k_; }
  //: print z values in the neighborhood centered at (i, j)
  void enable_print_spike_neighborhood(size_t i, size_t j);
  void disable_print_spike_neighborhood();

 private:
  //: create a border of width radius around the input
  // use linear interpolation to compute values
  bool expand_input_img();
  //: parameter block
  remove_spikes_params params_;
  //: invalid data value
  float invalid_z_;
  //: maximum number of cluster centers
  size_t max_k_;
  //: radius of k-means neighborhood
  float nbrhd_radius_;
  //: tolerance in z to belong to a cluster
  float cluster_tol_;
  //: an image of number of clusters at each (i,j)
  vil_image_view<float> n_k_;
  //: an image of step edge locations, edge at (i,j)==true
  vil_image_view<vxl_byte> edge_img_;
  //: the input dsm
  vil_image_view<float> input_img_;
  //: input image with border of nbrhd_radius
  vil_image_view<float> expanded_input_img_;
  //: the filtered result
  vil_image_view<float> filtered_img_;
  //: debug - location to print neighborhood
  bool prt_;
  size_t id_;
  size_t jd_;
};
