//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//
//

#include "brec_part_hierarchy_detector.h"
#include "brec_part_base.h"
#include "brec_part_gaussian.h"
#include "brec_part_hierarchy.h"

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>

#include <vcl_utility.h>

brec_part_hierarchy_detector::~brec_part_hierarchy_detector()
{
  vcl_map<unsigned, Rtree_type*>::iterator it = map_rtree_.begin();
  for ( ; it != map_rtree_.end(); it++) {
    delete (*it).second;
  }

  map_rtree_.clear();

  vcl_map<unsigned, vcl_vector<brec_part_instance_sptr> >::iterator it2 = map_instance_.begin();
  for ( ; it2 != map_instance_.end(); it2++) {
    ((*it2).second).clear();
  }

  map_instance_.clear();
}

bool brec_part_hierarchy_detector::detect(vil_image_resource_sptr img)
{
  //: start from the primitives
  vcl_vector<brec_part_instance_sptr> dumm_ins = h_->get_dummy_primitive_instances();
  vcl_vector<brec_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!extract_gaussian_primitives(img, p->lambda0_, p->lambda1_, p->theta_, p->bright_, p->cutoff_percentage_, 0.1f, p->type_, parts_prims)) {
        vcl_cout << "problems in extracting gaussian primitives!!\n";
        return false;
      }
    }
  }

  map_instance_[0] = parts_prims;

  //: create an rtree
  Rtree_type *tr = new Rtree_type();
  map_rtree_.insert(vcl_pair<unsigned, Rtree_type*> (0, tr));
  for (unsigned i = 0; i < parts_prims.size(); i++) {
    tr->add(parts_prims[i]);
  }

  unsigned highest = h_->highest_layer_id();
  vcl_vector<brec_part_instance_sptr> parts_upper_most(parts_prims);
  Rtree_type *rtree_upper_most = tr;
  for (unsigned l = 1; l <= highest; l++) {
    vcl_vector<brec_part_instance_sptr> parts_current;
   
    extract_upper_layer(parts_upper_most, img->ni(), img->nj(), rtree_upper_most, parts_current);
    vcl_cout << "extracted " << parts_current.size() << " parts of layer " << l << vcl_endl;
    
    map_instance_[l] = parts_current;
    
    Rtree_type *rtree_current = new Rtree_type();
    map_rtree_.insert(vcl_pair<unsigned, Rtree_type*> (l, rtree_current));

    for (unsigned i = 0; i < parts_current.size(); i++) {
      rtree_current->add(parts_current[i]);
    }

    parts_upper_most.clear();
    parts_upper_most = parts_current;

    rtree_upper_most = rtree_current;
  }

  vcl_cout << "\textracted " << parts_upper_most.size() << " of highest layer: " << highest << " parts, rtree size: " << rtree_upper_most->size() << vcl_endl;


  return true;
}

//: extracts instances of each layer in the given image, by rotating the detector with the given amount
bool brec_part_hierarchy_detector::detect(vil_image_resource_sptr img, float angle) {
  //: start from the primitives
  vcl_vector<brec_part_instance_sptr> dumm_ins = h_->get_dummy_primitive_instances();
  vcl_vector<brec_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!extract_gaussian_primitives(img, p->lambda0_, p->lambda1_, p->theta_+angle, p->bright_, p->cutoff_percentage_, 0.1f, p->type_, parts_prims)) {
        vcl_cout << "problems in extracting gaussian primitives!!\n";
        return false;
      }
    }
  }

  map_instance_[0] = parts_prims;

  //: create an rtree
  Rtree_type *tr = new Rtree_type();
  map_rtree_.insert(vcl_pair<unsigned, Rtree_type*> (0, tr));
  for (unsigned i = 0; i < parts_prims.size(); i++) {
    tr->add(parts_prims[i]);
  }

  unsigned highest = h_->highest_layer_id();
  vcl_vector<brec_part_instance_sptr> parts_upper_most(parts_prims);
  Rtree_type *rtree_upper_most = tr;
  for (unsigned l = 1; l <= highest; l++) {
    vcl_vector<brec_part_instance_sptr> parts_current;
   
    extract_upper_layer(parts_upper_most, img->ni(), img->nj(), rtree_upper_most, parts_current);
    vcl_cout << "extracted " << parts_current.size() << " parts of layer " << l << vcl_endl;
    
    map_instance_[l] = parts_current;
    
    Rtree_type *rtree_current = new Rtree_type();
    map_rtree_.insert(vcl_pair<unsigned, Rtree_type*> (l, rtree_current));

    for (unsigned i = 0; i < parts_current.size(); i++) {
      rtree_current->add(parts_current[i]);
    }

    parts_upper_most.clear();
    parts_upper_most = parts_current;

    rtree_upper_most = rtree_current;
  }

  vcl_cout << "\textracted " << parts_upper_most.size() << " of highest layer: " << highest << " parts, rtree size: " << rtree_upper_most->size() << vcl_endl;


  return true;
}


