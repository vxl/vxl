// This is gel/vtol/vtol_face_2d.h
#ifndef vtol_face_2d_h_
#define vtol_face_2d_h_
//:
// \file
// \brief Represents the basic 2D topological entity with 2d geometry (region)
//
//  The vtol_face_2d class is used to represent a topological face.
//  A vtol_face_2d maintains a pointer to the region which describes the
//  mathematical geometry of the face.  The connectivity between
//  faces may be obtained from the superior 2-chains of the face.
//  The boundary of the face may be obtained from the inferior 1-chains
//  of the face.
//
// \verbatim
//  Modifications:
//   JLM Dec 1995: Added timeStamp (Touch) to
//       operations which affect bounds.
//   JLM Dec 1995: Added method for ComputeBoundingBox
//       (Need to decide proper policy for curved surfaces
//       and possibly inconsistent planar surface geometry)
//   JSL Computed Area()
//   JLM Sep 1996: Fixed the face copy constructor which
//       did not handle the construction of new vtol_edge_2d(s) properly.
//       The old implementation always constructed ImplicitLine(s)
//       for the curve of each new edge.  See vtol_edge_2d.h for the required
//       alterations of the vtol_edge_2d constructors.  There is still an
//       issue with proper copying of the vtol_face_2d's Surface.  It isn't
//       done correctly.
//   PVR Aug 97: is_within_projection() implementation restored.
//   AWF Jul 1998: General topology speedup by replacing calls to
//       vertices() et al with iterators.  Benchmark: constructing
//       40K triangles, old: 37 sec, new: 9 sec.
//   PTU may-2000 ported to vxl
//   Dec. 2002, Peter Vanroose -interface change: vtol objects -> smart pointers
//   9 Jan. 2003, Peter Vanroose - added "copy_geometry()"
// \endverbatim

#include <vcl_iosfwd.h>
#include <vsol/vsol_region_2d_sptr.h>
#include <vtol/vtol_face.h>

class vtol_vertex_2d;
class vtol_edge_2d;
class vtol_one_chain_2d;
class vtol_two_chain_2d;

class vtol_face_2d : public vtol_face
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  vsol_region_2d_sptr surface_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  vtol_face_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor
  //  REQUIRE: verts.size()>2
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vertex_list const& verts);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(one_chain_list const& onechs);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vtol_one_chain_sptr const& edgeloop);
 private:
  // Deprecated
  explicit vtol_face_2d(vtol_one_chain &edgeloop);
 public:
  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vsol_region_2d &facesurf);

  //---------------------------------------------------------------------------
  //: Pseudo copy constructor.  Deep copy.
  //---------------------------------------------------------------------------
  vtol_face_2d(vtol_face_2d_sptr const& other);
 private:
  //---------------------------------------------------------------------------
  //: Copy constructor.  Deep copy.  Deprecated.
  //---------------------------------------------------------------------------
  vtol_face_2d(const vtol_face_2d &other);
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_face_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  // Accessors

  virtual vsol_region_2d_sptr surface(void) const;
  virtual void set_surface(vsol_region_2d_sptr const& newsurf);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_face_2d *cast_to_face_2d(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_face_2d *cast_to_face_2d(void) { return this; }

  //---------------------------------------------------------------------------
  //: Copy with no links. Only copy the surface if it exists
  //---------------------------------------------------------------------------
  virtual vtol_face *shallow_copy_with_no_links(void) const;

  virtual bool operator==(const vtol_face_2d &other)const;
  inline bool operator!=(const vtol_face_2d &other)const{return !operator==(other);}
  bool operator==(const vtol_face &other)const; // virtual of vtol_face
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void print(vcl_ostream &strm=vcl_cout) const;

  virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  //:  copy the geometry
  virtual void copy_geometry(const vtol_face &other);

  //: provide a mechanism to compare geometry
  virtual bool compare_geometry(const vtol_face &other) const;

 protected:
  //: this should not called by a client
  virtual vtol_face* copy_with_arrays(topology_list &verts, topology_list &edges) const;
};

#endif // vtol_face_2d_h_
