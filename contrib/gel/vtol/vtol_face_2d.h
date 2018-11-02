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

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  vtol_face_2d() : surface_(nullptr) {}

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
  ~vtol_face_2d() override = default;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vtol_face_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vtol_face::is_class(cls); }

  // Accessors

  virtual vsol_region_2d_sptr surface() const;
  virtual void set_surface(vsol_region_2d_sptr const& newsurf);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a 2D face, 0 otherwise
  //---------------------------------------------------------------------------
  const vtol_face_2d *cast_to_face_2d() const override { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a 2D face, 0 otherwise
  //---------------------------------------------------------------------------
  vtol_face_2d *cast_to_face_2d() override { return this; }

  //---------------------------------------------------------------------------
  //: Copy with no links. Only copy the surface if it exists
  //---------------------------------------------------------------------------
  vtol_face *shallow_copy_with_no_links() const override;

  virtual bool operator==(const vtol_face_2d &other)const;
  inline bool operator!=(const vtol_face_2d &other)const{return !operator==(other);}
  bool operator==(const vtol_face &other)const override; // virtual of vtol_face
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  void print(std::ostream &strm=std::cout) const override;

  void describe(std::ostream &strm=std::cout, int blanking=0) const override;

  //:  copy the geometry
  void copy_geometry(const vtol_face &other) override;

  //: provide a mechanism to compare geometry
  bool compare_geometry(const vtol_face &other) const override;

 protected:
  //: this should not called by a client
  vtol_face* copy_with_arrays(topology_list &verts, topology_list &edges) const override;
};

#endif // vtol_face_2d_h_
