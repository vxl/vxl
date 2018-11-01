// This is core/vgl/vgl_frustum_3d.h
#ifndef vgl_frustum_3d_h
#define vgl_frustum_3d_h
//:
// \file
// \brief A polygonal cone truncated by parallel planes
// \author J.L. Mundy
// \date   December 1, 2013
//
// \verbatim
//  Modifications
// None
// \endverbatim

#include <iosfwd>
#include <vector>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h> // forward declare vgl datatypes

//: A 3D frustum is the portion of a solid (normally a cone or pyramid)
// that lies between two parallel planes cutting it.
//
template <class Type>
class vgl_frustum_3d
{
 public:
  //: default constructor
 vgl_frustum_3d(): near_plane_(0), far_plane_(0), n_top_bot_face_verts_(0){}

  //: Construct the frustum from rays
  // the corner rays intersect in a common origin point, i.e., the apex
  // of the frustum cone. It is assumed the rays are in sorted order
  // around the cone surface, so that sequential rays are coplanar
  // with a cone surface plane and the cross product r[i].dir X r[i+1].dir
  // defines an outward-pointing normal. norm is the vector
  // perpendicular to the parallel frustum faces. d0 is the distance in
  // the norm vector dirction from the apex to the closest parallel face.
  // d1 the distance in the norm direction from the apex to the far face.
  //
  vgl_frustum_3d(std::vector<vgl_ray_3d<Type> >  const& corner_rays,
                 vgl_vector_3d<Type> const& norm, Type d0, Type d1);


  //: Equality test
  inline bool operator==(vgl_frustum_3d<Type> const& other) const;

  // Data Access---------------------------------------------------------------
  const vgl_point_3d<Type>& apex() const{
    return apex_;}

  const std::vector<vgl_plane_3d<Type> >& surface_planes() const
  {return surface_planes_;}

  const vgl_plane_3d<Type>& near_plane() const
  {return surface_planes_[near_plane_];}

  const vgl_plane_3d<Type>& far_plane() const
  {return surface_planes_[far_plane_];}

  const std::vector<vgl_point_3d<Type> >& verts() const
  {return verts_;}

  const std::map<int, std::vector<int> >& faces() const
  {return faces_;}

  vgl_box_3d<Type> bounding_box() const;

  //: Get the centroid point
  vgl_point_3d<Type> centroid() const;

  //: test if the frustum is convex
  bool is_convex() const;

  // Data Control--------------------------------------------------------------

  //: Return true iff the point p is inside this frustum
  // assumes that the frustum is a convex solid
  bool contains(vgl_point_3d<Type> const& p) const;

  //: Return true if \a (x,y,z) is inside this frustum
  // assumes that the frustum is a convex solid
  bool contains(Type const& x, Type const& y, Type const& z) const;

  // I/O-----------------------------------------------------------------------


  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------
  vgl_point_3d<Type> apex_;
  vgl_vector_3d<Type> norm_;
  //: planes bounding the frustum volume including near and far plane
  std::vector<vgl_plane_3d<Type> > surface_planes_;
  int near_plane_;
  int far_plane_;
  std::vector<vgl_point_3d<Type> > verts_;
  //: the number of verts in the top or bottom face
  int n_top_bot_face_verts_;
  // key corresponds to plane index, value is clockwise verts on face boundary
  // clockwise with respect the the face normal
  std::map<int, std::vector<int> > faces_;
};

//: Write frustum to stream
// \relatesalso vgl_frustum_3d
template <class Type>
std::ostream&  operator<<(std::ostream& s, vgl_frustum_3d<Type> const& p);



#define VGL_FRUSTUM_3D_INSTANTIATE(T) extern "please include vgl/vgl_frustum_3d.hxx first"

#endif // vgl_frustum_3d_h
