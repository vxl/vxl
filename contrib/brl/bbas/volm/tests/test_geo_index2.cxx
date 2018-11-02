#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_tile.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_utm.h>
#include <bkml/bkml_parser.h>

static void test_geo_index2()
{
#if 0
  double min_size = 0.05;
  //std::string dir = "./";
  std::string dir = "D:/work/find/phase_1b/ROI/tiles/";
  vpgl_utm u;
  // wr1
  std::vector<volm_tile> tiles;
  tiles = volm_tile::generate_p1b_wr1_tiles();
  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[i], min_size);
    unsigned tree_depth = volm_geo_index2::depth(root);
    for (unsigned d_idx = 0; d_idx <= tree_depth; d_idx++) {
      std::stringstream kml_file;  kml_file << dir << "p1b_wr1_tile_" << i << "_depth_" << d_idx << ".kml";
      volm_geo_index2::write_to_kml(root, d_idx, kml_file.str());
    }
    double x, y;
    int zone_id, zone_id2;
    u.transform(tiles[i].bbox_double().min_point().y(), tiles[i].bbox_double().min_point().x(), x, y, zone_id);
    u.transform(tiles[i].bbox_double().max_point().y(), tiles[i].bbox_double().max_point().x(), x, y, zone_id2);
    std::cout << " p1b_wr1_tile_" << i << " has utm zone " << zone_id << " and utm zone " << zone_id2 << std::endl;
  }

  // wr2
  tiles.clear();
  tiles = volm_tile::generate_p1b_wr2_tiles();
  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[i], min_size);
    unsigned tree_depth = volm_geo_index2::depth(root);
    for (unsigned d_idx = 0; d_idx <= tree_depth; d_idx++) {
      std::stringstream kml_file;  kml_file << dir << "p1b_wr2_tile_" << i << "_depth_" << d_idx << ".kml";
      volm_geo_index2::write_to_kml(root, d_idx, kml_file.str());
    }
    double x, y;
    int zone_id, zone_id2;
    u.transform(tiles[i].bbox_double().min_point().y(), tiles[i].bbox_double().min_point().x(), x, y, zone_id);
    u.transform(tiles[i].bbox_double().max_point().y(), tiles[i].bbox_double().max_point().x(), x, y, zone_id2);
    std::cout << " p1b_wr2_tile_" << i << " has utm zone " << zone_id << " and utm zone " << zone_id2 << std::endl;
  }

  // wr3
  tiles.clear();
  tiles = volm_tile::generate_p1b_wr3_tiles();
  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[i], min_size);
    unsigned tree_depth = volm_geo_index2::depth(root);
    for (unsigned d_idx = 0; d_idx <= tree_depth; d_idx++) {
      std::stringstream kml_file;  kml_file << dir << "p1b_wr3_tile_" << i << "_depth_" << d_idx << ".kml";
      volm_geo_index2::write_to_kml(root, d_idx, kml_file.str());
    }
    double x, y;
    int zone_id, zone_id2;
    u.transform(tiles[i].bbox_double().min_point().y(), tiles[i].bbox_double().min_point().x(), x, y, zone_id);
    u.transform(tiles[i].bbox_double().max_point().y(), tiles[i].bbox_double().max_point().x(), x, y, zone_id2);
    std::cout << " p1b_wr3_tile_" << i << " has utm zone " << zone_id << " and utm zone " << zone_id2 << std::endl;
  }

  // wr4
  tiles.clear();
  tiles = volm_tile::generate_p1b_wr4_tiles();
  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[i], min_size);
    unsigned tree_depth = volm_geo_index2::depth(root);
    for (unsigned d_idx = 0; d_idx <= tree_depth; d_idx++) {
      std::stringstream kml_file;  kml_file << dir << "p1b_wr4_tile_" << i << "_depth_" << d_idx << ".kml";
      volm_geo_index2::write_to_kml(root, d_idx, kml_file.str());
    }
    double x, y;
    int zone_id, zone_id2;
    u.transform(tiles[i].bbox_double().min_point().y(), tiles[i].bbox_double().min_point().x(), x, y, zone_id);
    u.transform(tiles[i].bbox_double().max_point().y(), tiles[i].bbox_double().max_point().x(), x, y, zone_id2);
    std::cout << " p1b_wr4_tile_" << i << " has utm zone " << zone_id << " and utm zone " << zone_id2 << std::endl;
  }

  // wr5
  min_size = 0.05;
  tiles.clear();
  tiles = volm_tile::generate_p1b_wr5_tiles();
  std::string kml_roi_file = "D:/work/find/phase_1b/ROI/taiwan-WR.kml";
  vgl_polygon<double> roi_poly = bkml_parser::parse_polygon(kml_roi_file);
  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[i], min_size, roi_poly);
    unsigned tree_depth = volm_geo_index2::depth(root);
    for (unsigned d_idx = 0; d_idx <= tree_depth; d_idx++) {
      std::stringstream kml_file;  kml_file << dir << "p1b_wr5_tile_" << i << "_depth_" << d_idx << ".kml";
      volm_geo_index2::write_to_kml(root, d_idx, kml_file.str());
    }
    double x, y;
    int zone_id, zone_id2;
    u.transform(tiles[i].bbox_double().min_point().y(), tiles[i].bbox_double().min_point().x(), x, y, zone_id);
    u.transform(tiles[i].bbox_double().max_point().y(), tiles[i].bbox_double().max_point().x(), x, y, zone_id2);
    std::cout << " p1b_wr5_tile_" << i << " has utm zone " << zone_id << " and utm zone " << zone_id2 << std::endl;
  }
