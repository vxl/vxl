#ifndef vtol_face_2d_h
#define vtol_face_2d_h
//:
//  \file
// \brief Represents the basic 2D topological entity
//
//  The vtol_face_2d class is used to represent a topological face.
//  A vtol_face_2d maintains a pointer to the Surface which describes the
//  mathematical geometry of the face.  The connectivity between
//  faces may be obtained from the superior two_chains of the face.
//  The boundary of the face may be obtained from the inferior vtol_one_chain_2ds
//  of the face.
//
// \verbatim
// Modifications:
//    JLM Dec 1995: Added timeStamp (Touch) to
//        operations which affect bounds.
//    JLM Dec 1995: Added method for ComputeBoundingBox
//        (Need to decide proper policy for curved surfaces
//        and possibly inconsistent planar surface geometry)
//    JSL Computed Area()
//    JLM Sep 1996: Fixed the face copy constructor which
//        did not handle the construction of new vtol_edge_2d(s) properly.
//        The old implementation always constucted ImplicitLine(s)
//        for the curve of each new edge.  See vtol_edge_2d.h for the required
//        alterations of the vtol_edge_2d constructors.  There is still an
//        issue with proper copying of the vtol_face_2d's Surface.  It isn't
//        done correctly.
//    PVR Aug 97: is_within_projection() implementation restored.
//    AWF Jul 1998: General topology speedup by replacing calls to
//        vertices() et al with iterators.  Benchmark: constructing
//        40K triangles, old: 37 sec, new: 9 sec.
//     PTU ported to vxl may-20
// \endverbatim

#include <vtol/vtol_face_2d_sptr.h>

#include <vtol/vtol_topology_object.h>
#include <vcl_vector.h>
#include <vsol/vsol_region_2d_sptr.h>
#include <vtol/vtol_face.h>

class vtol_vertex_2d;
class vtol_edge_2d;
class vtol_one_chain_2d;
class vtol_two_chain_2d;

class vtol_face_2d
  : public vtol_face
{
public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor
  //  REQUIRE: verts.size()>2
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vertex_list &verts);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(one_chain_list &onechs);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vtol_one_chain &edgeloop);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vsol_region_2d &facesurf);


  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vtol_face_2d(const vtol_face_2d &other);

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

  //***************************************************************************
  // Status report
  //***************************************************************************


  //---------------------------------------------------------------------------
  //: Copy with no links. Only copy the surface if it exists
  //---------------------------------------------------------------------------
  virtual vtol_face *shallow_copy_with_no_links(void) const;

  virtual bool operator==(const vtol_face_2d &other)const;
  inline bool operator!=(const vtol_face_2d &other)const{return !operator==(other);}
  bool operator==(const vtol_face &other)const; // virtual of vtol_face
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void print(vcl_ostream &strm=vcl_cout) const;

  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;

  //: provide a mechanism to compare geometry
  virtual bool compare_geometry(const vtol_face &other) const;

  //: this should not called by a client
  virtual vtol_face *
  copy_with_arrays(vcl_vector<vtol_topology_object_sptr> &verts,
                   vcl_vector<vtol_topology_object_sptr> &edges) const;

private:
  vsol_region_2d_sptr surface_;
};

#endif // vtol_face_2d_h
