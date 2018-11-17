#include "sdet_vrml_display.h"
//
#include <vtol/vtol_intensity_face.h>
#include <vtol/vtol_edge_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_3d.h>

void sdet_vrml_display::write_vrml_header(std::ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n"
      << "PointLight {\n"
      << "  on FALSE\n"
      << "  intensity 1\n"
      << "ambientIntensity 0\n"
      << "color 1 1 1\n"
      << "location 0 0 0\n"
      << "attenuation 1 0 0\n"
      << "radius 100\n"
      << "}\n";
}

static void write_index_preamble(std::ofstream& str)
{
  str << "Transform {\n"
      << "translation 0 0  0\n"
      << " children [\n"
      << " Shape {\n"
      << "  appearance Appearance{\n"
      << "   material Material\n"
      << "  {\n"
      << "   diffuseColor 0.0 1.0 0.0\n"
      << "   emissiveColor 0.0 1.0 0.0\n"
      << "   }\n"
      << " }\n"
      << " geometry IndexedLineSet\n"
      << " {\n"
      << "  coord Coordinate{\n"
      << "   point[\n";
}

static void write_coor_index(std::ofstream& str, unsigned n)
{
  str << " coordIndex [\n";
  for (unsigned i = 0; i<n; ++i)
    str << i << ',';
  str << -1 << ", ]\n"
      << "}\n"
      << "} ]\n"
      << "}\n";
}

void sdet_vrml_display::
write_intensity_regions_3d(std::ofstream& str,
                           std::vector<vtol_intensity_face_sptr> const& faces)
{
  for (const auto & face : faces)
  {
    const vtol_intensity_face_sptr& f = face;
    if (f->area()==0) continue;
    //average region height
    double z0 = f->Io();
    //get the outer boundary
    vtol_one_chain_sptr och = f->get_boundary_cycle();
    unsigned nedges = och->num_edges();
    for (unsigned i = 0; i<nedges; ++i)
    {
      vtol_edge_sptr e = och->edge(i);
      auto* e2d = (vtol_edge_2d*)(e.ptr());
      vsol_curve_2d_sptr c = e2d->curve();
      vdgl_digital_curve* dc = c->cast_to_vdgl_digital_curve();
      if (!dc) continue;
      write_index_preamble(str);
      auto n = static_cast<unsigned>(dc->n_pts());
      if (n<2) continue;
      double ds = 1.0/(n-1);
      for (unsigned j = 0; j<n; ++j){
        double s = j*ds;
        double x = dc->get_x(s), y = dc->get_y(s);
        str << x << ' ' << y << ' ' << z0 << '\n';
      }
      str << "   ]\n";
      str << " }\n";
      write_coor_index(str, n);
    }
  }
}

void sdet_vrml_display::
write_vsol_polys_3d(std::ofstream& str,
                    std::vector<vsol_polygon_3d_sptr> const& polys)
{
  for (const auto& poly : polys)
  {
    unsigned n = poly->size();
    if (!n)
      continue;
    write_index_preamble(str);
    for (unsigned i = 0; i<n; ++i){
      vsol_point_3d_sptr p = poly->vertex(i);
      str << p->x() << ' ' << p->y() << ' ' << p->z() << '\n';
    }
    str << "   ]\n";
    str << " }\n";
    write_coor_index(str, n);
  }
}

void sdet_vrml_display::
write_vrml_height_map(std::ofstream& str,
                      vil_image_view<float> const & z_of_xy,
                      float r, float g, float b)
{
  unsigned ni = z_of_xy.ni(), nj = z_of_xy.nj();
  //normalize the z values to produce a rough cube of points
  unsigned n = ni*nj;
  float max = 0;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      if (z_of_xy(i,j)>max)
        max = z_of_xy(i,j);
  unsigned w = ni;
  if (nj>w)
    w = nj;
  float z_scale = 1.0f;
  if (max)
    z_scale = w/max;
  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "       color[\n";
  for (unsigned i =0; i<n; i++)
    str << r << ' '
        << g << ' '
        << b << '\n';
  str << "   ]\n  }\n"
      << "      coord Coordinate{\n"
      << "       point[\n";

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      str << i << ' ' << j << ' ' << z_of_xy(i,j)*z_scale << '\n';
  str << "   ]\n  }\n }\n}\n";
}
