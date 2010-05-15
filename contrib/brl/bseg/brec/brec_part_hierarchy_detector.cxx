// This is brl/bseg/brec/brec_part_hierarchy_detector.cxx
#include "brec_part_hierarchy_detector.h"
//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008

#include "brec_part_base.h"
#include "brec_part_gaussian.h"
#include "brec_part_hierarchy.h"

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>

#include <vcl_utility.h>
#include <vcl_algorithm.h>

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
  // start from the primitives
  vcl_vector<brec_part_instance_sptr> dumm_ins = h_->get_dummy_primitive_instances();
  vcl_vector<brec_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!extract_gaussian_primitives(img, p->lambda0_, p->lambda1_, p->theta_, p->bright_, p->cutoff_percentage_, p->detection_threshold_, p->type_, parts_prims)) {
        vcl_cout << "problems in extracting gaussian primitives!!\n";
        return false;
      }
    }
  }

  map_instance_[0] = parts_prims;

  // create an rtree
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
bool brec_part_hierarchy_detector::detect(vil_image_resource_sptr img, float angle)
{
  // start from the primitives
  vcl_vector<brec_part_instance_sptr> dumm_ins = h_->get_dummy_primitive_instances();
  vcl_vector<brec_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!extract_gaussian_primitives(img, p->lambda0_, p->lambda1_, p->theta_+angle, p->bright_, p->cutoff_percentage_, p->detection_threshold_, p->type_, parts_prims)) {
        vcl_cout << "problems in extracting gaussian primitives!!\n";
        return false;
      }
    }
  }
  map_instance_[0] = parts_prims;
  vcl_cout << "extracted " << parts_prims.size() << " parts of layer 0\n";
  // create an rtree
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

bool brec_part_hierarchy_detector::detect_primitives_using_trained_response_models(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, float angle, float prior_class)
{
  // start from the primitives
  vcl_vector<brec_part_instance_sptr> dumm_ins = h_->get_dummy_primitive_instances();
  vcl_vector<brec_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!p->extract(img, fg_prob_img, angle, h_->model_dir(), parts_prims, prior_class)) {
        vcl_cout << "problems in extracting gaussian primitives!!\n";
        return false;
      }
    }
  }

  map_instance_[0] = parts_prims;

  // create an rtree
  Rtree_type *tr = new Rtree_type();
  map_rtree_.insert(vcl_pair<unsigned, Rtree_type*> (0, tr));
  for (unsigned i = 0; i < parts_prims.size(); i++) {
    tr->add(parts_prims[i]);
  }

  return true;
}

//: sets rho parameter of the primitives differently during training
//  Rotates the detector with the given amount
bool brec_part_hierarchy_detector::detect_primitives_for_training(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, float angle)
{
  // start from the primitives
  vcl_vector<brec_part_instance_sptr> dumm_ins = h_->get_dummy_primitive_instances();
  vcl_vector<brec_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!p->extract(inp, fg_prob_img, angle, parts_prims)) {
        vcl_cout << "problems in extracting gaussian primitives!!\n";
        return false;
      }
    }
  }
  map_instance_[0] = parts_prims;
  vcl_cout << "extracted " << parts_prims.size() << " primitives\n";

  // create an rtree
  Rtree_type *tr = new Rtree_type();
  map_rtree_.insert(vcl_pair<unsigned, Rtree_type*> (0, tr));
  for (unsigned i = 0; i < parts_prims.size(); i++) {
    tr->add(parts_prims[i]);
  }

  return true;
}

