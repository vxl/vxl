#include "volm_candidate_list.h"
//:
// \file
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_area.h>
#include <vgl/vgl_area.hxx>

volm_candidate_list::volm_candidate_list(vil_image_view<vxl_byte> const& image,
                                         unsigned threshold)
  : thres_(threshold), image_(image)
{
  hull_poly_.clear();
  // search for the sorted pixels that are larger than threshold
  //mymap::iterator mit = pt_map_.begin();
  mymap::iterator mit;
  for (unsigned u = 0; u < image.ni(); u++)
    for (unsigned v = 0; v < image.nj(); v++)
      if (image(u,v) > thres_)
        pt_map_.insert(std::pair<unsigned, vgl_point_2d<int> >(image(u,v), vgl_point_2d<int>(u,v)));

  // create candidate polygons based on searched pixels
  for (mit = pt_map_.begin(); mit != pt_map_.end(); ++mit) {
    bool is_contain = false;;
    for (unsigned sh_idx = 0; (!is_contain && sh_idx < poly_.num_sheets()); sh_idx++) {
      is_contain = this->contains(sh_idx, mit->second.x(), mit->second.y());
    }
    if (is_contain)
      continue;
    std::vector<vgl_point_2d<int> > sheet;
    //sheet.push_back(mit->second);
    std::vector<int> bi,bj;
    vil_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(thres_),mit->second.x(),mit->second.y());
    for (unsigned i = 0; i < bi.size(); i++) {
      if (!poly_.contains(bi[i],bj[i]))
        sheet.emplace_back(bi[i],bj[i]);
    }
    if(sheet.size() == 0) sheet.push_back(mit->second);
    poly_.push_back(sheet);
    // create a hull polygon associated with current sheet
    this->create_expand_polygon(sheet);
  }
  n_sheet_ = poly_.num_sheets();

}

bool volm_candidate_list::create_expand_polygon(std::vector<vgl_point_2d<int> > const& sheet)
{
  auto n_points = (unsigned)sheet.size();
  std::vector<vgl_point_2d<double> > points;
  for (unsigned i = 0; i < n_points; i++) {
    points.emplace_back(sheet[i].x()+1.0, sheet[i].y()+1.0);
    points.emplace_back(sheet[i].x()-1.0, sheet[i].y()+1.0);
    points.emplace_back(sheet[i].x()-1.0, sheet[i].y()-1.0);
    points.emplace_back(sheet[i].x()-1.0, sheet[i].y()-1.0);
  }
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  hull_poly_.push_back(poly);
  return true;
}

// this function is for speed-up purpose
bool volm_candidate_list::top_locations(std::vector<std::vector<vgl_point_2d<int> > >& top_locs,
                                        std::vector<std::vector<unsigned> >& top_loc_scores)
{
  if (top_locs.size() != n_sheet_)
    return false;
  // initialize the list
  for (unsigned i = 0; i < top_locs.size(); i++) {
    top_locs[i].assign(1,vgl_point_2d<int>(-1,-1));
    top_loc_scores[i].assign(1, 127);
  }
  // create a bounding box to fetch the maximum
  for (unsigned sh_idx = 0; sh_idx < n_sheet_; sh_idx++) {
    auto n_verts = (unsigned)poly_[sh_idx].size();
    vgl_box_2d<int> bbox;
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++)
      bbox.add(poly_[sh_idx][v_idx]);
    int max_u = 0, max_v = 0;
    unsigned max_score = thres_;
    for (int u = bbox.min_x(); u <= bbox.max_x(); u++) {
      for (int v = bbox.min_y(); v <= bbox.max_y(); v++) {
        if (image_(u,v) > max_score ) {
          max_u = u;  max_v = v;  max_score = image_(u,v);
        }
      }
    }
    vgl_point_2d<int> p(max_u, max_v);
    top_locs[sh_idx][0] = p;
    top_loc_scores[sh_idx][0] = max_score;
  }
  return true;
}

