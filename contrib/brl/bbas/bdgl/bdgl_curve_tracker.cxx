//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <bdgl/bdgl_curve_tracker.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <bdgl/bdgl_curve_matcher.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>
#include <bdgl/bdgl_curve_region.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>


//-----------------------------------------------------------------------------
void bdgl_curve_tracker::track()
{
  bdgl_curve_region                           reg;
  vcl_vector< bdgl_curve_region >             regions;
  bdgl_curve_tracker_primitive                primitive;
  vcl_vector< bdgl_curve_tracker_primitive >  primitive_list;
  vcl_vector< int >                           is_used;
  bdgl_curve_matcher                          matcher(params_.match_params_);
  int best_id;
  double best_val, dist;

  vcl_cout<<"-- start tracking --\n";

  if (input_curve_.size()==0) return;
  vcl_cout<<input_curve_.size()<<" images\n";
  for (unsigned int i=0;i<input_curve_.size();i++)
    vcl_cout<<input_curve_[i].size()<<" curves on image "<<i<<"\n";

  // init : copy the first curves
  output_curve_.clear();
  primitive_list.clear();

  for (unsigned int i=0;i<input_curve_[0].size();i++){
    primitive.init(i, input_curve_[0][i]);
    primitive_list.insert(primitive_list.end(), primitive);
  }
  output_curve_.insert(output_curve_.end(), primitive_list);


  // for all images:
  for (unsigned int t=1;t<input_curve_.size();t++)
  {
    // init : duplicate empty primitive lists
    primitive_list.clear();

    // init the used flag
    is_used.clear();
    for (unsigned int j=0;j<input_curve_[t].size();j++)
      is_used.insert(is_used.end(), 0);

    // compute regions
    vcl_cout<<"-> compute regions\n";

    regions.clear();
    for (unsigned int j=0;j<input_curve_[t].size();j++){
      reg.init(input_curve_[t][j]);
      regions.insert(regions.end(), reg);
    }

    // take every primitive and find the best related curve
    for (unsigned int i=0;i<output_curve_[t-1].size();i++)
    {
      best_id = -1;
      best_val = 1e6;
      //vcl_cout<<".";
#if 0
      vcl_cout<<"primitive "<<i<<" ( ["
              <<vcl_floor(output_curve_[t-1][i].region_.x())<<","
              <<vcl_floor(output_curve_[t-1][i].region_.y())<<"] "
              <<output_curve_[t-1][i].region_.r()<<")\n";
#endif // 0

      // look for curves in the neighborhood
      for (unsigned int j=0;j<input_curve_[t].size();j++)
      {
        // test for neighborhood
        dist = vcl_sqrt( (regions[j].x()- output_curve_[t-1][i].region_.x())
                        *(regions[j].x()- output_curve_[t-1][i].region_.x())
                       + (regions[j].y()- output_curve_[t-1][i].region_.y())
                        *(regions[j].y()- output_curve_[t-1][i].region_.y()) );

        if (dist < regions[j].r() + output_curve_[t-1][i].region_.r() ){
          //vcl_cout<<"try ("<<i<<"->"<<j<<")";
          matcher.init(output_curve_[t-1][i], input_curve_[t][j]);
          matcher.match();
          //vcl_cout<<"->"<<matcher.score()<<"\n";

          // no handling of multiple curve matching : to do
          if (matcher.score() < best_val){
            best_id = j;
            best_val = matcher.score();
          }
        }
      }
      //vcl_cout<<"("<<i<<"->"<<best_id<<")";
      //vcl_cout<<"->"<<best_val<<"\n";

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
          primitive.init(output_curve_[t-1][i].get_id(), input_curve_[t][best_id]);
          // update the ids
          output_curve_[t-1][i].next_num_ = primitive_list.size();
          primitive.prev_num_ = i;
          primitive.next_num_ = -1;

          primitive_list.insert(primitive_list.end(), primitive);
          is_used[best_id]=1;
          vcl_cout<<".";
        }
      }
    }
    vcl_cout<<"-unmatched primitives-\n";
    // take every curve left, and build a primitive for it
    for (unsigned int j=0;j<input_curve_[t].size();j++){
      if (!is_used[j]){
        primitive.init(primitive_list.size(), input_curve_[t][j]);
        primitive_list.insert(primitive_list.end(), primitive);
      }
    }
    output_curve_.insert(output_curve_.end(), primitive_list);
  }

  return;
}

