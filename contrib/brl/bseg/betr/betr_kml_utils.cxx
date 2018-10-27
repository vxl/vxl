#include <iomanip>
#include "betr_kml_utils.h"
#include "vsol_mesh_3d.h"
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_3d.h>
void betr_kml_utils::write_geo_box(std::ofstream& ostr, betr_geo_box_3d const& box){
  double max_elv = box.max_elv();
  // debug
  double min_lon = box.min_lon(), max_lon = box.max_lon();
  double min_lat = box.min_lat(), max_lat = box.max_lat();
  ostr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  ostr << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
  ostr << "  <Placemark>\n";
  ostr << "    <Polygon>\n";
  ostr << "      <extrude>1</extrude>\n";
  ostr << "      <altitudeMode>relativeToGround</altitudeMode>\n";
  ostr << "      <outerBoundaryIs>\n";
  ostr << "        <LinearRing>\n";
  ostr << "           <coordinates>\n";
  ostr << "             " << min_lon << ',' << min_lat << ',' << max_elv << '\n';
  ostr << "             " << max_lon << ',' << min_lat << ',' << max_elv << '\n';
  ostr << "             " << max_lon << ',' << max_lat << ',' << max_elv << '\n';
  ostr << "             " << min_lon << ',' << max_lat << ',' << max_elv << '\n';
  ostr << "             " << min_lon << ',' << min_lat << ',' << max_elv << '\n';
  ostr << "           </coordinates>\n";
  ostr << "        </LinearRing>\n";
  ostr << "      </outerBoundaryIs>\n";
  ostr << "    </Polygon>\n";
  ostr << "  </Placemark>\n";
  ostr << "</kml>\n";
}

void betr_kml_utils::write_mesh(std::ofstream& ostr, betr_geo_object_3d const& geo_obj){
  vsol_spatial_object_3d_sptr so = geo_obj.obj();
  if(!so)
    return;
  vsol_volume_3d* vol = so->cast_to_volume();
  if(!vol)
    return;
  vsol_mesh_3d* mesh = vol->cast_to_mesh();
  if(!mesh)
    return;
  vsol_polygon_3d_sptr poly = mesh->extract_top_face();
  if(!poly)
    return;
  unsigned n = poly->size();
  vpgl_lvcs lvcs = geo_obj.lvcs();
  ostr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  ostr << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
  ostr << "  <Placemark>\n";
  ostr << "    <Polygon>\n";
  ostr << "      <extrude>1</extrude>\n";
  ostr << "      <altitudeMode>relativeToGround</altitudeMode>\n";
  ostr << "      <outerBoundaryIs>\n";
  ostr << "        <LinearRing>\n";
  ostr << "           <coordinates>\n";
  double lon = 0, lat=0, elev=0;
  ostr.precision(15);
  for(unsigned i = 0; i<n; ++i){
    vsol_point_3d_sptr v = poly->vertex(i);
    lvcs.local_to_global(v->x(), v->y(), v->z(), vpgl_lvcs::wgs84, lon, lat, elev);
    ostr << "             " << lon << ',' << lat << ',' << elev << '\n';
  }
  ostr << "           </coordinates>\n";
  ostr << "        </LinearRing>\n";
  ostr << "      </outerBoundaryIs>\n";
  ostr << "    </Polygon>\n";
  ostr << "  </Placemark>\n";
  ostr << "</kml>\n";
}
