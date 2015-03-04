#include "boxm2_volm_conf_matcher.h"
//:
// \file

#include <vul/vul_file.h>
#include <vcl_algorithm.h>
#include <vgl/io/vgl_io_polygon.h>
#include <volm/volm_utils.h>
#include <volm/volm_candidate_list.h>


boxm2_volm_conf_matcher::boxm2_volm_conf_matcher(volm_conf_query_sptr const& query,
                                                 unsigned const& tile_id,
                                                 vcl_vector<volm_geo_index_node_sptr> const& loc_leaves,
                                                 vcl_string const& index_folder,
                                                 vcl_string const& out_folder,
                                                 vcl_string const& cand_folder,
                                                 float const& buffer_capacity)
 : query_(query), tile_id_(tile_id), loc_leaves_(loc_leaves), index_folder_(index_folder),
   buffer_capacity_(buffer_capacity), cand_folder_(cand_folder),
   out_folder_(out_folder)
{}

boxm2_volm_conf_matcher::boxm2_volm_conf_matcher(volm_camera_space_sptr const& cam_space, depth_map_scene_sptr const& depth_scene,
                                                 unsigned const& tile_id,
                                                 vcl_vector<volm_geo_index_node_sptr> const& loc_leaves,
                                                 vcl_string const& index_folder,
                                                 vcl_string const& out_folder,
                                                 vcl_string const& cand_folder,
                                                 float const& buffer_capacity,
                                                 unsigned tol_in_pixel)
 : tile_id_(tile_id), loc_leaves_(loc_leaves), index_folder_(index_folder),
   buffer_capacity_(buffer_capacity), cand_folder_(cand_folder), out_folder_(out_folder)
{
  query_ = new volm_conf_query(cam_space, depth_scene, tol_in_pixel);
}

int boxm2_volm_conf_matcher::conf_match_cpp(vcl_string const& index_name, bool const& use_height)
{
  // loop over each leaf to match
  unsigned matched_locs = 0;
  unsigned n_leaves = loc_leaves_.size();
  for (unsigned l_idx = 0; l_idx < n_leaves; l_idx++)
  {
    volm_geo_index_node_sptr leaf = loc_leaves_[l_idx];
    bool is_cand = false;
    // load and check candidate region
    vcl_string outer_region_file = cand_folder_ + "/cand_region_outer_" + leaf->get_string() + ".bin";
    vgl_polygon<double> cand_outer, cand_inner;
    cand_outer.clear();
    cand_inner.clear();
    if (vul_file::exists(outer_region_file)) {  // read the exterior boundaries of candidate region
      is_cand = true;
      vsl_b_ifstream ifs_out(outer_region_file);
      vsl_b_read(ifs_out, cand_outer);
      ifs_out.close();
    }
    if (cand_outer.num_sheets() == 0)
      continue;  // leaf is entirely outside the candidate region
    vcl_string inner_region_file = cand_folder_ + "/cand_region_inner_" + leaf->get_string() + ".bin";
    if (vul_file::exists(inner_region_file)) {
      vsl_b_ifstream ifs_in(inner_region_file);
      vsl_b_read(ifs_in, cand_inner);
      ifs_in.close();
    }
    // check whether the leaf is entirely covered by the candidate region
    if (cand_inner.num_sheets() == 0) {
      for (unsigned i = 0; (i < cand_outer.num_sheets() && is_cand); i++)
        if (volm_utils::poly_contains(cand_outer[i], leaf->extent_))
          is_cand = false;
    }
    // create an index buffer for current leaf
    vcl_stringstream index_file_pre;
    index_file_pre << index_folder_ << "/conf_index_tile_" << tile_id_;
    vcl_string bin_file_name = index_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    //if (!vul_file::exists(bin_file_name)) {
    //  vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: can not find index binary file " << bin_file_name << " for leaf " << l_idx << " and index name " << index_name << "!\n";
    //  return -1;
    //}
    vcl_cout << "loading index from: " << bin_file_name << " using " << buffer_capacity_ << " GB buffer" << vcl_flush << vcl_endl;
    volm_conf_buffer<volm_conf_object> ind(buffer_capacity_);
    if (!ind.initialize_read(bin_file_name)) {
      vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: can not initialize reading index binary file: " << bin_file_name << "!\n";
      return -1;
    }
    // create an index buffer for writing score
    vcl_stringstream score_file_pre;
    score_file_pre << out_folder_ << "/conf_score_tile_" << tile_id_;
    vcl_string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity_);
    if (!score_idx.initialize_write(score_bin_file)) {
      vcl_cout << "In boxm2_volm_conf_matcher::conf_matcher: can not initialize writing score binary file: " << score_bin_file << "!\n";
      return -1;
    }
    // loop over each location inside leaf
    vgl_point_3d<double> h_pt;
    unsigned cnt = 0;
    while ( leaf->hyps_->get_next(0,1,h_pt))
    {
      if (cnt++/1000 == 0)  vcl_cout << '.' << vcl_flush;
      // get index from index database
      vcl_vector<volm_conf_object> values;
      if (!ind.get_next(values)) {
        vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: get index for location " << h_pt.x() << ", " << h_pt.y() << " failed!\n";
        return -1;
      }
      // check candidate region
      if (is_cand)
        if (!volm_candidate_list::inside_candidate_region(cand_inner, cand_outer, h_pt.x(), h_pt.y()))
          continue;
      volm_conf_score score;
      this->matching(values, score, use_height);
      score_idx.add_to_index(score);
#if 0
      vcl_cout << "location: " << vcl_setprecision(8) << h_pt.x() << ", " << vcl_setprecision(8) << h_pt.y() << "): ---> ";
      score.print(vcl_cout);
#endif
      matched_locs++;
    }
    // write out the score
    score_idx.finalize();
    vcl_cout << '\n';
  }
  return matched_locs;

