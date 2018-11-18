// This is brl/bseg/brec/brec_part_hierarchy_learner.h
#ifndef brec_part_hierarchy_learner_h_
#define brec_part_hierarchy_learner_h_
//:
// \file
// \brief class to build hierarchies for a given number of classes by estimating parameter distributions from labeled imgs and their background models
//
//        The classes to be learned are assumed to be "foreground" regions wrt the supplied background models
//        (or they will only be recognized when they are "foreground" after this learning stage)
//
//        Layer 0: primitive parts. This layer is common to all classes and learned on the sole basis of being foreground or background
//                 The parts with high rho = foreground_posterior/background_posterior on average are kept as layer 0
//                 ranked from highest to lowest rho
//                 Initialize a number of primitive part types, make two passes on the training set:
//                    1st pass: collect strength statistics for each part
//                    fit parametric distributions for each type
//                    2nd pass: measure rho's (posterior ratios for being foreground and background) of each instance of each type
//                              measure expected value of log of rho's of each type
//                 Rank parts based on measure value and pick top N
//
//        Starting from layer 1, upper layers are constructed recursively
//        Layer n: stages:
//                 1) construction of parts as compositions of two layer_n-1 parts
//                 2) construction of parts as compositions of one layer_n-1 part and one layer_n-2 parts
//
//                 Implementation
//                 1) a) Initialize N*N pairs for each class
//                    b) collect stats for angle and distance distributions for each pair of each class
//                    c) find peaks at angle and distance distributions via mean-shift
//                    d) if pa angle peaks and pd distance peaks, find data likelihood for pa*pd models
//                    e) pick top M models of pairs for each class
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Jan 19, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vector>
#include <iostream>
#include <utility>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

#include <vil/vil_image_view.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/algo/bsta_mean_shift.h>

#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_part_hierarchy_detector_sptr.h>


class brec_part_hierarchy_learner : public vbl_ref_count
{
 public:

   typedef bsta_sample_set<double,2>* sample_set_ptr;
   typedef bsta_sample_set<double,2> sample_set;
   typedef bsta_histogram<double>* hist_ptr;
   typedef bsta_histogram<double> hist;
   //typedef std::pair<sample_set_ptr,sample_set_ptr> sample_set_ptr_pair;
   typedef std::pair<hist_ptr, hist_ptr> hist_ptr_pair;
   //typedef std::map<unsigned, std::map<std::pair<unsigned, unsigned>, std::pair<brec_part_instance_sptr, std::pair<hist_ptr_pair, sample_set_ptr_pair> > >* > layer_n_map;
   typedef std::map<unsigned, std::map<std::pair<unsigned, unsigned>, std::pair<brec_part_instance_sptr, std::pair<hist_ptr_pair, sample_set_ptr> > >* > layer_n_map;
   typedef std::map<std::pair<unsigned, unsigned>, std::pair<brec_part_instance_sptr, std::pair<hist_ptr_pair, sample_set_ptr> > > class_map;

   brec_part_hierarchy_learner() : n_(10) {}

   void initialize_layer0_as_gaussians(int ndirs, float lambda_range, float lambda_inc, int n);

   ~brec_part_hierarchy_learner() override {
     for (auto & i : stats_layer0_) {
       delete i.second;
     }
     stats_layer0_.clear();

     layer_n_map::iterator it;
     for (it = stats_layer_n_.begin(); it != stats_layer_n_.end(); it++) {
       class_map* v = it->second;
       for (auto & itm : *v)
       {
         delete (itm.second).second.first.first;
         delete (itm.second).second.first.second;
         delete (itm.second).second.second;
         (itm.second).first = nullptr;
       }
       v->clear();
       delete v;
     }
     stats_layer_n_.clear();
   }

  // assumes float img with values in [0,1] range
  // collect stats for foreground and background regions in the image using the fg_prob_img
  void layer0_collect_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask);
  void layer0_collect_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img);

  void layer0_fit_parametric_dist();

  void layer0_collect_posterior_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask,
                                      vil_image_view<float>& mean_img, vil_image_view<float>& std_dev_img);
  void layer0_collect_posterior_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img,
                                      vil_image_view<float>& mean_img, vil_image_view<float>& std_dev_img);

  //: create a part hierarchy of primitive parts which are added with respect to their average rho_ (posterior ratios)
  //  This will be used to construct layers 1 and above
  //  Clears \p stats_layer0_ and initializes \p stats_layer_n_
  brec_part_hierarchy_sptr layer0_rank_and_create_hierarchy(int N);

  //: initialize learner to construct layer_n as pairs of layer_n-1 of the given hierarchy
  //  Radius is used to initialize the histograms
  //  We use 8 bins for angle in [0, 2*pi] range and 8 bins for distance in [0,radius] range
  bool initialize_layer_n_as_pairs(const brec_part_hierarchy_sptr& h, unsigned layer_id, unsigned nclasses, float radius);

  //: collect stats to construct parts of layer with layer_id using detected parts of layer_id-1
  //  Collect stats for a pair if they exist within radius pixels of each other
  bool layer_n_collect_stats(const brec_part_hierarchy_detector_sptr& hd, unsigned layer_id, unsigned class_id);

  //: uses the joint histograms to fit gaussian distributions to distance for 8 orientations
  //  Replaces the histograms with the fitted distributions' histograms
  bool layer_n_fit_distributions(unsigned class_id, unsigned layer_id, unsigned M);

  std::vector<std::pair<brec_part_instance_sptr, bsta_histogram<float>*> >& stats_layer0() { return stats_layer0_; }

  void print_to_m_file_layer0(const std::string& file_name);
  void print_to_m_file_layer0_fitted_dists(const std::string& file_name);
  void print_layer0();

  void print_to_m_file_layer_n(const std::string& file_name, unsigned class_id, bool print_set);

 public:

  // collect stats for each type of primitive part types
  std::vector<std::pair<brec_part_instance_sptr, bsta_histogram<float>*> > stats_layer0_;

  //: for each class, collect stats
  layer_n_map stats_layer_n_;
  float radius_;  // radius to search for pair compositions for layer_n
  float d_bandwidth_;  // bandwidth to run mean-shift on 1D distance sample set
  float a_bandwidth_;  // bandwidth to run mean-shift on 1D angle sample set

  unsigned type_cnt_;

  //: a visualization parameter, set by initialize method according to the type of primitive initialized
  int n_;

  brec_part_hierarchy_sptr h_; // keep the current hierarchy for visualization purposes.

  //: create a hierarchy instance for each class
  std::map<unsigned, brec_part_hierarchy_sptr> h_map_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value
void vsl_b_write(vsl_b_ostream & os, brec_part_hierarchy_learner const &hl);
void vsl_b_read(vsl_b_istream & is, brec_part_hierarchy_learner &hl);
void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy_learner* hl);
void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_learner* &hl);

#endif // brec_part_hierarchy_learner_h_
