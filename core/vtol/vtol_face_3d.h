
#ifndef vtol_face_3d_H
#define vtol_face_3d_H
// .NAME vtol_face_3d - Represents the basic 3D topological entity
// .LIBRARY vtol
// .HEADER vxl Package
// .INCLUDE vtol/vtol_face_3d.h
// .FILE vtol_face_3d.h
// .FILE vtol_face_3d.cxx
// .SECTION Description
//  The vtol_face_3d class is used to represent a topological face.
//  A vtol_face_3d maintains a pointer to the Surface which describes the
//  mathematical geometry of the face.  The connectivity between
//  faces may be obtained from the superior two_chains of the face.
//  The boundary of the face may be obtained from the inferior vtol_one_chain_3ds
//  of the face.
//
// .SECTION Modifications:
//    JLM Dec 1995: Added timeStamp (Touch) to
//        operations which affect bounds.
//    JLM Dec 1995: Added method for ComputeBoundingBox
//        (Need to decide proper policy for curved surfaces
//        and possibly inconsistent planar surface geometry)
//    JSL Computed Area()
//    JLM Sep 1996: Fixed the face copy constructor which 
//        did not handle the construction of new vtol_edge_3d(s) properly.
//        The old implementation always constucted ImplicitLine(s)
//        for the curve of each new edge.  See vtol_edge_3d.h for the required
//        alterations of the vtol_edge_3d constructors.  There is still an
//        issue with proper copying of the vtol_face_3d's Surface.  It isn't
//        done correctly.
//    PVR Aug 97: is_within_projection() implementation restored.
//    AWF Jul 1998: General topology speedup by replacing calls to
//        vertices() et al with iterators.  Benchmark: constructing
//        40K triangles, old: 37 sec, new: 9 sec.
//     PTU ported to vxl may-20
//
///

class vtol_face_3d;

#include <vtol/vtol_face_3d_ref.h>
#include <vtol/vtol_topology_object_3d.h>
#include <vcl/vcl_vector.h>

class vtol_vertex_3d;
class vtol_edge_3d;
class vtol_one_chain_3d;
class vtol_two_chain_3d;
#include <vsol/vsol_surface_3d_ref.h>

class vtol_face_3d
  : public vtol_topology_object_3d
{
public:

  //: Constructors and Destructors
  explicit vtol_face_3d(void);
  vtol_face_3d(const vtol_face_3d &other);
  explicit vtol_face_3d(vcl_vector<vtol_vertex_3d *> *verts);
  explicit vtol_face_3d(vcl_vector<vtol_one_chain_3d *> &onechs);
  explicit vtol_face_3d(vtol_one_chain_3d *edgeloop);
  explicit vtol_face_3d(vsol_surface_3d_ref facesurf);
  explicit vtol_face_3d(vcl_vector<vtol_edge_3d *> *edges);//for faces with interior holes
  virtual ~vtol_face_3d();
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  //: Accessors
  
  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  vsol_surface_3d_ref get_surface(void) const;
  void set_surface(vsol_surface_3d_ref newsurf);
  vtol_one_chain_3d *get_one_chain(int which = 0);
  vtol_one_chain_3d *get_boundary_cycle(void);
  bool add_hole_cycle(vtol_one_chain_3d *new_hole);
  vcl_vector<vtol_one_chain_3d *> *get_hole_cycles(void);

  // Methods that are here for now in transition.. :x

  //: Inferior/Superior Accessor Methods
  vtol_face_3d *copy_with_arrays(vcl_vector<vtol_topology_object_3d *> &verts,
				 vcl_vector<vtol_topology_object_3d *> &edges);

  vtol_face_3d *cast_to_face_3d(void) { return this; }
 
  vcl_vector<vtol_vertex_3d *> *outside_boundary_vertices(void);
  vcl_vector<vtol_vertex_3d *> *vertices(void);
  vcl_vector<vtol_zero_chain_3d *> *outside_boundary_zero_chains(void);
  vcl_vector<vtol_zero_chain_3d *> *zero_chains(void);
  vcl_vector<vtol_edge_3d *> *outside_boundary_edges(void);
  vcl_vector<vtol_edge_3d *> *edges(void);
  vcl_vector<vtol_one_chain_3d *> *outside_boundary_one_chains(void);
  vcl_vector<vtol_one_chain_3d *> *one_chains(void);
  vcl_vector<vtol_face_3d *> *faces(void);
  vcl_vector<vtol_two_chain_3d *> *two_chains(void);
  vcl_vector<vtol_block_3d *> *blocks(void);

  bool add_edge_loop(vtol_one_chain_3d *);
  bool add_one_chain(vtol_one_chain_3d *);
  bool remove_edge_loop(vtol_one_chain_3d *);
  bool remove_one_chain(vtol_one_chain_3d *);
  bool add_face_loop(vtol_two_chain_3d *);
  bool add_two_chain(vtol_two_chain_3d *);
  bool remove_face_loop(vtol_two_chain_3d *);
  bool remove_two_chain(vtol_two_chain_3d *);

  // Editing Functions

  //: Utility Functions and overloaded operators

  void reverse_normal(void);
  int get_num_edges(void);
   
  //: deep copy
  vtol_face_3d *copy(void) { return new vtol_face_3d(*this); } // deep copy
  virtual vtol_topology_object_3d * shallow_copy_with_no_links( void );

  virtual vsol_spatial_object_3d *spatial_copy() { return this->copy(); }


  virtual void compute_bounding_box(void); //A local implementation


  bool operator== (const vtol_face_3d &) const;
  bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d

  void print (ostream &strm=cout) const;

  void describe(ostream &strm=cout, int blanking=0) const;


  bool remove(vtol_one_chain_3d *one_chain,
              vcl_vector<vtol_topology_object_3d *> &changes,
              vcl_vector<vtol_topology_object_3d *> &deleted);
  void deep_remove(vcl_vector<vtol_topology_object_3d *> &removed);

  virtual bool disconnect(vcl_vector<vtol_topology_object_3d *> &changes,
                          vcl_vector<vtol_topology_object_3d *> &deleted);

  bool shares_edge_with(vtol_face_3d *f);

 private:
  vsol_surface_3d_ref _surface;
};

#endif