#if 0
  // loop over each leaf to match
  unsigned matched_locs = 0;
  unsigned n_leaves = loc_leaves_.size();
  for (unsigned l_idx = 0; l_idx < n_leaves; l_idx++)
  {
    if (leaf_idx >= 0 && leaf_idx < loc_leaves_.size())
      if (l_idx != leaf_idx)
        continue;
    volm_geo_index_node_sptr leaf = loc_leaves_[l_idx];
    // create a index buffer for current leaf
    vcl_stringstream index_file_pre;
    index_file_pre << index_folder_ << "/conf_index_tile_" << tile_id_;
    vcl_string bin_file_name = index_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    if (!vul_file::exists(bin_file_name)) {
      vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: can not find index binary file " << bin_file_name << " for leaf " << leaf_idx << " and index name " << index_name << "!\n";
      return 0;
    }
    vcl_cout << "loading index from: " << bin_file_name << " using " << buffer_capacity_ << " GB buffer" << vcl_flush << vcl_endl;
    volm_conf_buffer<volm_conf_object> ind(buffer_capacity_);
    if (!ind.initialize_read(bin_file_name)) {
      vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: can not initialize reading index binary file: " << bin_file_name << "!\n";
      return 0;
    }
    // create a index buffer for writing score
    vcl_stringstream score_file_pre;
    score_file_pre << out_folder_ << "/conf_score_tile_" << tile_id_;
    vcl_string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity_);
    if (!score_idx.initialize_write(score_bin_file)) {
      vcl_cout << "In boxm2_volm_conf_matcher::conf_matcher: can not initialize writing score binary file: " << score_bin_file << "!\n";
      return 0;
    }
    // loop over each location inside leaf
    vgl_point_3d<double> h_pt;
    while ( leaf->hyps_->get_next(0, 1, h_pt) )
    {
      // get index from index database
      //vcl_cout << "matching location: " << h_pt.x() << ", " << h_pt.y() << " (tile " << tile_id_ << "!!!!!!!!!!!!!!!!!!!!!!!" << vcl_flush << vcl_endl;
      vcl_vector<volm_conf_object> values;
      if (!ind.get_next(values)) {
        vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: get index for location " << h_pt.x() << ", " << h_pt.y() << " failed!\n";
        return 0;
      }
      //vcl_cout << "index has size: " << values.size() << vcl_endl;
      // check whether the location is inside candidate region
#if 0
      // use both inner boundary and outer boundary
      if (is_cand_ && !volm_candidate_list::inside_candidate_region(cand_poly_in_, cand_poly_out_, h_pt.x(), h_pt.y()))
        continue;
#endif
      if (is_cand_ && !volm_candidate_list::inside_candidate_region(cand_poly_out_, h_pt.x(), h_pt.y()))
        continue;
      volm_conf_score score;
      this->matching(values, score, use_height);
      //vcl_cout << " score: "; score.print(vcl_cout);
      score_idx.add_to_index(score);
      matched_locs++;
    }
    // write out the score
    score_idx.finalize();
  } // end of loop over location leaves

  return matched_locs;
