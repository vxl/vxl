// This is brl/bseg/bmdl/bmdl_mesh.cxx
#include "bmdl_mesh.h"
//:
// \file

#include <vgl/vgl_intersection.h>
#include <vgl/vgl_lineseg_test.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_distance.h>
#include <vil/vil_bilin_interp.h>
#include <vcl_cassert.h>
#include <vcl_map.h>

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


namespace
{
  // return a reference to the edge bin corresponding the edge between p1 and p2
  unsigned int& get_edge_bin(vil_image_view<unsigned int>& edge_bins,
                             const vgl_point_2d<double>& p1,
                             const vgl_point_2d<double>& p2)
  {
    unsigned int *binp = 0;
    if (vcl_abs(p2.x() - p1.x()) < 0.5)
    {
      binp = &edge_bins(static_cast<unsigned int>(p1.x()+1.0),
                        static_cast<unsigned int>((p1.y()+p2.y())/2.0+0.5), 0);
    }
    else
    {
      binp = &edge_bins(static_cast<unsigned int>((p1.x()+p2.x())/2.0+0.5),
                        static_cast<unsigned int>(p1.y()+1.0), 1);
    }
    return *binp;
  }
}


//: extract shared boundary edges from the polygon boundaries
unsigned int bmdl_mesh::link_boundary_edges(const vil_image_view<unsigned int>& labels,
                                            const vcl_vector<vgl_polygon<double> >& polygons,
                                            vcl_vector<bmdl_edge>& edges,
                                            vcl_vector<bmdl_region>& regions)
{
  unsigned int ni = labels.ni();
  unsigned int nj = labels.nj();
  vil_image_view<unsigned int> edge_labels(ni+1, nj+1, 2);
  edge_labels.fill(0);

  unsigned int joint_count = 0;
  unsigned int last_joint = static_cast<unsigned int>(-1);

  regions.resize(polygons.size());
  for (unsigned int i=0; i<polygons.size(); ++i)
  {
    const vgl_polygon<double>& poly = polygons[i];
    if (poly.num_sheets() < 1)
      continue;
    // determine which sheets are holes (CW vs CCW orientation)
    unsigned int num_cw=0, num_ccw=0;
    vcl_vector<bool> is_hole(poly.num_sheets(),false);
    for (unsigned int j=0; j<poly.num_sheets(); ++j)
    {
      double area = vgl_area_signed(vgl_polygon<double>(poly[j]));
      is_hole[j] = (area > 0.0);
      if (is_hole[j])
        ++num_ccw;
      else
        ++num_cw;
    }
    // expect to find exactly 1 CW sheet
    if (num_cw == 0)
      continue;
    else if (num_cw > 1)
    {
      vcl_cerr << "Ignoring polygon with multiple connected components "<< i << vcl_endl;
      for (unsigned int k=0; k<is_hole.size(); ++k)
        if (!is_hole[k])
        {
          vcl_cout << "  "<<poly[k][0].x()<<", "<<poly[k][0].y()<<vcl_endl;
        }
      continue;
    }
    bmdl_region& region = regions[i];
    if (num_ccw > 0)
      region.hole_edge_idxs.resize(num_ccw);
    unsigned int hole_count = 0;
    for (unsigned int j=0; j<poly.num_sheets(); ++j)
    {
      const vcl_vector<vgl_point_2d<double> >& pts = poly[j];
      vcl_vector<unsigned int>& edge_idxs = (!is_hole[j]) ?
                                            region.edge_idxs :
                                            region.hole_edge_idxs[hole_count++];
      for (unsigned int k1=pts.size()-1,k2=0; k2<pts.size(); k1=k2++)
      {
        vgl_vector_2d<double> v(pts[k2]-pts[k1]);
        vgl_point_2d<double> p = pts[k1] + 0.5*v;
        p += vgl_vector_2d<double>(-0.5*v.y(), 0.5*v.x());
        int x = static_cast<int>(p.x()+0.5);
        int y = static_cast<int>(p.y()+0.5);
        unsigned l = 0;
        if (x >= 0 && (unsigned)x < ni && y >=0 && (unsigned)y < nj)
        {
          l = labels(x,y);
          if (l > 0)
            l -= 1;
        }

        // find the bin for storing indices of edges already created
        unsigned int& bin = get_edge_bin(edge_labels,pts[k1],pts[k2]);

        if (bin > 0) // the label already exists
        {
          if (edge_idxs.empty() || edge_idxs.back() != bin-1)
          {
            // correct last joint if just created
            if (!edge_idxs.empty() && edges[edge_idxs.back()].building1 == i+1)
            {
              edges[edge_idxs.back()].joint2 = edges[bin-1].joint2;
              --joint_count;
            }
            edge_idxs.push_back(bin-1);
            assert(edges[edge_idxs.back()].building1 == l);
            assert(edges[edge_idxs.back()].building2 == i+1);
            last_joint = edges[edge_idxs.back()].joint1;
          }
        }
        else // create and build a new edge
        {
          if (edge_idxs.empty() || edges[edge_idxs.back()].building2 != l)
          {
            // start a new edge
            edge_idxs.push_back(edges.size());
            edges.push_back(bmdl_edge(i+1,l));
            edges.back().pts.push_back(pts[k1]);
            edges.back().joint1 = last_joint;
            edges.back().joint2 = joint_count++;
            last_joint = edges.back().joint2;
          }
          edges[edge_idxs.back()].pts.push_back(pts[k2]);
          bin = edge_idxs.back()+1;
        }
      }
      // handle special cases at the starting point
      if (edge_idxs.size() > 1)
      {
        // A region may touch the same previously created edge at beginning and end
        if (edge_idxs.front() == edge_idxs.back() &&
            edges[edge_idxs.front()].building2 == i+1)
        {
          edge_idxs.pop_back();
        }
        // Created edges at beginning and end may touch the same region (merge them)
        else if (edges[edge_idxs.front()].building2 == edges[edge_idxs.back()].building2 &&
                 edges[edge_idxs.front()].building2 != i+1)
        {
          vcl_vector<vgl_point_2d<double> >& pts1 = edges[edge_idxs.front()].pts;
          vcl_vector<vgl_point_2d<double> >& pts2 = edges[edge_idxs.back()].pts;
          // relabel bins
          for (unsigned int k=1; k<pts2.size(); ++k)
          {
            unsigned int& bin = get_edge_bin(edge_labels,pts2[k-1],pts2[k]);
            bin = edge_idxs.front()+1;
          }
          assert((pts1.front() - pts2.back()).length() < 1e-6);
          pts2.insert(pts2.end(),pts1.begin()+1,pts1.end());
          pts1.swap(pts2);
          assert(edge_idxs.back()+1 == edges.size());
          last_joint = edges[edge_idxs.back()].joint1;
          edge_idxs.pop_back();
          edges.pop_back();
          --joint_count;
        }
        // correct last joint if just created and the first edge was preexisting
        else if (edges[edge_idxs.back()].building1 == i+1 &&
                 edges[edge_idxs.front()].building2 == i+1 &&
                 edges[edge_idxs.back()].joint2 != edges[edge_idxs.front()].joint2 )
        {
          edges[edge_idxs.back()].joint2 = edges[edge_idxs.front()].joint2;
          --joint_count;
          --last_joint;
        }
      }
      // fix starting joint by completing the cycle
      if (edges[edge_idxs.front()].building1 == i+1)
      {
        edges[edge_idxs.front()].joint1 = last_joint;
      }
    }
  }
  return joint_count;
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


//: simplify an edge by fitting lines
// \a tol is the tolerance for line fitting
void bmdl_mesh::simplify_edge( vcl_vector<vgl_point_2d<double> >& pts, double tol )
{
  vgl_fit_lines_2d<double> line_fitter(3,tol);
  line_fitter.add_curve(pts);
  line_fitter.fit();
  const vcl_vector<vgl_line_segment_2d<double> >& segs = line_fitter.get_line_segs();
  const vcl_vector<int>& indices = line_fitter.get_indices();
  vcl_vector<vgl_point_2d<double> > new_pts;
  // the first point must be retained
  new_pts.push_back(pts.front());
  vgl_point_2d<double> isect;
  for ( unsigned int i=0; i< indices.size(); )
  {
    if (indices[i] < 0)
    {
      if (i==0 || i==indices.size()-1)
        ++i;
      else if (new_pts.empty() || new_pts.back() != pts[i])
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
    // avoid duplicating the first point
    else if (i>0 || (seg.point1()-pts.front()).length()>0.5)
      new_pts.push_back(seg.point1());

    // remove duplicate points
    if (new_pts.size()>1 && new_pts.back() == new_pts[new_pts.size()-2])
      new_pts.pop_back();

    // skip to the next point not part of this line
    for (int curr_seg_idx = indices[i];
         i<indices.size() && indices[i] == curr_seg_idx; ++i)
      ;

    // avoid duplicating the last point
    if (i<indices.size() || (seg.point2()-pts.back()).length()>0.5)
      new_pts.push_back(seg.point2());

    if (new_pts.size()>1 && new_pts.back() == new_pts[new_pts.size()-2])
      new_pts.pop_back();
  }
  // the last point must be retained
  new_pts.push_back(pts.back());
  pts.swap(new_pts);
}


//: simplify the linked edges by fitting lines
void bmdl_mesh::simplify_edges( vcl_vector<bmdl_edge>& edges )
{
  for (unsigned int i=0; i<edges.size(); ++i)
  {
    vcl_vector<vgl_point_2d<double> >& pts = edges[i].pts;

    // shift point to the edge centers (diagonals line up better this way)
    vcl_vector<vgl_point_2d<double> > new_pts;
    new_pts.push_back(pts.front());
    for (unsigned j=1; j<pts.size(); ++j) {
      new_pts.push_back(vgl_point_2d<double>((pts[j-1].x()+pts[j].x())/2,
                                             (pts[j-1].y()+pts[j].y())/2));
    }
    new_pts.push_back(pts.back());
    pts.swap(new_pts);

    simplify_edge(pts, 0.01);
    simplify_edge(pts, 0.5);
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


namespace{
  unsigned int add_joint_vertex(imesh_vertex_array<3>& verts,
                                vcl_vector<unsigned int>& vert_stack,
                                const imesh_vertex<3>& v)
  {
    bool found = false;
    vcl_vector<unsigned int>::iterator i=vert_stack.begin();
    for (; i!=vert_stack.end() && verts[*i][2] <= v[2]; ++i)
    {
      if (verts[*i][2] == v[2])
      {
        found = true;
        break;
      }
    }
    if (found)
      return *i;

    unsigned int bi = verts.size();
    verts.push_back(v);
    vert_stack.insert(i,bi);
    return bi;
  }


  inline vcl_vector<unsigned int>::const_iterator
  find_by_height(const vcl_vector<unsigned int>& idxs,
                 const imesh_vertex_array<3>& verts,
                 double height)
  {
    for (vcl_vector<unsigned int>::const_iterator i=idxs.begin(); i!=idxs.end(); ++i)
      if (verts[*i][2] == height)
        return i;
    return idxs.end();
  }
}

//: construct a mesh out of data and labels using linked edges
// The coordinate system is flipped over the x-axis to make it right handed
// i.e. (x,y) -> (x,-y)
void bmdl_mesh::mesh_lidar(const vcl_vector<bmdl_edge>& edges,
                           const vcl_vector<bmdl_region>& regions,
                           unsigned int num_joints,
                           const vil_image_view<unsigned int>& labels,
                           const vil_image_view<double>& heights,
                           const vil_image_view<double>& ground,
                           imesh_mesh& mesh)
{
  unsigned ni = labels.ni();
  unsigned nj = labels.nj();

  // recover the vector of building heights
  vcl_vector<double> bld_heights(regions.size());
  for (unsigned int j=0; j<nj; ++j)
  {
    for (unsigned int i=0; i<ni; ++i)
    {
      if (labels(i,j) > 1){
        bld_heights[labels(i,j)-2] = heights(i,j);
      }
    }
  }

  vcl_vector<vcl_vector<unsigned int> > face_list(regions.size());
  for (unsigned int i = 0; i<regions.size(); ++i)
  {
    const vcl_vector<unsigned int>& edge_idxs = regions[i].edge_idxs;
    for (unsigned int j = 0; j<edge_idxs.size(); ++j)
    {
      if (edges[edge_idxs[j]].building1 == i+1)
      {
        face_list[i].push_back(edges[edge_idxs[j]].joint1);
        // insert two pseudo edge points for disambiguation
        face_list[i].push_back(num_joints + 2*edge_idxs[j]);
        face_list[i].push_back(num_joints + 2*edge_idxs[j] + 1);
      }
      else
      {
        face_list[i].push_back(edges[edge_idxs[j]].joint2);
        // insert two pseudo edge points for disambiguation
        face_list[i].push_back(num_joints + 2*edge_idxs[j] + 1);
        face_list[i].push_back(num_joints + 2*edge_idxs[j]);
      }
    }
  }
  imesh_half_edge_set he2d(face_list);

  // store the indices of vertices connected below each edge point
  vcl_vector<vcl_vector<unsigned int> > edge_to_mesh(edges.size());
  vcl_vector<vcl_vector<unsigned int> > joint_vert_stack(num_joints);

  imesh_vertex_array<3> *verts = new imesh_vertex_array<3>;
  imesh_face_array *faces = new imesh_face_array;

  // precompute a mesh vertices located at joints
  for (unsigned int e=0; e<edges.size(); ++e)
  {
    const bmdl_edge& edge = edges[e];
    const vgl_point_2d<double>& pt1 = edge.pts.front();
    const vgl_point_2d<double>& pt2 = edge.pts.back();
    vcl_vector<unsigned int>& jvs1 = joint_vert_stack[edge.joint1];
    vcl_vector<unsigned int>& jvs2 = joint_vert_stack[edge.joint2];

    bool b1_gnd = edge.building1 == 0 || regions[edge.building1-1].edge_idxs.empty();
    bool b2_gnd = edge.building2 == 0 || regions[edge.building2-1].edge_idxs.empty();
    // building 1 on ground
    if (b1_gnd || b2_gnd)
    {
      double g1 = vil_bilin_interp_safe_extend(ground, pt1.x(), pt1.y());
      add_joint_vertex(*verts, jvs1, imesh_vertex<3>(pt1.x(),-pt1.y(), g1));
      double g2 = vil_bilin_interp_safe_extend(ground, pt2.x(), pt2.y());
      add_joint_vertex(*verts, jvs2, imesh_vertex<3>(pt2.x(),-pt2.y(), g2));
    }
    if (!b1_gnd)
    {
      double h = bld_heights[edge.building1-1];
      add_joint_vertex(*verts, jvs1, imesh_vertex<3>(pt1.x(),-pt1.y(),h));
      add_joint_vertex(*verts, jvs2, imesh_vertex<3>(pt2.x(),-pt2.y(),h));
    }
    if (!b2_gnd)
    {
      double h = bld_heights[edge.building2-1];
      add_joint_vertex(*verts, jvs1, imesh_vertex<3>(pt1.x(),-pt1.y(),h));
      add_joint_vertex(*verts, jvs2, imesh_vertex<3>(pt2.x(),-pt2.y(),h));
    }
  }

  typedef vcl_pair<unsigned int, unsigned int> uint_pair;
  vcl_map<uint_pair, vcl_vector<unsigned int> > revised_stack;
  // find non-manifold corner junctions
  for (unsigned int i=0; i<joint_vert_stack.size(); ++i){
    // get the 4 adjacent faces (if there are 4)
    imesh_half_edge_set::v_const_iterator e1 = he2d.vertex_begin(i), ei = e1;
    imesh_half_edge_set::v_const_iterator invalid_e(imesh_invalid_idx,he2d);
    if (ei == invalid_e)
      continue;
    int f1 = static_cast<int>(ei->face_index());
    if (e1 == ++ei)
      continue;
    int f2 = static_cast<int>(ei->face_index());
    if (e1 == ++ei)
      continue;
    int f3 = static_cast<int>(ei->face_index());
    if (e1 == ++ei)
      continue;
    int f4 = static_cast<int>(ei->face_index());
    assert(e1 == ++ei);

    // look for double peaks
    if ((f1 > f2 && f1 > f4 && f3 > f2 && f3 > f4) ||
        (f2 > f1 && f2 > f3 && f4 > f1 && f4 > f3) )
    {
      if (f1 > f2) // shift
      {
        int tmp = f1;
        f1 = f2;
        f2 = f3;
        f3 = f4;
        f4 = tmp;
      }

      // split the old stack
      const vcl_vector<unsigned int>& jvs = joint_vert_stack[i];

      double h1 = (f1<0)?(*verts)[jvs.front()][2]:bld_heights[f1];
      double h2 = bld_heights[f2];
      double h3 = (f3<0)?(*verts)[jvs.front()][2]:bld_heights[f3];
      vcl_vector<unsigned int> new_stack1;
      for (unsigned int k=0; k<jvs.size(); ++k)
      {
        double z = (*verts)[jvs[k]][2];
        if (z == h1 || z == h2 || z == h3)
          new_stack1.push_back(jvs[k]);
      }
      revised_stack[uint_pair(f2,i)] = new_stack1;

      h1 = (f3<0)?(*verts)[jvs.front()][2]:bld_heights[f3];
      h2 = bld_heights[f4];
      h3 = (f1<0)?(*verts)[jvs.front()][2]:bld_heights[f1];
      vcl_vector<unsigned int> new_stack2;
      for (unsigned int k=0; k<jvs.size(); ++k)
      {
        double z = (*verts)[jvs[k]][2];
        if (z == h1 || z == h2 || z == h3)
          new_stack2.push_back(jvs[k]);
      }
      revised_stack[uint_pair(f4,i)] = new_stack2;
    }
  }


  // create the buildings
  for (unsigned int b=0; b<regions.size(); ++b) {
    if (regions[b].edge_idxs.empty())
      continue;
    unsigned int roof_face = 0;
    for (int h=-1; h<(int)regions[b].hole_edge_idxs.size(); ++h)
    {
      const vcl_vector<unsigned int>& edge_idxs = (h<0) ? regions[b].edge_idxs :
                                                          regions[b].hole_edge_idxs[h];
      vcl_vector< unsigned int > roof;
      for (unsigned int e=0; e<edge_idxs.size(); ++e)
      {
        const bmdl_edge& edge = edges[edge_idxs[e]];
        vcl_vector<unsigned int> e2m = edge_to_mesh[edge_idxs[e]];
        vcl_vector<vgl_point_2d<double> > pts = edge.pts;
        unsigned int other = 0;
        unsigned int j1 = edge.joint1;
        unsigned int j2 = edge.joint2;
        if (edge.building1 == b+1)
        {
          other = edge.building2;
        }
        else if (edge.building2 == b+1)
        {
          other = edge.building1;
          vcl_reverse(pts.begin(), pts.end());
          vcl_reverse(e2m.begin(), e2m.end());
          vcl_swap(j1,j2);
        }
        else
          vcl_cout << "mismatch "<<b+1<<", "<<edge.building1<<", "<<edge.building2<<vcl_endl;

        // if this has been marked as a non-manifold corner then a revised stack exists
        // otherwise use the original vertex stack
        vcl_map<uint_pair, vcl_vector<unsigned int> >::const_iterator rvs1 = revised_stack.find(uint_pair(b,j1));
        vcl_map<uint_pair, vcl_vector<unsigned int> >::const_iterator rvs2 = revised_stack.find(uint_pair(b,j2));
        const vcl_vector<unsigned int>& jvs1 = (rvs1==revised_stack.end())
                                               ? joint_vert_stack[j1]
                                               : rvs1->second;
        const vcl_vector<unsigned int>& jvs2 = (rvs2==revised_stack.end())
                                                ? joint_vert_stack[j2]
                                                : rvs2->second;

        // is this edge attached to the ground?
        bool on_ground = other == 0 || regions[other-1].edge_idxs.empty();

        typedef vcl_vector<unsigned int>::const_iterator vfitr;
        typedef vcl_vector<unsigned int>::const_reverse_iterator vritr;

        vfitr i1beg = jvs1.begin(); // first vertex is on ground
        vfitr i1end = find_by_height(jvs1, *verts, bld_heights[b]);
        vfitr i2beg = jvs2.begin(); // first vertex is on ground
        vfitr i2end = find_by_height(jvs2, *verts, bld_heights[b]);
        assert(i1end != jvs1.end());
        assert(i2end != jvs2.end());
        if (!on_ground)
        {
          i1beg = find_by_height(jvs1, *verts, bld_heights[other-1]);
          i2beg = find_by_height(jvs2, *verts, bld_heights[other-1]);
          assert(i1beg != jvs1.end());
          assert(i2beg != jvs2.end());
          if (bld_heights[other-1] > bld_heights[b])
          {
            vcl_swap(i1beg, i1end);
            vcl_swap(i2beg, i2end);
          }
        }

        if (!on_ground && edge.building1 == b+1) // don't create side faces yet
        {
          // create the base vertices for later faces and attach the current roof
          vcl_vector<unsigned int>& e2m_set = edge_to_mesh[edge_idxs[e]];
          roof.push_back(*i1beg);
          for (unsigned int i=1; i<pts.size()-1; ++i) {
            const vgl_point_2d<double>& pt = pts[i];
            unsigned int bi = verts->size();
            verts->push_back(imesh_vertex<3>(pt.x(),-pt.y(),bld_heights[b]));
            roof.push_back(bi);
            e2m_set.push_back(bi);
          }
          continue;
        }

        vcl_vector<unsigned int> face;
        // start the next face
        roof.push_back(*i1end);
        ++i1end;
        face.insert(face.end(), vritr(i1end), vritr(i1beg));

        if (on_ground) // attach to the ground
        {
          // add all faces in the middle
          for (unsigned int i=1; i<pts.size()-1; ++i) {
            const vgl_point_2d<double>& pt = pts[i];
            double g = vil_bilin_interp_safe_extend(ground, pt.x(), pt.y());
            unsigned int bi = verts->size();
            verts->push_back(imesh_vertex<3>(pt.x(),-pt.y(), g));
            verts->push_back(imesh_vertex<3>(pt.x(),-pt.y(),bld_heights[b]));
            roof.push_back(bi+1);
            // finish last face
            face.push_back(bi);
            face.push_back(bi+1);
            faces->push_back(face);
            // start the next face
            face.clear();
            face.push_back(bi+1);
            face.push_back(bi);
          }
        }
        else // attach to previous level
        {
          if (e2m.empty() && pts.size() > 2)
            continue;
          for (unsigned int i=1; i<pts.size()-1; ++i) {
            const vgl_point_2d<double>& pt = pts[i];
            unsigned int bi = verts->size();
            verts->push_back(imesh_vertex<3>(pt.x(),-pt.y(),bld_heights[b]));
            roof.push_back(bi);
            // finish last face
            face.push_back(e2m[i-1]);
            face.push_back(bi);
            faces->push_back(face);
            // start the next face
            face.clear();
            face.push_back(bi);
            face.push_back(e2m[i-1]);
          }
        }
        // finish the last face
        ++i2end;
        face.insert(face.end(), i2beg, i2end);
        faces->push_back(face);
        face.clear();
      }
      if (roof.size() > 2)
      {
        if (h<0){
          roof_face = faces->size();
          faces->push_back(roof);
        }
        else
          roof_subtract_hole(*verts,(*faces)[roof_face],roof);
      }
    }
  }

  vcl_auto_ptr<imesh_vertex_array_base> vb(verts);
  vcl_auto_ptr<imesh_face_array_base> fb(faces);
  mesh.set_vertices(vb);
  mesh.set_faces(fb);
}


//: Subtract a hole from an existing face in a mesh
void bmdl_mesh::roof_subtract_hole(const imesh_vertex_array<3>& verts,
                                   vcl_vector<unsigned int>& face,
                                   const vcl_vector<unsigned int>& hole)
{
  unsigned int i1, i2;
  bool valid = false;
  for (i1=0; i1<face.size(); ++i1)
  {
    vgl_point_2d<double> v1(verts[face[i1]][0],verts[face[i1]][1]);
    for (i2=0; i2<hole.size(); ++i2)
    {
      vgl_point_2d<double> v2(verts[hole[i2]][0],verts[hole[i2]][1]);
      // look for self intersections
      valid = true;
      for (unsigned int j1 = face.size()-1, j2=0; j2<face.size() && valid; j1=j2, ++j2)
      {
        if (j1 == i1 || j2 == i1)
          continue;
        vgl_point_2d<double> v3(verts[face[j1]][0],verts[face[j1]][1]);
        vgl_point_2d<double> v4(verts[face[j2]][0],verts[face[j2]][0]);
        valid = !vgl_lineseg_test_lineseg(v1.x(),v1.y(),v2.x(),v2.y(),
                                          v3.x(),v3.y(),v4.x(),v4.y());
      }
      for (unsigned int j1 = hole.size()-1, j2=0; j2<hole.size() && valid; j1=j2, ++j2)
      {
        if (j1 == i2 || j2 == i2)
          continue;
        vgl_point_2d<double> v3(verts[hole[j1]][0],verts[hole[j1]][1]);
        vgl_point_2d<double> v4(verts[hole[j2]][0],verts[hole[j2]][1]);
        valid = !vgl_lineseg_test_lineseg(v1.x(),v1.y(),v2.x(),v2.y(),
                                          v3.x(),v3.y(),v4.x(),v4.y());
      }
      if (valid)
        break;
    }
    if (valid)
      break;
  }

  if (!valid)
  {
    vcl_cout << "couldn't find connection for hole"<<vcl_endl;
    return;
  }

  vcl_vector<unsigned int> new_idxs;
  new_idxs.push_back(face[i1]);
  new_idxs.insert(new_idxs.end(), hole.begin()+i2, hole.end());
  new_idxs.insert(new_idxs.end(), hole.begin(), hole.begin()+i2);
  new_idxs.push_back(hole[i2]);
  face.insert(face.begin()+i1, new_idxs.begin(), new_idxs.end());
}