bool volm_candidate_list::top_locations(std::vector<std::vector<vgl_point_2d<int> > >& top_locs,
                                        std::vector<std::vector<unsigned> >& top_loc_scores,
                                        unsigned const& size)
{
  if (size == 1)
    return this->top_locations(top_locs, top_loc_scores);

  if (top_locs.size() != n_sheet_)
    return false;
  // initialize the list
  for (unsigned i = 0; i < top_locs.size(); i++) {
    top_locs[i].assign(size,vgl_point_2d<int>(-1,-1));
    top_loc_scores[i].assign(size, 127);
  }
  for (unsigned sh_idx = 0; sh_idx < n_sheet_; sh_idx++) {
    auto n_verts = (unsigned)poly_[sh_idx].size();
    vgl_box_2d<int> bbox;
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++)
      bbox.add(poly_[sh_idx][v_idx]);
    mymap points;

    //std::cout << "\n bounding box are " << bbox << ", points are \n";
    for (int u = bbox.min_x(); u <= bbox.max_x(); u++) {
      for (int v = bbox.min_y(); v <= bbox.max_y(); v++) {
        //assert(u > 0);  assert(v > 0);  assert((unsigned)u < image_.ni());  assert((unsigned)v < image_.nj());
        if (u < 0 || v < 0 || u > (int)image_.ni() || v > (int)image_.nj())
          continue;
        if (this->contains(sh_idx, u, v) && image_(u,v) > thres_ )
          points.insert(std::pair<unsigned, vgl_point_2d<int> >(image_(u,v), vgl_point_2d<int>(u,v)));
      }
    }
#if 0
    for (mymap::iterator mit = points.begin(); mit != points.end(); ++mit)
      std::cout << "\t score = " << mit->first << ", points = " << mit->second << std::endl;
#endif
    // fill the top number of points from map
    auto mit = points.begin();
    if (points.size() < size) {
      unsigned cnt = 0;
      for (; mit != points.end(); ++mit) {
        top_locs[sh_idx][cnt] = mit->second;
        top_loc_scores[sh_idx][cnt++] = mit->first;
      }
    }
    else {
      for (unsigned i = 0; i < size; i++, ++mit) {
        top_locs[sh_idx][i] = mit->second;
        top_loc_scores[sh_idx][i] = mit->first;
      }
    }
  }
  return true;
}

bool volm_candidate_list::top_locations(std::vector<vgl_point_2d<int> >& top_locs, std::vector<unsigned>& top_loc_scores, unsigned const& size, unsigned const& sh_idx)
{
  if (sh_idx > n_sheet_)
    return false;
  std::vector<std::vector<vgl_point_2d<int> > > best_locs(n_sheet_);
  std::vector<std::vector<unsigned> > best_scores(n_sheet_);
  if (!this->top_locations(best_locs,best_scores,size))
    return false;
  for(unsigned i = 0; i < size; i++) {
    top_locs.push_back(best_locs[sh_idx][i]);
    top_loc_scores.push_back(best_scores[sh_idx][i]);
  }
  return true;
}

bool volm_candidate_list::top_locations(std::vector<vgl_point_2d<double> >& top_locs, std::vector<unsigned>& top_loc_scores, volm_tile& tile, unsigned const& size, unsigned const& sh_idx)
{
  std::vector<vgl_point_2d<int> > top_locs_pixel;
  if( !this->top_locations(top_locs_pixel, top_loc_scores, size, sh_idx) )
    return false;
  for (unsigned i = 0; i < size; i++) {
    double lon, lat;
    auto u = (unsigned)top_locs_pixel[i].x();
    auto v = (unsigned)top_locs_pixel[i].y();
    if (u > image_.ni() || v > image_.nj() || image_(u,v) < thres_)
      continue;
    tile.img_to_global((unsigned)top_locs_pixel[i].x(), (unsigned)top_locs_pixel[i].y(), lon, lat);
    top_locs.emplace_back(lon, lat);
  }
  return true;
}