#endif
}

int boxm2_volm_conf_matcher::conf_match_cpp_no_candidate(vcl_string& index_name, bool const& use_height)
{
  // loop over each leaf to match
  unsigned matched_locs = 0;
  unsigned n_leaves = (unsigned)loc_leaves_.size();
  for (unsigned l_idx = 0; l_idx < n_leaves; l_idx++)
  {
    volm_geo_index_node_sptr leaf = loc_leaves_[l_idx];
    // create an index buffer for current leaf
    vcl_stringstream index_file_pre;
    index_file_pre << index_folder_ << "/conf_index_tile_" << tile_id_;
    vcl_string bin_file_name = index_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    //if (!vul_file::exists(bin_file_name)) {
    //  vcl_cerr << "in boxm2_volm_conf_matcher::conf_match_cpp_no_candidate: can not find index binary file " << bin_file_name << " for leaf " << l_idx << " and index name " << index_name << "!\n";
    //  return -1;
    //}
    vcl_cout << "loading index from: " << bin_file_name << " using " << buffer_capacity_ << " GB buffer" << vcl_flush << vcl_endl;
    volm_conf_buffer<volm_conf_object> ind(buffer_capacity_);
    if (!ind.initialize_read(bin_file_name)) {
      vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: can not initialize reading index binary file: " << bin_file_name << "!\n";
      return -1;
    }
    // create an index buffer for writing score
    vcl_stringstream score_file_pre;
    score_file_pre << out_folder_ << "/conf_score_tile_" << tile_id_;
    vcl_string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity_);
    if (!score_idx.initialize_write(score_bin_file)) {
      vcl_cout << "In boxm2_volm_conf_matcher::conf_matcher: can not initialize writing score binary file: " << score_bin_file << "!\n";
      return -1;
    }
    // loop over each location inside leaf
    vgl_point_3d<double> h_pt;
    unsigned cnt = 0;
    while ( leaf->hyps_->get_next(0,1,h_pt))
    {
      if (cnt++/1000 == 0)  vcl_cout << '.' << vcl_flush;
      // get index from index database
      vcl_vector<volm_conf_object> values;
      if (!ind.get_next(values)) {
        vcl_cerr << "In boxm2_volm_conf_matcher::conf_matcher: get index for location " << h_pt.x() << ", " << h_pt.y() << " failed!\n";
        return -1;
      }
      volm_conf_score score;
      this->matching(values, score, use_height);
      score_idx.add_to_index(score);
      matched_locs++;
    }
    // write out the score
    score_idx.finalize();
  }
  return matched_locs;
}

