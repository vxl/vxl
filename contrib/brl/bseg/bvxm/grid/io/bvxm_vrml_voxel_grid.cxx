#include "bvxm_vrml_voxel_grid.h"
#include <vnl/vnl_math.h>


void bvxm_vrml_voxel_grid::write_vrml_header(vcl_ofstream& str)
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


void bvxm_vrml_voxel_grid::write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold)
{
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();

  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "        color[\n";
  // write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j) > threshold))
          str <<"        " << (*grid_it)(i,j) << ' '<< (*grid_it)(i,j) << ' '<< (*grid_it)(i,j)
              << '\n';
      }
    }
  }
  str << "        ]\n     }\n"
      << "      coord Coordinate{\n"
      << "        point[\n";

  // write the coordinates
  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j) > threshold)
          str<<"        " << i << ' ' <<j << ' ' << k <<  '\n';
      }
    }
  }
  str << "        ]\n     }\n   }\n}\n";
}

void bvxm_vrml_voxel_grid::write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> > *grid, float threshold)
{
  bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >::iterator grid_it = grid->begin();

  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "        color[\n";
  // write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((((*grid_it)(i,j)).mean() > threshold))
          str <<"        " << ((*grid_it)(i,j)).mean() << ' '<< ((*grid_it)(i,j)).mean() << ' '<< ((*grid_it)(i,j)).mean()
              << '\n';
      }
    }
  }
  str << "        ]\n     }\n"
      << "      coord Coordinate{\n"
      << "        point[\n";

  // write the coordinates
  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j)).mean() > threshold)
          str<<"        " << i << ' ' <<j << ' ' << k <<  '\n';
      }
    }
  }
  str << "        ]\n     }\n   }\n}\n";
}


void bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold)
{
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();
  vgl_vector_3d<unsigned> dim=grid->grid_size();
  unsigned s=3;
  // write the colors
  for (unsigned k=dim.z()-1; grid_it != grid->end(); ++grid_it, --k) {
//  if (k%1==0) // always true !?!
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j) > threshold)){
#if 0
          vgl_sphere_3d<float> sphere((float)i,(float)j,(float)k,0.5f);
          write_vrml_sphere(str, sphere, (*grid_it)(i,j),(*grid_it)(i,j),(*grid_it)(i,j),1-(*grid_it)(i,j));
#else // 0
          vgl_sphere_3d<float> sphere((float)i/s,(float)j/s,(float)k/s,0.25f);
          write_vrml_sphere(str, sphere, 1.f,0.f,0.f,0.f);
#endif // 0
        }
      }
    }
  }
}

void bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s)
{
  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();
  // write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i){
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j)[3]/255.0f > threshold) {
          vgl_sphere_3d<float> sphere((float)i/s,(float)j/s,(float)k/s,0.5f);
          write_vrml_sphere(str, sphere, 1.f,0.f,0.f,0.f);
        }
      }
    }
  }
}


void bvxm_vrml_voxel_grid::write_vrml_grid_as_pointers(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s)
{
  s=1;
  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();
  vgl_point_3d<double> origin(0.0,0.0,0.0);
  vgl_vector_3d<double> dirx(1,0,0);
  vgl_vector_3d<double> diry(0,1,0);
  vgl_vector_3d<double> dirz(0,0,1);
  write_vrml_line(str,origin,dirx,1.f,1.f,0.f,0.f);
  write_vrml_line(str,origin,diry,1.f,0.f,1.f,0.f);
  write_vrml_line(str,origin,dirz,1.f,0.f,0.f,1.f);

  vgl_vector_3d<unsigned> dim=grid->grid_size();
  // write the colors
  for (unsigned k=dim.z()-1; grid_it != grid->end(); ++grid_it, --k) {
    if (k%s==0) {
      for (unsigned i=0; i<(*grid_it).nx(); i+=s) {
        for (unsigned j=0; j < (*grid_it).ny(); j+=s) {
          if (((*grid_it)(i,j)[3] > threshold)) {
            vgl_vector_3d<double> dir((*grid_it)(i,j)[0],(*grid_it)(i,j)[1],(*grid_it)(i,j)[2]);
            vgl_point_3d<double> pt((double)i/s,(double)j/s,(double)k/s);
            write_vrml_line(str, pt,dir,4*((*grid_it)(i,j)[3]-0.5f),1.f,0.f,0.f);
            write_vrml_disk(str, pt,dir,2*((*grid_it)(i,j)[3]-0.5f),0.f,.5f,0.f);
          }
        }
      }
    }
  }
}