bool volm_candidate_list::img_to_golbal(unsigned const& sh_idx, volm_tile& tile, std::vector<vgl_point_2d<double> >& region_global)
{
  if (sh_idx > n_sheet_)
    return false;
  auto n_point = (unsigned)poly_[sh_idx].size();
  double deg_per_half_pixel_i = 1.0 * tile.scale_i()/(tile.ni() - 1);
  double deg_per_half_pixel_j = 1.0 * tile.scale_j()/(tile.nj() - 1);
  std::vector<vgl_point_2d<double> > points;
  for (unsigned i = 0; i < n_point; i++) {
    double lon, lat;
    tile.img_to_global(poly_[sh_idx][i].x(), poly_[sh_idx][i].y(), lon, lat);
    // expand one pixel location to its 4 corners ( start from top right, arrange counterclock wise
      points.emplace_back(lon + deg_per_half_pixel_i, lat + deg_per_half_pixel_j);
      points.emplace_back(lon - deg_per_half_pixel_i, lat + deg_per_half_pixel_j);
      points.emplace_back(lon - deg_per_half_pixel_i, lat - deg_per_half_pixel_j);
      points.emplace_back(lon + deg_per_half_pixel_i, lat - deg_per_half_pixel_j);
  }

  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  auto vit = poly[0].begin();
  for (; vit != poly[0].end(); ++vit)
    region_global.push_back(*vit);
  return true;
}

bool volm_candidate_list::region_score(std::vector<unsigned>& scores)
{
  std::vector<std::vector<vgl_point_2d<int> > > best_locs(n_sheet_);
  std::vector<std::vector<unsigned> > best_scores(n_sheet_);
  if (!this->top_locations(best_locs,best_scores,1))
    return false;
  for (unsigned sh_idx = 0; sh_idx < n_sheet_; sh_idx++) {
    scores.push_back(best_scores[sh_idx][0]);
  }
  return true;
}

bool volm_candidate_list::find(unsigned const& i, unsigned const& j, unsigned& sh_idx, unsigned& loc_score)
{
  if ( i > image_.ni() || j > image_.nj()) {
    sh_idx = n_sheet_;
    loc_score = image_(i,j);
    return false;
  }
  for (unsigned sh_id = 0; sh_id < n_sheet_; sh_id++) {
    if (this->contains(sh_id, i, j)) {
      sh_idx = sh_id;
      loc_score = image_(i,j);
      return true;
    }
  }
  sh_idx = n_sheet_;
  loc_score = image_(i,j);
  return false;
}

bool volm_candidate_list::contains(unsigned const& sheet_id, unsigned const& u, unsigned const& v)
{
  assert( sheet_id <= hull_poly_.size() && " in candidate list, the polygon sheet id is larger than the size of created polygon ");
  return hull_poly_[sheet_id].contains(u, v);
}

bool volm_candidate_list::candidate_list_image(vil_image_view<vxl_byte>& image)
{
  if (image.ni() != image_.ni() || image.nj() != image_.nj())
    image.set_size(image_.ni(), image_.nj());
  image.fill((vxl_byte)127);
  for (unsigned sh_idx = 0; sh_idx < n_sheet_; sh_idx++) {
    auto n_verts = (unsigned)poly_[sh_idx].size();
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++)
      image(poly_[sh_idx][v_idx].x(), poly_[sh_idx][v_idx].y()) = (vxl_byte)255;
  }
  return true;
}

