// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p0.cxx
#include "boxm2_volm_matcher_p0.h"
//:
// \file
#include "boxm2_volm_wr3db_index.h"
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>

vnl_random rng;

boxm2_volm_matcher_p0::~boxm2_volm_matcher_p0()
{
}

//: find the score of orientation of all the flat and vertical objects
float boxm2_volm_matcher_p0::orientation_score(unsigned cam_id, vcl_vector<unsigned char> const& index_values)
{
  return 0.0;
}
//: find the score measuring overlap of sky region
float  boxm2_volm_matcher_p0::sky_score(unsigned cam_id, vcl_vector<unsigned char> const& index_values)
{
  // get the sky ray ids from the query
  return 1.0;
}

//: matcher function
bool boxm2_volm_matcher_p0::match(vgl_point_3d<double> const& hyp, vcl_vector<unsigned char> const& index_values, volm_score_sptr score)
{
  // for now randomly fill the score array

  unsigned n = cam_space_->valid_indices().size();

  float max_score = 0.0f;
  unsigned max_cam_id = 0;
  float min_score_in_list = 0.0f;
  vcl_vector<unsigned>& cam_ids = score->cam_id_;
  vcl_vector<float> cam_scores;
  for (unsigned i = 0; i < n; i++) {
#if 1
    // match and get a score -- for now get a random score
    float s = (float)rng.drand32();
#endif

    // match the orientations

    // decide to keep the camera or not
    if (s > threshold_) {
      if (cam_ids.size() < max_cam_per_loc_) {
        cam_ids.push_back(i);
        cam_scores.push_back(s);
      }
      else if (s > min_score_in_list) {  // if we need to kick one out, check if s is better than min score in the current list
        // update min score and replace
        min_score_in_list = cam_scores[0];
        for (unsigned jj = 0; jj < cam_ids.size(); jj++) {
          if (s > cam_scores[jj]) {
            cam_scores[jj] = s;
            cam_ids[jj] = i;
          }
          if (min_score_in_list > cam_scores[jj])
            min_score_in_list = cam_scores[jj];
        }
      }
    }

    if (s > max_score) {
      max_score = s;
      max_cam_id = i;
    }
  }

  score->max_score_ = max_score;
  score->max_cam_id_ = max_cam_id;
  return true;
}
