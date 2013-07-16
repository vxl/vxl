#include "volm_candidate_list.h"
//:
// \file
#include <vcl_cassert.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_area.txx>

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
        pt_map_.insert(vcl_pair<unsigned, vgl_point_2d<int> >(image(u,v), vgl_point_2d<int>(u,v)));

  // create candidate polygons based on searched pixels
  for (mit = pt_map_.begin(); mit != pt_map_.end(); ++mit) {
    bool is_contain = false;;
    for (unsigned sh_idx = 0; (!is_contain && sh_idx < poly_.num_sheets()); sh_idx++) {
      is_contain = this->contains(sh_idx, mit->second.x(), mit->second.y());
    }
    if (is_contain)
      continue;
    vcl_vector<vgl_point_2d<int> > sheet;
    //sheet.push_back(mit->second);
    vcl_vector<int> bi,bj;
    vil_find_4con_boundary_above_threshold(bi,bj,image,vxl_byte(thres_),mit->second.x(),mit->second.y());
    for (unsigned i = 0; i < bi.size(); i++) {
      if (!poly_.contains(bi[i],bj[i]))
        sheet.push_back(vgl_point_2d<int>(bi[i],bj[i]));
    }
    if(sheet.size() == 0) sheet.push_back(mit->second);
    poly_.push_back(sheet);
    // create a hull polygon associated with current sheet
    this->create_expand_polygon(sheet);
  }
  n_sheet_ = poly_.num_sheets();

}

bool volm_candidate_list::create_expand_polygon(vcl_vector<vgl_point_2d<int> > const& sheet)
{
  unsigned n_points = (unsigned)sheet.size();
  vcl_vector<vgl_point_2d<double> > points;
  for (unsigned i = 0; i < n_points; i++) {
    points.push_back(vgl_point_2d<double>(sheet[i].x()+0.5, sheet[i].y()+0.5));
    points.push_back(vgl_point_2d<double>(sheet[i].x()-0.5, sheet[i].y()+0.5));
    points.push_back(vgl_point_2d<double>(sheet[i].x()-0.5, sheet[i].y()-0.5));
    points.push_back(vgl_point_2d<double>(sheet[i].x()-0.5, sheet[i].y()-0.5));
  }
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  hull_poly_.push_back(poly);
  return true;
}

// this function is for speed-up purpose
bool volm_candidate_list::top_locations(vcl_vector<vcl_vector<vgl_point_2d<int> > >& top_locs,
                                        vcl_vector<vcl_vector<unsigned> >& top_loc_scores)
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
    unsigned n_verts = (unsigned)poly_[sh_idx].size();
    vgl_box_2d<int> bbox;
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++)
      bbox.add(poly_[sh_idx][v_idx]);
    int max_u, max_v;
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

bool volm_candidate_list::top_locations(vcl_vector<vcl_vector<vgl_point_2d<int> > >& top_locs,
                                        vcl_vector<vcl_vector<unsigned> >& top_loc_scores,
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
    unsigned n_verts = (unsigned)poly_[sh_idx].size();
    vgl_box_2d<int> bbox;
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++)
      bbox.add(poly_[sh_idx][v_idx]);
    mymap points;
    
    //vcl_cout << "\n bounding box are " << bbox << ", points are \n";
    for (int u = bbox.min_x(); u <= bbox.max_x(); u++) {
      for (int v = bbox.min_y(); v <= bbox.max_y(); v++) {
        //assert(u > 0);  assert(v > 0);  assert((unsigned)u < image_.ni());  assert((unsigned)v < image_.nj());
        if (u < 0 || v < 0 || u > (int)image_.ni() || v > (int)image_.nj())
          continue;
        if (this->contains(sh_idx, u, v) && image_(u,v) > thres_ )
          points.insert(vcl_pair<unsigned, vgl_point_2d<int> >(image_(u,v), vgl_point_2d<int>(u,v)));
      }
    }
#if 0
    for (mymap::iterator mit = points.begin(); mit != points.end(); ++mit)
      vcl_cout << "\t score = " << mit->first << ", points = " << mit->second << vcl_endl;
#endif
    // fill the top number of points from map
    mymap::iterator mit = points.begin();
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

bool volm_candidate_list::top_locations(vcl_vector<vgl_point_2d<int> >& top_locs, vcl_vector<unsigned>& top_loc_scores, unsigned const& size, unsigned const& sh_idx)
{
  if (sh_idx > n_sheet_)
    return false;
  vcl_vector<vcl_vector<vgl_point_2d<int> > > best_locs(n_sheet_);
  vcl_vector<vcl_vector<unsigned> > best_scores(n_sheet_);
  if (!this->top_locations(best_locs,best_scores,size))
    return false;
  for(unsigned i = 0; i < size; i++) {
    top_locs.push_back(best_locs[sh_idx][i]);
    top_loc_scores.push_back(best_scores[sh_idx][i]);
  }
  return true;
}

