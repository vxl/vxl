//:
// \file
#include "bkml_write.h"
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::string rgb_color_to_hex_color(int alpha, int rNum, int gNum, int bNum)
{
  std::string result;
  char a[255];
  std::snprintf(a, 255, "%.2x", alpha);
  result.append(a );
  char b[255];
  std::snprintf(b, 255, "%.2x", bNum);
  result.append(b );
  char g[255];
  std::snprintf(g, 255, "%.2x", gNum);
  result.append(g );
  char r[255];
  std::snprintf(r, 255, "%.2x", rNum);
  result.append(r );
  return result;
}

//: Write KML header and open document tag
void bkml_write::open_document(std::ofstream& str)
{
  str << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n<Document>\n";
  return;
}

//: end document tag
void bkml_write::close_document(std::ofstream& str)
{
  str << "</Document>\n</kml>\n";
}

//: Write a box
void bkml_write::write_box(std::ofstream &ofs, const std::string& name, const std::string& description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr)
{
  ofs.precision(8);
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n"
      << "  <description>" << description << "</description>\n"
      << "  <Style>\n"
      << "    <PolyStyle>\n"
      << "      <colorMode>Random</colorMode>\n"
      << "      <color>ffffffff</color>\n"
      << "      <fill>0</fill>\n"
      << "    </PolyStyle>\n"
      << "    <LineStyle>\n"
      << "      <colorMode>Random</colorMode>\n"
      << "      <color>ffffffff</color>\n"
      << "      <width>3</width>\n"
      << "    </LineStyle>\n"
      << "  </Style>\n"
      << "  <Polygon>\n"
      << "   <tessellate>1</tessellate>\n"
      << "   <outerBoundaryIs>\n"
      << "     <LinearRing>\n"
      << "       <coordinates>\n"
      << "          "
      << std::setprecision(12) << ul[1] << ',' << std::setprecision(12) << ul[0] << ",0  "
      << std::setprecision(12) << ur[1] << ',' << std::setprecision(12) << ur[0] << ",0  "
      << std::setprecision(12) << lr[1] << ',' << std::setprecision(12) << lr[0] << ",0  "
      << std::setprecision(12) << ll[1] << ',' << std::setprecision(12) << ll[0] << ",0  "
      << std::setprecision(12) << ul[1] << ',' << std::setprecision(12) << ul[0] << ",0\n"
      << "       </coordinates>\n"
      << "     </LinearRing>\n"
      << "   </outerBoundaryIs>\n"
      << "  </Polygon>\n"
      << "</Placemark>\n" << std::endl;
}

//: kml requires lon, lat, elev in polygon definition
void bkml_write::write_box(std::ofstream &ofs, std::string name, std::string description, vgl_box_2d<double> bbox)
{
  // in ul x is lat y is lon, in vgl bbox x is lon, y is lat so reverse
  vnl_double_2 ul(bbox.max_y(), bbox.min_x());
  vnl_double_2 ur(bbox.max_y(), bbox.max_x());
  vnl_double_2 ll(bbox.min_y(), bbox.min_x());
  vnl_double_2 lr(bbox.min_y(), bbox.max_x());
  bkml_write::write_box(ofs, std::move(name), std::move(description), ul, ur, ll, lr);
}

//: Write a box with color
void bkml_write::write_box(std::ofstream &ofs, const std::string& name, const std::string& description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr, const std::string& hex_color, unsigned const& fill)
{
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n"
      << "  <description>" << description << "</description>\n"
      << "  <Style>\n"
      << "    <PolyStyle>\n"
      << "      <colorMode>normal</colorMode>\n"
      << "      <color>" << hex_color << "</color>\n"
      << "      <fill>" << fill << "</fill>\n"
      << "    </PolyStyle>\n"
      << "    <LineStyle>\n"
      << "      <colorMode>normal</colorMode>\n"
      << "      <color>" << hex_color << "</color>\n"
      << "      <width>3</width>\n"
      << "    </LineStyle>\n"
      << "  </Style>\n"
      << "  <Polygon>\n"
      << "   <tessellate>1</tessellate>\n"
      << "   <outerBoundaryIs>\n"
      << "     <LinearRing>\n"
      << "       <coordinates>\n"
      << "          "
      << std::setprecision(12) << ul[1] << ',' << std::setprecision(12) << ul[0] << ",0  "
      << std::setprecision(12) << ur[1] << ',' << std::setprecision(12) << ur[0] << ",0  "
      << std::setprecision(12) << lr[1] << ',' << std::setprecision(12) << lr[0] << ",0  "
      << std::setprecision(12) << ll[1] << ',' << std::setprecision(12) << ll[0] << ",0  "
      << std::setprecision(12) << ul[1] << ',' << std::setprecision(12) << ul[0] << ",0\n"
      << "       </coordinates>\n"
      << "     </LinearRing>\n"
      << "   </outerBoundaryIs>\n"
      << "  </Polygon>\n"
      << "</Placemark>\n" << std::endl;
}