void bvxm_vrml_voxel_grid::write_vrml_sphere(vcl_ofstream& str, vgl_sphere_3d<float> const& sphere,
                                             const float r, const float g, const float b,
                                             const float transparency)
{
  double x0 = sphere.centre().x(), y0 = sphere.centre().y(), z0 = sphere.centre().z();
  double rad = sphere.radius();
  str << "Transform {\n"
      << "translation " << x0 << ' ' << y0 << ' '
      << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << rad << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}


void bvxm_vrml_voxel_grid::write_vrml_disk(vcl_ofstream& str,
                                           vgl_point_3d<double> const& pt,
                                           vgl_vector_3d<double> const &dir,
                                           float radius,
                                           const float r, const float g, const float b)
{
  double denom=vcl_sqrt(dir.x()*dir.x()+dir.y()*dir.y());
  float axis_x;
  float axis_y;

  if (denom==0.0)
  {
    axis_x=1.0f;
    axis_y=0.0f;
  }
  else
  {
    axis_x = -float(dir.y()/denom);
    axis_y = float(dir.x()/denom);
  }
  float phi = float(vcl_acos(dir.z()));

  double rad = radius;
  double height = 0.1;

  str << "Transform {\n"
      << "translation " << pt.x() << ' ' << pt.y() << ' '
      << ' ' << pt.z() << '\n'
      << "rotation 1 0 0 -1.570796\n"
      << "children [\n"
      << "Transform {\n"
      << "rotation " << axis_x << " 0.0 " << axis_y <<' ' <<phi<< '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Cylinder\n"
      << "{\n"
      << "  radius " << rad << '\n'
      << "  height " << height << '\n'
      << "   }\n"
      << "  }\n"
      << " ]\n"
      << "  }\n"
      << " ]\n"
      << "}\n";
}

void bvxm_vrml_voxel_grid::write_vrml_line(vcl_ofstream& str,vgl_point_3d<double> pt,
                                           vgl_vector_3d<double> dir,
                                           float response,
                                           const float r,
                                           const float g,
                                           const float b)
{
  str << "Transform {\n"
      << "translation 0 0 0\n"
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      emissiveColor " << r << ' ' << g << ' ' << b << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedLineSet\n"
      <<   "{\n"
      << "      coord Coordinate{\n"
      << "       point[\n"
      << pt.x()-dir.x()*response*0.0 << ' ' << pt.y()-dir.y()*response*0.0 << ' ' << pt.z()-dir.z()*response*0.0 << '\n'
      << pt.x()+dir.x()*response*0.5 << ' ' << pt.y()+dir.y()*response*0.5 << ' ' << pt.z()+dir.z()*response*0.5 << '\n'
      << "   ]\n\n }"
      << "   coordIndex [\n"
      << "     0,1  ]\n  }\n}"
      <<  " ]\n"
      << "}\n";
}

void bvxm_vrml_voxel_grid::write_vrml_box(vcl_ofstream& str, vgl_box_3d<int> const& box,
                                          const float r , const float g , const float b ,
                                          const float transparency )
{
  int x0 = box.centroid().x(), y0 = box.centroid().y(), z0 = box.centroid().z();
  int w = box.max_point().x() - box.min_point().x();
  int h = box.max_point().y() - box.min_point().y();
  int d = box.max_point().z() - box.min_point().z();

  str << "Transform {\n"
      << "translation " << x0 << ' ' << y0 << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Box\n"
      <<   "{\n"
      << "  size " << w << ' ' << h << ' ' << d << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

