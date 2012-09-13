// This is brl/bbas/imesh/algo/imesh_generate_mesh.cxx
#include "imesh_generate_mesh.h"
//:
// \file
extern "C" {
#include <triangle.h>
}
#include <vcl_cassert.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#define GRID_SIZE 1000 // defined by the tolerance for equal vertices

// a grid index to find coincident vertices
class point_index
{
 public:
  point_index(vgl_box_2d<double> const&  bb)
  : vi_(0), x_min_(bb.min_x()), y_min_(bb.min_y()),
    x_size_(bb.width()), y_size_(bb.height())
  {
    img_.set_size(GRID_SIZE, GRID_SIZE);
    img_.fill(-1);//no vert at location
  }

  void pt_to_img(vgl_point_2d<double> const& pt, unsigned& i, unsigned& j)
  {
    double xi = pt.x()-x_min_, yi = pt.y()-y_min_;
    i = static_cast<unsigned>((xi*GRID_SIZE)/x_size_);
    j = static_cast<unsigned>((yi*GRID_SIZE)/y_size_);
    if (i>=GRID_SIZE) i=GRID_SIZE-1;
    if (j>=GRID_SIZE) j=GRID_SIZE-1;
  }

  void insert_point(vgl_point_2d<double> const& pt)
  {
    unsigned i=0, j=0;
    pt_to_img(pt, i, j);
    if (img_(i,j)>=0)
      return;//point already exists
    //insert vertex index
    img_(i,j) = vi_++;
  }

  int vert_index(vgl_point_2d<double> const& pt)
  {
    unsigned i=0, j=0;
    pt_to_img(pt, i, j);
    int index = img_(i,j);
    assert(index>=0);
    return index;
  }

  unsigned size() const {return vi_;}

 private:
  unsigned vi_;
  double x_min_;
  double y_min_;
  double x_size_;
  double y_size_;
  vil_image_view<int> img_;
};

