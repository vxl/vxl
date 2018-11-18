//:
// \file
// \author Isabel Restrepo
// \date 21-Jul-2011

#include <iostream>
#include <fstream>
#include "bvpl_global_corners.h"

#include <bsta/bsta_histogram.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>

#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>
#include <bvpl_octree/bvpl_corner_detector.h>
#include <bvpl_octree/bvpl_corner_functors.h>

#include <vul/vul_file.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Constructor  from xml file
bvpl_global_corners::bvpl_global_corners(const std::string &path)
{
  std::cout << "Loading corners info from xml-file" << std::endl;
  path_out_ = path;
  std::ifstream xml_ifs(xml_path().c_str());
  if (!xml_ifs.is_open()){
    std::cerr << "Error: could not open xml info file: " << xml_path() << '\n';
    throw;
  }
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("global_corners_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    std::cerr << "Error: bvpl_discover_pca_kernels - could not parse xml root\n";
    throw;
  }

  //Parse scenes
  bxml_element scenes_query("scene");
  std::vector<bxml_data_sptr> scenes_data = bxml_find_all_with_name(root, scenes_query);

  unsigned nscenes=scenes_data.size();
  std::cout << "Number of scenes: " << nscenes << std::endl;

  aux_dirs_.clear();
  aux_dirs_.resize(nscenes);

  finest_cell_length_.clear();
  finest_cell_length_.resize(nscenes);

  for (unsigned si = 0; si < nscenes; si++)
  {
    auto* scenes_elm = dynamic_cast<bxml_element*>(scenes_data[si].ptr());
    int id = -1;
    scenes_elm->get_attribute("id", id);
    scenes_elm->get_attribute("aux_dir", aux_dirs_[id]);
    scenes_elm->get_attribute("cell_length" , finest_cell_length_[id]);

    if (!(vul_file::exists(aux_dirs_[id]) && vul_file::is_directory(aux_dirs_[id]))){
      vul_file::make_directory(aux_dirs_[id]);
    }
  }

  //Parse kernels
  bxml_element params_query("parameters");
  bxml_data_sptr params_data = bxml_find_by_name(root, params_query);
  if (params_data)
  {
    auto* params_elm = dynamic_cast<bxml_element*>(params_data.ptr());

    params_elm->get_attribute("harris_k", harris_k_);
    std::cout << "Harris_k is " << harris_k_ << '\n';
  }
  else {
    std::cerr << " In Gobal Corners: no parameters element\n";
  }
}


//: Compute Harris' measure extension to 3-d as proposed by:
//  I. Laptev. On space-time interest points. Int. J. Computer Vision, 64(2):107--123, 2005
void bvpl_global_corners::compute_laptev_corners(const bvpl_global_taylor_sptr& global_taylor,int scene_id, int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > taylor_cell_type;

  boxm_scene_base_sptr proj_scene_base =global_taylor->load_projection_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = global_taylor->load_valid_scene(scene_id);

  auto* proj_scene = dynamic_cast<boxm_scene<taylor_tree_type>*>(proj_scene_base.as_pointer());
  auto* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  boxm_scene<boct_tree<short, float> >* corner_scene =
  new boxm_scene<boct_tree<short, float> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
  corner_scene->set_appearance_model(BOXM_FLOAT);
  corner_scene->set_paths(aux_dirs_[scene_id], "corner_scene");
  corner_scene->write_scene("harris_scene.xml");

  if (!( proj_scene && valid_scene && corner_scene ))
  {
    std::cerr << "Error in bvpl_global_corners::threshold_corners: Could not cast input scenes\n";
    return;
  }

  //init variables
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();

  //operate on scene
  bvpl_corner_detector corner_detector;
  bvpl_harris_laptev_functor harris_functor(harris_k_);

  vgl_point_3d<int> max_neighborhood_idx = global_taylor->kernel_vector_->max();
  vgl_point_3d<int> min_neighborhood_idx = global_taylor->kernel_vector_->min();

  std::cout << "Neighborhood for Harris threshold: " << min_neighborhood_idx << " , " << max_neighborhood_idx << std::endl;

  corner_detector.compute_C(proj_scene, harris_functor, min_neighborhood_idx, max_neighborhood_idx, block_i, block_j, block_k, valid_scene, corner_scene, finest_cell_length_[scene_id]);

  //clean memory
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();
}


