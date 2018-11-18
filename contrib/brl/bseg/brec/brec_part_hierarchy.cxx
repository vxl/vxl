//:
// \file
// \author Ozge C. Ozcanli (ozge at lems dot brown dot edu)
// \date Oct. 16, 2008
//

#include "brec_part_hierarchy.h"
#include "brec_part_base.h"
#include "brec_part_gaussian_sptr.h"
#include "brec_part_gaussian.h"

#include <vil/vil_image_view.h>
#include <vul/vul_psfile.h>

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

//: generate a map from the activated parts
void brec_part_hierarchy::generate_map(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, vil_image_view<unsigned>& type_map)
{
  unsigned ni = map.ni();
  unsigned nj = map.nj();
  map.fill(0.0f);
  type_map.fill(0);
  for (const auto& p : extracted_parts) {
    auto ii = (unsigned)p->x_;
    auto jj = (unsigned)p->y_;
    if (ii > 0 && ii < ni && jj > 0 && jj < nj) {
      map(ii, jj, 0) = p->strength_;
      type_map(ii, jj, 0) = p->type_;
    }
  }
}

void brec_part_hierarchy::generate_map(std::vector<brec_part_instance_sptr>& extracted_parts, std::vector<std::vector<brec_part_instance_sptr> >& map)
{
  unsigned ni = map.size();
  unsigned nj = map[0].size();
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      map[i][j] = nullptr;

  for (const auto& p : extracted_parts) {
    auto ii = (unsigned)p->x_;
    auto jj = (unsigned)p->y_;
    if (ii > 0 && ii < ni && jj > 0 && jj < nj) {
      map[ii][jj] = p;
    }
  }
}

//: generate a float map with normalized strengths and receptive fields marked
void
brec_part_hierarchy::generate_output_map(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map)
{
  map.fill(0.0f);

  float max = -1e38f; // min float
  for (auto & extracted_part : extracted_parts) {
    if (extracted_part->strength_ > max)
      max = extracted_part->strength_;
  }

  for (const auto& p : extracted_parts) {
    p->mark_receptive_field(map, p->strength_/max);
  }
}

//: generate a float map with various posterior's
void
brec_part_hierarchy::generate_output_map_posterior(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, unsigned type)
{
  map.fill(0.0f);
  switch (type) {
    case brec_posterior_types::CLASS_FOREGROUND: {
      for (const auto& p : extracted_parts) {
        p->mark_receptive_field(map, float(p->rho_c_f_));
      }
      break;
                                                 }
    case brec_posterior_types::CLASS_BACKGROUND: {
      for (const auto& p : extracted_parts) {
        p->mark_receptive_field(map, float(p->rho_c_b_));
      }
      break;
                                                 }
    case brec_posterior_types::NON_CLASS_FOREGROUND: {
      for (const auto& p : extracted_parts) {
        p->mark_receptive_field(map, float(p->rho_nc_f_));
      }
      break;
                                                     }
    case brec_posterior_types::NON_CLASS_BACKGROUND: {
      for (const auto& p : extracted_parts) {
        p->mark_receptive_field(map, float(p->rho_nc_b_));
      }
      break;
                                                     }
    default: { std::cout << "ERROR: In brec_part_hierarchy::generate_output_map_posterior() - brec_posterior_type " << type << " not defined!\n";
             }
  }
}

void
brec_part_hierarchy::generate_output_map_posterior_centers(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, unsigned type)
{
  map.fill(0.0f);
  switch (type)
  {
    case brec_posterior_types::CLASS_FOREGROUND:
      for (const auto& p : extracted_parts) {
        p->mark_center(map, float(p->rho_c_f_));
      }
      break;
    case brec_posterior_types::CLASS_BACKGROUND:
      for (const auto& p : extracted_parts) {
        p->mark_center(map, float(p->rho_c_b_));
      }
      break;
    case brec_posterior_types::NON_CLASS_FOREGROUND:
      for (const auto& p : extracted_parts) {
        p->mark_center(map, float(p->rho_nc_f_));
      }
      break;
    case brec_posterior_types::NON_CLASS_BACKGROUND:
      for (const auto& p : extracted_parts) {
        p->mark_center(map, float(p->rho_nc_b_));
      }
      break;
    default:
      std::cout << "ERROR: In brec_part_hierarchy::generate_output_map_posterior() - brec_posterior_type " << type << " not defined!\n";
  }
}


