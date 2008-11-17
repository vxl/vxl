// This is brl/bseg/bmdl/bmdl_mesh.cxx
#include "bmdl_mesh.h"
//:
// \file

#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_distance.h>
#include <vil/vil_bilin_interp.h>
#include <vcl_cassert.h>

//: find the next trace point and direction
bool bmdl_mesh::next_trace_point(unsigned int& i, unsigned int& j, int& dir,
                                 const unsigned int* &p, unsigned int value,
                                 unsigned int ni1, unsigned int nj1,
                                 vcl_ptrdiff_t istep, vcl_ptrdiff_t jstep)
{
  bool moved = false;
  // else follow the straight edge
  switch (dir) {
    case 1:
      if (i<ni1 && p[istep]==value) { ++i; p+=istep; moved = true; }
      break;
    case 2:
      if (j<nj1 && p[jstep]==value) { ++j; p+=jstep; moved = true; }
      break;
    case 3:
      if (i>0 && p[-istep]==value) { --i; p-=istep; moved = true; }
      break;
    case 0:
      if (j>0 && p[-jstep]==value) { --j; p-=jstep; moved = true; }
      break;
    default:
      return false;
  }

  if (moved) {
    // if inside a concave corner, turn the corner
    switch (dir) {
      case 0:
        if (i<ni1 && p[istep]==value) { ++i; p+=istep; dir=1; }
        break;
      case 1:
        if (j<nj1 && p[jstep]==value) { ++j; p+=jstep; dir=2; }
        break;
      case 2:
        if (i>0 && p[-istep]==value) { --i; p-=istep; dir=3; }
        break;
      case 3:
        if (j>0 && p[-jstep]==value) { --j; p-=jstep; dir=0; }
        break;
      default:
        return false;
    }
  }
  else {
    // inside a convex corner, turn the corner
    dir = (dir+3)%4;
  }
  return true;
}


//: trace a single boundary starting and location (i,j)
bool bmdl_mesh::trace_boundary(vcl_vector<vgl_point_2d<double> >& pts,
                               unsigned int value,
                               const vil_image_view<unsigned int>& labels,
                               vil_image_view<bool>& visited,
                               unsigned int i, unsigned int j)
{
  unsigned int ni = labels.ni();
  unsigned int nj = labels.nj();
  vcl_ptrdiff_t istep = labels.istep(), jstep=labels.jstep();
  const unsigned* p = &labels(i,j);
  if (*p != value)
    return false;

  // first direction
  int dir = 2;

  unsigned int i0=i, j0=j;
  unsigned int ni1=ni-1, nj1=nj-1;

  do {
    if (dir == 2) {
      if (visited(i,j))
        return false;
      visited(i,j) = true;
    }
    unsigned int pi = i+((dir==0||dir==3)?1:0);
    unsigned int pj = j+((dir/2==0)?1:0);
    pts.push_back(vgl_point_2d<double>(pi-0.5, pj-0.5));
    next_trace_point(i,j,dir,p,value,ni1,nj1,istep,jstep);
  }
  while (i!=i0 || j!=j0 || dir!=2);

  return true;
}


//: trace the boundaries of the building labels into polygons
// If \a dropped_clipped is true then buildings clipped by the image boundaries are not traced
vcl_vector<vgl_polygon<double> >
bmdl_mesh::trace_boundaries(const vil_image_view<unsigned int>& labels,
                            bool drop_clipped)
{
  int ni = labels.ni();
  int nj = labels.nj();
  // find the largest label
  unsigned int num_labels = 0;
  for (int j=0; j<nj; ++j)
    for (int i=0; i<ni; ++i)
      if (labels(i,j) > num_labels)
        num_labels = labels(i,j);

  vcl_vector<vgl_polygon<double> > boundaries(num_labels,vgl_polygon<double>(0));
  for (unsigned i=0; i<num_labels; ++i)
    boundaries[i].clear();
  vil_image_view<bool> visited(ni,nj);
  visited.fill(false);

  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      unsigned int l = labels(i,j);
      if (l<2) continue;
      if (!visited(i,j) && (i==0 || labels(i-1,j)!=l)) {
        assert(l-2<boundaries.size());
        vgl_polygon<double>& poly = boundaries[l-2];
        vcl_vector<vgl_point_2d<double> > sheet;

        // trace the boundary and keep it only if it is not clipped
        // (or we don't care about clipping)
        if (trace_boundary(sheet,l,labels,visited,i,j) &&
            (!drop_clipped || !is_clipped(sheet,ni,nj)))
          poly.push_back(sheet);
      }
    }
  }

  return boundaries;
}

//: test if a boundary is clipped by the image of size \a ni by \a nj
bool bmdl_mesh::is_clipped(const vcl_vector<vgl_point_2d<double> >& poly,
                           unsigned ni, unsigned nj)
{
  // the maximum pixels are ni-1 and nj-1
  --ni;
  --nj;
  for (unsigned int i=0; i<poly.size(); ++i)
  {
    if (poly[i].x() < 0.0 || poly[i].y() < 0.0 ||
        poly[i].x() > ni || poly[i].y() > nj)
      return true;
  }

  return false;
}