bool brec_part_hierarchy_detector::detect(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, float angle, unsigned rho_calculation_method, double radius, float prior_class, unsigned layer_id)
{
  switch (rho_calculation_method) {
      case brec_detector_methods::POSTERIOR_NUMERATOR :
      case brec_detector_methods::POSTERIOR :
        if (!detect_primitives_using_trained_response_models(img, fg_prob_img, angle, prior_class))
          return false;
        break;
      case brec_detector_methods::DENSITY_FOR_TRAINING :
        if (!detect_primitives_for_training(img, fg_prob_img, angle))
          return false;
        break;
      default:
        vcl_cout << "Warning: unknown brec_detector_method encountered: " << rho_calculation_method << vcl_endl;
        break;
  }

  vcl_cout << "extracted " << map_instance_[0].size() << " primitive parts." << vcl_endl;

  unsigned highest = h_->highest_layer_id();
  if (layer_id < highest)
    highest = layer_id;

  vcl_vector<brec_part_instance_sptr> parts_upper_most(map_instance_[0]);
  Rtree_type *rtree_upper_most = map_rtree_[0];
  for (unsigned l = 1; l <= highest; l++) {
    vcl_vector<brec_part_instance_sptr> parts_current;

    extract_upper_layer(parts_upper_most, rtree_upper_most, parts_current, rho_calculation_method, l*radius);
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
                                     brec_part_instance_sptr central_p, unsigned /*ni*/, unsigned /*nj*/, // FIXME - ni and nj unused
                                     Rtree_type* lower_rtree,
                                     float det_threshold)
{
  // first check if types and layers of central_p instance matches with upper_p's info
  if (upper_p->central_part()->type_ != central_p->type_ || upper_p->layer_ != central_p->layer_ + 1) {
    vcl_cout << "central_p instance passed is not compatible with the upper layer part passed\n";
    return 0;
  }

  brec_part_instance_sptr pi = new brec_part_instance(upper_p->layer_, upper_p->type_, brec_part_instance_kind::COMPOSED, central_p->x_, central_p->y_, 0.0f);
  brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(pi->cast_to_base(), central_p->cast_to_base(), true);
  pi->add_outgoing_edge(e1);

  // now for each other part of upper_p, check whether they exist in the map
  float cx = central_p->x_; float cy = central_p->y_;
  brec_part_hierarchy::edge_iterator eit = upper_p->out_edges_begin();
  eit++;  // skip the central part
  double strength = 1.0;

  for ( ; eit != upper_p->out_edges_end(); eit++) {
    vgl_box_2d<float> probe = (*eit)->get_probe_box(central_p);
    vcl_vector<brec_part_instance_sptr> found;
    lower_rtree->get(probe, found);

    double best_fit = 0.0;
    double best_fit_str = 1.0;
    brec_part_instance_sptr best_part;
    for (unsigned i = 0; i < found.size(); i++) {
      if (found[i]->strength_ > det_threshold && found[i]->type_ == (*eit)->target()->type_)
      {
        vnl_vector_fixed<float, 2> v(found[i]->x_-cx, found[i]->y_-cy);
        float dist, angle;
        (*eit)->calculate_dist_angle(central_p, v, dist, angle);
        double str = (*eit)->prob_density(dist, angle);

        if (str < double(det_threshold))
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
      brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(pi->cast_to_base(), best_part->cast_to_base(), false);
      pi->add_outgoing_edge(e2);
    }
  }
  strength *= central_p->strength_;

  // if all of them have been detected then declare existence at the central parts location
  pi->strength_ = float(strength);

  return pi;
}

//: check for existence of \p upper_p with \p central_p as its central part and map will tell if all the other parts exist
//  No thresholding, \return a probabilistic score
brec_part_instance_sptr
brec_part_hierarchy_detector::exists(brec_part_base_sptr upper_p,
                                     brec_part_instance_sptr central_p,
                                     Rtree_type* lower_rtree)
{
  // first check if types and layers of central_p instance matches with upper_p's info
  if (upper_p->central_part()->type_ != central_p->type_ || upper_p->layer_ != central_p->layer_ + 1) {
    vcl_cout << "central_p instance passed is not compatible with the upper layer part passes\n";
    return 0;
  }

  brec_part_instance_sptr pi = new brec_part_instance(upper_p->layer_, upper_p->type_, brec_part_instance_kind::COMPOSED, central_p->x_, central_p->y_, 0.0f);
  brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(pi->cast_to_base(), central_p->cast_to_base(), true);
  pi->add_outgoing_edge(e1);

  // now for each other part of upper_p, check whether they exist in the map
  float cx = central_p->x_; float cy = central_p->y_;
  brec_part_hierarchy::edge_iterator eit = upper_p->out_edges_begin();
  eit++;  // skip the central part
  double strength = 1.0;
  for ( ; eit != upper_p->out_edges_end(); eit++) {
    vgl_box_2d<float> probe = (*eit)->get_probe_box(central_p);
    vcl_vector<brec_part_instance_sptr> found;
    lower_rtree->get(probe, found);

    double best_fit = 0.0;
    double best_fit_str = 1.0;
    brec_part_instance_sptr best_part;
    for (unsigned i = 0; i < found.size(); i++) {
      if (found[i]->type_ == (*eit)->target()->type_) {
        vnl_vector_fixed<float, 2> v(found[i]->x_-cx, found[i]->y_-cy);
        float dist, angle;
        (*eit)->calculate_dist_angle(central_p, v, dist, angle);
        double str = (*eit)->prob_density(dist, angle);
        if (best_fit < str) {
          best_fit = str;
          best_fit_str = found[i]->strength_;
          best_part = found[i];
        }
      }
    }

    if (best_fit <= 0 || !best_part)
      return 0;  // this sub-part not found

    brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(pi->cast_to_base(), best_part->cast_to_base(), false);
    pi->add_outgoing_edge(e2);
    strength *= best_fit*best_fit_str;
  }
  strength *= central_p->strength_;

  // if all of them have been detected then declare existence at the central parts location
  pi->strength_ = float(strength);

  return pi;
}

//: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
void brec_part_hierarchy_detector::extract_upper_layer(vcl_vector<brec_part_instance_sptr>& extracted_parts, unsigned ni, unsigned nj,
                                                       Rtree_type* extracted_parts_rtree,
                                                       vcl_vector<brec_part_instance_sptr>& extracted_upper_parts)
{
  // for each detected part, check for the existence of each upper layer part that uses it as a central part
  for (unsigned i = 0; i < extracted_parts.size(); i++) {
    brec_part_instance_sptr p = extracted_parts[i];
    // find this type in the primitive layer of the hierarchy
    brec_part_base_sptr hp = h_->get_node(p->layer_, p->type_);
    if (!hp)
      continue;

    // find the all the upper layer parts that use hp as a central part
    // check the incoming edges of hp
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

//: check for existence of \p upper_p with \p central_p as its central part and map will tell if all the other parts exist
//  No thresholding, \return a probabilistic score
brec_part_instance_sptr
brec_part_hierarchy_detector::exists_for_training(brec_part_base_sptr upper_p,
                                                  brec_part_instance_sptr central_p,
                                                  Rtree_type* lower_rtree)
{
  // first check if types and layers of central_p instance matches with upper_p's info
  if (upper_p->central_part()->type_ != central_p->type_ || upper_p->layer_ != central_p->layer_ + 1) {
    vcl_cout << "central_p instance passed is not compatible with the upper layer part passes\n";
    return 0;
  }

  brec_part_instance_sptr pi = new brec_part_instance(upper_p->layer_, upper_p->type_, brec_part_instance_kind::COMPOSED, central_p->x_, central_p->y_, 0.0f);
  brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(pi->cast_to_base(), central_p->cast_to_base(), true);
  pi->add_outgoing_edge(e1);

  // now for each other part of upper_p, check whether they exist in the map
  float cx = central_p->x_; float cy = central_p->y_;
  brec_part_hierarchy::edge_iterator eit = upper_p->out_edges_begin();
  eit++;  // skip the central part
  double rho = 1.0;
  for ( ; eit != upper_p->out_edges_end(); eit++) {
    vgl_box_2d<float> probe = (*eit)->get_probe_box(central_p);
    vcl_vector<brec_part_instance_sptr> found;
    lower_rtree->get(probe, found);

    double best_fit = -100000.0;
    brec_part_instance_sptr best_part;
    for (unsigned i = 0; i < found.size(); i++) {
      if (found[i]->type_ == (*eit)->target()->type_) {
        vnl_vector_fixed<float, 2> v(found[i]->x_-cx, found[i]->y_-cy);
        float dist, angle;
        (*eit)->calculate_dist_angle(central_p, v, dist, angle);
        double rho = (*eit)->prob_density(dist, angle)*found[i]->rho_c_f_;
        if (best_fit < rho) {
          best_fit = rho;
          best_part = found[i];
        }
      }
    }

    if (best_fit <= 0 || !best_part)
      return 0;  // this sub-part not found

    brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(pi->cast_to_base(), best_part->cast_to_base(), false);
    pi->add_outgoing_edge(e2);
    rho *= best_fit;
  }
  rho *= central_p->rho_c_f_;

  // if all of them have been detected then declare existence at the central parts location
  pi->rho_c_f_ = rho;
  pi->strength_ = float(rho);

  return pi;
}

//: check for existence of \p upper_p with \p central_p as its central part and map will tell if all the other parts exist
//  No thresholding, \return a probabilistic score
brec_part_instance_sptr
brec_part_hierarchy_detector::exists_using_hierarchies(brec_part_base_sptr upper_p,
                                                       brec_part_instance_sptr central_p,
                                                       Rtree_type* lower_rtree, double radius)
{
  // first check if types and layers of central_p instance matches with upper_p's info
  if (upper_p->central_part()->type_ != central_p->type_ || upper_p->layer_ != central_p->layer_ + 1) {
    vcl_cout << "central_p instance passed is not compatible with the upper layer part passes\n";
    return 0;
  }

  double uniform = 1.0/radius * 1.0/8.0;

  brec_part_instance_sptr pi = new brec_part_instance(upper_p->layer_, upper_p->type_, brec_part_instance_kind::COMPOSED, central_p->x_, central_p->y_, 0.0f);
  brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(pi->cast_to_base(), central_p->cast_to_base(), true);
  pi->add_outgoing_edge(e1);

  // now for each other part of upper_p, check whether they exist in the map
  float cx = central_p->x_; float cy = central_p->y_;

  brec_part_hierarchy::edge_iterator eit = upper_p->out_edges_begin();
  eit++;  // skip the central part

  float prior_non_c_b = 1.0f - (prior_c_f_ + prior_non_c_f_ + prior_c_b_);

  double rho_c_f = central_p->rho_c_f_ * prior_c_f_;
  double rho_c_b = central_p->rho_c_b_ * prior_c_b_;
  double rho_nc_f = central_p->rho_nc_f_ * prior_non_c_f_;
  double rho_nc_b = central_p->rho_nc_b_ * prior_non_c_b;

  for ( ; eit != upper_p->out_edges_end(); eit++) {
    vgl_box_2d<float> probe = (*eit)->get_probe_box(central_p);
    vcl_vector<brec_part_instance_sptr> found;
    lower_rtree->get(probe, found);

    double best_score = -100000.0;
    brec_part_instance_sptr best_part;
    brec_hierarchy_edge_sptr best_edge;
    for (unsigned i = 0; i < found.size(); i++) {
      if (found[i] == central_p)  // skip itself
        continue;
      if (found[i]->type_ == (*eit)->target()->type_) {
        vnl_vector_fixed<float, 2> v(found[i]->x_-cx, found[i]->y_-cy);
        float dist, angle;
        (*eit)->calculate_dist_angle(central_p, v, dist, angle);
        double dens = (*eit)->prob_density(dist, angle);
        double rho_c_f_i = dens*found[i]->rho_c_f_*rho_c_f;
        double rho_c_b_i = dens*found[i]->rho_c_b_*rho_c_b;
        double rho_nc_f_i = uniform*found[i]->rho_nc_f_*rho_nc_f;
        double rho_nc_b_i = uniform*found[i]->rho_nc_b_*rho_nc_b;

        double s = vcl_min(rho_c_f_i/rho_c_b_i,rho_c_f_i/rho_nc_f_i);
        s = vcl_min(s, rho_c_f_i/rho_nc_b_i);

        if (best_score < s) {
          best_score = s;
          best_part = found[i];
          best_edge = (*eit);
        }
      }
    }

    if (best_score <= 0 || !best_part)
      return 0;  // this sub-part not found

    brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(pi->cast_to_base(), best_part->cast_to_base(), false);
    e2->set_model(best_edge->dist_model_, best_edge->angle_model_, best_edge->weight_);
    pi->add_outgoing_edge(e2);
  }

  // now compute the score for the part with a second pass
  eit = pi->out_edges_begin();
  eit++;  // skip the central part
  for ( ; eit != pi->out_edges_end(); eit++) {
    brec_part_instance_sptr pi2 = (*eit)->target()->cast_to_instance();
    vnl_vector_fixed<float, 2> v(pi2->x_-cx, pi2->y_-cy);
    float dist, angle;
    (*eit)->calculate_dist_angle(central_p, v, dist, angle);
    double dens = (*eit)->prob_density(dist, angle);
    rho_c_f *= dens*pi2->rho_c_f_;
    rho_c_b *= dens*pi2->rho_c_b_;
    rho_nc_f *= uniform*pi2->rho_nc_f_;
    rho_nc_b *= uniform*pi2->rho_nc_b_;
  }

#if 0 // OZGE TODO: implement the contributions from the other classes
  // compute the denominator by locating a part with the same primitives with this one in each hierarchy in the list
  double sum = 0.0;
  for (unsigned i = 0; i < class_hierarchies_.size(); i++) {
    vcl_vector<double> scores;
    if (class_hierarchies_[i]->get_score(pi, scores))
      for (unsigned k = 0; k < scores.size(); k++)
        sum += scores[k];
  }
#endif // 0

  // if all of them have been detected then declare existence at the central parts location
  double den = (rho_c_f+rho_c_b+rho_nc_f+rho_nc_b);
  pi->rho_c_f_ = rho_c_f/den;
  pi->rho_c_b_ = rho_c_b/den;
  pi->rho_nc_f_ = rho_nc_f/den;
  pi->rho_nc_b_ = rho_nc_b/den;

  pi->strength_ = float(pi->rho_c_f_);

  return pi;
}


//: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
//  No thresholding, \return a probabilistic score
//  rho_calculation_method = 0 if probabilistic score
//  rho_calculation_method = 1 if training
//  rho_calculation_method = 2 if using other hierarchies to compute a posterior
void brec_part_hierarchy_detector::extract_upper_layer(vcl_vector<brec_part_instance_sptr>& extracted_parts,
                                                       Rtree_type* extracted_parts_rtree,
                                                       vcl_vector<brec_part_instance_sptr>& extracted_upper_parts, unsigned rho_calculation_method, double radius)
{
  // for each detected part, check for the existence of each upper layer part that uses it as a central part
  for (unsigned i = 0; i < extracted_parts.size(); i++) {
    brec_part_instance_sptr p = extracted_parts[i];
    // find this type
    brec_part_base_sptr hp = h_->get_node(p->layer_, p->type_);
    if (!hp)
      continue;

    if (p->layer_ == 1 && p->x_ == 165 && p->y_ == 189) {
      vcl_cout << "here!\n";
    }

    // find the all the upper layer parts that use hp as a central part
    // check the incoming edges of hp
    for (brec_part_hierarchy::edge_iterator eit = hp->in_edges_begin(); eit != hp->in_edges_end(); eit++) {
      if ((*eit)->to_central() && hp == (*eit)->source()->central_part()) {
        brec_part_base_sptr hp_upper = (*eit)->source();

        // now check for the existence of other primitives wrt to the central part and initiate an instance of it if so
        brec_part_instance_sptr hp_upper_instance;

        // p will be its central part and map will tell if all the other parts exist
        switch (rho_calculation_method) {
          case brec_detector_methods::POSTERIOR_NUMERATOR : hp_upper_instance = exists(hp_upper, p, extracted_parts_rtree); break;
          case brec_detector_methods::DENSITY_FOR_TRAINING : hp_upper_instance = exists_for_training(hp_upper, p, extracted_parts_rtree); break;
          case brec_detector_methods::POSTERIOR : hp_upper_instance = exists_using_hierarchies(hp_upper, p, extracted_parts_rtree, radius); break;
          default: vcl_cout << "Warning: unknown brec_detector_method encountered: " << rho_calculation_method << vcl_endl; break;
        }

        if (!hp_upper_instance)
          continue;
        extracted_upper_parts.push_back(hp_upper_instance);
      }
    }
  }
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, brec_part_hierarchy_detector const & /*ph*/)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value\n";
  return;
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_read(vsl_b_istream & /*is*/, brec_part_hierarchy_detector & /*ph*/)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy_detector* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    brec_part_hierarchy_sptr dummy;
    ph = new brec_part_hierarchy_detector(dummy);
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_detector* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}


