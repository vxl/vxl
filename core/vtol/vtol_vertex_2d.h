#ifndef VTOL_VERTEX_2D_H
#define VTOL_VERTEX_2D_H

// .NAME vtol_vertex_2d - Topological container for a spatial point
// .LIBRARY vtol
// .HEADER vxl Package
// .INCLUDE vtol/vtol_vertex_2d.h
// .FILE vtol_vertex_2d.h
// .FILE vtol_vertex_2d.cxx
// .SECTION Description
//  The vtol_vertex_2d class is used to represent either a 2D or 2D point on
//  a topological structure.  A vtol_vertex_2d maintains a pointer to the IUPoint
//  which is the actual spatial point.
//
// .SECTION Modifications:
//     JLM December 1995, Added timeStamp (touch) to
//                        operations which affect bounds.
//
//     JLM October 1996,  Added the method EuclideanDistance(vtol_vertex_2d &)
//     to permit Charlie Rothwell's Polyhedra code to be more
//     generic.  Note this is distance, NOT squared distance.
//     LEG May 2000. ported to vxl
//
/// .EXAMPLE vtol_vertex_2d.example


#include <vtol/vtol_topology_object_2d.h>
// #include <vsol/vsol_point_2d.h>
#include <vnl/vnl_double_2.h>
//#include <vtol/sol_stubs.h>
#include <vsol/vsol_point_2d_ref.h>

typedef vnl_double_2  vector_2d;

class vtol_vertex_2d
  : public vtol_topology_object_2d
{
public:

  // Constructors and Destructors

  vtol_vertex_2d(void);
  vtol_vertex_2d(const vsol_point_2d_ref);
  vtol_vertex_2d(const vsol_point_2d &);   // for some peculiar cases.
  vtol_vertex_2d(const vtol_vertex_2d &other);
  vtol_vertex_2d(const vector_2d &);
  vtol_vertex_2d(double,double);

  ~vtol_vertex_2d();
  
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_ref clone(void) const;

  //  vector_2d *get_location() { return _point->get_location();}
  //  vector_2d *get_orientation() { return _point->get_orientation();}
  //  vector_2d *get_size() { return _point->get_size();}
  //void set_location(double x, double y) { _point->set_location(x, y);}
  //void set_location(vector_2d&m) { _point->set_location(m);}
  //void set_orientation(double x, double y) { _point->set_orientation(x,y);}
  //void set_orientation(vector_2d & m) {_point->set_orientation(m);}
  //void set_size(double x, double y) {_point->set_size(x, y);}
  //void set_size(vector_2d&m) {_point->set_size(m);}

  // Accessors

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_2d_type topology_type(void) const;

  virtual vsol_point_2d_ref get_point(void) const;
  virtual void set_point(vsol_point_2d_ref pt);

  // vtol_topology_object_2d::topology_object_type get_topology_type() const { return vtol_topology_object_2d::VERTEX; }
  
  vertex_list_2d *vertices(void);
  edge_list_2d *edges(void);
  zero_chain_list_2d *zero_chains(void);
  one_chain_list_2d *one_chains(void);
  face_list_2d *faces(void);
  two_chain_list_2d *two_chains(void);
  block_list_2d *blocks(void);

  void explore_vertex(vertex_list_2d&);
  // Methods called on Vertex
  // for vsol_point_2d.   These are here
  // during the transition period.
  // Looks like forever now - JLM

  virtual double x(void) const;
  virtual double y(void) const;

  virtual void set_x(double val);
  virtual void set_y(double val);

  //  bool Transform(CoolTransform const& );

  bool operator==(const vsol_spatial_object_2d &obj) const; // virtual of vsol_spatial_object_2d.
  bool operator==(const vtol_vertex_2d &) const;
  vtol_vertex_2d& operator=(const vtol_vertex_2d&);
  vtol_vertex_2d *cast_to_vertex(void);
  vtol_vertex_2d *copy(void);
  virtual vsol_spatial_object_2d *spatial_copy(void);

  int order(void);
  bool is_connected(vtol_vertex_2d *);
  bool eq(vtol_vertex_2d &);
  vtol_edge_2d *new_edge(vtol_vertex_2d *);
  vtol_vertex_2d *vertex_diff (vtol_vertex_2d &);
  bool is_endpointp (const vtol_edge_2d &);
  double distance_from(const vector_2d &);
  double euclidean_distance(vtol_vertex_2d &v); //actual distance, not squared - JLM
  void merge_references(vtol_vertex_2d*);
  //  void calculate_average_normal(IUE_vector<double>&);


  void print(ostream &strm=cout);
  void describe(ostream &strm=cout,
                int blanking=0);

  void deep_remove(topology_list_2d &removed);
  virtual vtol_topology_object_2d *shallow_copy_with_no_links(void);
  virtual bool disconnect(topology_list_2d &changes,
                          topology_list_2d &deleted);
protected:
  //  vsol_point_2d *_point;

  //---------------------------------------------------------------------------
  // Description: point associated to the vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_ref _point;
};

#endif