//: generate a float map with strengths and receptive fields marked
//  Stretch the values to be used for imaging
void
brec_part_hierarchy::generate_output_map3(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map)
{
  map.fill(0.0f);

  // find the mean value and stretch the values onto [0, mean];
  float mean = 0.0f;
#if 0
  for (unsigned i = 0; i < extracted_parts.size(); ++i) {
    brec_part_instance_sptr p = extracted_parts[i];
    mean += p->strength_;
  }
  mean /= extracted_parts.size();
#endif // 0
  mean = 0.00000005f; // we want to see all the detections, this value is the smallest threshold used to create the ROC

  for (const auto& p : extracted_parts) {
    if (p->strength_ > mean)
      p->mark_receptive_field(map, 1.0f);
    else
      p->mark_receptive_field(map, p->strength_/mean);
  }
}

//: output_img needs to have 3 planes
void
brec_part_hierarchy::generate_output_img(std::vector<brec_part_instance_sptr>& extracted_parts,
                                         vil_image_view<vxl_byte>& input_img,
                                         vil_image_view<vxl_byte>& output_img, unsigned posterior_type)
{
  unsigned ni = input_img.ni();
  unsigned nj = input_img.nj();
  output_img.fill(0);

  vil_image_view<float> map(ni, nj), map_center(ni, nj);
  generate_output_map_posterior(extracted_parts, map, posterior_type);
  generate_output_map_posterior_centers(extracted_parts, map_center, posterior_type);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      output_img(i,j,0) = input_img(i,j);  // original grey scale intensity
      output_img(i,j,1) = (vxl_byte)(map_center(i,j)*255);
      output_img(i,j,2) = (vxl_byte)(map(i,j)*255);
    }
}

brec_part_base_sptr brec_part_hierarchy::get_node(unsigned layer, unsigned type)
{
  for (auto it = this->vertices_begin(); it != this->vertices_end(); it++) {
    if ((*it)->layer_ == layer && (*it)->type_ == type)
      return *it;
  }
  return nullptr;
}

brec_part_instance_sptr brec_part_hierarchy::get_node_instance(unsigned layer, unsigned type)
{
  if (layer != 0)
    return nullptr;
  for (auto & dummy_primitive_instance : dummy_primitive_instances_) {
    if (dummy_primitive_instance->type_ == type && dummy_primitive_instance->layer_ == layer) {
      return dummy_primitive_instance;
    }
  }
  return nullptr;
}

//: draw the nodes of the given layer side by side to the output image
// \todo not implemented yet
bool brec_part_hierarchy::draw_to_image(unsigned /*N*/, unsigned layer_id, float /*drawing_radius*/, const std::string& /*output_img*/)
{
  std::vector<brec_part_instance_sptr> ins_to_draw;
  for (auto it = this->vertices_begin(); it != this->vertices_end(); it++) {
    if ((*it)->layer_ == layer_id)
      break;
  }

  return false; // not implemented - TODO
}

unsigned brec_part_hierarchy::highest_layer_id()
{
  unsigned highest = 0;
  for (auto it = this->vertices_begin(); it != this->vertices_end(); it++) {
    if ((*it)->layer_ > highest)
      highest = (*it)->layer_;
  }
  return highest;
}

//: number of vertices in the layer
unsigned brec_part_hierarchy::layer_cnt(unsigned layer)
{
  unsigned cnt = 0;
  for (auto it = this->vertices_begin(); it != this->vertices_end(); it++) {
    if ((*it)->layer_ == layer)
      cnt++;
  }
  return cnt;
}