//: Compute corner measure extension to 3-d as proposed by:
// P. Beaudet, Rotationally invariant image operators, in Proc. 4th Int. Joint Conf. Patt. Recog. 1978.
void bvpl_global_corners::compute_beaudet_corners(const bvpl_global_taylor_sptr& global_taylor,int scene_id, int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > taylor_cell_type;

  boxm_scene_base_sptr proj_scene_base =global_taylor->load_projection_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = global_taylor->load_valid_scene(scene_id);

  auto* proj_scene = dynamic_cast<boxm_scene<taylor_tree_type>*>(proj_scene_base.as_pointer());
  auto* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  boxm_scene<boct_tree<short, float> >* corner_scene =
  new boxm_scene<boct_tree<short, float> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
  corner_scene->set_appearance_model(BOXM_FLOAT);
  corner_scene->set_paths(aux_dirs_[scene_id], "corner_scene");
  corner_scene->write_scene("harris_scene.xml");

  if (!( proj_scene && valid_scene && corner_scene ))
  {
    std::cerr << "Error in bvpl_global_corners::threshold_corners: Could not cast input scenes\n";
    return;
  }

  //init variables
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();

  //operate on scene
  bvpl_corner_detector corner_detector;

  corner_detector.compute_det_H(proj_scene, block_i, block_j, block_k, valid_scene, corner_scene);

  //clean memory
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();
}


//: Threshold a percentage of corners, based of the Harris' measure extension to 3-d proposed by:
//  I. Laptev. On space-time interest points. Int. J. Computer Vision, 64(2):107--123, 2005
void bvpl_global_corners::threshold_laptev_corners(const bvpl_global_taylor_sptr& global_taylor,int scene_id, float harris_thresh, const std::string& output_path)
{
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;

  boxm_scene_base_sptr valid_scene_base = global_taylor->load_valid_scene(scene_id);
  boxm_scene_base_sptr corner_scene_base = this->load_corner_scene(scene_id);
  boxm_scene_base_sptr proj_scene_base =global_taylor->load_projection_scene(scene_id);

  auto* proj_scene = dynamic_cast<boxm_scene<taylor_tree_type>*>(proj_scene_base.as_pointer());

  auto* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());

  auto* corner_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (corner_scene_base.as_pointer());

  if (!(vul_file::exists(output_path) && vul_file::is_directory(output_path)))
    vul_file::make_directory(output_path);

  boxm_scene<boct_tree<short, bool> >* valid_corner_scene =
  new boxm_scene<boct_tree<short, bool> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
  valid_corner_scene->set_appearance_model(BOXM_BOOL);

  std::stringstream valid_ss;
  valid_ss << "valid_scene_" << scene_id ;

  valid_corner_scene->set_paths(output_path, valid_ss.str());
  valid_corner_scene->write_scene(valid_ss.str() + ".xml");
  valid_scene->clone_blocks(*valid_corner_scene);

  std::stringstream proj_ss;
  proj_ss << "proj_taylor_scene_" << scene_id ;

  boxm_scene<taylor_tree_type >* copy_proj_scene =
  new boxm_scene<taylor_tree_type >(proj_scene->lvcs(), proj_scene->origin(), proj_scene->block_dim(), proj_scene->world_dim(), proj_scene->max_level(), proj_scene->init_level());
  copy_proj_scene->set_appearance_model(VNL_DOUBLE_10);
  copy_proj_scene->set_paths(proj_scene->path(), proj_scene->block_prefix());
  copy_proj_scene->write_scene(output_path, proj_ss.str() + ".xml");

  delete copy_proj_scene;

  if (!( valid_scene && corner_scene && valid_corner_scene))
  {
    std::cerr << "Error in bvpl_global_corners::threshold_corners: Could not cast input scenes\n";
    return;
  }

  //(1) Traverse the scene - is there an easy way to modify the cell iterator so to only use leaf cells at level 0;
  boxm_cell_iterator<boct_tree<short, bool > > valid_it = valid_scene->cell_iterator(&boxm_scene<boct_tree<short, bool > >::load_block, true);
  valid_it.begin();

  boxm_cell_iterator<boct_tree<short, float > > corners_it = corner_scene->cell_iterator(&boxm_scene<boct_tree<short, float > >::load_block, true);
  corners_it.begin();

  boxm_cell_iterator<boct_tree<short, bool > > valid_corners_it = valid_corner_scene->cell_iterator(&boxm_scene<boct_tree<short, bool > >::load_block);
  valid_corners_it.begin();

  while ( !(valid_it.end() || corners_it.end() || valid_corners_it.end()) )
  {
    boct_tree_cell<short,bool> *valid_cell = *valid_it;
    boct_tree_cell<short,float> *corner_cell = *corners_it;
    boct_tree_cell<short,bool> *valid_corner_cell = *valid_corners_it;

    boct_loc_code<short> valid_code = valid_cell->get_code();
    boct_loc_code<short> corner_code = corner_cell->get_code();
    boct_loc_code<short> valid_corner_code = valid_corner_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((valid_cell->level() != corner_cell->level()) || (valid_cell->level() != valid_corner_cell->level()) || !(valid_code.isequal(&corner_code)) || !(valid_code.isequal(&valid_corner_code))){
      std::cerr << " Error in threshold_laptev_corners: Cells don't have the same structure\n";
      return;
    }

    /*****CAUTION: Only looking at cells at level 0*/
    if (valid_cell->level()!=0) {
      ++valid_it;
      ++corners_it;
      valid_corner_cell->set_data(false);
      ++valid_corners_it;
      continue;
    }

    if (corner_cell->data() < harris_thresh)
      valid_corner_cell->set_data(false);
    else
      valid_corner_cell->set_data(true);

    ++valid_it;
    ++corners_it;
    ++valid_corners_it;
  }

  //clean memory
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();
  valid_corner_scene->unload_active_blocks();

  delete valid_corner_scene;
}

