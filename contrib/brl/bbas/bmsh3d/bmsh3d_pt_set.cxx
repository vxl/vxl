// This is brl/bbas/bmsh3d/bmsh3d_pt_set.cxx
//---------------------------------------------------------------------
#include <iostream>
#include <cstdlib>
#include "bmsh3d_pt_set.h"
//:
// \file
// \brief Basic 3d point sample
//
// \author
//  MingChing Chang  Feb 10, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_printf.h>

void bmsh3d_pt_set::reset_vertices_ids ()
{
  std::vector<bmsh3d_vertex*> vertices;

  auto it = vertexmap_.begin();
  for (; it != vertexmap_.end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    vertices.push_back (V);
  }

  //Reset id and re-insert each point into map in order.
  vertexmap_.clear();
  for (unsigned int i=0; i<vertices.size(); i++) {
    bmsh3d_vertex* V = vertices[i];
    V->set_id (i);
    vertexmap_.insert (std::pair<int, bmsh3d_vertex*> (i, V));
  }

  //Reset id_counter
  vertex_id_counter_ = vertices.size();
  vertices.clear();
}

//: Reset all vertices' i_value_ to 0.
void bmsh3d_pt_set::reset_vertices_i_value (const int i_value)
{
  auto it = vertexmap_.begin();
  for (; it != vertexmap_.end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    V->set_i_visited (i_value);
  }
}

// #####################################################################

//: Clone the point set.
void clone_ptset (bmsh3d_pt_set* targetPS, bmsh3d_pt_set* inputPS)
{
  auto it = inputPS->vertexmap().begin();
  for (; it != inputPS->vertexmap().end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    bmsh3d_vertex* newV = targetPS->_new_vertex (V->id());
    targetPS->_add_vertex (newV);
  }
  targetPS->set_vertex_id_counter (inputPS->vertex_id_counter());
}

//: Compute the bounding box of all pointset_[].
bool detect_bounding_box (bmsh3d_pt_set* pt_set, vgl_box_3d<double>& bbox)
{
  if (pt_set->vertexmap().size() == 0)
    return false;
  vul_printf (std::cerr, "  detect_bounding_box(): %u boundary points.\n",
              pt_set->vertexmap().size());
  bbox.empty();
  auto it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    bbox.add (V->pt());
  }
  vul_printf (std::cerr, "    (%lf, %lf, %lf) - (%lf, %lf, %lf).\n",
              bbox.min_x(), bbox.min_y(), bbox.min_z(),
              bbox.max_x(), bbox.max_y(), bbox.max_z());
  return !bbox.is_empty();
}

bool detect_geom_center (bmsh3d_pt_set* pt_set, vgl_point_3d<double>& C)
{
  vul_printf (std::cerr, "  detect_geom_center(): ");
  const int sz = pt_set->vertexmap().size();
  if (sz==0) {
    vul_printf (std::cerr, " fail, 0 point in set.\n");
    return false;
  }

  double x = 0, y = 0, z = 0;
  auto it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    x += V->pt().x();
    y += V->pt().y();
    z += V->pt().z();
  }

  x /= sz;
  y /= sz;
  z /= sz;
  vul_printf (std::cerr, "(%lf, %lf, %lf).\n", x, y, z);
  C.set (x, y, z);
  return true;
}

// #####################################################################

bmsh3d_pt_set* clone_pt_set_3d (bmsh3d_pt_set* PS)
{
  auto* newPS = new bmsh3d_pt_set ();
  //Clone all elements of PS.
  //Note: use _new_vertex() to create a new vertex.
  auto it = PS->vertexmap().begin();
  for (; it != PS->vertexmap().end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    bmsh3d_vertex* newV = newPS->_new_vertex (V->id());
    newV->set_pt (V->pt());
    newPS->_add_vertex (V);
  }
  newPS->set_vertex_id_counter (PS->vertex_id_counter());
  newPS->set_free_objects_in_destructor (PS->b_free_objects_in_destructor());
  return newPS;
}


//: Run a brute-force check for duplicate points.
//  If found, remove them.
void remove_duplicate_points (bmsh3d_pt_set* pt_set)
{
  vul_printf (std::cerr, "\nremove_duplicate_points(): total input %d points.\n", pt_set->vertexmap().size());
  std::cout<< "  Finding duplicates: ";

  // index to be deleted
  std::vector<int> idToBeDeleted;

  auto it1 = pt_set->vertexmap().begin();
  auto end = pt_set->vertexmap().end();
  end--;
  for (; it1 != end; it1++) {
    bmsh3d_vertex* v1 = (*it1).second;

    auto it2 = it1;
    it2++;
    for (; it2 != pt_set->vertexmap().end(); it2++) {
      bmsh3d_vertex* v2 = (*it2).second;

      if (v1->pt().x() == v2->pt().x() &&
          v1->pt().y() == v2->pt().y() &&
          v1->pt().z() == v2->pt().z()) {
        //found
        idToBeDeleted.push_back (v1->id());
        std::cout<< v1->id() << ' ';
        break; //break j loop. go to the next i.
      }
    }
  }

  // perform deletion
  std::cout<< "\n\tDeleting "<< idToBeDeleted.size() <<" duplicates...\n";
  for (int id : idToBeDeleted) {
    auto it = pt_set->vertexmap().find (id);
    delete (*it).second;
    pt_set->vertexmap().erase (it);
  }
  idToBeDeleted.clear();
  std::cout<< "\tRemaining # of points: "<< pt_set->vertexmap().size() << std::endl;
}


