//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include "bdgl_curve_tracker.h"
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <bdgl/bdgl_curve_matcher.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>
#include <bdgl/bdgl_curve_region.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>


//-----------------------------------------------------------------------------
void bdgl_curve_tracker::track()
{
  int i,j,t;
  bdgl_curve_region                           reg;
  vcl_vector< bdgl_curve_region >             regions;
  bdgl_curve_tracker_primitive                primitive;
  vcl_vector< bdgl_curve_tracker_primitive >  primitive_list;
//vcl_vector< vdgl_edgel_chain_sptr >         curve;
  vcl_vector< int >                           is_used;
  bdgl_curve_matcher                          matcher(params_.match_params_);
  int best_id;
  double best_val, dist;

  vcl_cout<<"-- start tracking --\n";

  if (input_curve_.size()==0) return;
  vcl_cout<<input_curve_.size()<<" images\n";
  for (i=0;i<input_curve_.size();i++)
    vcl_cout<<input_curve_[i].size()<<" curves on image "<<i<<"\n";

  // init : copy the first curves
  output_curve_.clear();

  for (i=0;i<input_curve_[0].size();i++){
    primitive.init(i, input_curve_[0][i]);
    primitive_list.insert(primitive_list.end(), primitive);
  }
  output_curve_.insert(output_curve_.end(), primitive_list);

  // init : duplicate empty primitive lists
  primitive_list.clear();
  for (t=1;t<input_curve_.size();t++){
    output_curve_.insert(output_curve_.end(), primitive_list);
  }

  // for all images:
  for (t=1;t<input_curve_.size();t++){
    // init the used flag
    is_used.clear();
    for (j=0;j<input_curve_[t].size();j++)
      is_used.insert(is_used.end(), 0);

    // compute regions
    vcl_cout<<"-> compute regions\n";

    regions.clear();
    for (j=0;j<input_curve_[t].size();j++){
      reg.init(input_curve_[t][j]);
      regions.insert(regions.end(), reg);
    }

    // take every primitive and find the best related curve
    for (i=0;i<output_curve_[t-1].size();i++){
      best_id = -1;
      best_val = 1e6;
      vcl_cout<<"primitive "<<i<<" ( ["
              <<vcl_floor(output_curve_[t-1][i].region_.x())<<","
              <<vcl_floor(output_curve_[t-1][i].region_.y())<<"] "
              <<output_curve_[t-1][i].region_.r()<<")\n";

      // look for curves in the neighborhood
      for (j=0;j<input_curve_[t].size();j++){
        // test for neighborhood
        dist = vcl_sqrt( (regions[j].x()- output_curve_[t-1][i].region_.x())
                        *(regions[j].x()- output_curve_[t-1][i].region_.x())
                       + (regions[j].y()- output_curve_[t-1][i].region_.y())
                        *(regions[j].y()- output_curve_[t-1][i].region_.y()) );

        if (dist < regions[j].r() + output_curve_[t-1][i].region_.r() ){
          vcl_cout<<"try ("<<i<<"->"<<j<<")";
          matcher.init(output_curve_[t-1][i], input_curve_[t][j]);
          matcher.match();
          vcl_cout<<"->"<<matcher.score()<<"\n";

          // no handling of multiple curve matching : to do
          if (matcher.score() < best_val){
            best_id = j;
            best_val = matcher.score();
          }
        }
      }

      // if none, suppress the primitive
      if (best_id == -1){
        // do nothing: the primitive is not continued
        output_curve_[t-1][i].next_num_ = -1;
      } else {
        // check if above a thresholded value
        if (best_val > params_.match_thres_){
          // do nothing: the primitive is not continued
          output_curve_[t-1][i].next_num_ = -1;
        } else {
          // if good, refine the primitive from it
          primitive = output_curve_[t-1][i];
          primitive.prev_num_ = i;
          primitive.next_num_ = -1;
          primitive.num_ = output_curve_[t].size();
          primitive.set_curve(input_curve_[t][j]);
          primitive.set_region(regions[j]);
          output_curve_[t].insert(output_curve_[t].end(), primitive);
        }
      }
    }
    // take every curve left, and build a primitive for it
    for (j=0;j<input_curve_[t].size();j++){
      if (!is_used[j]){
        primitive.init(i, input_curve_[0][i]);
        output_curve_[t].insert(output_curve_[t].end(), primitive);
      }
    }
  }

  return;
}

//-----------------------------------------------------------------------------