bool volm_candidate_list::top_locations(vcl_vector<vgl_point_2d<double> >& top_locs, vcl_vector<unsigned>& top_loc_scores, volm_tile& tile, unsigned const& size, unsigned const& sh_idx)
{
  vcl_vector<vgl_point_2d<int> > top_locs_pixel;
  if( !this->top_locations(top_locs_pixel, top_loc_scores, size, sh_idx) )
    return false;
  for (unsigned i = 0; i < size; i++) {
    double lon, lat;
    unsigned u = (unsigned)top_locs_pixel[i].x();
    unsigned v = (unsigned)top_locs_pixel[i].y();
    if (u < 0 || u > image_.ni() || v < 0 || v > image_.nj() || image_(u,v) < thres_)
      continue;
    tile.img_to_global((unsigned)top_locs_pixel[i].x(), (unsigned)top_locs_pixel[i].y(), lon, lat);
    top_locs.push_back(vgl_point_2d<double>(lon, lat));
  }
  return true;
}

bool volm_candidate_list::img_to_golbal(unsigned const& sh_idx, volm_tile& tile, vcl_vector<vgl_point_2d<double> >& region_global)
{
  if (sh_idx > n_sheet_)
    return false;
  unsigned n_point = (unsigned)poly_[sh_idx].size();
  double deg_per_half_pixel_i = 0.5 * tile.scale_i()/(tile.ni() - 1);
  double deg_per_half_pixel_j = 0.5 * tile.scale_j()/(tile.nj() - 1);
  vcl_vector<vgl_point_2d<double> > points;
  for (unsigned i = 0; i < n_point; i++) {
    double lon, lat;
    tile.img_to_global(poly_[sh_idx][i].x(), poly_[sh_idx][i].y(), lon, lat);
    // expand one pixel location to its 4 corners ( start from top right, arrange counterclock wise
      points.push_back(vgl_point_2d<double>(lon + deg_per_half_pixel_i, lat + deg_per_half_pixel_j));
      points.push_back(vgl_point_2d<double>(lon - deg_per_half_pixel_i, lat + deg_per_half_pixel_j));
      points.push_back(vgl_point_2d<double>(lon - deg_per_half_pixel_i, lat - deg_per_half_pixel_j));
      points.push_back(vgl_point_2d<double>(lon + deg_per_half_pixel_i, lat - deg_per_half_pixel_j));
  }

  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();
  vcl_vector<vgl_point_2d<double> >::iterator vit = poly[0].begin();
  for (; vit != poly[0].end(); ++vit)
    region_global.push_back(*vit);
  return true;
}

bool volm_candidate_list::region_score(vcl_vector<unsigned>& scores)
{
  vcl_vector<vcl_vector<vgl_point_2d<int> > > best_locs(n_sheet_);
  vcl_vector<vcl_vector<unsigned> > best_scores(n_sheet_);
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
    unsigned n_verts = (unsigned)poly_[sh_idx].size();
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++)
      image(poly_[sh_idx][v_idx].x(), poly_[sh_idx][v_idx].y()) = (vxl_byte)255;
  }
  return true;
}

void volm_candidate_list::open_kml_document(vcl_ofstream& str, vcl_string const& name, float const& threshold)
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
}

void volm_candidate_list::close_kml_document(vcl_ofstream& str)
{
  str << "    <!-- Here ends the first Candidate list -->\n";
  str << "    </Folder>\n";
  str << "  </Document>\n</kml>\n";
}

void volm_candidate_list::write_kml_regions(vcl_ofstream& str,
                                                  vcl_vector<vgl_point_2d<double> >& region,
                                                  vcl_vector<vgl_point_2d<double> >& top_locs,
                                                  vcl_vector<cam_angles>& top_cameras,
                                                  vcl_vector<double>& right_fov,
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
        << "              <value>" << vcl_setprecision(10) << likelihood << "</value>\n"
        << "            </Data>\n"
        << "          </ExtendedData>\n";
    // write the candidate region polygon
    str << "          <Polygon>\n"
        << "            <tessellate>1</tessellate>\n"
        << "            <outerBoundaryIs>\n"
        << "              <LinearRing>\n"
        << "                <coordinates>\n";
    for (vcl_vector<vgl_point_2d<double> >::iterator vit = region.begin(); vit != region.end(); ++vit)
      str << "                " << vcl_setprecision(12) << vit->x() << ',' << vcl_setprecision(12) << vit->y() << ",0\n";
    str << "                " << vcl_setprecision(12) << region[0].x() << ',' << vcl_setprecision(12) << region[0].y() << ",0\n";
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
          << "              " << vcl_setprecision(12) << top_locs[idx].x() << ','
                              << vcl_setprecision(12) << top_locs[idx].y() << ','
                              << "1.6\n"
          << "            </coordinates>\n"
          << "          </Point>\n"
          << "        </Placemark>\n";
      // write the camera photoOverlay
      str << "        <PhotoOverlay>\n"
          << "          <name>CR" << rank << '_' << idx << " Overlay</name>\n"
          << "          <Camera>\n"
          << "            <longitude>" << vcl_setprecision(12) << top_locs[idx].x() << "</longitude>\n"
          << "            <latitude>"  << vcl_setprecision(12) << top_locs[idx].y() << "</latitude>\n"
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