// check for existence of upper_p with central_p as its central part and map will tell if all the other parts exist
brec_part_instance_sptr 
brec_part_hierarchy_detector::exists(brec_part_base_sptr upper_p, 
                                     brec_part_instance_sptr central_p, unsigned ni, unsigned nj,
                                     Rtree_type* lower_rtree, 
                                     float det_threshold)
{
  
  //: first check if types and layers of central_p instance matches with upper_p's info
  if (upper_p->central_part()->type_ != central_p->type_ || upper_p->layer_ != central_p->layer_ + 1) {
    vcl_cout << "central_p instance passed is not compatible with the upper layer part passes\n";
    return 0;
  }

  brec_part_instance_sptr pi = new brec_part_instance(upper_p->layer_, upper_p->type_, brec_part_instance_kind::COMPOSED, central_p->x_, central_p->y_, 0.0f); 
  brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(pi->cast_to_base(), central_p->cast_to_base());
  pi->add_outgoing_edge(e1);

  //: now for each other part of upper_p, check whether they exist in the map
  float cx = central_p->x_; float cy = central_p->y_;
  brec_part_hierarchy::edge_iterator eit = upper_p->out_edges_begin();
  eit++;  // skip the central part
  float strength = 1.0f;
  for ( ; eit != upper_p->out_edges_end(); eit++) {
    vgl_box_2d<float> probe = (*eit)->get_probe_box(central_p);
    vcl_vector<brec_part_instance_sptr> found;
    lower_rtree->get(probe, found);
    
    float best_fit = 0.0f;
    float best_fit_str = 1.0f;
    brec_part_instance_sptr best_part;
    for (unsigned i = 0; i < found.size(); i++) {    
      if (found[i]->strength_ > det_threshold && found[i]->type_ == (*eit)->target()->type_) {
        vnl_vector_fixed<float, 2> v(found[i]->x_-cx, found[i]->y_-cy);
        float dist, angle;
        (*eit)->calculate_dist_angle(central_p, v, dist, angle);
        float str = (*eit)->prob_density(dist, angle);
        if (str < det_threshold)
          continue;
        if (best_fit < str) {
          best_fit = str;
          best_fit_str = found[i]->strength_;
          best_part = found[i];
        }
      }
    }

    if (best_fit <= 0)
      return 0;  // this sub-part not found
    strength *= best_fit*best_fit_str;
    if (best_part) {
      brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(pi->cast_to_base(), best_part->cast_to_base());
      pi->add_outgoing_edge(e2);
    }

  }
  strength *= central_p->strength_;

  //: if all of them have been detected then declare existence at the central parts location
  pi->strength_ = strength;

  return pi;
}


//: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
void brec_part_hierarchy_detector::extract_upper_layer(vcl_vector<brec_part_instance_sptr>& extracted_parts, unsigned ni, unsigned nj, 
                                                       Rtree_type* extracted_parts_rtree,
                                                       vcl_vector<brec_part_instance_sptr>& extracted_upper_parts)
{
  
  //: for each detected part, check for the existence of each upper layer part that uses it as a central part
  for (unsigned i = 0; i < extracted_parts.size(); i++) {
    brec_part_instance_sptr p = extracted_parts[i];
    //: find this type in the primitive layer of the hierarchy
    brec_part_base_sptr hp = h_->get_node(p->layer_, p->type_);
    if (!hp)
      continue;
    
    if (i == 1402) {
      vcl_cout << " here \n";
    }
    //: find the all the upper layer parts that use hp as a central part
    //  check the incoming edges of hp
    for (brec_part_hierarchy::edge_iterator eit = hp->in_edges_begin(); eit != hp->in_edges_end(); eit++) {
      if (hp == (*eit)->source()->central_part()) {
        brec_part_base_sptr hp_upper = (*eit)->source();

        // now check for the existence of other primitives wrt to the central part and initiate an instance of it if so
        brec_part_instance_sptr hp_upper_instance = exists(hp_upper, p, ni, nj, extracted_parts_rtree, hp_upper->detection_threshold_); // p will be its central part and map will tell if all the other parts exist
        if (!hp_upper_instance)
          continue;
        if (hp_upper_instance->strength_ > hp_upper->detection_threshold_)
          extracted_upper_parts.push_back(hp_upper_instance);
      }
    }
  }
}