//-----------------------------------------------------------------------------
void bdgl_curve_tracker::track_frame(int frame)
{
  bdgl_curve_region                           reg;
  vcl_vector< bdgl_curve_region >             regions;
  bdgl_curve_tracker_primitive                primitive;
  vcl_vector< bdgl_curve_tracker_primitive >  primitive_list;
  vcl_vector< int >                           is_used;
  bdgl_curve_matcher                          matcher(params_.match_params_);
  int best_id;
  double best_val, dist;

  vcl_cout<<"-- tracking frame "<<frame<<" --\n";

  if (input_curve_.size()<frame) return;
  vcl_cout<<input_curve_[frame].size()<<" curves on image "<<frame<<"\n";

  // init : copy the first curves
  if (frame==0){
    for (unsigned int i=0;i<input_curve_[0].size();i++){
      primitive.init(i, input_curve_[0][i]);
      primitive_list.insert(primitive_list.end(), primitive);
    }
    output_curve_.insert(output_curve_.end(), primitive_list);
  }else{
    // init : duplicate empty primitive lists
    primitive_list.clear();

    // init the used flag
    is_used.clear();
    for (unsigned int j=0;j<input_curve_[frame].size();j++)
      is_used.insert(is_used.end(), 0);

    // compute regions
    vcl_cout<<"-> compute regions\n";

    regions.clear();
    for (unsigned int j=0;j<input_curve_[frame].size();j++){
      reg.init(input_curve_[frame][j]);
      regions.insert(regions.end(), reg);
    }

    // take every primitive and find the best related curve
    for (unsigned int i=0;i<output_curve_[frame-1].size();i++)
    {
      best_id = -1;
      best_val = 1e6;
      //vcl_cout<<".";
#if 0
      vcl_cout<<"primitive "<<i<<" ( ["
              <<vcl_floor(output_curve_[frame-1][i].region_.x())<<","
              <<vcl_floor(output_curve_[frame-1][i].region_.y())<<"] "
              <<output_curve_[frame-1][i].region_.r()<<")\n";
#endif // 0

      // look for curves in the neighborhood
      for (unsigned int j=0;j<input_curve_[frame].size();j++){
        // test for neighborhood
        dist = vcl_sqrt( (regions[j].x()- output_curve_[frame-1][i].region_.x())
                        *(regions[j].x()- output_curve_[frame-1][i].region_.x())
                       + (regions[j].y()- output_curve_[frame-1][i].region_.y())
                        *(regions[j].y()- output_curve_[frame-1][i].region_.y()) );

        if (dist < regions[j].r() + output_curve_[frame-1][i].region_.r() ){
          //vcl_cout<<"try ("<<i<<"->"<<j<<")";
          matcher.init(output_curve_[frame-1][i], input_curve_[frame][j]);
          matcher.match();
          //vcl_cout<<"->"<<matcher.score()<<"\n";

          // no handling of multiple curve matching : to do
          if (matcher.score() < best_val){
            best_id = j;
            best_val = matcher.score();
          }
        }
      }
      //vcl_cout<<"("<<i<<"->"<<best_id<<")";
      //vcl_cout<<"->"<<best_val<<"\n";

      // if none, suppress the primitive
      if (best_id == -1){
        // do nothing: the primitive is not continued
        output_curve_[frame-1][i].next_num_ = -1;
      } else {
        // check if above a thresholded value
        if (best_val > params_.match_thres_){
          // do nothing: the primitive is not continued
          output_curve_[frame-1][i].next_num_ = -1;
        } else {
          // if good, refine the primitive from it
          primitive.init(output_curve_[frame-1][i].get_id(), input_curve_[frame][best_id]);
          // update the ids
          output_curve_[frame-1][i].next_num_ = primitive_list.size();
          primitive.prev_num_ = i;
          primitive.next_num_ = -1;

          primitive_list.insert(primitive_list.end(), primitive);
          is_used[best_id]=1;
          vcl_cout<<".";
        }
      }
    }
    vcl_cout<<"-unmatched primitives-\n";
    // take every curve left, and build a primitive for it
    for (unsigned int j=0;j<input_curve_[frame].size();j++){
      if (!is_used[j]){
        primitive.init(primitive_list[primitive_list.size()-1].get_id(),
                        input_curve_[frame][j]);
        primitive_list.insert(primitive_list.end(), primitive);
      }
    }
    output_curve_.insert(output_curve_.end(), primitive_list);
  }

  return;
}

