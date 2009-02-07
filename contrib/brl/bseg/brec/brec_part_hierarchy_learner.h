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
//                              measure expected value of rho's of each type
//
//        Starting from layer 1, upper layers are constructed recursively
//        Layer n: first "pairs" are formed from previous layer using the highest rho part as the central part
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Jan 19, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

#include <vil/vil_image_view.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_part_hierarchy_detector_sptr.h>
#include <vcl_utility.h>


class brec_part_hierarchy_learner : public vbl_ref_count
{
 public:

   brec_part_hierarchy_learner() : n_(10) {}

   void initialize_layer0_as_gaussians(int ndirs, float lambda_range, float lambda_inc);

   ~brec_part_hierarchy_learner() {
     for (unsigned i = 0; i < stats_layer0_.size(); i++) {
       delete stats_layer0_[i].second;
     }
     stats_layer0_.clear();

     vcl_map<unsigned, vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> >* >::iterator it;
     for (it = stats_layer_n_.begin(); it != stats_layer_n_.end(); it++) {
       vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> >* v = it->second;
       for (unsigned i = 0; i < v->size(); i++) {
        delete (*v)[i].second;
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

  //: collect joint stats to construct parts of layer with layer_id using detected parts of layer_id-1
  bool layer_n_collect_stats(brec_part_hierarchy_detector_sptr hd, unsigned layer_id, unsigned class_id);

  vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_histogram<float>*> >& stats_layer0() { return stats_layer0_; }

  void print_to_m_file_layer0(vcl_string file_name);
  void print_to_m_file_layer0_fitted_dists(vcl_string file_name);
  void print_layer0();

 public:

  // collect stats for each type of primitive part types
  vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_histogram<float>*> > stats_layer0_;

  //: for each class, collect stats
  vcl_map<unsigned, vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> >* > stats_layer_n_;

  //: a visualization parameter, set by initialize method according to the type of primitive initialized
  int n_;

  //: create a hierarchy instance for each class
  vcl_map<unsigned, brec_part_hierarchy_sptr> h_map_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value
void vsl_b_write(vsl_b_ostream & os, brec_part_hierarchy_learner const &hl);
void vsl_b_read(vsl_b_istream & is, brec_part_hierarchy_learner &hl);
void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy_learner* hl);
void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_learner* &hl);

#endif // brec_part_hierarchy_learner_h_