void translate_points (bmsh3d_pt_set* pt_set,
                       const float tx, const float ty, const float tz)
{
  vgl_vector_3d<double> trans (tx, ty, tz);

  auto it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;
    v->get_pt() += trans;
  }
}

void rotate_points (bmsh3d_pt_set* pt_set,
                    const float rx, const float ry, const float rz)
{
  auto it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;

    double x0 = v->pt().x();
    double y0 = v->pt().y();
    double z0 = v->pt().z();

    double x, y, z;

    //1)Rotation about X
    y =  y0*std::cos(rx) + z0*std::sin(rx);
    z = -y0*std::sin(rx) + z0*std::cos(rx);
    y0=y;
    z0=z;

    //2)Rotation about Y
    x =  x0*std::cos(ry) - z0*std::sin(ry);
    z =  x0*std::sin(ry) + z0*std::cos(ry);
    x0=x;

    //3)Rotation about Z
    x =  x0*std::cos(rz) + y0*std::sin(rz);
    y = -x0*std::sin(rz) + y0*std::cos(rz);

    v->get_pt().set (x, y, z);
  }
}

void scale_points (bmsh3d_pt_set* pt_set, const float scale)
{
  // Just multiply the coords (x, y, z) with fScale.
  auto it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;

    v->get_pt().set (v->get_pt().x() * scale,
                     v->get_pt().y() * scale,
                     v->get_pt().z() * scale);
  }
}

void perturb_points (bmsh3d_pt_set* pt_set, const float max_perturb)
{
  auto it = pt_set->vertexmap().begin();
  for (unsigned int i=0; it != pt_set->vertexmap().end(); it++, i++) {
    bmsh3d_vertex* v = (*it).second;
    assert (v);

    double random = (double)std::rand() / (double)RAND_MAX - 0.5;
    double rx = random * max_perturb;

    random = (double)std::rand() / (double)RAND_MAX - 0.5;
    double ry = random * max_perturb;

    random = (double)std::rand() / (double)RAND_MAX - 0.5;
    double rz = random * max_perturb;

    v->get_pt().set (v->get_pt().x() + rx,
                     v->get_pt().y() + ry,
                     v->get_pt().z() + rz);
  }
}

void crop_points (bmsh3d_pt_set* pt_set,
                  const float minX, const float minY, const float minZ,
                  const float maxX, const float maxY, const float maxZ)
{
  // index to be deleted
  std::vector<int> idToBeDeleted;

  vgl_box_3d<double> bbox;
  detect_bounding_box (pt_set, bbox);

  vul_printf (std::cerr, "\n Bounding Box:\n");
  vul_printf (std::cerr, "     (minX, maxX) = (%lf, %lf)\n", bbox.min_x(), bbox.max_x());
  vul_printf (std::cerr, "     (minY, maxY) = (%lf, %lf)\n", bbox.min_y(), bbox.max_y());
  vul_printf (std::cerr, "     (minZ, maxZ) = (%lf, %lf)\n", bbox.min_z(), bbox.max_z());

  vul_printf (std::cerr, "\tCropping %d points\n", pt_set->vertexmap().size());

  auto it = pt_set->vertexmap().begin();
  while (it != pt_set->vertexmap().end()) {
    bmsh3d_vertex* v = (*it).second;

    if (v->pt().x() < (double) minX ||
        v->pt().y() < (double) minY ||
        v->pt().z() < (double) minZ ||
        v->pt().x() > (double) maxX ||
        v->pt().y() > (double) maxY ||
        v->pt().z() > (double) maxZ) {
      delete v;
      auto next = it;
      ++next;
      pt_set->vertexmap().erase (it);
      it = next;
    }
    else
      it++;
  }

  vul_printf (std::cerr, "\tRemaining %d points.\n", pt_set->vertexmap().size());
}

void shift_points_to_first_octant (bmsh3d_pt_set* pt_set)
{
  vgl_box_3d<double> bbox;
  detect_bounding_box (pt_set, bbox);

  double shiftX = bbox.min_point().x();
  double shiftY = bbox.min_point().y();
  double shiftZ = bbox.min_point().z();
  const vgl_vector_3d<double> shift (shiftX, shiftY, shiftZ);

  // 2)Move the box onto the first octant.
  auto it = pt_set->vertexmap().begin();
  for (; it != pt_set->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;
    v->get_pt() -= shift;
  }
}

//: Sub-sample the point cloud to be the specified # of points.
void subsample_points (bmsh3d_pt_set* pt_set, const unsigned int subsam_pts)
{
  vul_printf(std::cerr, "  subsample_points(): %d to %d points.\n",
             pt_set->vertexmap().size(), subsam_pts);

  vnl_random rand;
  rand.reseed (622);
  int total = pt_set->vertexmap().size();

  //Loop through the vertex map and randomly remove points.
  while (pt_set->vertexmap().size() > subsam_pts) {
    int rid;
    do {
      double random = rand.drand32 (0, 1);
      rid = (int) (random * total); ///pt_set->vertexmap().size());
    }
    while (pt_set->vertexmap().find (rid) == pt_set->vertexmap().end());
    pt_set->vertexmap().erase (rid);
  }

  assert (pt_set->vertexmap().size() == subsam_pts);
}
