#ifndef bdgl_curve_clustering_h_
#define bdgl_curve_clustering_h_
//:
// \file
// \brief to cluster the curves based on their transformations
// \author vj (vj@lems.brown.edu)
// \date   8/20/2003
// \verbatim
//  Modifications
// \endverbatim
// Input : list of curves matched to the previous frame and computed their transformation
// Output : clusters of curves have similar transformation.

#include <bdgl/bdgl_tracker_curve_sptr.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_fwd.h>

class bdgl_curve_clustering_params
{
 public:
  // Parameters
  unsigned int no_of_clusters;
  double min_cost_threshold;
  double foreg_backg_threshold;
  bdgl_curve_clustering_params(){no_of_clusters=0;min_cost_threshold=0.0;
                                 foreg_backg_threshold=0.0;}

  bdgl_curve_clustering_params(unsigned int num, double cost_threshold,double fg_bg_threshold)
  {
    no_of_clusters=num;
    min_cost_threshold=cost_threshold;
    foreg_backg_threshold=fg_bg_threshold;
  }

 ~bdgl_curve_clustering_params() {}
};

class bdgl_curve_cluster
{
 public:

  bdgl_curve_cluster();
  ~bdgl_curve_cluster() {}
  // list of member curves of a cluster
  vcl_vector<bdgl_tracker_curve_sptr>  curve_cluster_;
  // representative of the cluster
  bdgl_tracker_curve_sptr prototype;

  int get_cluster_id() const { return cluster_id_; }
  void set_cluster_id(int id) { cluster_id_=id; }
  // Similarity transformation parameters
  vnl_matrix<double> get_R() const { return R_; }
  vnl_matrix<double> get_T() const { return T_; }
  double get_scale() const { return scale_; }

  void set_R(vnl_matrix<double> R) { R_=R; }
  void set_T(vnl_matrix<double> T) { T_=T; }
  void set_scale(double s) { scale_=s; }
  double mu;
  double sigma;

 private:

  int cluster_id_;
  double scale_;
  vnl_matrix<double> R_;
  vnl_matrix<double> T_;
};

class bdgl_curve_clustering
{
 public:
  bdgl_curve_clustering(){}

  // constructor initializing parameters for clustering
  bdgl_curve_clustering(bdgl_curve_clustering_params &cp)
  {
    no_of_clusters_=cp.no_of_clusters;
    min_cost_threshold_= cp.min_cost_threshold;
    foreg_backg_threshold_=cp.foreg_backg_threshold;
  }
 ~bdgl_curve_clustering() {}
  // merges cluster j into i
  void merge_clusters(int i,int j);
    //computing transformation a curve given a T
  double compute_likelihood_distance(int i,int j);
  void compute_transformation(vcl_vector<vgl_point_2d<double> > p,
                              vcl_vector<vgl_point_2d<double> > & tp,
                              vnl_matrix<double> R,
                              vnl_matrix<double> T,
                              double s);
  // initializing clusters with each curve
  void init_clusters(vcl_vector<bdgl_tracker_curve_sptr> * curve_sets);
  // clustering curves
  void cluster_curves(vcl_vector<bdgl_tracker_curve_sptr> * curve_sets);
  // computes the distance between two clusters using the effect of transformation
  double compute_cluster_dist(int i,int j);
  //computes the distance between two curves after they are transformed using each others transformation
  double compute_euclidean_dist(int i,int j);
  // get the moving object clusters
  void get_moving_objects(int frame_no,vcl_vector<vcl_vector<bdgl_tracker_curve_sptr> > & curves_on_objects);
  void clustering();
  double compute_vector_distance(int i,int j);

  // compute hausdorff distance between two point sets
  double compute_hausdorff_distance(vcl_vector<vgl_point_2d<double> > curve1,
                                    vcl_vector<vgl_point_2d<double> > curve2);
  // list of clusters
  vcl_vector<bdgl_curve_cluster> clusters_;
  // lookup table for distance between two curves
  vcl_map<vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr>,double> distance_table;
  void write_distance_table(vcl_string filename,vcl_vector<bdgl_tracker_curve_sptr> * curve_sets);
 private:

  double compute_mean(vcl_vector<double> t);
  double compute_std(vcl_vector<double> t);
  double median(vcl_vector<double> vec);
  double min(vcl_vector<double> vec);
  double max(vcl_vector<double> vec);
  double thresh_;
  vcl_map<bdgl_tracker_curve_sptr,int> mapping_;

  int no_of_clusters_;
  double min_cost_threshold_;
  double foreg_backg_threshold_;
};

#endif