bool draw_instance_to_ps(vul_psfile& ps, const brec_part_instance_sptr& pi, float x, float y, float cr, float cg, float cb)
{
  // now draw the central part in red
  switch (pi->kind_) {
    case brec_part_instance_kind::GAUSSIAN: { draw_gauss_to_ps(ps, pi->cast_to_gaussian(), x, y, cr, cg, cb); break;
                                            }
    default: { return false; }
  }
  return true;
}

bool draw_part_to_ps(vul_psfile& ps, const brec_part_base_sptr& p, float x, float y, float cr, float cg, float cb, unsigned N, brec_part_hierarchy* ph)
{
  if (p->layer_ == 0) {
    brec_part_instance_sptr pi = ph->get_node_instance(0, p->type_);
    draw_instance_to_ps(ps, pi, x, y, cr, cg, cb);
    return true;
  }

  vnl_random rng;
  // draw the central part in red
  brec_part_base_sptr pc = p->edge_to_central_part()->target();
  draw_part_to_ps(ps, pc, x, y, 1.0f, 0.0f, 0.0f, 1, ph);
  // find the central part instance
  while (pc->layer_ != 0)
    pc = pc->edge_to_central_part()->target();
  brec_part_instance_sptr pci = ph->get_node_instance(0, pc->type_);
  // fix the angle, cause ps coordinate system origin is different from image coordinate system
  brec_part_gaussian_sptr dummy = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -pci->cast_to_gaussian()->theta_, true, pci->type_);

  // now sample from the other parts
  auto eit = p->out_edges_begin();
  eit++; // skip the central part
  for ( ; eit != p->out_edges_end(); eit++) {
    brec_part_base_sptr pt = (*eit)->target();
    if (N == 1) {  // draw the mean
      vnl_vector_fixed<float, 2> pos_v = (*eit)->mean_position(dummy->cast_to_instance(), x, y);
      draw_part_to_ps(ps, pt, pos_v[0], pos_v[1], 0.0f, 0.0f, 1.0f, 1, ph);
    }
    else {
      for (unsigned i = 0; i < N; i++) {
        //vnl_vector_fixed<float, 2> pos_v = (*eit)->sample_position(pci, y, x, rng);
        //draw_part_to_ps(ps, pt, pos_v[1], pos_v[0], 0.0f, 0.0f, 1.0f, 1, ph);
        vnl_vector_fixed<float, 2> pos_v = (*eit)->sample_position(dummy->cast_to_instance(), x, y, rng);
        draw_part_to_ps(ps, pt, pos_v[0], pos_v[1], 0.0f, 0.0f, 1.0f, 1, ph);
      }
    }
  }

  return true;
}

//: generate sampled parts, draw N samples from the distributions
//  Draw a ps image with sampled parts,
//  Assumes parts double in size as one goes in hierarchy, radius is the radius of a part in primitive layer
bool brec_part_hierarchy::draw_to_ps(unsigned N, const std::string& output_img, float radius)
{
  vul_psfile f(output_img.c_str(),true);
  //f.set_paper_layout(vul_psfile::CENTER);

  // Draw a horizontal red line of length 300
  f.set_fg_color(1.f,0.f,0.f);
  f.set_line_width(0.2f);
  f.line(0.f, 10.f, 300.f, 10.f); // (x1,y1,x2,y2)

  // draw each layer
  unsigned hl = highest_layer_id();

  // draw the primitives
  float y = 5.0f + radius;
  for (unsigned i = 0; i < dummy_primitive_instances_.size(); i++) {
    float x = y + float(i*2)*radius;
    draw_instance_to_ps(f, dummy_primitive_instances_[i], x, y, 0.0f, 1.0f, 0.0f);
  }

  for (unsigned l = 1; l <= hl; ++l) {
    float rad_l = float(l+1)*radius;
    y += float(2*l+1)*radius;

    unsigned cnt = 0;
    for (auto it = this->vertices_begin(); it != this->vertices_end(); it++) {
      if ((*it)->layer_ == l) {
        float x = 5.0f + float(cnt*2+1)*rad_l;
        brec_part_base_sptr p = (*it);
        draw_part_to_ps(f, p, x, y, 0.0f, 1.0f, 0.0f, N, this);
        cnt++;
      }
    }
  }

  // Draw a horizontal red line of length 300
  f.set_fg_color(1.f,0.f,0.f);
  f.set_line_width(0.2f);
  f.line(0.0f, -(y + float(2*hl+1)*radius), 300.f, -(y + float(2*hl+1)*radius)); // (x1,y1,x2,y2)

  f.close();
  return true;
}