bool boxm2_volm_conf_matcher::matching(vcl_vector<volm_conf_object> const& values, volm_conf_score& score, bool const& use_height)
{
  // transfer index to a map indexed by land id
  vcl_map<unsigned char, vcl_vector<volm_conf_object> > index_map;
  for (unsigned i = 0; i < values.size(); i++) {
    unsigned char land_id = values[i].land();
    index_map[land_id].push_back(values[i]);
  }

  vcl_vector<vcl_string> query_ref_obj_name = query_->ref_obj_name();
  vcl_vector<vcl_map<vcl_string, volm_conf_object_sptr> > query_conf_objects = query_->conf_objects();
  vcl_vector<vcl_map<vcl_string, vcl_pair<float, float> > > query_dist_tol = query_->conf_objects_d_tol();
  unsigned ncams = query_->ncam();

  // loop over each camera query
  vcl_map<unsigned, vcl_pair<float, float> > score_cam_map;
  vcl_map<unsigned, vcl_vector<volm_conf_object> > ref_obj_cam_map;
  for (unsigned cam_id = 0; cam_id < ncams; cam_id++)
  {
    vcl_map<vcl_string, volm_conf_object_sptr> query_map = query_conf_objects[cam_id];
    if (query_map.size() == 0) {
      score_cam_map.insert(vcl_pair<unsigned, vcl_pair<float, float> >(cam_id, vcl_pair<float, float>(0.0f,0.0f)));
      vcl_vector<volm_conf_object> ref_objs;  ref_objs.clear();
      ref_obj_cam_map.insert(vcl_pair<unsigned, vcl_vector<volm_conf_object> >(cam_id, ref_objs));
      continue;
    }
    // obtain reference objects
    vcl_map<vcl_string, volm_conf_object_sptr> ref_objs;
    for (vcl_vector<vcl_string>::iterator vit = query_ref_obj_name.begin(); vit != query_ref_obj_name.end(); ++vit)
      if (query_map.find(*vit) != query_map.end())
        ref_objs.insert(vcl_pair<vcl_string, volm_conf_object_sptr>(*vit, query_map[*vit]));

    vcl_map<vcl_string, vcl_pair<float,float> > score_ref_map;
    vcl_map<vcl_string, vcl_vector<volm_conf_object> > object_ref_map;
    // loop over all reference objects
    for (vcl_map<vcl_string, volm_conf_object_sptr>::iterator mit = ref_objs.begin(); mit != ref_objs.end(); ++mit)
    {
      vcl_string ref_obj_name = mit->first;
      double min_dist = query_dist_tol[cam_id][ref_obj_name].first;
      double max_dist = query_dist_tol[cam_id][ref_obj_name].second;
      // get the reference object in query
      volm_conf_object_sptr q_ref = mit->second;
      vcl_vector<volm_conf_object_sptr> q_objs;
      // get the non-ref object in query
      for (vcl_map<vcl_string, volm_conf_object_sptr>::iterator q_mit = query_map.begin();  q_mit != query_map.end(); ++q_mit)
        if ( !q_ref->is_same(q_mit->second) )
          q_objs.push_back(q_mit->second);

      // locate all the reference points in index by checking whether its distance value is within the tolerated distance
      unsigned char ref_land_id = q_ref->land();
      //vcl_vector<volm_conf_object> i_ref_objs = index_map[ref_land_id];
      vcl_vector<volm_conf_object> i_ref_objs;
      for (vcl_vector<volm_conf_object>::iterator vit = index_map[ref_land_id].begin();  vit != index_map[ref_land_id].end(); ++vit) {
        if (use_height) {
          if (vit->dist() >= min_dist && vit->dist() <= max_dist)
            if (vit->height() >= q_ref->height()-10.0 && vit->height() <= q_ref->height()+10.0)
              i_ref_objs.push_back(*vit);
        }
        else {
          if ( vit->dist() >= min_dist && vit->dist() <= max_dist )
            i_ref_objs.push_back(*vit);
        }
        
      }

      // loop over each reference object in index to get a score for given reference object in index
      vcl_map<float, float, vcl_greater<float> > score_ref;
      vcl_map<float, vcl_vector<volm_conf_object>, vcl_greater<float> > score_matched_objs;
      //vcl_map<float, float> score_ref;
      for (vcl_vector<volm_conf_object>::iterator vit = i_ref_objs.begin();  vit != i_ref_objs.end(); ++vit) {
        float score;
        vcl_vector<volm_conf_object> matched_objs;
        this->match_to_reference_h(*vit, q_ref, q_objs, index_map, score, matched_objs, use_height);
        // add the reference object into matched objects
        matched_objs.push_back(*vit);
        score_ref.insert(vcl_pair<float, float>(score, vit->theta()));
        score_matched_objs.insert(vcl_pair<float, vcl_vector<volm_conf_object> >(score, matched_objs));
      }
      // find the best score given current query reference object and put it into camera
      float score_ref_i = 0.0f;
      float theta_ref_i = 0.0f;
      vcl_vector<volm_conf_object> matched_objs_ref_i;
      if (!score_ref.empty()) {
        score_ref_i = score_ref.begin()->first;
        theta_ref_i = score_ref.begin()->second;
        matched_objs_ref_i = score_matched_objs[score_ref_i];
      }
      score_ref_map.insert(vcl_pair<vcl_string, vcl_pair<float, float> >(mit->first, vcl_pair<float, float>(score_ref_i, theta_ref_i)));
      object_ref_map.insert(vcl_pair<vcl_string, vcl_vector<volm_conf_object> >(mit->first,matched_objs_ref_i));
    }  // end of loop over reference ids

    // obtain the maximum score for current camera
    float max_score_cam = 0.0f, max_theta_cam = 0.0f;
    vcl_vector<volm_conf_object> max_matched_objs;
    for (vcl_map<vcl_string, vcl_pair<float,float> >::iterator mit = score_ref_map.begin(); mit != score_ref_map.end(); ++mit) {
      if (mit->second.first > max_score_cam) {
        max_score_cam = mit->second.first;  max_theta_cam = mit->second.second;
        max_matched_objs = object_ref_map[mit->first];
      }
    }
    score_cam_map.insert(vcl_pair<unsigned, vcl_pair<float, float> >(cam_id, vcl_pair<float, float>(max_score_cam, max_theta_cam)));
    ref_obj_cam_map.insert(vcl_pair<unsigned, vcl_vector<volm_conf_object> >(cam_id, max_matched_objs));
  } // end of loop over query cameras

  // obtain the maximum score from all cameras
  float max_score = 0.0f;
  float max_theta = 0.0f;
  vcl_vector<volm_conf_object> best_matched_objs;
  for (vcl_map<unsigned, vcl_pair<float, float> >::iterator mit = score_cam_map.begin(); mit != score_cam_map.end(); ++mit)
  {
    if (mit->second.first > max_score) {
      max_score = mit->second.first;  max_theta = mit->second.second;
      best_matched_objs = ref_obj_cam_map[mit->first];
    }
  }
  score.set_score(max_score);
  score.set_theta(max_theta);
  score.set_landmarks(best_matched_objs);
  return true;
}

