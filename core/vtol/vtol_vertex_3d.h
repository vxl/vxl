#ifndef VTOL_VERTEX_3D_H
#define VTOL_VERTEX_3D_H

// .NAME vtol_vertex_3d - Topological container for a spatial point
// .LIBRARY vtol
// .HEADER vxl Package
// .INCLUDE vtol/vtol_vertex_3d.h
// .FILE vtol_vertex_3d.h
// .FILE vtol_vertex_3d.cxx
// .SECTION Description
//  The vtol_vertex_3d class is used to represent either a 3D or 3D point on
//  a topological structure.  A vtol_vertex_3d maintains a pointer to the IUPoint
//  which is the actual spatial point.
//
// .SECTION Modifications:
//     JLM December 1995, Added timeStamp (touch) to
//                        operations which affect bounds.
//
//     JLM October 1996,  Added the method EuclideanDistance(vtol_vertex_3d &)
//     to permit Charlie Rothwell's Polyhedra code to be more
//     generic.  Note this is distance, NOT squared distance.
//     LEG May 2000. ported to vxl
//
/// .EXAMPLE vtol_vertex_3d.example


#include <vtol/vtol_topology_object_3d.h>
// #include <vsol/vsol_point_3d.h>
#include <vnl/vnl_double_2.h>
//#include <vtol/sol_stubs.h>
#include <vsol/vsol_point_3d_ref.h>

typedef vnl_double_2  vector_3d;

class vtol_vertex_3d
  : public vtol_topology_object_3d
{
public:

  // Constructors and Destructors

  vtol_vertex_3d(void);
  vtol_vertex_3d(const vsol_point_3d_ref);
  vtol_vertex_3d(const vsol_point_3d &);   // for some peculiar cases.
  vtol_vertex_3d(const vtol_vertex_3d &other);
  vtol_vertex_3d(const vector_3d &);
  vtol_vertex_3d(double,double,double);

  ~vtol_vertex_3d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_ref clone(void) const;

  //  vector_3d *get_location() { return _point->get_location();}
  //  vector_3d *get_orientation() { return _point->get_orientation();}
  //  vector_3d *get_size() { return _point->get_size();}
  //void set_location(double x, double y) { _point->set_location(x, y);}
  //void set_location(vector_3d&m) { _point->set_location(m);}
  //void set_orientation(double x, double y) { _point->set_orientation(x,y);}
  //void set_orientation(vector_3d & m) {_point->set_orientation(m);}
  //void set_size(double x, double y) {_point->set_size(x, y);}
  //void set_size(vector_3d&m) {_point->set_size(m);}

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_3d_type topology_type(void) const;

  virtual vsol_point_3d_ref get_point(void) const;
  virtual void set_point(vsol_point_3d_ref pt);

  // vtol_topology_object_3d::topology_object_type get_topology_type() const { return vtol_topology_object_3d::VERTEX; }
  
  vertex_list_3d *vertices(void);
  edge_list_3d *edges(void);
  zero_chain_list_3d *zero_chains(void);
  one_chain_list_3d *one_chains(void);
  face_list_3d *faces(void);
  two_chain_list_3d *two_chains(void);
  block_list_3d *blocks(void);

  void explore_vertex(vertex_list_3d&);
  // Methods called on Vertex
  // for vsol_point_3d.   These are here
  // during the transition period.
  // Looks like forever now - JLM

  virtual double x(void) const;
  virtual double y(void) const;
  virtual double z(void) const;
  

  virtual void set_x(double val);
  virtual void set_y(double val);
  virtual void set_z(double val);

  //  bool Transform(CoolTransform const& );

  bool operator==(const vsol_spatial_object_3d &obj) const; // virtual of vsol_spatial_object_3d.
  bool operator==(const vtol_vertex_3d &) const;
  vtol_vertex_3d& operator=(const vtol_vertex_3d&);
  vtol_vertex_3d *cast_to_vertex(void);
  vtol_vertex_3d *copy(void);
  virtual vsol_spatial_object_3d *spatial_copy(void);

  int order(void);
  bool is_connected(vtol_vertex_3d *);
  bool eq(vtol_vertex_3d &);
  vtol_edge_3d *new_edge(vtol_vertex_3d *);
  vtol_vertex_3d *vertex_diff (vtol_vertex_3d &);
  bool is_endpointp (const vtol_edge_3d &);
  double distance_from(const vector_3d &);
  double euclidean_distance(vtol_vertex_3d &v); //actual distance, not squared - JLM
  void merge_references(vtol_vertex_3d*);
  //  void calculate_average_normal(IUE_vector<double>&);


  void print(ostream &strm=cout);
  void describe(ostream &strm=cout,
                int blanking=0);

  void deep_remove(topology_list_3d &removed);
  virtual vtol_topology_object_3d *shallow_copy_with_no_links(void);
  virtual bool disconnect(topology_list_3d &changes,
                          topology_list_3d &deleted);
protected:
  //  vsol_point_3d *_point;

  //---------------------------------------------------------------------------
  // Description: point associated to the vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_ref _point;
};

#endif