// check for existence of upper_p with central_p as its central part and map will tell if all the other parts exist
// map is the activation map of the parts at the layer of central_p
brec_part_instance_sptr brec_part_hierarchy::exists(const brec_part_base_sptr& upper_p,
                                                    const brec_part_instance_sptr& central_p,
                                                    vil_image_view<float>& map,
                                                    vil_image_view<unsigned>& type_map,
                                                    std::vector<std::vector<brec_part_instance_sptr> >& part_map,
                                                    float det_threshold)
{
  unsigned ni = map.ni();
  unsigned nj = map.nj();

  // first check if types and layers of central_p instance matches with upper_p's info
  if (upper_p->central_part()->type_ != central_p->type_ || upper_p->layer_ != central_p->layer_ + 1) {
    std::cout << "central_p instance passed is not compatible with the upper layer part passes\n";
    return nullptr;
  }

  brec_part_instance_sptr pi = new brec_part_instance(upper_p->layer_, upper_p->type_, brec_part_instance_kind::COMPOSED, central_p->x_, central_p->y_, 0.0f);
  brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(pi->cast_to_base(), central_p->cast_to_base(), true);
  pi->add_outgoing_edge(e1);

  // now for each other part of upper_p, check whether they exist in the map
  float cx = central_p->x_; float cy = central_p->y_;
  auto eit = upper_p->out_edges_begin();
  eit++;  // skip the central part
  double strength = 1.0;
  for ( ; eit != upper_p->out_edges_end(); eit++)
  {
    //int mx = (int)std::floor(cx+(*eit)->mean().get(0)+0.5);
    //int my = (int)std::floor(cy+(*eit)->mean().get(1)+0.5);
    //int rad = (int)std::ceil(std::sqrt((*eit)->var())+3);
    int mx = (int)std::floor(cx+0.5);  // try all locations around center within dist+variance_dist radius
    int my = (int)std::floor(cy+0.5);
    int rad = (int)std::ceil((*eit)->mean_dist()+std::sqrt((*eit)->var_dist())+3);
    int si = mx - rad;  si = si < 0 ? 0 : si;
    int upper_i = mx + rad; upper_i = upper_i > (int)ni ? ni : upper_i;
    int sj = my - rad;  sj = sj < 0 ? 0 : sj;
    int upper_j = my + rad; upper_j = upper_j > (int)nj ? nj : upper_j;
    double best_fit = 0.0;
    double best_fit_str = 1.0;
    brec_part_instance_sptr best_part;
    for (int i = si ; i < upper_i; i++) {
      for (int j = sj ; j < upper_j; j++) {
        if (map(i, j) > det_threshold && type_map(i, j) == (*eit)->target()->type_) {
          vnl_vector_fixed<float, 2> v((float)i-cx, (float)j-cy);
          float dist, angle;
          (*eit)->calculate_dist_angle(central_p, v, dist, angle);
          double str = (*eit)->prob_density(dist, angle);
          if (str < det_threshold)
            continue;
          if (best_fit < str) {
            best_fit = str;
            best_fit_str = map(i,j);
            best_part = part_map[i][j];
          }
        }
      }
    }
    if (best_fit <= 0)
      return nullptr;  // this sub-part not found
    strength *= best_fit*best_fit_str;
    if (best_part) {
      brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(pi->cast_to_base(), best_part->cast_to_base(), false);
      pi->add_outgoing_edge(e2);
    }
  }
  strength *= central_p->strength_;

  // if all of them have been detected then declare existence at the central parts location
  pi->strength_ = (float)strength;

  return pi;
}