#if 0
// match query to a index with one reference point in index
void boxm2_volm_conf_matcher::match_to_reference(volm_conf_object const& ref_i, volm_conf_object_sptr const& ref_q,
                                                 vcl_vector<volm_conf_object_sptr> const& obj_q,
                                                 vcl_map<unsigned char, vcl_vector<volm_conf_object> >& obj_map_i,
                                                 float& score,
                                                 vcl_vector<volm_conf_object>& matched_objs)
{
  score = 0.0f;
  float ref_dist_i = ref_i.dist();
  float ref_dist_q = ref_q->dist();
  float ref_theta_i = ref_i.theta();
  float ref_theta_q = ref_q->theta();
  unsigned num_obj = obj_q.size();

  vcl_vector<float> obj_score;
  for (unsigned j = 0; j < obj_q.size(); j++)
  {
    // compute the configuration from query
    float dist_q = obj_q[j]->dist();  float theta_q = obj_q[j]->theta();  unsigned char land_q = obj_q[j]->land();
    float ratio_q   = dist_q / ref_dist_q;
    float delta_t_q = theta_q - ref_theta_q;

    // check the distance ratio and angular difference in index -- calculate the score of object j relative to current reference object
    vcl_vector<volm_conf_object> objs_i = obj_map_i[land_q];
    float max_score = 0.0f;
    // loop over all objects having land_id land_q in index to get the best match of object j
    for (vcl_vector<volm_conf_object>::iterator vit = objs_i.begin(); vit != objs_i.end(); ++vit)
    {
      float dist_i = vit->dist();  float theta_i = vit->theta();
      float ratio_i   = dist_i / ref_dist_i;
      float delta_t_i = theta_i - ref_theta_i;
      // compute score
      float score_dist = 0.0f;
      float score_theta = 0.0f;
      if (ratio_i <= ratio_q)  score_dist = ratio_i/ratio_q;
      else                     score_dist = ratio_q/ratio_i;
      if ( vcl_fabs(delta_t_i) <= vcl_fabs(delta_t_q) ) {
        if ( vcl_fabs(delta_t_i) < 0.1 && vcl_fabs(delta_t_q) < 0.1)
          score_theta = vcl_fabs(delta_t_i) / vcl_fabs(delta_t_q);
        else
          score_theta = delta_t_i / delta_t_q;
      }
      else {
        if ( vcl_fabs(delta_t_i) < 0.1 && vcl_fabs(delta_t_q) < 0.1)
          score_theta = vcl_fabs(delta_t_q) / vcl_fabs(delta_t_i);
        else
          score_theta = delta_t_q / delta_t_i;
      }
      float total_score = 0.4*score_dist + 0.6*score_theta;
      if (total_score > max_score)
        max_score = total_score;
    }
    // record the score
    obj_score.push_back(max_score);
  }
  // obtain the overall score for configurational matching based on reference object ref_q and ref_i
  for (vcl_vector<float>::iterator vit = obj_score.begin(); vit != obj_score.end(); ++vit)
    score += *vit;
  score /= obj_score.size();
  return;
}
#endif

