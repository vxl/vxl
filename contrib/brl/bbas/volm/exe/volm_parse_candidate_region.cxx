// This is contrib/brl/bbas/volm/exe/volm_create_bvxm_scene.cxx
//:
// \file
// \brief  executable create candidate regions for each geo index leaf.  For each leaf, it will output
//         the outer contour polygon and inner counter polygon, as binary format
//
// \author Yi Dong
// \date October 20, 2014
// \verbatim
//  Modifications
// \endverbatim
//
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vgl/vgl_intersection.h>
#include <vgl/io/vgl_io_polygon.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_io.h>
#include <volm/volm_candidate_region_parser.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_utils.h>
#include <volm/volm_candidate_list.h>

static void write_out_polygon(vcl_string const& out_folder, vcl_string const& region_str, vcl_string const& leaf_str_pre, vgl_polygon<double> const& poly);

int main(int argc, char** argv)
{
  // inputs
  vul_arg<unsigned>   tile_id("-tile", "ROI tile_id", 9999);
  vul_arg<vcl_string> geo_folder("-geo", "folder to read the geo hypotheses", "");
  vul_arg<vcl_string>  cand_file("-cand", "candidate region kml file", "");
  vul_arg<vcl_string> out_folder("-out", "output folder", "");
  vul_arg<vcl_string> region_name("-name", "candidate region name.  Default -- \"Region\"", "Region");
  vul_arg<bool>       use_inner("-inner", "option to use inner boundary", false);
  vul_arg<bool>       is_kml("-kml", "option to generate output kml", false);
  vul_arg<int>       leaf_id("-leaf", "leaf id to generate kml file from parsed candidate region", -1);
  vul_arg_parse(argc, argv);
  // input check
  if (geo_folder().compare("") == 0 || out_folder().compare("") == 0 || tile_id() == 9999)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  vcl_string log_file = out_folder() + "/log_candidate_region.xml";
  vcl_stringstream log;
  
  // load geo location
  vcl_stringstream file_name_pre;
  file_name_pre << geo_folder() << "/geo_index_tile_" << tile_id();
  if (!vul_file::exists(file_name_pre.str()+".txt")) {
    log << "ERROR: can not find file " << file_name_pre.str() << ".txt for tile " << tile_id() << "\n!";
    volm_io::write_error_log(log_file, log.str());  return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  vcl_vector<volm_geo_index_node_sptr> loc_leaves;
  loc_leaves.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves);
  vcl_cout << loc_leaves.size() << " location leaves exist for tile " << tile_id() << vcl_endl;

  if (!is_kml())  // create candidate polygon for each leaf
  {
    // parse the polygons from candidate region
    if (!vul_file::exists(cand_file())) {
      log << "ERROR: can not find candidate region kml file: " <<cand_file() << "!\n";
      volm_io::write_error_log(log_file, log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_cout << "============  START!!!! ============\n";
    vcl_cout << "  parsing candidate polygon from kml file: " << cand_file() << "..." << vcl_endl;
    vgl_polygon<double> outer;
    vgl_polygon<double> inner;
    unsigned n_out, n_in;
    vgl_polygon<double> poly_all = volm_candidate_region_parser::parse_polygon_with_inner(cand_file(), region_name(), outer, inner, n_out, n_in);
    vcl_cout << "    " << outer.num_sheets() << " candidate region are parsed\n";
    vcl_cout << "    " << inner.num_sheets() << " inner contour are parsed\n";

    // create candidate polygon for each leaf
    unsigned n_leaf = loc_leaves.size();
    for (unsigned l_idx = 0; l_idx < n_leaf; l_idx++) {
      volm_geo_index_node_sptr leaf = loc_leaves[l_idx];
      vcl_cout << "    generate candidate region for leaf " << l_idx << " -- " << leaf->extent_ << "..." << vcl_endl;
      // form the outer region
      // check whether there is an outer region covers the entire leaf
      bool found = false;
      for (unsigned s_idx = 0; (s_idx < n_out && !found); s_idx++) {
        if (volm_utils::poly_contains(outer[s_idx], leaf->extent_)) {
          found = true;
          // write out the polygon
          vgl_polygon<double> leaf_out(outer[s_idx]);
          write_out_polygon(out_folder(), "outer", leaf->get_string(), leaf_out);
        }
      }
      // obtain all outer region that intersects with leaf
      if (!found) {
        vgl_polygon<double> leaf_out;
        for (unsigned s_idx = 0; s_idx < n_out; s_idx++) {
          vgl_polygon<double> single_sheet(outer[s_idx]);
          if (vgl_intersection(leaf->extent_, single_sheet))
            leaf_out.push_back(outer[s_idx]);
        }
        // write out the polygon
        write_out_polygon(out_folder(), "outer", leaf->get_string(), leaf_out);
      }

      // form the inner region if necessary
      if (use_inner()) {
        // check whether there is an inner region covers the entire leaf
        bool found = false;
        for (unsigned s_idx = 0; (s_idx < n_in && !found); s_idx++) {
          if (volm_utils::poly_contains(inner[s_idx], leaf->extent_)) {
            found = true;
            // write out the polygon
            vgl_polygon<double> leaf_in(inner[s_idx]);
            write_out_polygon(out_folder(), "inner", leaf->get_string(), leaf_in);
          }
        }
        if (!found) {
          vgl_polygon<double> leaf_in;
          for (unsigned s_idx = 0; s_idx < n_in; s_idx++) {
            vgl_polygon<double> single_sheet(inner[s_idx]);
            if (vgl_intersection(leaf->extent_, single_sheet))
              leaf_in.push_back(inner[s_idx]);
          }
          // write out the inner polygon
          write_out_polygon(out_folder(), "inner", leaf->get_string(), leaf_in);
        }
      }
    } // end of the loop over each leaf
    return volm_io::SUCCESS;
  }
  else  // read the created polygon for given leaf
  {
    vcl_cout << "  generate candidate region kml for leaf " << leaf_id() << vcl_endl;
    // obtain the desired leaf
    if (leaf_id() < 0 || leaf_id() >= loc_leaves.size()) {
      log << "ERROR: input leaf id " << leaf_id() << " is incorrect!\n";
      volm_io::write_error_log(log_file, log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    volm_geo_index_node_sptr leaf = loc_leaves[leaf_id()];
    // load the created candidate region
    vcl_string outer_region_file = out_folder() + "/cand_region_outer_" + leaf->get_string() + ".bin";
    if (!vul_file::exists(outer_region_file)) {
      vcl_cout << "leaf " << leaf->extent_ << " is outside of candidate region" << vcl_endl;
      return volm_io::SUCCESS;
    }
    vgl_polygon<double> outer;
    vsl_b_ifstream ifs_out(outer_region_file);
    vsl_b_read(ifs_out, outer);
    ifs_out.close();
    // load the inner region if exists
    vcl_string inner_region_file = out_folder() + "/cand_region_inner_" + leaf->get_string() + ".bin";
    vgl_polygon<double> inner;
    inner.clear();
    if (vul_file::exists(inner_region_file)) {
      vsl_b_ifstream ifs_in(inner_region_file);
      vsl_b_read(ifs_in, inner);
      ifs_in.close();
    }
    vcl_string kml_file = out_folder() + "/cand_region_" + leaf->get_string() + ".kml";
    vcl_ofstream ofs(kml_file.c_str());
    bkml_write::open_document(ofs);
    // write out the kml file
    if (inner.num_sheets() == 0) {
      
      vcl_stringstream name;
      name << "leaf_" << leaf_id() << "_cand_" << outer.num_sheets();
      bkml_write::write_polygon(ofs, outer, name.str(), "", 1.0, 3.0, 0.45, 0, 255, 0);
    }
    else {
      // form the polygon pair
      vcl_vector<vcl_pair<vgl_polygon<double>, vgl_polygon<double> > > cand_poly;
      for (unsigned s_idx = 0; s_idx < outer.num_sheets(); s_idx++) {
        vgl_polygon<double> inside = volm_utils::poly_contains(outer[s_idx], inner);
        vcl_pair<vgl_polygon<double>, vgl_polygon<double> > cand_region(vgl_polygon<double>(outer[s_idx]), inside);
        cand_poly.push_back(cand_region);
      }
      // write out the kml file
      vcl_stringstream name;
      name << "leaf_" << leaf_id() << "_cand_" << cand_poly.size();
      bkml_write::write_polygon(ofs, cand_poly, name.str(), "", 1.0, 3.0, 0.45, 0, 255, 0);
    }

    // write the location hypotheses into kml file for visualization
    if (volm_utils::poly_contains(outer, leaf->extent_) && inner.num_sheets() == 0) {
      // the leaf is entirely inside the candidate region, put all loc into kml
      vcl_cout << leaf->hyps_->locs_.size() << " locations are inside candidate region" << vcl_endl;
      vgl_point_3d<double> h_pt;
      while ( leaf->hyps_->get_next(0, 1, h_pt) ) {
        vcl_stringstream loc_name;
        loc_name << "hyps_" << vcl_setprecision(6) << h_pt.x() << vcl_setprecision(6) << h_pt.y();
        bkml_write::write_location_as_box(ofs, h_pt.x(), h_pt.y(), h_pt.z(), loc_name.str(), "", 1E-5, 255, 131, 250);
      }
      
    }
    else {
      vgl_point_3d<double> h_pt;
      unsigned cnt = 0;
      while ( leaf->hyps_->get_next(0, 1, h_pt) ) {
        if (volm_candidate_list::inside_candidate_region(inner, outer, h_pt.x(), h_pt.y())) {
          cnt++;
          vcl_stringstream loc_name;
          loc_name << "hyps_" << vcl_setprecision(6) << h_pt.x() << vcl_setprecision(6) << h_pt.y();
          bkml_write::write_location_as_box(ofs, h_pt.x(), h_pt.y(), h_pt.z(), loc_name.str(), "", 2E-5, 255, 131, 250);
        }
      }
      vcl_cout << cnt << " locations are inside candidate region" << vcl_endl;
    }
    bkml_write::close_document(ofs);
    ofs.close();
    return volm_io::SUCCESS;;
  }
  return volm_io::SUCCESS;
}

void write_out_polygon(vcl_string const& out_folder, vcl_string const& region_str, vcl_string const& leaf_str_pre, vgl_polygon<double> const& poly)
{
  vcl_string out_file = out_folder + "/cand_region_" + region_str + "_" + leaf_str_pre + ".bin";
  vsl_b_ofstream ofs(out_file);
  vsl_b_write(ofs, poly);
  ofs.close();
}