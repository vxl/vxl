#include "bvxm_vrml_voxel_grid.h"
#include <vnl/vnl_float_4.h>

void bvxm_vrml_voxel_grid::write_vrml_grid(std::ofstream& str, bvxm_voxel_grid<float> *grid, float threshold)
{
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();

  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "        color[\n";
  //write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j) > threshold))
          str <<"        " << (*grid_it)(i,j) << ' '<< (*grid_it)(i,j) << ' '<< (*grid_it)(i,j) << '\n';
      }
    }
  }
  str << "        ]\n     }\n"
      << "      coord Coordinate{\n"
      << "        point[\n";

  //write the coordinates
  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    std::cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j) > threshold)
          str<<"        " << i << ' ' <<j << ' ' << k <<  '\n';
      }
    }
  }
  str << "        ]\n     }\n   }\n}\n";
}

void bvxm_vrml_voxel_grid::write_vrml_grid(std::ofstream& str, bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> > *grid, float threshold)
{
  bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >::iterator grid_it = grid->begin();

  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color Color{\n"
      << "        color[\n";
  //write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((((*grid_it)(i,j)).mean() > threshold))
          str <<"        " << ((*grid_it)(i,j)).mean() << ' '<< ((*grid_it)(i,j)).mean() << ' '<< ((*grid_it)(i,j)).mean() << '\n';
      }
    }
  }
  str << "        ]\n     }\n"
      << "      coord Coordinate{\n"
      << "        point[\n";

  //write the coordinates
  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    std::cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if (((*grid_it)(i,j)).mean() > threshold)
          str<<"        " << i << ' ' <<j << ' ' << k <<  '\n';
      }
    }
  }
  str << "        ]\n     }\n   }\n}\n";
}


void bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(std::ofstream& str, bvxm_voxel_grid<float> *grid, float threshold)
{
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();
  vgl_vector_3d<unsigned> dim=grid->grid_size();
unsigned s=3;
  //write the colors
  for (unsigned k=dim.z()-1; grid_it != grid->end(); ++grid_it, --k) {
    if (k%1==0) // always true ...
    {
      for (unsigned i=0; i<(*grid_it).nx(); i+=s) {
        for (unsigned j=0; j < (*grid_it).ny(); j+=s) {
          if (((*grid_it)(i,j) > threshold)) {
#if 0
            vgl_sphere_3d<float> sphere((float)i,(float)j,(float)k,0.5f);
            write_vrml_sphere(str, sphere, (*grid_it)(i,j),(*grid_it)(i,j),(*grid_it)(i,j),1-(*grid_it)(i,j));
#endif
            vgl_sphere_3d<float> sphere((float)i/(float)s,(float)j/(float)s,(float)k/(float)s,0.4f);
            bvrml_write::write_vrml_sphere(str, sphere, (*grid_it)(i,j),(*grid_it)(i,j),(*grid_it)(i,j),0.5f-(*grid_it)(i,j));
          }
        }
      }
    }
  }
}

void bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(std::ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s)
{
  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();
  //write the colors
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, k+=s) {
    for (unsigned i=0; i<(*grid_it).nx(); i+=s) {
      for (unsigned j=0; j < (*grid_it).ny(); j+=s) {
        if ((*grid_it)(i,j)[3]/255.0f > threshold) {
          vgl_sphere_3d<float> sphere((float)i/(float)s,(float)j/(float)s,(float)k/(float)s,0.5f);
          bvrml_write::write_vrml_sphere(str, sphere, 1,0,0,0);
        }
      }
    }
  }
}

void bvxm_vrml_voxel_grid::write_vrml_grid_as_pointers(std::ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s)
{
  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();
  vgl_point_3d<double> origin(0.0,0.0,0.0);
  vgl_vector_3d<double> dirx(1,0,0);
  vgl_vector_3d<double> diry(0,1,0);
  vgl_vector_3d<double> dirz(0,0,1);
  bvrml_write::write_vrml_line(str,origin,dirx,1,1,0,0);
  bvrml_write::write_vrml_line(str,origin,diry,1,0,1,0);
  bvrml_write::write_vrml_line(str,origin,dirz,1,0,0,1);

  vgl_vector_3d<unsigned> dim=grid->grid_size();
  //write the colors
  for (unsigned k=dim.z()-1; grid_it != grid->end(); ++grid_it, --k) {
    if (k%s==0)
    {
      for (unsigned i=0; i<(*grid_it).nx(); i+=s) {
        for (unsigned j=0; j < (*grid_it).ny(); j+=s) {
          if (((*grid_it)(i,j)[3] > threshold)) {
            vgl_vector_3d<double> dir((*grid_it)(i,j)[0],(*grid_it)(i,j)[1],(*grid_it)(i,j)[2]);
            vgl_point_3d<double> pt((double)i/s,(double)j/s,(double)k/s);
            bvrml_write::write_vrml_line(str, pt,dir,4*((*grid_it)(i,j)[3]-.5f),1.f,0.f,0.f);
            bvrml_write::write_vrml_disk(str, pt,dir,2*((*grid_it)(i,j)[3]-.5f),0.f,.5f,0.f);
          }
        }
      }
    }
  }
}