//: simplify a polygon by fitting lines
// \a tol is the tolerance for line fitting
void bmdl_mesh::simplify_polygon( vcl_vector<vgl_point_2d<double> >& pts, double tol )
{
  vgl_fit_lines_2d<double> line_fitter(3,tol);
  // duplicate the last point to close the loop
  line_fitter.add_point(pts.back());
  line_fitter.add_curve(pts);
  line_fitter.fit();
  const vcl_vector<vgl_line_segment_2d<double> >& segs = line_fitter.get_line_segs();
  const vcl_vector<int>& indices = line_fitter.get_indices();
  vcl_vector<vgl_point_2d<double> > new_pts;
  vgl_point_2d<double> isect;
  for ( unsigned int i=0; i< indices.size(); )
  {
    if (indices[i] < 0)
    {
      new_pts.push_back(pts[i++]);
      continue;
    }
    const vgl_line_segment_2d<double>& seg = segs[indices[i]];
    // fix very short segments that trim corners
    if (new_pts.size() > 1 && vgl_distance(new_pts.back(),seg.point1())< 1.0 &&
        angle(vgl_vector_2d<double>(new_pts.back()-new_pts[new_pts.size()-2]),
              vgl_vector_2d<double>(seg.point1()-seg.point2())) > 0.5) {
      vgl_intersection(vgl_line_2d<double>(new_pts[new_pts.size()-2], new_pts.back()),
                       vgl_line_2d<double>(seg.point1(),seg.point2()),
                       isect);
      new_pts.back() = isect;
    }
    else
      new_pts.push_back(seg.point1());
    new_pts.push_back(seg.point2());
    // skip to the next point not part of this line
    for (int curr_seg_idx = indices[i];
         i<indices.size() && indices[i] == curr_seg_idx; ++i)
      ;
  }
  if (new_pts.size() > 3 && vgl_distance(new_pts.back(),new_pts.front())<1.0 &&
      angle(vgl_vector_2d<double>(new_pts.back()-new_pts[new_pts.size()-2]),
            vgl_vector_2d<double>(new_pts[0]-new_pts[1])) > 0.5) {
    vgl_intersection(vgl_line_2d<double>(new_pts[new_pts.size()-2], new_pts.back()),
                     vgl_line_2d<double>(new_pts[0],new_pts[1]),
                     isect);
    new_pts[0] = isect;
    new_pts.pop_back();
  }
  pts.swap(new_pts);
}


//: simplify the boundaries by fitting lines
void bmdl_mesh::simplify_boundaries( vcl_vector<vgl_polygon<double> >& boundaries )
{
  // fit lines to the data points
  for (unsigned int i=0; i<boundaries.size(); ++i)
  {
    vgl_polygon<double>& poly = boundaries[i];
    for (unsigned int j=0; j<poly.num_sheets(); ++j)
    {
      vcl_vector<vgl_point_2d<double> >& pts = poly[j];
      // shift point to the edge centers (diagonals line up better this way)
      vcl_vector<vgl_point_2d<double> > new_pts;
      for (unsigned p1=pts.size()-1, p2=0; p2<pts.size(); p1=p2, ++p2) {
        new_pts.push_back(vgl_point_2d<double>((pts[p1].x()+pts[p2].x())/2, (pts[p1].y()+pts[p2].y())/2));
      }
      pts.swap(new_pts);

      simplify_polygon(pts,0.01);
      simplify_polygon(pts,0.5);
    }
  }
}


//: construct a mesh out of data and labels
// The coordinate system is flipped over the x-axis to make it right handed
// i.e. (x,y) -> (x,-y)
void bmdl_mesh::mesh_lidar(const vcl_vector<vgl_polygon<double> >& boundaries,
                           const vil_image_view<unsigned int>& labels,
                           const vil_image_view<double>& heights,
                           const vil_image_view<double>& ground,
                           imesh_mesh& mesh)
{
  unsigned ni = labels.ni();
  unsigned nj = labels.nj();

  // recover the vector of building heights
  vcl_vector<double> bld_heights(boundaries.size());
  for (unsigned int j=0; j<nj; ++j){
    for (unsigned int i=0; i<ni; ++i){
      if (labels(i,j) > 1){
        bld_heights[labels(i,j)-2] = heights(i,j);
      }
    }
  }

  imesh_vertex_array<3> *verts = new imesh_vertex_array<3>;
  imesh_face_array *faces = new imesh_face_array;

  // create the buildings
  for (unsigned int b=0; b<boundaries.size(); ++b) {
    if (boundaries[b].num_sheets() == 0)
      continue;
    const vcl_vector<vgl_point_2d<double> >& pts = boundaries[b][0];
    if (pts.size()<3)
      continue;

    unsigned int first_pt = verts->size();
    vcl_vector< unsigned int > roof;
    for (unsigned i=0; i<pts.size(); ++i) {
      const vgl_point_2d<double>& pt = pts[i];
      double g = vil_bilin_interp_safe_extend(ground, pt.x(), pt.y());
      verts->push_back(imesh_vertex<3>(pt.x(),-pt.y(), g));
      verts->push_back(imesh_vertex<3>(pt.x(),-pt.y(),bld_heights[b]));
      unsigned int bi = first_pt + 2*i;
      faces->push_back(imesh_quad(bi+1,bi,bi+2,bi+3));
      roof.push_back(bi+1);
    }
    // correct to loop back around to the start
    vcl_vector< unsigned int >& last_face = (*faces)[faces->size()-1];
    last_face[2] = first_pt;
    last_face[3] = first_pt+1;

    faces->push_back(roof);
  }

  vcl_auto_ptr<imesh_vertex_array_base> vb(verts);
  vcl_auto_ptr<imesh_face_array_base> fb(faces);
  mesh.set_vertices(vb);
  mesh.set_faces(fb);
}