// write a box with color
void bkml_write::write_box(std::ofstream &ofs, std::string name, std::string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr,
                           unsigned char const& r, unsigned char const& g, unsigned char const& b, unsigned char const&a, unsigned const& fill)
{
  std::string hex_color = rgb_color_to_hex_color((int)a, (int)r, (int)g, (int)b);
  bkml_write::write_box(ofs, std::move(name), std::move(description), ul, ur, ll, lr, hex_color, fill);
}

// write a polygon with color (only outerBoundary)
void bkml_write::write_polygon(std::ofstream& ofs, vgl_polygon<double> const& poly,
                               std::string const& name,
                               std::string const& description,
                               double const& scale,  double const& line_width, double const& alpha,
                               unsigned char const& r, unsigned char const& g, unsigned char const& b)
{
  // obtain line color
  std::string line_color = rgb_color_to_hex_color(1, (int)r, (int)g, (int)b);

  // obtain polygon color
  int alpha_int = (int)(alpha*255);
  std::string poly_color = rgb_color_to_hex_color(alpha_int, (int)r, (int)g, (int)b);

  unsigned num_sheet = poly.num_sheets();
  for (unsigned s_idx = 0; s_idx < num_sheet; s_idx++) {
    std::vector<vgl_point_2d<double> > verts = poly[s_idx];
    if (verts.empty())
      continue;
    ofs << "<Placemark>\n"
        << "  <name>" << name << "</name>\n";
    if (description.compare("") != 0)
      ofs << "  <description>" << description << "</description>\n";
    ofs << "  <Style>\n"
        << "    <LabelStyle> <scale>" << scale << "</scale> </LabelStyle>\n"
        << "    <LineStyle> <color>" << line_color << "</color> <width>" << line_width << "</width> </LineStyle>\n"
        << "    <PolyStyle>\n"
        << "      <color>" << poly_color << "</color>\n"
        << "      <fill>1</fill>\n"
        << "      <outline>0</outline>\n"
        << "    </PolyStyle>\n"
        << "  </Style>\n";
    ofs << "  <Polygon>\n"
        << "    <tessellate>1</tessellate>\n"
        << "    <outerBoundaryIs>\n"
        << "      <LinearRing>\n"
        << "        <coordinates>\n";
    for (auto & vert : verts)
      ofs << "          " << std::setprecision(12) << vert.x() << ',' << std::setprecision(12) << vert.y() << ",0\n";
    ofs << "          " << verts[0].x() << ',' << verts[0].y() << ",0\n";
    ofs << "        </coordinates>\n"
        << "      </LinearRing>\n"
        << "    </outerBoundaryIs>\n"
        << "  </Polygon>\n"
        << "</Placemark>\n";
  }
}

//: write a path with color and line width
void bkml_write::write_path(std::ofstream& ofs, std::vector<vgl_point_2d<double> > path,
                         std::string const& name,
                         std::string const& description,
                         double const& scale,
                         double const& line_width,
                         double const& alpha,
                         unsigned char const& r,
                         unsigned char const& g,
                         unsigned char const& b)
{
  // obtain line color
  int alpha_int = (int)(alpha*255);
  std::string line_color = rgb_color_to_hex_color(alpha_int, (int)r, (int)g, (int)b);
  if (path.empty())
    return;
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n";
  if (description.compare("") != 0)
    ofs << "  <description>" << description << "</description>\n";
  ofs << "  <Style>\n"
      << "    <LabelStyle> <scale>" << scale << "</scale> </LabelStyle>\n"
      << "    <LineStyle> <color>" << line_color << "</color> <width>" << line_width << "</width> </LineStyle>\n"
      << "  </Style>\n";
  ofs << "  <LineString>\n"
      << "    <tessellate>1</tessellate>\n"
      << "      <coordinates>\n        ";
  for (auto & vit : path)
    ofs << std::setprecision(12) << vit.x() << ',' << std::setprecision(12) << vit.y() << ",0 ";
  ofs << "\n      </coordinates>\n"
      << "  </LineString>\n"
      << "</Placemark>\n";
}