void volm_candidate_list::open_kml_document(std::ofstream& str, std::string const& name, float const& threshold)
{
  str << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
  str << "  <Document>\n";
  str << "    <name>" << name << "</name>\n";
  str << "    <Style id=\"CR_style\">\n"
      << "      <LabelStyle> <scale>0.0</scale> </LabelStyle>\n"
      << "      <LineStyle> <color>ff0000ff</color> <width>3</width> </LineStyle>\n"
      << "      <PolyStyle> <color>1a0000ff</color> </PolyStyle>\n    </Style>\n";
  str << "    <Style id=\"OR_style\">\n"
      << "      <LabelStyle> <scale>0.0</scale> </LabelStyle>\n"
      << "      <LineStyle> <color>ff0000ff</color><width>1</width> </LineStyle>\n"
      << "      <PolyStyle> <color>1a0000ff</color> </PolyStyle>\n"
      << "    </Style>\n";
  str << "    <Style id=\"CamView_style\">\n"
      << "      <LabelStyle><scale>0.5</scale></LabelStyle>\n"
      << "      <LineStyle> <color>ffff5500</color> <width>3</width> </LineStyle>\n"
      << "      <PolyStyle><color>4a009900</color></PolyStyle>\n"
      << "    </Style>\n";
  str << "    <Style id=\"CameraPoint_style\">\n"
      << "      <LabelStyle> <scale>0.0</scale> </LabelStyle>\n"
      << "      <IconStyle> <scale>0.6</scale> <color>ff0000ff</color>\n"
      << "        <Icon><href>http://google.com/mapfiles/ms/micons/camera.png</href></Icon>\n"
      << "      </IconStyle>\n"
      << "    </Style>\n";
  str << "    <Style id=\"ObjectPoint_style\">\n"
      << "      <LabelStyle> <scale>0.0</scale></LabelStyle>\n"
      << "      <IconStyle><scale>0.6</scale><color>ff0000ff</color>\n"
      << "        <Icon><href>http://google.com/mapfiles/ms/micons/flag.png</href></Icon>\n"
      << "      </IconStyle>\n"
      << "    </Style>\n";
#if 1
  str << "    <!-- The following <Folder> is the Candidate List -->\n";
  str << "    <Folder>\n"
      << "      <name>Candidate List</name>\n"
      << "      <open>1</open>\n"
      << "      <ExtendedData>\n"
      << "        <Data name = \"threshold\">\n"
      << "          <displayName>Likelihood Threshold</displayName>\n"
      << "          <value>" << threshold << "</value>\n"
      << "        </Data>\n"
      << "      </ExtendedData>\n";
#endif
}

void volm_candidate_list::close_kml_document(std::ofstream& str)
{
  str << "    <!-- Here ends all Candidate lists -->\n";
  str << "    </Folder>\n";
  str << "  </Document>\n</kml>\n";
}

