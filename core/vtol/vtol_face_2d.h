#ifndef vtol_face_2d_H
#define vtol_face_2d_H
// .NAME vtol_face_2d - Represents the basic 2D topological entity
// .LIBRARY vtol
// .HEADER vxl Package
// .INCLUDE vtol/vtol_face_2d.h
// .FILE vtol_face_2d.h
// .FILE vtol_face_2d.cxx
// .SECTION Description
//  The vtol_face_2d class is used to represent a topological face.
//  A vtol_face_2d maintains a pointer to the Surface which describes the
//  mathematical geometry of the face.  The connectivity between
//  faces may be obtained from the superior two_chains of the face.
//  The boundary of the face may be obtained from the inferior vtol_one_chain_2ds
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
//
///
#include <vtol/vtol_face_2d_ref.h>

#include <vtol/vtol_topology_object_2d.h>
#include <vcl/vcl_vector.h>
#include <vsol/vsol_region_2d_ref.h>

class vtol_vertex_2d;
class vtol_edge_2d;
class vtol_one_chain_2d;
class vtol_two_chain_2d;

class vtol_face_2d
  : public vtol_topology_object_2d
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
  //: REQUIRE: verts.size()>2
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vertex_list_2d &verts);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(one_chain_list_2d &onechs);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vtol_one_chain_2d &edgeloop);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(vsol_region_2d &facesurf);

  //---------------------------------------------------------------------------
  //: Constructor
  //---------------------------------------------------------------------------
  explicit vtol_face_2d(edge_list_2d &edges);//for faces with interior holes

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
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  //: Accessors
  
  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  virtual vsol_region_2d_ref surface(void) const;
  virtual void set_surface(vsol_region_2d *const newsurf);
  virtual vtol_one_chain_2d *get_one_chain(int which = 0);
  virtual vtol_one_chain_2d *get_boundary_cycle(void);
  virtual bool add_hole_cycle(vtol_one_chain_2d &new_hole);
  virtual vcl_vector<vtol_one_chain_2d_ref> *get_hole_cycles(void);

  // Methods that are here for now in transition.. :x

  //: Inferior/Superior Accessor Methods
  virtual vtol_face_2d *
  copy_with_arrays(vcl_vector<vtol_topology_object_2d_ref> &verts,
                   vcl_vector<vtol_topology_object_2d_ref> &edges) const;

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_face_2d *cast_to_face(void) const;
  
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_face_2d *cast_to_face(void);

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_inferior_type(const vtol_topology_object_2d &inferior) const;

  //---------------------------------------------------------------------------
  //: Is `superior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool
  valid_superior_type(const vtol_topology_object_2d &superior) const;
  
  //: accessors for boundary elements 
  virtual vertex_list_2d *outside_boundary_vertices(void);
  virtual zero_chain_list_2d *outside_boundary_zero_chains(void);
  virtual edge_list_2d *outside_boundary_edges(void);
  virtual one_chain_list_2d *outside_boundary_one_chains(void);

  //: Warning these methods should not be used by clients 

  virtual vcl_vector<vtol_vertex_2d*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge_2d*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain_2d*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face_2d*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain_2d*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block_2d*> *compute_blocks(void);

  virtual vcl_vector<vtol_vertex_2d*> *outside_boundary_compute_vertices(void);
  virtual vcl_vector<vtol_zero_chain_2d*> *outside_boundary_compute_zero_chains(void);
  virtual vcl_vector<vtol_edge_2d*> *outside_boundary_compute_edges(void);
  virtual vcl_vector<vtol_one_chain_2d*> *outside_boundary_compute_one_chains(void);



  virtual void add_one_chain(vtol_one_chain_2d &);

  // Editing Functions

  //: Utility Functions and overloaded operators

  virtual void reverse_normal(void);
  virtual int get_num_edges(void) const;
   
  //---------------------------------------------------------------------------
  //: Copy with no links. Only copy the surface if it exists
  //---------------------------------------------------------------------------
  virtual vtol_face_2d *shallow_copy_with_no_links(void) const;

  virtual void compute_bounding_box(void); //A local implementation

  virtual bool operator==(const vtol_face_2d &other)const;

  virtual void print(ostream &strm=cout) const;

  virtual void describe(ostream &strm=cout,
                        int blanking=0) const;
  //---------------------------------------------------------------------------
  //: Does `this' share an edge with `f' ? Comparison of edge pointers,
  //: not geometric values
  //---------------------------------------------------------------------------
  virtual bool shares_edge_with(vtol_face_2d &f);

private:
  vsol_region_2d_ref _surface;
};

#endif