//: put a pin at the given location
void bkml_write::write_location(std::ofstream &ofs, const std::string& name, const std::string& description, double lat, double lon, double elev)
{
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n"
      << "  <description>" << description << "</description>\n"
      << "  <styleUrl>#m_ylw-pushpin</styleUrl>\n"
      << "  <Point>\n"
      << "    <coordinates>" << std::setprecision(12) << lon << ", " << std::setprecision(12) << lat << ", " << elev << "</coordinates>\n"
      << "  </Point>\n"
      << "</Placemark>\n" << std::endl;
}

void bkml_write::write_location(std::ofstream& ofs, vgl_point_2d<double> const& loc,
                                std::string const& name,
                                std::string const& description,
                                double const& scale,
                                unsigned char const& r,
                                unsigned char const& g,
                                unsigned char const& b)
{
  std::string color = rgb_color_to_hex_color(255, (int)r, (int)g, (int)b);
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n";
  if (description.compare("") != 0)
    ofs << "  <description>" << description << "</description>\n";
  ofs << "  <Style>\n"
      << "    <LabelStyle> <scale>" << scale << "</scale> </LabelStyle>\n"
      << "    <IconStyle> <scale>" << scale << "</scale> <color>" << color << "</color>\n"
      << "      <Icon><href>http://maps.google.com/mapfiles/kml/paddle/pink-blank.png</href></Icon>"
      << "    </IconStyle>\n"
      << "  </Style>\n"
      << "  <Point>\n"
      << "    <coordinates>" << std::setprecision(12) << loc.x() << ',' << std::setprecision(12) << loc.y() << ",0</coordinates>\n";
  ofs << "  </Point>\n"
      << "</Placemark>\n";
}

void bkml_write::write_location(std::ofstream& ofs, double lat, double lon, double elev,
                                std::string const& name, std::string const& description, double const& scale,
                                unsigned char const& r, unsigned char const& g, unsigned char const& b)
{
  std::string color = rgb_color_to_hex_color(255, (int)r, (int)g, (int)b);
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n";
  if (description.compare("") != 0)
    ofs << "  <description>" << description << "</description>\n";
  ofs << "  <Style>\n"
      << "    <LabelStyle> <scale>" << scale << "</scale> </LabelStyle>\n"
      << "    <IconStyle> <scale>" << scale << "</scale> <color>" << color << "</color>\n"
      << "      <Icon><href>http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png</href></Icon>"
      << "    </IconStyle>\n"
      << "  </Style>\n"
      << "  <Point>\n"
      << "    <coordinates>" << std::setprecision(12) << lon << ','
                             << std::setprecision(12) << lat << ','
                             << std::setprecision(12) << elev
      << "</coordinates>\n";
  ofs << "  </Point>\n"
      << "</Placemark>\n";
}

void bkml_write::write_location_as_box(std::ofstream& ofs, double lon, double lat, double  /*elev*/,
                                       std::string const& name,
                                       std::string const& description,
                                       double const& size,
                                       unsigned char const& r, unsigned char const& g, unsigned char const& b)
{
  vnl_double_2 ul, ll, lr, ur;
  ll[0] = lat; ll[1] = lon;
  ul[0] = lat+size; ul[1] = lon;
  lr[0] = lat; lr[1] = lon+size;
  ur[0] = lat+size; ur[1] = lon+size;
  bkml_write::write_box(ofs, name, description, ul, ur, ll, lr, r, g, b);
}

void bkml_write::write_photo_overlay(std::ofstream& ofs, const std::string& name,
                                     double lon, double lat, double alt,
                                     double head, double tilt, double roll,
                                     double t_fov, double r_fov, double value)
{
  if (value != 0.0) {
    ofs << "<value>" << value << "</value>\n";
  }
  ofs << "<PhotoOverlay>\n"
      << "  <name>" << name << "</name>\n"
      << "  <Camera>\n"
      << "    <longitude>" << std::setprecision(12) << lon << "</longitude>\n"
      << "    <latitude>"  << std::setprecision(12) << lat << "</latitude>\n"
      << "    <altitude>"  << alt << "</altitude>\n"
      << "    <heading>"   << head << "</heading>\n"
      << "    <tilt>"      << tilt << "</tilt>\n"
      << "    <roll>"      << roll << "</roll>\n"
      << "    <altitudeMode>relativeToGround</altitudeMode>\n"
      << "  </Camera>\n"
      << "  <ViewVolume>\n"
      << "    <leftFov>"   << -1*r_fov << "</leftFov>\n"
      << "    <rightFov>"  << r_fov    << "</rightFov>\n"
      << "    <bottomFov>" << -1*t_fov << "</bottomFov>\n"
      << "    <topFov>"    << t_fov    << "</topFov>\n"
      << "    <near></near>\n"
      << "  </ViewVolume>\n"
      << "</PhotoOverlay>\n" << std::endl;
}