void volm_candidate_list::write_kml_regions(std::ofstream& str,
                                            std::vector<vgl_point_2d<double> >& region,
                                            std::vector<vgl_point_2d<double> >& top_locs,
                                            std::vector<cam_angles>& top_cameras,
                                            std::vector<double>& right_fov,
                                            float const& likelihood,
                                            unsigned const& rank)
{
  if (region.size() != 0) {
    str << "      <!-- The following folders are candidate regions -->\n";
    str << "      <!-- Each candidate region contains no more than " << top_locs.size() << " camera estimate -->\n";
    str << "      <Folder>\n";
    str << "        <name>" << rank << "</name>\n";
    str << "        <Placemark>\n";
    str << "          <name>CR" << rank << "</name>\n";
    str << "          <styleUrl>#CR_style</styleUrl>\n";
    str << "          <ExtendedData>\n";
    str << "            <Data name = \"likelihood\">\n"
        << "              <displayName>Likelihood</displayName>\n"
        << "              <value>" << std::setprecision(10) << likelihood << "</value>\n"
        << "            </Data>\n"
        << "          </ExtendedData>\n";
    // write the candidate region polygon
    str << "          <Polygon>\n"
        << "            <tessellate>1</tessellate>\n"
        << "            <outerBoundaryIs>\n"
        << "              <LinearRing>\n"
        << "                <coordinates>\n";
    for (auto & vit : region)
      str << "                " << std::setprecision(12) << vit.x() << ',' << std::setprecision(12) << vit.y() << ",0\n";
    str << "                " << std::setprecision(12) << region[0].x() << ',' << std::setprecision(12) << region[0].y() << ",0\n";
    str << "                </coordinates>\n"
        << "              </LinearRing>\n"
        << "            </outerBoundaryIs>\n"
        << "          </Polygon>\n"
        << "        </Placemark>\n";

    // write the top locations for this region and associate camera associates
    for (unsigned idx = 0; idx < top_locs.size(); idx++) {
      // write the caemra location
      str << "        <Placemark>\n"
          << "          <name>CR" << rank << '_' << idx << " Camera Estimate</name>\n"
          << "          <styleUrl>#CameraPoint_style</styleUrl>\n"
          << "          <Point>\n"
          << "            <altitudeMode>relativeToGround</altitudeMode>\n"
          << "            <coordinates>\n"
          << "              " << std::setprecision(12) << top_locs[idx].x() << ','
                              << std::setprecision(12) << top_locs[idx].y() << ','
                              << "1.6\n"
          << "            </coordinates>\n"
          << "          </Point>\n"
          << "        </Placemark>\n";
      // write the camera photoOverlay
      str << "        <PhotoOverlay>\n"
          << "          <name>CR" << rank << '_' << idx << " Overlay</name>\n"
          << "          <Camera>\n"
          << "            <longitude>" << std::setprecision(12) << top_locs[idx].x() << "</longitude>\n"
          << "            <latitude>"  << std::setprecision(12) << top_locs[idx].y() << "</latitude>\n"
          << "            <altitude>1.6</altitude>\n"
          << "            <heading>" << top_cameras[idx].heading_ << "</heading>\n"
          << "            <tilt>" << top_cameras[idx].tilt_ << "</tilt>\n"
          << "            <roll>" << top_cameras[idx].roll_ << "</roll>\n"
          << "            <altitudeMode>relativeToGround</altitudeMode>\n"
          << "          </Camera>\n"
          << "          <Icon><href>p1a_res06_dirtroad_031.jpg</href></Icon>\n"
          << "          <ViewVolume>\n"
          << "            <leftFov>" << -1*right_fov[idx] << "</leftFov>\n"
          << "            <rightFov>" << right_fov[idx] << "</rightFov>\n"
          << "            <bottomFov>" << -1*top_cameras[idx].top_fov_ << "</bottomFov>\n"
          << "            <topFov>" << top_cameras[idx].top_fov_ << "</topFov>\n"
          << "            <near>.56</near>\n"
          << "          </ViewVolume>\n"
          << "        </PhotoOverlay>\n";
    }
    str << "      </Folder>\n\n";
  }

}

void volm_candidate_list::write_kml_regions(std::ofstream& str,
                                            std::vector<vgl_point_2d<double> >& region,
                                            vgl_point_2d<double>& top_loc,
                                            cam_angles const& camera,
                                            double const& right_fov,
                                            float const& likelihood,
                                            unsigned const& rank)
{
  std::vector<vgl_point_2d<double> > loc_vec;  loc_vec.push_back(top_loc);
  std::vector<cam_angles> cam_vec;             cam_vec.push_back(camera);
  std::vector<double> right_fov_vec;           right_fov_vec.push_back(right_fov);

  volm_candidate_list::write_kml_regions(str, region, loc_vec, cam_vec, right_fov_vec, likelihood, rank);
}