void
imesh_generate_mesh_2d(vcl_vector<vgl_point_2d<double> > const& convex_hull,
                       vcl_vector<vgl_line_segment_2d<double> > const& segs,
                       imesh_mesh& mesh)
{
  // form a grid to store vertex indices in order to detect
  // shared vertices
  unsigned nch = convex_hull.size();
  vgl_box_2d<double> bb;
  for (unsigned i = 0; i<nch; ++i)
    bb.add(convex_hull[i]);
  //insert all points into a grid index
  point_index pindx(bb);
  for (unsigned i =0; i<nch; ++i)
    pindx.insert_point(convex_hull[i]);
  unsigned nsegs = segs.size();
  for (unsigned i = 0; i<nsegs; ++i) {
    pindx.insert_point(segs[i].point1());
    pindx.insert_point(segs[i].point2());
  }
  // pindx now contains a single vertex pointer for each pair
  // of coincident line segment endpoints as well as for each
  // isolated endpoint
  unsigned npts = pindx.size();
  //set up the Delunay constrained triangulation input parameters
  struct triangulateio in, out, vorout;
  in.numberofholes = 0;
  in.numberofregions = 0;
  in.numberofpointattributes = 0;
  in.numberofpoints = npts;
  in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
  for (unsigned i = 0; i<nch; ++i) {
    unsigned pi = pindx.vert_index(convex_hull[i]);
    in.pointlist[2*pi]=convex_hull[i].x();
    in.pointlist[2*pi+1]=convex_hull[i].y();
  }
  // line segments to be inserted into the triangulation
  in.numberofsegments = nsegs;
  in.segmentlist = (int *) malloc(in.numberofsegments * 2 * sizeof(int));
  unsigned m = 0;
  for (unsigned i = 0; i<nsegs; ++i) {
    unsigned pi1 = pindx.vert_index(segs[i].point1());
    unsigned pi2 = pindx.vert_index(segs[i].point2());
    in.pointlist[2*pi1]   = segs[i].point1().x();
    in.pointlist[2*pi1+1] = segs[i].point1().y();
    in.pointlist[2*pi2]   = segs[i].point2().x();
    in.pointlist[2*pi2+1] = segs[i].point2().y();
    in.segmentlist[m]=pi1;     in.segmentlist[m+1]=pi2;
  }
  //attributes and markers are not used in this algorithm
  in.pointattributelist = NULL;
  in.pointmarkerlist = (int*)malloc(in.numberofpoints * sizeof(int));
  for (int i = 0; i<in.numberofpoints; ++i)
    in.pointmarkerlist[i]=0;
  in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));;
  for (int i = 0; i<in.numberofsegments; ++i)
    in.segmentmarkerlist[i]=0;

  out.pointlist = (REAL *) NULL;            // Not needed if -N switch used.
  // Not needed if -N switch used or number of point attributes is zero:
  out.pointattributelist = (REAL *) NULL;
  out.pointmarkerlist = (int *) NULL; // Not needed if -N or -B switch used.
  out.trianglelist = (int *) NULL;          // Not needed if -E switch used.
  out.neighborlist = (int *) NULL;         // Needed only if -n switch used.
  // Needed only if segments are output (-p or -c) and -P not used:
  out.segmentlist = (int *) (REAL *) malloc(in.numberofsegments * 2 * sizeof(REAL)); // Suspicious. Why the double cast? and why sizeof(REAL) if it's really int*?
  // Needed only if segments are output (-p or -c) and -P and -B not used:
  out.segmentmarkerlist = (int *) NULL;
  out.edgelist = (int *) NULL;             // Needed only if -e switch used.
  out.edgemarkerlist = (int *) NULL;   // Needed if -e used and -B not used.

  //A string of switch characters must be provided
  // Triangulate the points.  Switches are chosen to read and write a
  //   PSLG (p), preserve the convex hull (c), number everything from
  //   zero (z), produce an edge list (e), and a triangle neighbor list (n)

  triangulate("Qpczen", &in, &out, &vorout);

  // construct the imesh data structure
  //construct vertices
  imesh_vertex_array<2>* verts = new imesh_vertex_array<2>();
  unsigned k = 0;
  for (unsigned i = 0; i<npts; ++i, k+=2)
    verts->push_back(imesh_vertex<2>(out.pointlist[k], out.pointlist[k+1]));
  vcl_auto_ptr<imesh_vertex_array_base> v(verts);
  mesh.set_vertices(v);
  //construct triangular faces
  unsigned ntri = static_cast<unsigned>(out.numberoftriangles);
  imesh_regular_face_array<3>* faces = new imesh_regular_face_array<3>();
  for (unsigned t = 0; t<ntri; ++t)
  {
    imesh_tri tri(out.trianglelist[t*3],
                  out.trianglelist[t*3+1],
                  out.trianglelist[t*3+2]);
    faces->push_back(tri);
  }
  //set the faces on the mesh
  vcl_auto_ptr<imesh_face_array_base> f(faces);
  mesh.set_faces(f);
}