//: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
void brec_part_hierarchy::extract_upper_layer(std::vector<brec_part_instance_sptr>& extracted_parts,
                                              unsigned ni, unsigned nj,
                                              std::vector<brec_part_instance_sptr>& extracted_upper_parts)
{
  vil_image_view<float> map(ni, nj, 1);  // the second plane will hold the types of the primitives
  vil_image_view<unsigned> type_map(ni, nj, 1);  // the second plane will hold the types of the primitives
  generate_map(extracted_parts, map, type_map);

  std::vector<brec_part_instance_sptr> dummy(nj, nullptr);
  std::vector<std::vector<brec_part_instance_sptr> > part_map(ni, dummy);
  generate_map(extracted_parts, part_map);

  // we'll generate a list of instantiations for each part in the upper parts of the hierarchy
  //std::map<std::pair<unsigned, unsigned>, std::vector<brec_part_instance_sptr> > instantiations;

  // for each detected part, check for the existence of each upper layer part that uses it as a central part
  for (const auto& p : extracted_parts)
  {
    // find this type in the primitive layer of the hierarchy
    brec_part_base_sptr hp = this->get_node(p->layer_, p->type_);
    if (!hp)
      continue;

    // find the all the upper layer parts that use hp as a central part
    // check the incoming edges of hp
    for (auto eit = hp->in_edges_begin(); eit != hp->in_edges_end(); eit++) {
      if (hp == (*eit)->source()->central_part()) {
        brec_part_base_sptr hp_upper = (*eit)->source();

        // now check for the existence of other primitives wrt to the central part and initiate an instance of it if so
        brec_part_instance_sptr hp_upper_instance = exists(hp_upper, p, map, type_map, part_map, hp_upper->detection_threshold_); // p will be its central part and map will tell if all the other parts exist
        if (!hp_upper_instance)
          continue;
        if (hp_upper_instance->strength_ > hp_upper->detection_threshold_)
          extracted_upper_parts.push_back(hp_upper_instance);
      }
    }
  }
}

//: reader adds the edges of each vertex in the order read from the xml file
//  CAUTION: assumes that central part is added prior to other children of a parent part hence its edge is added to the hierarchy before other parts
void brec_part_hierarchy::write_xml(std::ostream& os)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("hierarchy");
  doc.set_root_element(root);
  root->append_text("\n");
  root->set_attribute("name", name_);
  root->set_attribute("model_dir", model_dir_);

  bxml_element * prims = new bxml_element("primitive_instances");
  prims->append_text("\n");
  root->append_data(prims);
  root->append_text("\n");
  for (auto & dummy_primitive_instance : dummy_primitive_instances_) {
    bxml_data_sptr ins = dummy_primitive_instance->xml_element();
    prims->append_text("  ");
    prims->append_data(ins);
    prims->append_text("\n");
  }

  bxml_element *vertices = new bxml_element("vertices");
  vertices->append_text("\n");
  root->append_data(vertices);
  root->append_text("\n");

  std::map<brec_part_base_sptr, unsigned> vert_map;
  unsigned id = 0;
  for (auto it = this->vertices_begin(); it != this->vertices_end(); it++) {
    bxml_data_sptr v = (*it)->xml_element();
    vertices->append_text("  ");
    vertices->append_data(v);
    vertices->append_text("\n");
    vert_map[(*it)] = id;
    id++;
  }

  bxml_element *edges = new bxml_element("edges");
  edges->append_text("\n");
  root->append_data(edges);
  root->append_text("\n");

  for (auto it = this->edges_begin(); it != this->edges_end(); it++) {
    bxml_data_sptr e = (*it)->xml_element();
    edges->append_text("  ");
    edges->append_data(e);
    edges->append_text("\n");

    bxml_element *e_con = new bxml_element("connectivity");
    e_con->set_attribute("source", vert_map[(*it)->source()]);
    e_con->set_attribute("target", vert_map[(*it)->target()]);
    e_con->append_text("\n");
    ((bxml_element*)e.ptr())->append_data(e_con);
    ((bxml_element*)e.ptr())->append_text("\n");
  }

  bxml_write(os,doc);
}

