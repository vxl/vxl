#include "bkml_write.h"
//#include <vcl_cmath.h>

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
   ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n"
      << "  <description>" << description << "</description>\n"
      << "  <Style>\n"
      << "    <PolyStyle>\n"
      << "      <colorMode>random</colorMode>\n"
      << "      <color>ffffffff</color>\n"
      << "      <fill>0</fill>\n"
      << "    </PolyStyle>\n"
      << "    <LineStyle>\n"
      << "      <colorMode>random</colorMode>\n"
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

  return;
}