//: Take a histogram of corners and get different values for percentage of thresholds
void bvpl_global_corners::explore_corner_statistics(const bvpl_global_taylor_sptr& global_taylor,int scene_id)
{
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;

  boxm_scene_base_sptr valid_scene_base = global_taylor->load_valid_scene(scene_id);
  boxm_scene_base_sptr corner_scene_base = this->load_corner_scene(scene_id);

  auto* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());

  auto* corner_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (corner_scene_base.as_pointer());

  if (!( valid_scene && corner_scene))
  {
    std::cerr << "Error in bvpl_global_corners::explore_corner_statistics: Could not cast input scenes\n";
    return;
  }

  //init variables
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();

  //compute scene statistics at valid cells only

  //(1) Traverse the scene - is there an easy way to modify the cell iterator so to only use leaf cells at level 0;
  boxm_cell_iterator<boct_tree<short, bool > > valid_it = valid_scene->cell_iterator(&boxm_scene<boct_tree<short, bool > >::load_block, true);
  valid_it.begin();

  boxm_cell_iterator<boct_tree<short, float > > corners_it = corner_scene->cell_iterator(&boxm_scene<boct_tree<short, float > >::load_block, true);
  corners_it.begin();

  float cell_count = 0;
  auto max = (float)(*corners_it)->data();
  float min = max;
  float this_val = max;
  while ( !(valid_it.end() || corners_it.end()) )
  {
    boct_tree_cell<short,bool> *valid_cell = *valid_it;
    boct_tree_cell<short,float> *corner_cell = *corners_it;

    boct_loc_code<short> valid_code = valid_cell->get_code();
    boct_loc_code<short> corner_code = corner_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((valid_cell->level() != corner_cell->level()) || !(valid_code.isequal(&corner_code))){
      std::cerr << " Error in threshold_laptev_corners: Cells don't have the same structure\n";
      return;
    }

    /*****CAUTION: Only looking at cells at level 0*/
    if (valid_cell->level()!=0) {
      ++valid_it;
      ++corners_it;
      continue;
    }

    cell_count++;
    this_val = (float)(*corners_it)->data();
    if ( this_val > max)  max = this_val;
    if ( this_val < min)  min = this_val;

    ++valid_it;
    ++corners_it;
  }

  auto nbins = (unsigned int)std::floor(std::sqrt(cell_count));
  bsta_histogram<float>  corner_hist(min, max, nbins);
  valid_it.begin();
  corners_it.begin();
  while ( !(valid_it.end() || corners_it.end()) )
  {
    boct_tree_cell<short,bool> *valid_cell = *valid_it;
    boct_tree_cell<short,float> *corner_cell = *corners_it;

    boct_loc_code<short> valid_code = valid_cell->get_code();
    boct_loc_code<short> corner_code = corner_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((valid_cell->level() != corner_cell->level()) || !(valid_code.isequal(&corner_code))){
      std::cerr << " Error in threshold_laptev_corners: Cells don't have the same structure\n";
      return;
    }

    /*****CAUTION: Only looking at cells at level 0*/
    if (valid_cell->level()!=0) {
      ++valid_it;
      ++corners_it;
      continue;
    }

    corner_hist.upcount((float)(*corners_it)->data(), 1.0f);

    ++valid_it;
    ++corners_it;
  }

  std::string file = aux_dirs_[scene_id] + "/corner_threshold_values.txt";
  float threshold[] = {0.01f, 0.02f, 0.05f, 0.1f, 0.2f, 0.5f, 0.8f};
  std::ofstream ofs(file.c_str());
  ofs.precision(7);
  for (float i : threshold) {
    ofs << i << ' ' << corner_hist.value_with_area_above(i) << '\n';
  }

  //clean memory
  ofs.close();
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();
}