bool brec_part_hierarchy::read_xml(std::istream& is)
{
  bxml_document doc = bxml_read(is);
  bxml_element query("hierarchy");

  bxml_data_sptr hierarchy_root = bxml_find_by_name(doc.root_element(), query);

  if (!hierarchy_root) {
    std::cout << "brec_part_hierarchy::read_xml() - could not find the main node with name hierarchy!\n";
    return false;
  }

  auto* re = (bxml_element*)hierarchy_root.ptr();
  re->get_attribute("name", name_);
  std::cout << "reading hierarchy with name: " << name_ << std::endl;
  re->get_attribute("model_dir", model_dir_);

  bxml_element query2("primitive_instances");
  bxml_data_sptr prims_root = bxml_find_by_name(hierarchy_root, query2);

  if (!prims_root || prims_root->type() != bxml_data::ELEMENT) {
    std::cout << "brec_part_hierarchy::read_xml() - could not find the primitive instances node!\n";
    return false;
  }

  auto* pe = (bxml_element*)prims_root.ptr();

  for (auto it = pe->data_begin(); it != pe->data_end(); it++)
  {
    if ((*it)->type() != bxml_data::ELEMENT)
      continue;

    brec_part_instance_sptr ins = new brec_part_instance();
    if (!ins->xml_parse_element(*it))
      return false;

    switch (ins->kind_) {
      case brec_part_instance_kind::GAUSSIAN :
      {
        brec_part_gaussian_sptr g_p = new brec_part_gaussian();
        g_p->xml_parse_element(*it);
        dummy_primitive_instances_.emplace_back(g_p->cast_to_instance());
        break;
      }
      default: {
        std::cout << "brec_part_hierarchy::read_xml() - primitive part kind: " << ins->kind_ << " not recognized by the parser!\n";
        return false;
      }
    }
  }

  bxml_element query3("vertices");
  bxml_data_sptr vert_root = bxml_find_by_name(hierarchy_root, query3);
  if (!vert_root || vert_root->type() != bxml_data::ELEMENT) {
    std::cout << "brec_part_hierarchy::read_xml() - could not find the vertices node!\n";
    return false;
  }

  std::map<unsigned, brec_part_base_sptr> vert_map;
  unsigned id = 0;
  pe = (bxml_element*)vert_root.ptr();
  for (auto it = pe->data_begin(); it != pe->data_end(); it++) {
    if ((*it)->type() != bxml_data::ELEMENT)
      continue;

    brec_part_base_sptr p = new brec_part_base();
    if (!p->xml_parse_element(*it))
      return false;

    vert_map[id] = p;
    this->add_vertex(p);
    id++;
  }

  bxml_element query4("edges");
  bxml_data_sptr e_root = bxml_find_by_name(hierarchy_root, query4);
  if (!e_root || e_root->type() != bxml_data::ELEMENT) {
    std::cout << "brec_part_hierarchy::read_xml() - could not find the edges node!\n";
    return false;
  }

  pe = (bxml_element*)e_root.ptr();
  for (auto it = pe->data_begin(); it != pe->data_end(); it++)
  {
    if ((*it)->type() != bxml_data::ELEMENT)
      continue;

    brec_hierarchy_edge_sptr e = new brec_hierarchy_edge();
    if (!e->xml_parse_element(*it))
      return false;

    bxml_element query("connectivity");
    bxml_data_sptr r = bxml_find_by_name((*it), query);
    if (!r || r->type() != bxml_data::ELEMENT) {
      std::cout << "brec_part_hierarchy::read_xml() - could not find the edge node: connectivity!\n";
      return false;
    }

    unsigned source_id, target_id;
    ((bxml_element*)(r.ptr()))->get_attribute("source", source_id);
    ((bxml_element*)(r.ptr()))->get_attribute("target", target_id);
    e->set_source(vert_map[source_id]);
    e->set_target(vert_map[target_id]);
    vert_map[source_id]->add_outgoing_edge(e);
    vert_map[target_id]->add_incoming_edge(e);
    this->add_edge_no_check(e);
  }

  return true;
}

