#include "bkml_write.h"
//:
// \file

//: Write KML header and open document tag
void bkml_write::open_document(vcl_ofstream& str)
{
  str << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n<Document>\n";
  return;
}

//: end document tag
void bkml_write::close_document(vcl_ofstream& str)
{
  str << "</Document>\n</kml>\n";
}

//: Write a box
void bkml_write::write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr)
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
       << ul[1] << ',' << ul[0] << ",0  "
       << ur[1] << ',' << ur[0] << ",0  "
       << lr[1] << ',' << lr[0] << ",0  "
       << ll[1] << ',' << ll[0] << ",0  "
       << ul[1] << ',' << ul[0] << ",0\n"
       << "       </coordinates>\n"
       << "     </LinearRing>\n"
       << "   </outerBoundaryIs>\n"
       << "  </Polygon>\n"
       << "</Placemark>\n" << vcl_endl;
}
//: kml requires lon, lat, elev in polygon definition
void bkml_write::write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vgl_box_2d<double> bbox)
{
  // in ul x is lat y is lon, in vgl bbox x is lon, y is lat so reverse
  vnl_double_2 ul(bbox.max_y(), bbox.min_x());
  vnl_double_2 ur(bbox.max_y(), bbox.max_x());
  vnl_double_2 ll(bbox.min_y(), bbox.min_x());
  vnl_double_2 lr(bbox.min_y(), bbox.max_x());
  bkml_write::write_box(ofs, name, description, ul, ur, ll, lr); 
}

//: Write a box with color
void bkml_write::write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr, vcl_string hex_color)
{
     ofs << "<Placemark>\n"
       << "  <name>" << name << "</name>\n"
       << "  <description>" << description << "</description>\n"
       << "  <Style>\n"
       << "    <PolyStyle>\n"
       << "      <colorMode>normal</colorMode>\n"
       << "      <color>" << hex_color << "</color>\n"
       << "      <fill>0</fill>\n"
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
       << ul[1] << ',' << ul[0] << ",0  "
       << ur[1] << ',' << ur[0] << ",0  "
       << lr[1] << ',' << lr[0] << ",0  "
       << ll[1] << ',' << ll[0] << ",0  "
       << ul[1] << ',' << ul[0] << ",0\n"
       << "       </coordinates>\n"
       << "     </LinearRing>\n"
       << "   </outerBoundaryIs>\n"
       << "  </Polygon>\n"
       << "</Placemark>\n" << vcl_endl;
}

//: put a pin at the given location
void bkml_write::write_location(vcl_ofstream &ofs, vcl_string name, vcl_string description, double lat, double lon, double elev)
{
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n"
      << "  <description>" << description << "</description>\n"
      << "  <styleUrl>#m_ylw-pushpin</styleUrl>\n"
		  << "  <Point>\n"
			<< "    <coordinates>" << lon << ", " << lat << ", " << elev << "</coordinates>\n"
		  << "  </Point>\n"
      << "</Placemark>\n" << vcl_endl;
}

void bkml_write::write_photo_overlay(vcl_ofstream& ofs, vcl_string name,
                                     double lon, double lat, double alt,
                                     double head, double tilt, double roll,
                                     double t_fov, double r_fov)
{
  ofs << "<PhotoOverlay>\n"
      << "  <name>" << name << "</name>\n"
      << "  <Camera>\n"
      << "    <longitude>" << lon << "</longitude>\n"
      << "    <latitude>"  << lat << "</latitude>\n"
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
      << "</PhotoOverlay>\n" << vcl_endl;
}