void bkml_write::write_kml_style(std::ofstream& ofs,
                                 const std::string& style_name,
                                 double const& scale,
                                 double const& line_width,
                                 double const& alpha,
                                 unsigned char const& r,
                                 unsigned char const& g,
                                 unsigned char const& b)
{
  // obtain line color
  std::string line_color = rgb_color_to_hex_color(1, (int)b, (int)r, (int)g);

  // obtain polygon color
  int alpha_int = (int)(alpha*255);
  std::string poly_color = rgb_color_to_hex_color(alpha_int, (int)b, (int)g, (int)r);

  ofs << "<Style id=\"" << style_name << "\">\n"
      << "  <LableStyle> <scale>" << scale << "</scale> </LabelStyle>\n"
      << "  <LineStyle> <color>" << line_color << "</color> <width>" << line_width << "</width> </LineStyle>\n"
      << "  <PolyStyle>\n"
      << "    <color>" << poly_color << "</color>\n"
      << "    <fill>1</fill>\n"
      << "    <outline>0</outline>\n"
      << "  </PolyStyle>\n"
      << "</Style>\n";
}

void bkml_write::write_polygon(std::ofstream& ofs,
                               std::vector<std::pair<vgl_polygon<double>, vgl_polygon<double> > > const& polygon,
                               std::string const& name, std::string const& description,
                               double const& scale, double const& line_width,
                               double const& alpha, unsigned char const& r, unsigned char const& g, unsigned char const& b)
{
  // obtain line color
  std::string line_color = rgb_color_to_hex_color(255, (int)r, (int)g, (int)b);
  // obtain polygon color
  int alpha_int = (int)(alpha*255);
  std::string poly_color = rgb_color_to_hex_color(alpha_int, (int)r, (int)g, (int)b);

  for (const auto & i : polygon) {
    vgl_polygon<double> outer = i.first;
    vgl_polygon<double> inner = i.second;
    if (outer[0].empty())
      continue;
    ofs << "<Placemark>\n"
        << "  <name>" << name << "</name>\n";
    if (description.compare("") != 0)
      ofs << "  <description>" << description << "</description>\n";
    ofs << "  <Style>\n"
        << "    <LabelStyle> <scale>" << scale << "</scale> </LabelStyle>\n"
        << "    <LineStyle> <color>" << line_color << "</color> <width>" << line_width << "</width> </LineStyle>\n"
        << "    <PolyStyle>\n"
        << "      <color>" << poly_color << "</color>\n"
        << "      <fill>0</fill>\n"
        << "      <outline>1</outline>\n"
        << "    </PolyStyle>\n"
        << "  </Style>\n";
    ofs << "  <Polygon>\n"
        << "    <tessellate>1</tessellate>\n"
        << "    <outerBoundaryIs>\n"
        << "      <LinearRing>\n"
        << "        <coordinates>";
    for (auto & vit : outer[0])
      ofs << std::setprecision(12) << vit.x() << ',' << std::setprecision(12) << vit.y() << ",0 ";
    ofs << std::setprecision(12) << outer[0][0].x() << ',' << std::setprecision(12) << outer[0][0].y() << ",0";
    ofs << "</coordinates>\n"
        << "      </LinearRing>\n"
        << "    </outerBoundaryIs>\n";
    // write out the inner boundary
    for (unsigned in_idx = 0; in_idx < inner.num_sheets(); in_idx++) {
      ofs << "    <innerBoundaryIs>\n"
          << "      <LinearRing>\n"
          << "        <coordinates>";
      for (auto & vit : inner[in_idx])
        ofs << std::setprecision(12) << vit.x() << ',' << std::setprecision(12) << vit.y() << ",0 ";
      ofs << std::setprecision(12) << inner[in_idx][0].x() << ',' << std::setprecision(12) << inner[in_idx][0].y() << ",0";
      ofs << "</coordinates>\n"
          << "      </LinearRing>\n"
          << "    </innerBoundaryIs>\n";
    }
    ofs << "  </Polygon>\n"
        << "</Placemark>\n";
  }
  return;
}