void boxm2_volm_conf_matcher::match_to_reference_h(volm_conf_object const& ref_i, volm_conf_object_sptr const& ref_q,
                                                   vcl_vector<volm_conf_object_sptr> const& obj_q,
                                                   vcl_map<unsigned char, vcl_vector<volm_conf_object> >& obj_map_i,
                                                   float& score,
                                                   vcl_vector<volm_conf_object>& matched_objs,
                                                   bool const& use_height)
{
  matched_objs.clear();
  score = 0.0f;
  float ref_dist_i = ref_i.dist();
  float ref_dist_q = ref_q->dist();
  float ref_theta_i = ref_i.theta();
  float ref_theta_q = ref_q->theta();
  vcl_vector<float> obj_score;
  for (unsigned j = 0; j < obj_q.size(); j++)
  {
    // get a score for each query object
    float dist_q = obj_q[j]->dist();  float theta_q = obj_q[j]->theta();  float height_q = obj_q[j]->height();
    unsigned char land_q = obj_q[j]->land();
    float ratio_d_q = dist_q / ref_dist_q;
    float delta_t_q = theta_q - ref_theta_q;
    vcl_vector<volm_conf_object> objs_i = obj_map_i[land_q];
    float max_score = 0.0f;
    volm_conf_object best_objs(0.0f, 0.0f, -1.0f, 0);
    unsigned cnt = 0;
    for (vcl_vector<volm_conf_object>::iterator vit = objs_i.begin(); vit != objs_i.end(); ++vit)
    {
      float dist_i = vit->dist();  float theta_i = vit->theta();  float height_i = vit->height();
      float ratio_d_i = dist_i / ref_dist_i;
      float delta_t_i = theta_i - ref_theta_i;
      // compute distance score
      float score_d = 0.0f, score_t = 0.0f, score_h = -1.0f;
      if (ratio_d_i <= ratio_d_q)  score_d = ratio_d_i / ratio_d_q;
      else                         score_d = ratio_d_q / ratio_d_i;
      if ( vcl_fabs(delta_t_i) <= vcl_fabs(delta_t_q) ) {
        if ( vcl_fabs(delta_t_i) < 0.1 && vcl_fabs(delta_t_q) < 0.1)
          score_t = vcl_fabs(delta_t_i) / vcl_fabs(delta_t_q);
        else
          score_t = delta_t_i / delta_t_q;
      }
      else {
        if ( vcl_fabs(delta_t_i) < 0.1 && vcl_fabs(delta_t_q) < 0.1)
          score_t = vcl_fabs(delta_t_q) / vcl_fabs(delta_t_i);
        else
          score_t = delta_t_q / delta_t_i;
      }
      if (volm_osm_category_io::volm_land_table[land_q].name_ == "building" || volm_osm_category_io::volm_land_table[land_q].name_ == "tall_building") {
        if (height_i > 0 && height_q > 0) {
          if (height_i <= height_q)  score_h = height_i / height_q;
          else                       score_h = height_q / height_i;
        }
      }
      float total_score = 0.0f;
      if (score_h < 0 || !use_height)  // consider only distance score and angular score
        total_score = 0.4f*score_d + 0.6f*score_t;
      else
        total_score = 0.3f*score_d + 0.3f*score_t + 0.4f*score_h;
      if (total_score > max_score) {
        max_score = total_score;
        best_objs = *vit;
      }
#if 0
      vcl_cout << "qobj: " << j << ", ind " << cnt++ << ": " << dist_i << ", " << theta_i << ", " << height_i
               << " --> d_t_i: " << delta_t_i << ", d_t_q: " << delta_t_q << ", s_t: " << score_t
               << " r_i: " << ratio_d_i << ", r_q: " << ratio_d_q << ", s_d: " << score_d
               << " s_h: " << score_h << " total score: " << total_score << ", max_score " << max_score << vcl_endl;
#endif
    }
    // record the score
    obj_score.push_back(max_score);
    // record the best matched locations
    matched_objs.push_back(best_objs);
  } // loop over all query object

  // obtain the total score for configurational matching based on reference object ref_q and ref_i
  for (vcl_vector<float>::iterator vit = obj_score.begin();  vit != obj_score.end(); ++vit)
    score += *vit;
  score /= obj_score.size();
  return;
}