void volm_candidate_list::write_kml_regions(std::ofstream& str,
                                            std::vector<vgl_point_2d<double> >& region,
                                            vgl_point_2d<double>& top_loc,
                                            std::vector<vgl_point_2d<double> >&  /*heading*/,
                                            std::vector<vgl_point_2d<double> >& viewing,
                                            std::vector<vgl_point_2d<double> >& landmarks,
                                            std::vector<unsigned char>& landmark_types,
                                            float const& likelihood,
                                            unsigned const& rank)
{
  if (region.size() != 0) {
    str << "      <!-- The following folders are candidate regions -->\n";
    str << "      <!-- Each candidate region contains no more than 1 camera estimate -->\n";
    str << "      <Folder>\n";
    str << "        <name>Candidate " << rank << "</name>\n";
    str << "        <Placemark>\n";
    str << "          <name>Region" << rank << "</name>\n";
    str << "          <styleUrl>#CR_style</styleUrl>\n";
    str << "          <ExtendedData>\n";
    str << "            <Data name = \"likelihood\">\n"
        << "              <displayName>Likelihood</displayName>\n"
        << "              <value>" << std::setprecision(10) << likelihood << "</value>\n"
        << "            </Data>\n"
        << "          </ExtendedData>\n";
    // write the candidate region polygon
    str << "          <Polygon>\n"
        << "            <tessellate>1</tessellate>\n"
        << "            <outerBoundaryIs>\n"
        << "              <LinearRing>\n"
        << "                <coordinates>\n";
    for (auto & vit : region)
      str << "                " << std::setprecision(12) << vit.x() << ',' << std::setprecision(12) << vit.y() << ",0\n";
    str << "                " << std::setprecision(12) << region[0].x() << ',' << std::setprecision(12) << region[0].y() << ",0\n";
    str << "                </coordinates>\n"
        << "              </LinearRing>\n"
        << "            </outerBoundaryIs>\n"
        << "          </Polygon>\n"
        << "        </Placemark>\n";
    // write the top location for this region
    str << "        <Placemark>\n"
        << "          <name>Region" << rank << "_Camera Estimate</name>\n"
        << "          <styleUrl>#CameraPoint_style</styleUrl>\n"
        << "          <Point>\n"
        << "            <altitudeMode>relativeToGround</altitudeMode>\n"
        << "            <coordinates>\n"
        << "              " << std::setprecision(12) << top_loc.x() << ','
                            << std::setprecision(12) << top_loc.y() << ','
                            << "1.6\n"
        << "            </coordinates>\n"
        << "          </Point>\n"
        << "        </Placemark>\n";

    // write the heading direction (represented as a line)
    //str << "        <Placemark>\n"
    //    << "          <name>CR" << rank << "_1_Camera_Heading</name>\n"
    //    << "          <styleUrl>#CamView_style</styleUrl>\n"
    //    << "          <LineString>\n"
    //    << "            <tessellate>1</tessellate>\n"
    //    << "            <coordinates>\n";
    //for (std::vector<vgl_point_2d<double> >::iterator vit = heading.begin(); vit != heading.end(); ++vit)
    //  str << "              " << std::setprecision(12) << vit->x() << ',' << std::setprecision(12) << vit->y() << ",0 ";
    //str << "            </coordinates>\n"
    //    << "          </LineString>\n"
    //    << "        </Placemark>\n";
    // write the camera viewing
    str << "        <Placemark>\n"
        << "          <name>Region" << rank << "_Camera_Heading</name>\n"
        << "          <styleUrl>#CamView_style</styleUrl>\n"
        << "          <Polygon>\n"
        << "            <tessellate>1</tessellate>\n"
        << "            <outerBoundaryIs>\n"
        << "              <LinearRing>\n"
        << "                <coordinates>\n";
    for (auto & vit : viewing)
      str << "              " << std::setprecision(12) << vit.x() << ',' << std::setprecision(12) << vit.y() << ",0\n";
    str << "                </coordinates>\n"
        << "              </LinearRing>\n"
        << "            </outerBoundaryIs>\n"
        << "          </Polygon>\n"
        << "        </Placemark>\n";

    // write the landmarks
    str << "        <Folder>\n"
        << "          <name>Landmarks</name>\n";
    for (unsigned l_idx = 0; l_idx < landmarks.size(); l_idx++)
    {
      str << "          <Placemark>\n"
          << "            <name>" << volm_osm_category_io::volm_land_table[landmark_types[l_idx]].name_ << "_" << rank << "</name>\n"
          << "            <Point>\n"
          << "              <altitudeMode>relativeToGround</altitudeMode>\n"
          << "              <coordinates>\n"
          << "                " << std::setprecision(12) << landmarks[l_idx].x() << ','
                                << std::setprecision(12) << landmarks[l_idx].y() << ','
                                << "0.0\n"
          << "              </coordinates>\n"
          << "            </Point>\n"
          << "          </Placemark>\n";
    }
    str << "        </Folder>\n";
    str << "      </Folder>\n\n";
  }
}