#endif

#if 1
  // test the construct tree
  std::vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();
  float min_size = 0.25f;

  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[0], min_size);
  unsigned depth = volm_geo_index2::depth(root);
  for (unsigned l_idx = 0; l_idx <= depth; l_idx++) {
    std::stringstream kml_file;  kml_file << "geo_index2_depth_" << l_idx << ".kml";
    volm_geo_index2::write_to_kml(root, l_idx, kml_file.str());
  }

  vgl_polygon<double> poly;
  poly.new_sheet();
  poly.push_back(-81.50779118919101,31.04919038787528);
  poly.push_back(-81.69107025826281,30.88627732444415);
  poly.push_back(-81.40298761098124,30.80890688022819);
  poly.push_back(-81.41208234307915,31.07645716368274);
  poly.new_sheet();
  poly.push_back(-81.30350143481877,30.61031054112167);
  poly.push_back(-81.4396644145867,30.37357775240261);
  poly.push_back(-81.0666805347681,30.31653878823842);
  poly.push_back(-81.09443342163743,30.63005476523718);
  poly.push_back(-81.14047185086301,30.81053757545839);
  volm_geo_index2_node_sptr root_less = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(tiles[0], min_size, poly);
  depth = volm_geo_index2::depth(root_less);
  for (unsigned l_idx = 0; l_idx <= depth; l_idx++) {
    std::stringstream kml_file;  kml_file << "geo_index2_less_depth_" << l_idx << ".kml";
    volm_geo_index2::write_to_kml(root_less, l_idx, kml_file.str());
  }

  // test the prune tree method (by polygon)
  volm_geo_index2::prune_tree(root, poly);
  unsigned depth_prune_poly = volm_geo_index2::depth(root);
  for (unsigned l_idx = 0; l_idx <= depth_prune_poly; l_idx++) {
    std::stringstream kml_file;  kml_file << "geo_index2_prune_depth_" << l_idx << ".kml";
    volm_geo_index2::write_to_kml(root, l_idx, kml_file.str());
  }
  TEST("prune by polygon method", depth_prune_poly, depth);
  // test the prune method by utm zone
  volm_geo_index2::prune_by_zone(root_less, 17);
  unsigned depth_utm_17 = volm_geo_index2::depth(root_less);
  TEST("prune by zone method", depth_utm_17, depth);

  // test the tree text IO
  std::string txt_file = "./geo_index2_tree_str.txt";
  volm_geo_index2::write(root, txt_file, min_size);
  double min_size_read;
  volm_geo_index2_node_sptr root_read = volm_geo_index2::read_and_construct<volm_loc_hyp_sptr>(txt_file, min_size_read);
  unsigned depth_read = volm_geo_index2::depth(root_read);
  bool test_txt_io = (depth_read == depth) && (min_size==min_size_read);
  TEST("text tree structure IO", test_txt_io, true);
  for (unsigned l_idx = 0; l_idx <= depth_read; l_idx++) {
    std::stringstream kml_file;  kml_file << "geo_index2_read_depth_" << l_idx << ".kml";
    volm_geo_index2::write_to_kml(root_read, l_idx, kml_file.str());
  }

  // test the get leave method
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  std::cout << " \t leaves for current tree: \n\t";
  for (auto & leave : leaves) {
    leave->extent_.print(std::cout);
    std::cout << "\n\t";
  }
  std::cout << std::endl;
  TEST("get leaves method", leaves.size(), 7);
  vgl_polygon<double> poly1;
  poly1.push_back(poly[0]);
  leaves.clear();
  volm_geo_index2::get_leaves(root, leaves, poly1);
  TEST("get leaves method by polygon", leaves.size(), 2);
  vgl_point_2d<double> ref_pt(leaves[1]->extent_.min_point().x(), leaves[1]->extent_.min_point().y());
  vgl_box_2d<double> bbox(ref_pt.x()-0.5*min_size, ref_pt.x()+0.5*min_size, ref_pt.y()-0.5*min_size, ref_pt.y()+0.5*min_size);

  leaves.clear();
  volm_geo_index2::get_leaves(root, leaves, bbox);
  std::cout << " \t given bounding box: \n\t";
  bbox.print(std::cout);
  std::cout << std::endl;
  std::cout << " \t intersect leaves are: \n\t";
  for (auto & leave : leaves) {
    leave->extent_.print(std::cout);
    std::cout << "\n\t";
  }
  std::cout << std::endl;
  TEST("get leaves method by 2D bounding box", leaves.size(), 3);
  bool cast_correct = true;
  for (auto & leave : leaves)
    cast_correct = cast_correct && dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leave.ptr());
  TEST("type of the contest in each leave", cast_correct, true);
