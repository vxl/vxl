#include <volm/conf/volm_conf_land_map_indexer.h>
//:
// \file
#include <vil/vil_crop.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>
#include <brip/brip_roi.h>
#include <vcl_algorithm.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_blob.h>
#include <bkml/bkml_write.h>
#include <sdet/sdet_detector.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vgl/vgl_vector_2d.h>
#include <vcl_cassert.h>
#include <vul/vul_file.h>
#include <volm/volm_io_tools.h>
#include <bil/algo/bil_blob_finder.h>
#include <vgl/vgl_intersection.h>

static void convert_bbox(vgl_box_2d<float> const& box_float, vgl_box_2d<double>& box_double)
{
  box_double.empty();
  box_double.set_min_point(vgl_point_2d<double>((double)box_float.min_x(), (double)box_float.min_y()));
  box_double.set_max_point(vgl_point_2d<double>((double)box_float.max_x(), (double)box_float.max_y()));
}

volm_conf_land_map_indexer::volm_conf_land_map_indexer(vgl_box_2d<double> const& bbox, double const& density) :
  bbox_(bbox), density_(density)
{
  // create a lvcs for defined region, set lvcs as wgs84 and use the lower left as origin
  double lon_min, lat_min;
  lon_min = bbox_.min_x();
  lat_min = bbox_.min_y();
  lvcs_ = new vpgl_lvcs(lat_min, lon_min, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // clear loc data
  land_locs_.clear();
}

volm_conf_land_map_indexer::volm_conf_land_map_indexer(vgl_box_2d<float>  const& bbox, float  const& density)
{
  vgl_box_2d<double> bbox_double;
  convert_bbox(bbox, bbox_double);
  bbox_ = bbox_double;
  density_ = (float)density;
  double lon_min, lat_min;
  lon_min = bbox_.min_x();
  lat_min = bbox_.min_y();
  lvcs_ = new vpgl_lvcs(lat_min, lon_min, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // clear loc data
  land_locs_.clear();
}

volm_conf_land_map_indexer::volm_conf_land_map_indexer(double const& min_lon, double const& max_lon, double const& min_lat, double const& max_lat,
                                                       double const& density) :
  density_(density)
{
  bbox_.empty();
  bbox_.set_min_point(vgl_point_2d<double>(min_lon, min_lat));
  bbox_.set_max_point(vgl_point_2d<double>(max_lon, max_lat));
  double lon_min, lat_min;
  lon_min = bbox_.min_x();
  lat_min = bbox_.min_y();
  lvcs_ = new vpgl_lvcs(lat_min, lon_min, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // clear loc data
  land_locs_.clear();
}

volm_conf_land_map_indexer::volm_conf_land_map_indexer(float  const& min_lon, float  const& max_lon, float  const& min_lat, float  const& max_lat,
                                                       float  const& density)
{
  density_ = (double)density;
  bbox_.empty();
  bbox_.set_min_point(vgl_point_2d<double>((double)min_lon, (double)min_lat));
  bbox_.set_max_point(vgl_point_2d<double>((double)max_lon, (double)max_lat));
  double lon_min, lat_min;
  lon_min = bbox_.min_x();
  lat_min = bbox_.min_y();
  lvcs_ = new vpgl_lvcs(lat_min, lon_min, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  // clear loc data
  land_locs_.clear();
}

volm_conf_land_map_indexer::volm_conf_land_map_indexer(vcl_string const& bin_file)
{
  vsl_b_ifstream is(bin_file.c_str());
  assert(is);
  this->b_read(is);
  is.close();
}

// create a string signature
vcl_string volm_conf_land_map_indexer::box_string() const
{
  vcl_stringstream str;
  str << vcl_setprecision(6) << bbox_.min_x() << "_" << vcl_setprecision(6) << bbox_.min_y();
  return str.str();
}

// return list of all land types in current database
vcl_vector<unsigned char> volm_conf_land_map_indexer::land_types() const
{
  vcl_vector<unsigned char> land_types;
  for (volm_conf_loc_map::const_iterator mit = land_locs_.begin(); mit != land_locs_.end(); ++mit)
    land_types.push_back(mit->first);
  return land_types;
}

// number of locations in the database
unsigned volm_conf_land_map_indexer::nlocs() const
{
  unsigned nlocs = 0;
  for (volm_conf_loc_map::const_iterator mit = land_locs_.begin();  mit != land_locs_.end(); ++mit)
    nlocs += mit->second.size();
  return nlocs;
}

// number of locations that have given land type
unsigned volm_conf_land_map_indexer::nlocs(unsigned char const& land_id) const
{
  if (land_locs_.find(land_id) == land_locs_.end())
    return 0;
  else
    return (unsigned)land_locs_.find(land_id)->second.size();
}

// add a location into database
bool volm_conf_land_map_indexer::add_locations(vgl_point_2d<double> const& loc, unsigned char const& land_id)
{
  if (bbox_.contains(loc))
    land_locs_[land_id].push_back(loc);
  return true;
}

// add locations from a land map
bool volm_conf_land_map_indexer::add_locations(vil_image_view<vxl_byte> const& image, vpgl_geo_camera* geocam, vcl_string const& img_type)
{
  // obtain the cropped image first
  brip_roi broi(image.ni(), image.nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  double min_u, min_v, max_u, max_v;
  geocam->global_to_img(bbox_.min_x(), bbox_.min_y(), 0.0, min_u, min_v);
  bb->add_point(min_u, min_v);
  geocam->global_to_img(bbox_.max_x(), bbox_.max_y(), 0.0, max_u, max_v);
  bb->add_point(max_u, max_v);
  bb = broi.clip_to_image_bounds(bb);
  if (bb->width() <=0 || bb->height() <=0) {
    return true;
  }
  double min_uu, min_vv, max_uu, max_vv;
  geocam->global_to_img(bbox_.min_x(), bbox_.max_y(), 0.0, min_uu, min_vv);
  geocam->global_to_img(bbox_.max_x(), bbox_.min_y(), 0.0, max_uu, max_vv);
  unsigned ci = vcl_floor(min_uu), cj = vcl_floor(min_vv);
  unsigned cni = vcl_floor(max_uu-min_uu+0.5);
  unsigned cnj = vcl_floor(max_vv-min_vv+0.5);
  vcl_cout << "min_x: " << bbox_.min_x() << " min_y: " << bbox_.max_y() << ", min_u: " << min_uu << ", min_v: " << min_vv << vcl_endl;
  vcl_cout << "max_x: " << bbox_.max_x() << " max_y: " << bbox_.min_y() << ", max_u: " << max_uu << ", max_v: " << max_vv << vcl_endl;
  vcl_cout << "crop image: " << ci << "x" << cj << ", ni: " << cni << ", nj: " << cnj << vcl_endl;
  vcl_cout << "original image: " << image.ni() << "x" << image.nj() << vcl_endl;
  vil_image_resource_sptr image_ptr = vil_new_image_resource_of_view(image);
  vil_image_resource_sptr crop_res = vil_crop(image_ptr, ci, cni, cj, cnj);
  vil_image_view_base_sptr out_sptr = vil_new_image_view_base_sptr(*(crop_res->get_view()));
  vil_image_view<vxl_byte>* crop_img = dynamic_cast<vil_image_view<vxl_byte>*>(out_sptr.ptr());
  // obtain the edge map for each land category
  vcl_set<unsigned char> land_types;
  for (unsigned i = 0; i < crop_img->ni(); i++) {
    for (unsigned j = 0; j < crop_img->nj(); j++) {
      land_types.insert((*crop_img)(i,j));
    }
  }
  vcl_cout << land_types.size() << " land types are in " << img_type << " image: ";
  for (vcl_set<unsigned char>::iterator sit = land_types.begin(); sit != land_types.end();  ++sit)
    vcl_cout << (int)*sit << ", ";
  vcl_cout << vcl_endl;
  // loop over each land type to add locations
  for (vcl_set<unsigned char>::iterator sit = land_types.begin(); sit != land_types.end();  ++sit)
  {
    // define the land type
    unsigned char land_id;
    if (img_type == "nlcd" || img_type == "NLCD")
        land_id = volm_osm_category_io::nlcd_land_table[*sit].id_;
    else if (img_type == "geocover")
        land_id = volm_osm_category_io::geo_land_table[*sit].id_;
    else {
      vcl_cout << "In volm_conf_land_map_indexer, unknown image type: " << img_type << ", only nlcd/NLCD/geocover allowed" << vcl_endl;
      return false;
    }
    // ignore the invalid land type
    if (volm_osm_category_io::volm_land_table[land_id].name_ == "invalid")
      continue;
    // threshold the image
    vil_image_view<bool> thres_img(crop_img->ni(), crop_img->nj());
    vil_threshold_inside(*crop_img, thres_img, *sit, *sit);
    // closing the holes or gaps
    vil_structuring_element selem;
    selem.set_to_disk(1.0);
    vil_image_view<bool> temp(thres_img.ni(), thres_img.nj());
    vil_binary_closing(thres_img, temp, selem);
    vil_image_view<vxl_byte> thres_img_closed(crop_img->ni()+2, crop_img->nj()+2);  // enlarge the image by 2 pixels
    thres_img_closed.fill(0);
    for (unsigned i = 0; i < crop_img->ni(); i++)
      for (unsigned j = 0; j < crop_img->nj(); j++)
        if (temp(i,j))  thres_img_closed(i+1,j+1) = 255;

    // use sdet_detector to perform edge detection for binary image (use default parameter)
    sdet_detector_params dp;
    sdet_detector detector(dp);
    vil_image_resource_sptr thres_img_res_sptr = vil_new_image_resource_of_view(thres_img_closed);
    detector.SetImage(thres_img_res_sptr);
    detector.DoContour();
    vcl_vector<vtol_edge_2d_sptr>* edges = detector.GetEdges();
    // form the location points
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > edge_locs;
    for (unsigned e_idx = 0; e_idx < edges->size(); e_idx++)
    {
      vcl_vector<vsol_point_2d_sptr> edge_loc;
      edge_loc.clear();
      vdgl_digital_curve_sptr dc = ((*edges)[e_idx]->curve())->cast_to_vdgl_digital_curve();
      if (!dc)
        continue;
      vdgl_interpolator_sptr intp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();

      // iterate over each point in the connected edge component
      for (unsigned j = 0; j < ec->size(); j++) {
        vdgl_edgel curr_edgel = ec->edgel(j);
        unsigned cr_x = vcl_floor(curr_edgel.get_x()+0.5);
        unsigned cr_y = vcl_floor(curr_edgel.get_y()+0.5);
        if (cr_x == 0 || cr_y == 0 || cr_x == thres_img_closed.ni()-1 || cr_y == thres_img_closed.nj()-1)
          continue;  // ignore the boundary edge
        unsigned oi, oj;
        oi = cr_x + ci;  oj = cr_y + cj;
        double loc_lon, loc_lat;
        geocam->img_to_global(oi, oj, loc_lon, loc_lat);
        edge_loc.push_back(new vsol_point_2d(loc_lon, loc_lat));
      }
      if (edge_loc.size()>0)
        edge_locs.push_back(edge_loc);
    }
    // upsample the locations to desired density
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > sampled_edge_locs;
    for (unsigned e_idx = 0; e_idx < edge_locs.size(); e_idx++) {
        vcl_vector<vsol_point_2d_sptr> sampled_locs;
        this->upsample_location_list(edge_locs[e_idx],sampled_locs);
        sampled_edge_locs.push_back(sampled_locs);
    }
    // put the valid locations into database
    for (unsigned e_idx = 0; e_idx < sampled_edge_locs.size(); e_idx++)
      for (vcl_vector<vsol_point_2d_sptr>::iterator vit = sampled_edge_locs[e_idx].begin();  vit != sampled_edge_locs[e_idx].end(); ++vit)
        this->add_locations(vgl_point_2d<double>((*vit)->x(), (*vit)->y()), land_id);
  } // end of loop over all possible land id of input image
  return true;
}

// add locations from a list of location points
bool volm_conf_land_map_indexer::add_locations(vcl_vector<vgl_point_2d<double> > const& locs, unsigned char const& land, double density)
{
  // obtain the line segment that lies inside the leaf
  vcl_vector<vgl_point_2d<double> > locs_in;
  if (!volm_io_tools::line_inside_the_box(bbox_, locs, locs_in))
    locs_in = locs;
  // upsample the input location lines to desired density
  unsigned num_locs = locs_in.size();
  vcl_vector<vsol_point_2d_sptr> in_list;
  for (vcl_vector<vgl_point_2d<double> >::const_iterator vit = locs_in.begin(); vit != locs_in.end(); ++vit)
    in_list.push_back(new vsol_point_2d(*vit));

  vcl_vector<vsol_point_2d_sptr> out_list;
  this->upsample_location_list(in_list, out_list, density);
  // put the upsampled locations into database
  for (unsigned i = 0; i < out_list.size(); i++)
    this->add_locations(vgl_point_2d<double>(out_list[i]->x(), out_list[i]->y()), land);
  return true;
}

// add locations from a region boundary
bool volm_conf_land_map_indexer::add_locations(vgl_polygon<double> const& poly, unsigned char const& land)
{
  // upsample the input polygon and generate the location list
  vcl_vector<vsol_point_2d_sptr> out_locs;
  this->upsample_region_boundary(poly, out_locs);
  // put the upsampled locations into database
  for (unsigned i = 0; i < out_locs.size(); i++)
    this->add_locations(vgl_point_2d<double>(out_locs[i]->x(), out_locs[i]->y()), land);
  return true;
}

// add locations by searching over the intersections inside a line network
bool volm_conf_land_map_indexer::add_locations(vcl_vector<vcl_vector<vgl_point_2d<double> > > const& lines, vcl_vector<unsigned char> const& lines_prop)
{
  if (lines.size() != lines_prop.size()) {
    vcl_cerr << " In volm_conf_land_map_indexer::add_location: number of defined line properties is unequal to number of lines in the network" << vcl_endl;
    return false;
  }
  // look for the lines that inside the region
  double max_lx, max_ly, max_lz;
  lvcs_->global_to_local(bbox_.max_x(), bbox_.max_y(), 0.0, vpgl_lvcs::wgs84, max_lx, max_ly, max_lz);
  vcl_vector<vcl_vector<vgl_point_2d<double> > > lines_in_local;
  vcl_vector<volm_land_layer> lines_in_prop;
  unsigned n_lines = lines.size();
  for (unsigned r_idx = 0; r_idx < n_lines; r_idx++)
  {
    vcl_vector<vgl_point_2d<double> > line_global;
    // check and obtain the road segment that lies inside the region
    if (!volm_io_tools::line_inside_the_box(bbox_, lines[r_idx], line_global))
      continue;
    // go from global coord to local coord
    vcl_vector<vgl_point_2d<double> > line_local;
    for (unsigned p_idx = 0; p_idx < line_global.size(); p_idx++)
    {
      if (!bbox_.contains(line_global[p_idx]))
        continue;
      double lx, ly, lz;
      lvcs_->global_to_local(line_global[p_idx].x(), line_global[p_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      line_local.push_back(vgl_point_2d<double>(lx, ly));
    }
    if (line_local.size() >= 2) {
      lines_in_local.push_back(line_local);
      lines_in_prop.push_back(volm_osm_category_io::volm_land_table[lines_prop[r_idx]]);
    }
  }

  // find all intersection points
  unsigned n_lines_in = lines_in_local.size();
  for (unsigned r_idx = 0; r_idx < n_lines_in; r_idx++)
  {
    vcl_vector<vgl_point_2d<double> > curr_line = lines_in_local[r_idx];
    volm_land_layer curr_prop = lines_in_prop[r_idx];
    vcl_vector<vcl_vector<vgl_point_2d<double> > > net;
    vcl_vector<volm_land_layer> net_props;
    for (unsigned i = 0; i < n_lines_in; i++)
      if ( i != r_idx ) {
        net.push_back(lines_in_local[i]);  net_props.push_back(lines_in_prop[i]);
      }
    // find all intersections for current road
    vcl_vector<vgl_point_2d<double> > cross_pts;
    vcl_vector<volm_land_layer> cross_props;
    if (!volm_io_tools::search_junctions(curr_line, curr_prop, net, net_props, cross_pts, cross_props)) {
      vcl_cerr << "In volm_conf_land_map_indexer::add_location: find line intersection failed for line " << r_idx << vcl_endl;
      return false;
    }
    // put the intersections points into database
    for (unsigned i = 0; i < cross_pts.size(); i++)
    {
      // transfer back to global coordinates
      double lat, lon, gz;
      lvcs_->local_to_global(cross_pts[i].x(), cross_pts[i].y(), 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
      if ( volm_osm_category_io::volm_land_table[cross_props[i].id_].name_ == "invalid")
        continue;
      if ( vcl_find(land_locs_[cross_props[i].id_].begin(), land_locs_[cross_props[i].id_].end(), vgl_point_2d<double>(lon, lat)) == land_locs_[cross_props[i].id_].end() )
        this->add_locations(vgl_point_2d<double>(lon, lat), cross_props[i].id_);
    }
  }
  return true;
}

// upsample given list of points
void volm_conf_land_map_indexer::upsample_location_list(vcl_vector<vsol_point_2d_sptr> const& in_locs, vcl_vector<vsol_point_2d_sptr>& out_locs, double const& density)
{
  out_locs.clear();
  // case where there is single location
  if (in_locs.size() == 1) {
    out_locs.push_back(in_locs[0]);
    return ;
  }
  double dist_interval = density_;
  if (density > 0)  dist_interval = density;
  double ori_lon, ori_lat, ori_elev;
  lvcs_->get_origin(ori_lon, ori_lat, ori_elev);
  // transfer location from wgs84 to local coord
  vcl_vector<vsol_point_2d_sptr> locs;
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator vit = in_locs.begin();  vit != in_locs.end(); ++vit) {
    double lx, ly, lz;
    lvcs_->global_to_local((*vit)->x(), (*vit)->y(), ori_elev, vpgl_lvcs::wgs84, lx, ly, lz);
    locs.push_back(new vsol_point_2d(lx, ly));  
  }

  // upsample the locations given density_
  unsigned ori_size = locs.size();
  vcl_vector<vsol_point_2d_sptr> new_locs;
  new_locs.push_back(locs[0]);
  for (unsigned i = 1; i < ori_size; i++)
  {
    vgl_vector_2d<double> seg_vec  = locs[i]->get_p() - locs[i-1]->get_p();
    vgl_vector_2d<double> unit_vec = normalized(seg_vec);
    double len = seg_vec.length();
    if (len < dist_interval) {
      // insert end point on current segment
      if (vcl_find(new_locs.begin(), new_locs.end(), locs[i]) == new_locs.end())
        new_locs.push_back(locs[i]);
      continue;
    }
    unsigned cnt = 0;
    while ( (len - cnt*dist_interval) > 1.5*dist_interval) {
      cnt++;
      double nx = cnt*dist_interval*unit_vec.x() + locs[i-1]->x();
      double ny = cnt*dist_interval*unit_vec.y() + locs[i-1]->y();
      vsol_point_2d_sptr new_pt = new vsol_point_2d(nx, ny);
      if (vcl_find(new_locs.begin(), new_locs.end(), new_pt) == new_locs.end())
        new_locs.push_back(new vsol_point_2d(nx, ny));
    }
    // insert end point on current segment
    new_locs.push_back(locs[i]);
  }

  // transfer back to global coordinates
  for (vcl_vector<vsol_point_2d_sptr>::iterator vit = new_locs.begin();  vit != new_locs.end(); ++vit) {
    double lon, lat, gz;
    lvcs_->local_to_global((*vit)->x(), (*vit)->y(), 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
    out_locs.push_back(new vsol_point_2d(lon, lat));
  }
  return;
}

// upsample the boundary of a input polygon
void volm_conf_land_map_indexer::upsample_region_boundary(vgl_polygon<double> const& poly, vcl_vector<vsol_point_2d_sptr>& out_locs, double const& density)
{
  out_locs.clear();

  if (!vgl_intersection(bbox_, poly))
    return;

  // upsample each sheet of the input polygon
  unsigned num_sheet = poly.num_sheets();
  for (unsigned s_idx = 0; s_idx < num_sheet; s_idx++)
  {
    unsigned num_vertices = poly[s_idx].size();
    // upsample point 0 to point n-1 line segment
    vcl_vector<vsol_point_2d_sptr> in_locs;
    for (unsigned v_idx = 0; v_idx < num_vertices; v_idx++)
      in_locs.push_back(new vsol_point_2d(poly[s_idx][v_idx]));
    vcl_vector<vsol_point_2d_sptr> out_list;
    this->upsample_location_list(in_locs, out_list, density);

    for (unsigned i = 0; i < out_list.size(); i++)
      out_locs.push_back(out_list[i]);

    // upsample last segment
    in_locs.clear();
    in_locs.push_back(new vsol_point_2d(poly[s_idx][num_vertices-1]));
    in_locs.push_back(new vsol_point_2d(poly[s_idx][0]));
    this->upsample_location_list(in_locs, out_list, density);
    for (unsigned i = 1; i < (out_list.size()-1); i++)
      out_locs.push_back(out_list[i]);
  }
  return;
}

// visualize locations database in kml
void volm_conf_land_map_indexer::write_out_kml_locs(vcl_ofstream& ofs,
                                                    vcl_vector<vgl_point_2d<double> > const& locations, unsigned char land_id,
                                                    double const& size,  bool const& is_write_as_dot) const
{
  unsigned num_locs = locations.size();
  for (unsigned i = 0; i < num_locs; i++)
  {
    double lon = locations[i].x();
    double lat = locations[i].y();
    unsigned char land = land_id;
    vcl_stringstream name;
    name << i << "_" << vcl_setprecision(6) << lon << '_' << vcl_setprecision(6) << lat << '_' <<  volm_osm_category_io::volm_land_table[land_id].name_;
    if (is_write_as_dot) {
      vgl_point_2d<double> pt(lon, lat);
      bkml_write::write_location(ofs, pt, name.str(), "", 0.6);
    } else {
      vnl_double_2 ul, ll, lr, ur;
      ll[0] = lat; ll[1] = lon;
      ul[0] = lat+size; ul[1] = lon;
      lr[0] = lat; lr[1] = lon+size;
      ur[0] = lat+size; ur[1] = lon+size;
      vil_rgb<vxl_byte> color = volm_osm_category_io::volm_land_table[land_id].color_;
      bkml_write::write_box(ofs, name.str(), "", ul, ur, ll, lr, color.r, color.g, color.b);
    }
  }
  return;
}

bool volm_conf_land_map_indexer::write_out_kml(vcl_string const& kml_file, unsigned char const& land_id, double const& size,  bool const& is_write_as_dot) const
{
  if (land_locs_.find(land_id) == land_locs_.end()) {
    vcl_cout << "In volm_conf_land_map_indexer: no land id " << (int)land_id << " in database, kml won't be created" << vcl_endl;
    return false;
  }

  vcl_ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  //// write out the bounding box
  //bkml_write::write_box(ofs, this->box_string(), "", bbox_);

  // write out locations
  vcl_vector<vgl_point_2d<double> > locations = land_locs_.find(land_id)->second;
  this->write_out_kml_locs(ofs, locations, land_id, size, is_write_as_dot);
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

bool volm_conf_land_map_indexer::write_out_kml(vcl_string const& kml_file, double const& size,  bool const& is_write_as_dot) const
{
  if (land_locs_.empty()) {
    vcl_cout << "In volm_conf_land_map_indexer: no location in database, kml won't be created" << vcl_endl;
    return false;
  }
  vcl_ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  //// write out the bounding box
  //bkml_write::write_box(ofs, this->box_string(), "", bbox_);
  // loop over all land type in database to 
  for (volm_conf_loc_map::const_iterator mit = land_locs_.begin(); mit != land_locs_.end(); ++mit)
  {
    unsigned land_id = mit->first;
    vcl_vector<vgl_point_2d<double> > locations = mit->second;
    this->write_out_kml_locs(ofs, locations, land_id, size, is_write_as_dot);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

// binary write out
bool volm_conf_land_map_indexer::write_out_bin(vcl_string const& bin_file) const
{
  if (land_locs_.empty())
    return true;
  vsl_b_ofstream os(bin_file.c_str());
  if (!os)
    return false;
  this->b_write(os);
  os.close();
  return true;
}

// binary write
void volm_conf_land_map_indexer::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, this->version());
  vsl_b_write(os, bbox_);
  vsl_b_write(os, density_);
  vsl_b_write(os, land_locs_);
  return;
}

// binary read
void volm_conf_land_map_indexer::b_read(vsl_b_istream& is)
{
  bbox_.empty();
  land_locs_.clear();
  unsigned ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
    case 1:
      {
        vsl_b_read(is, bbox_);
        vsl_b_read(is, density_);
        vsl_b_read(is, land_locs_);
        // create the lvcs
        double lon_min, lat_min;
        lon_min = bbox_.min_x();
        lat_min = bbox_.min_y();
        lvcs_ = new vpgl_lvcs(lat_min, lon_min, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
        break;
      }
    default:
      vcl_cerr << "I/O ERROR: volm_conf_land_map_indexer::b_read(vsl_b_istream&): Unknown version number " << ver << vcl_endl;
      is.is().clear(vcl_ios::badbit); // set an unrecoverable IO error on stream
  }
  return;
}