//: Load scene info
boxm_scene_base_sptr bvpl_global_corners::load_corner_scene(int scene_id)
{
  if (scene_id<0 || scene_id>((int)aux_dirs_.size() -1))
  {
    std::cerr << "Error in bvpl_global_corners::load_corner_scene: Invalid scene id\n";
    return nullptr;
  }
  //load scene
  boxm_scene_base_sptr scene_base = new boxm_scene_base();
  boxm_scene_parser scene_parser;
  std::stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/harris_scene.xml";
  scene_base->load_scene(aux_scene_ss.str(), scene_parser);

  //cast scene
  auto *scene= new boxm_scene<boct_tree<short, float > >();
  if (scene_base->appearence_model() == BOXM_FLOAT){
    scene->load_scene(scene_parser);
    scene_base = scene;
  }
  else {
    std::cerr << "Error in bvpl_global_corners::load_corner_scene: Invalid appearance model\n";
    return nullptr;
  }

  return scene_base;
}

//: Load auxiliary scene indicating if a cell is valid. e.g border cells are not valid
boxm_scene_base_sptr bvpl_global_corners::load_valid_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)aux_dirs_.size() -1))
  {
    std::cerr << "Error in bvpl_global_corners::load_scene: Invalid scene id\n";
    return nullptr;
  }
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  std::stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/valid_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);

  //cast scene
  auto *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL){
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_corners::load_aux_scene: Invalid appearance model\n";
    return nullptr;
  }

  return aux_scene_base;
}

//: Write this class to xml file
void bvpl_global_corners::xml_write()
{
  bxml_document doc;
  bxml_element *root = new bxml_element("global_corners_info");
  doc.set_root_element(root);
  root->append_text("\n");

  //write the scene elements
  for (unsigned i =0; i<aux_dirs_.size(); i++)
  {
    bxml_element* scenes_elm = new bxml_element("scene");
    scenes_elm->append_text("\n");
    scenes_elm->set_attribute("id", i);
    scenes_elm->set_attribute("aux_dir", aux_dirs_[i]);
    scenes_elm->set_attribute("cell_length", finest_cell_length_[i]);
    root->append_data(scenes_elm);
    root->append_text("\n");
  }

  //write parameters for corner detection
  bxml_element* param_elm = new bxml_element("parameters");
  param_elm->append_text("\n");
  param_elm->set_attribute("harris_k", harris_k_);
  root->append_data(param_elm);
  root->append_text("\n");

  //write to disk
  std::ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();
}