#endif

#if 0
  // add content into valid leaves
  leaves.clear();
  volm_geo_index2::get_leaves(root, leaves);
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    volm_geo_index2_node<volm_loc_hyp_sptr>* ptr;
    if ( ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr())) {
      if (!ptr->contents_)
        ptr->contents_ = new volm_loc_hyp();
      ptr->contents_->add(leaves[l_idx]->extent_.min_point().x()+0.5*min_size, leaves[l_idx]->extent_.min_point().y()+0.5*min_size, 1.6);
      ptr->contents_->add(leaves[l_idx]->extent_.min_point().x()+0.25*min_size, leaves[l_idx]->extent_.min_point().y()+0.25*min_size, 1.6);
    }
  }
  // write them into binary file
  std::string dir = "./geo_index_tile_0";
  for (unsigned l_idx = 0; l_idx < leaves.size();  l_idx++) {
    std::string hyp_file = leaves[l_idx]->get_label_name(dir, "hypo");
    std::cout << "leaves " << l_idx << " , filename = " << hyp_file << std::endl;
    volm_geo_index2_node<volm_loc_hyp_sptr>* ptr;
    if ( ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr()))
      ptr->contents_->write_hypotheses(hyp_file);
  }

  // load the content to the tree
  leaves.clear();
  volm_geo_index2::get_leaves(root_read, leaves);
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    std::string hyp_file = leaves[l_idx]->get_label_name(dir, "hypo");
    volm_geo_index2_node<volm_loc_hyp_sptr>* ptr;
    if ( ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr()))
      ptr->contents_ = new volm_loc_hyp(hyp_file);
  }

  // print out
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    vgl_point_3d<double> h_pt;
    volm_geo_index2_node<volm_loc_hyp_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr());
    while(ptr->contents_->get_next(h_pt))
      std::cout << " in leaf " << l_idx << " we have location " << h_pt << std::endl;
  }

  std::cout << " test another type: " << std::endl;
  // test another type
  volm_geo_index2_node_sptr root_locs = volm_geo_index2::read_and_construct<volm_loc_hyp>(txt_file, min_size_read);
  leaves.clear();
  volm_geo_index2::get_leaves(root_locs, leaves);
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    ;
    if ( volm_geo_index2_node<volm_loc_hyp_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr())) {
      if (!ptr->contents_)
        ptr->contents_ = new volm_loc_hyp();
      ptr->contents_->add(leaves[l_idx]->extent_.min_point().x()+0.5*min_size, leaves[l_idx]->extent_.min_point().y()+0.5*min_size, 1.6);
    }
    else if ( volm_geo_index2_node<volm_loc_hyp>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp>* >(leaves[l_idx].ptr()))
      ptr->contents_.add(leaves[l_idx]->extent_.min_point().x()+0.5*min_size, leaves[l_idx]->extent_.min_point().y()+0.5*min_size, 2.0);
  }
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    if (volm_geo_index2_node<volm_loc_hyp_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr())) {
      std::string hyp_file = leaves[l_idx]->get_label_name(dir, "hypo_sptr");
      ptr->contents_->write_hypotheses(hyp_file);
    }
    else if (volm_geo_index2_node<volm_loc_hyp>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp>* >(leaves[l_idx].ptr())) {
      std::string hyp_file = leaves[l_idx]->get_label_name(dir, "hypo_locs");
      ptr->contents_.write_hypotheses(hyp_file);
    }
  }

  // load it
  volm_geo_index2_node_sptr root_locs_read = volm_geo_index2::read_and_construct<volm_loc_hyp>(txt_file, min_size_read);
  leaves.clear();
  volm_geo_index2::get_leaves(root_locs, leaves);
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    if (volm_geo_index2_node<volm_loc_hyp_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr())) {
      std::string hyp_file = leaves[l_idx]->get_label_name(dir, "hypo_sptr");
      ptr->contents_ = new volm_loc_hyp(hyp_file);
    }
    else if (volm_geo_index2_node<volm_loc_hyp>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp>* >(leaves[l_idx].ptr())) {
      std::string hyp_file = leaves[l_idx]->get_label_name(dir, "hypo_locs");
      ptr->contents_ = volm_loc_hyp(hyp_file);
    }
  }

  // print out
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    vgl_point_3d<double> h_pt;
    if (volm_geo_index2_node<volm_loc_hyp_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp_sptr>* >(leaves[l_idx].ptr())) {
      while(ptr->contents_->get_next(h_pt))
        std::cout << " in leaf " << l_idx << " we have location " << h_pt << std::endl;
    }
    else if (volm_geo_index2_node<volm_loc_hyp>* ptr = dynamic_cast<volm_geo_index2_node<volm_loc_hyp>* >(leaves[l_idx].ptr())) {
      while(ptr->contents_.get_next(h_pt))
        std::cout << " in leaf " << l_idx << " we have location " << h_pt << std::endl;
    }
  }
#endif
}

TESTMAIN(test_geo_index2);