#if 0 // Ozge TODO: adapt to different posterior types
bool brec_part_hierarchy::get_score_helper(brec_part_instance_sptr ins_p, brec_part_base_sptr p, double& score)
{
  if (ins_p->kind_ == brec_part_instance_kind::COMPOSED)
  {
    if (ins_p->layer_ != p->layer_)
      return false;

    // first verify and get scores from all parts
    if (ins_p->out_degree() != p->out_degree())
      return false;

    edge_iterator i_e_it = ins_p->out_edges_begin();
    edge_iterator e_it = p->out_edges_begin();
    std::vector<double> part_scores;
    for ( ; i_e_it != ins_p->out_edges_end(); i_e_it++, e_it++) {
      double e_s;
      if (!get_score_helper((*i_e_it)->target()->cast_to_instance(), (*e_it)->target(), e_s))
        return false;
      part_scores.push_back(e_s);
    }

    // all parts verified
    brec_part_instance_sptr central_p = ins_p->central_part()->cast_to_instance();

    i_e_it = ins_p->out_edges_begin();
    e_it = p->out_edges_begin();
    i_e_it++; e_it++; // skip central
    double cx = central_p->x_; double cy = central_p->y_;
    unsigned cnt = 1;
    score = 1.0;
    for ( ; i_e_it != ins_p->out_edges_end(); i_e_it++, e_it++) {
      double e_s = part_scores[cnt];
      cnt++;
      // calculate the edge score
      brec_part_instance_sptr found = (*i_e_it)->target()->cast_to_instance();
      vnl_vector_fixed<float, 2> v(float(found->x_-cx), float(found->y_-cy));
      float dist, angle;
      (*e_it)->calculate_dist_angle(central_p, v, dist, angle);
      score *= (*e_it)->prob_density(dist, angle)*e_s;
    }
    //score *= part_scores[0]*p->prior_prob_;
    score *= part_scores[0];
    return true;
  }
  else {
    if (ins_p->type_ == p->type_) {
      score = ins_p->rho_;
      return true;
    }
  }

  return false;
}

//: compute the probabilistic existence score for a given part (upper_p) if this one or equivalent ones exist in this hierarchy
//  Equivalence is determined by the types of the primitive layer parts
//  (i.e. all layer 1 parts with alpha and alpha_prime as primitive parts are equivalent)
bool brec_part_hierarchy::get_score(brec_part_instance_sptr upper_p, std::vector<double>& scores)
{
  //  this upper part with the same primitive set may not even exist in this hierarchy, first try to find a similar part,
  //  assume upper_part's layer will be the same as its counter-parts layers in this hierarchy
  //  assume when we hit a primitive central part, we hit layer 0 and our upper part is layer 1
  for (vertex_iterator v_it = this->vertices_begin(); v_it != this->vertices_end(); v_it++) {
    if ((*v_it)->layer_ != upper_p->layer_)
      continue;

    double s;
    if (!get_score_helper(upper_p, (*v_it), s))
      continue;
    scores.push_back(s);
  }
  if (!scores.size())
    return false;

  return true;
}
#endif // 0

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, brec_part_hierarchy const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, brec_part_hierarchy & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new brec_part_hierarchy();
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