bool volm_candidate_list::generate_pin_point_circle(vgl_point_2d<double> const& center, double const& radius, std::vector<vgl_point_2d<double> >& circle)
{
  // construct a local lvcs
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(center.y(), center.x(), 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double deg_to_rad = vnl_math::pi_over_180;
  double d_theta = 6*deg_to_rad;
  double theta = 0;
  while (theta < vnl_math::twopi)
  {
    double dx = radius * std::cos(theta);
    double dy = radius * std::sin(theta);
    double lon, lat, gz;
    lvcs->local_to_global(dx, dy, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
    circle.emplace_back(lon, lat);
    theta += d_theta;
  }
  return true;
}

// generate a heading directional line given the camera center and heading direction
// Note that the input heading angular value is relative to East
bool volm_candidate_list::generate_heading_direction(vgl_point_2d<double> const& center, float const& heading_angle, float const& length, float const& right_fov,
                                                     std::vector<vgl_point_2d<double> >& heading_line,
                                                     std::vector<vgl_point_2d<double> >& viewing)
{
  // create a local lvcs
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(center.y(), center.x(), 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double lx = length * std::cos(heading_angle);
  double ly = length * std::sin(heading_angle);
  double e_lon, e_lat, e_gz;
  lvcs->local_to_global(lx, ly, 0.0, vpgl_lvcs::wgs84, e_lon, e_lat, e_gz);
  heading_line.push_back(center);
  heading_line.emplace_back(e_lon, e_lat);

  // generate camera viewing volume
  float left_angle  = heading_angle - right_fov;
  float right_angle = heading_angle + right_fov;
  while(left_angle < 0)
    left_angle += vnl_math::twopi;
  while(right_angle > vnl_math::twopi)
    right_angle -= vnl_math::twopi;
  viewing.push_back(center);
  double lon, lat, gz;
  lx = length * std::cos(left_angle);
  ly = length * std::sin(left_angle);
  lvcs->local_to_global(lx, ly, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
  viewing.emplace_back(lon, lat);
  lx = length * std::cos(right_angle);
  ly = length * std::sin(right_angle);
  lvcs->local_to_global(lx, ly, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
  viewing.emplace_back(lon, lat);
  viewing.push_back(center);
  return true;
}

// check whether the given point is inside the polygon (avoid using polygon contain method because we may have overlapped sheets)
bool volm_candidate_list::inside_candidate_region(vgl_polygon<double> const& cand_poly, double const& lon, double const& lat)
{
  return volm_candidate_list::inside_candidate_region(cand_poly, vgl_point_2d<double>(lon, lat));
}

bool volm_candidate_list::inside_candidate_region(vgl_polygon<double> const& cand_poly, vgl_point_2d<double> const& pt)
{
  for (unsigned i = 0; i < cand_poly.num_sheets(); i++)
  {
    vgl_polygon<double> single_sheet(cand_poly[i]);
    if (single_sheet.contains(pt))
      return true;
  }
  return false;
}

bool volm_candidate_list::inside_candidate_region(vgl_polygon<double> const& cand_poly_in, vgl_polygon<double> const& cand_poly_out, double const& lon, double const& lat)
{
  return volm_candidate_list::inside_candidate_region(cand_poly_in, cand_poly_out, vgl_point_2d<double>(lon, lat));
}

bool volm_candidate_list::inside_candidate_region(vgl_polygon<double> const& cand_poly_in, vgl_polygon<double> const& cand_poly_out, vgl_point_2d<double> const& pt)
{
  // check whether the point is inside any of the inner boundary of candidate region
  unsigned num_in = cand_poly_in.num_sheets();
  for (unsigned i = 0; i < num_in; i++) {
    vgl_polygon<double> single_sheet(cand_poly_in[i]);
    if (single_sheet.contains(pt))
      return false;
  }
  // point is not in any of the inner region, check whether it is inside the outer boundary
  unsigned num_out = cand_poly_out.num_sheets();
  for (unsigned i = 0; i < num_out; i++) {
    vgl_polygon<double> single_sheet(cand_poly_out[i]);
    if (single_sheet.contains(pt))
      return true;
  }
  return false;
}