void
imesh_generate_mesh_2d_2(vcl_vector<vgl_point_2d<double> > const& convex_hull,
                         vcl_vector<vgl_line_segment_2d<double> > const& segs,
                         vcl_vector<vgl_point_2d<double> > const & points,
                         imesh_mesh& mesh)
{
  // form a grid to store vertex indices in order to detect
  // shared vertices
  unsigned nch = convex_hull.size();
  vgl_box_2d<double> bb;
  for (unsigned i = 0; i<nch; ++i)
    bb.add(convex_hull[i]);
  //insert all points into a grid index
  point_index pindx(bb);
  for (unsigned i =0; i<nch; ++i)
    pindx.insert_point(convex_hull[i]);
  unsigned nsegs = segs.size();
  for (unsigned i = 0; i<nsegs; ++i) {
    pindx.insert_point(segs[i].point1());
    pindx.insert_point(segs[i].point2());
  }

  for (unsigned i = 0; i<points.size(); ++i) {
      pindx.insert_point(points[i]);
  }
  // pindx now contains a single vertex pointer for each pair
  // of coincident line segment endpoints as well as for each
  // isolated endpoint
  unsigned npts = pindx.size();
  //set up the Delunay constrained triangulation input parameters
  struct triangulateio in, out, vorout;
  in.numberofholes = 0;
  in.numberofregions = 0;
  in.numberofpointattributes = 0;
  in.numberofpoints = npts;
  in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
  for (unsigned i = 0; i<nch; ++i) {
    unsigned pi = pindx.vert_index(convex_hull[i]);
    in.pointlist[2*pi]=convex_hull[i].x();
    in.pointlist[2*pi+1]=convex_hull[i].y();
  }
  for (unsigned i = 0; i<points.size(); ++i) {
    unsigned pi = pindx.vert_index(points[i]);
    in.pointlist[2*pi]=points[i].x();
    in.pointlist[2*pi+1]=points[i].y();
  }

  // line segments to be inserted into the triangulation
  in.numberofsegments = nsegs;
  in.segmentlist = (int *) malloc(in.numberofsegments * 2 * sizeof(int));
  unsigned m = 0;
  for (unsigned i = 0; i<nsegs; ++i) {
    unsigned pi1 = pindx.vert_index(segs[i].point1());
    unsigned pi2 = pindx.vert_index(segs[i].point2());
    in.pointlist[2*pi1]   = segs[i].point1().x();
    in.pointlist[2*pi1+1] = segs[i].point1().y();
    in.pointlist[2*pi2]   = segs[i].point2().x();
    in.pointlist[2*pi2+1] = segs[i].point2().y();
    in.segmentlist[m]=pi1;     in.segmentlist[m+1]=pi2;
  }
  //attributes and markers are not used in this algorithm
  in.pointattributelist = NULL;
  in.pointmarkerlist = (int*)malloc(in.numberofpoints * sizeof(int));
  for (int i = 0; i<in.numberofpoints; ++i)
    in.pointmarkerlist[i]=0;
  in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));;
  for (int i = 0; i<in.numberofsegments; ++i)
    in.segmentmarkerlist[i]=0;

  out.pointlist = (REAL *) NULL;            // Not needed if -N switch used.
  // Not needed if -N switch used or number of point attributes is zero:
  out.pointattributelist = (REAL *) NULL;
  out.pointmarkerlist = (int *) NULL; // Not needed if -N or -B switch used.
  out.trianglelist = (int *) NULL;          // Not needed if -E switch used.
  out.neighborlist = (int *) NULL;         // Needed only if -n switch used.
  // Needed only if segments are output (-p or -c) and -P not used:
  out.segmentlist = (int *) (REAL *) malloc(in.numberofsegments * 2 * sizeof(REAL)); // Suspicious. Why the double cast? and why sizeof(REAL) if it's really int*?
  // Needed only if segments are output (-p or -c) and -P and -B not used:
  out.segmentmarkerlist = (int *) NULL;
  out.edgelist = (int *) NULL;             // Needed only if -e switch used.
  out.edgemarkerlist = (int *) NULL;   // Needed if -e used and -B not used.

  //A string of switch characters must be provided
  // Triangulate the points.  Switches are chosen to read and write a
  //   PSLG (p), preserve the convex hull (c), number everything from
  //   zero (z), produce an edge list (e), and a triangle neighbor list (n)

  triangulate("Qpczen", &in, &out, &vorout);

  // construct the imesh data structure
  //construct vertices
  imesh_vertex_array<2>* verts = new imesh_vertex_array<2>();
  unsigned k = 0;
  for (unsigned i = 0; i<npts; ++i, k+=2)
    verts->push_back(imesh_vertex<2>(out.pointlist[k], out.pointlist[k+1]));
  vcl_auto_ptr<imesh_vertex_array_base> v(verts);
  mesh.set_vertices(v);
  //construct triangular faces
  unsigned ntri = static_cast<unsigned>(out.numberoftriangles);
  imesh_regular_face_array<3>* faces = new imesh_regular_face_array<3>();
  for (unsigned t = 0; t<ntri; ++t)
  {
    imesh_tri tri(out.trianglelist[t*3],
                  out.trianglelist[t*3+1],
                  out.trianglelist[t*3+2]);
    faces->push_back(tri);
  }
  //set the faces on the mesh
  vcl_auto_ptr<imesh_face_array_base> f(faces);
  mesh.set_faces(f);
}
