#ifndef VCSL_SPATIAL_H
#define VCSL_SPATIAL_H
//*****************************************************************************
//
// .NAME vcsl_spatial - A spatial coordinate system
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_spatial.h
// .INCLUDE vcsl/vcsl_spatial_ref.h
// .INCLUDE vcsl/vcsl_coordinate_system.h
// .INCLUDE vnl/vnl_vector.h
// .INCLUDE vcl/vcl_vector.h
// .INCLUDE vcsl/vcsl_spatial_transformation_ref.h
// .INCLUDE vcsl/vcsl_graph_ref.h
// .FILE    vcsl/vcsl_spatial.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/07/10 François BERTEL Creation
//*****************************************************************************

#include <vcsl/vcsl_spatial_ref.h>

#include <vcsl/vcsl_coordinate_system.h>
#include <vnl/vnl_vector.h>
#include <vcl/vcl_vector.h>
#include <vcsl/vcsl_spatial_transformation_ref.h>
#include <vcsl/vcsl_graph_ref.h>

// Because VXL does not use dynamic_cast<> :-(

class vcsl_cartesian_2d;
class vcsl_polar;

class vcsl_cartesian_3d;
class vcsl_cylindrical;
class vcsl_spherical;

class vcsl_spatial
  :public vcsl_coordinate_system
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_spatial(void);

  //: Destructor
  virtual ~vcsl_spatial();

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************
  
  virtual const vcsl_spatial *cast_to_spatial(void) const;
  virtual const vcsl_cartesian_2d *cast_to_cartesian_2d(void) const;
  virtual const vcsl_polar *cast_to_polar(void) const;
  virtual const vcsl_cartesian_3d *cast_to_cartesian_3d(void) const;
  virtual const vcsl_cylindrical *cast_to_cylindrical(void) const;
  virtual const vcsl_spherical *cast_to_spherical(void) const;
  
  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the list of time clocks
  virtual vcl_vector<double> *beat(void) const;

  //: Return the list of parent coordinate system along the time
  virtual vcl_vector<vcsl_spatial_ref> *parent(void) const;

  //: Return the list of transformations along the time
  virtual vcl_vector<vcsl_spatial_transformation_ref> *motion(void) const;

  //: Is `time' between the two time bounds ?
  virtual bool valid_time(const double time) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the list of time clocks
  virtual void set_beat(vcl_vector<double> &new_beat);

  //: Set the list of parent coordinate system along the time
  virtual void set_parent(vcl_vector<vcsl_spatial_ref> &new_parent);

  //: Set the list of transformations along the time
  virtual
  void set_motion(vcl_vector<vcsl_spatial_transformation_ref> &new_motion);

  //: Set the unique parent and the unique motion
  virtual void set_unique(const vcsl_spatial_ref &new_parent,
                          const vcsl_spatial_transformation_ref &new_motion);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Return the index of the beat inferior or equal to `time'
  //: REQUIRE: parent()!=0
  //: REQUIRE: valid_time(time)
  virtual int matching_interval(const double time) const;

  //: Does a path from `this' to `other' exist ?
  virtual bool path_from_local_to_cs_exists(const vcsl_spatial_ref &other,
                                            const double time);

  //: Is `this' an absolute spatial coordinate system at time `time'?
  //: REQUIRE: valid_time(time)
  virtual bool is_absolute(const double time) const;

  
  //: From a vector `v' exprimed in `this',
  //: return a vector exprimed in the spatial coordinate system `other'
  //: REQUIRE: path_from_local_to_cs_exists(other,time)
  virtual vnl_vector<double> *from_local_to_cs(const vnl_vector<double> &v,
                                               const vcsl_spatial_ref &other,
                                               const double time);

  virtual void set_graph(const vcsl_graph_ref &new_graph);

  //: WARNING: Only used by vcsl_spatial and vcsl_graph
  //: DO NOT USE IT DIRECTLY
  virtual void set_reached(const bool &new_reached);

  virtual bool reached(void) const;

  //***************************************************************************
  // Implementation
  //***************************************************************************
protected:
  //: Does a path from `this' to `other' exist ? Called only by
  //: path_to_cs_exists()
  virtual bool
  recursive_path_from_local_to_cs_exists(const vcsl_spatial_ref &other,
                                         const double time);
  //: Find the sequence of transformations from `this' to `other'
  //: REQUIRE: path.size()==0 and sens.size()==0
  //: REQUIRE: path_from_local_to_cs_exists()
  virtual void
  path_from_local_to_cs(const vcsl_spatial_ref &other,
                        const double time,
                        vcl_vector<vcsl_spatial_transformation_ref> &path,
                        vcl_vector<bool> &sens);

  //: Find the sequence of transformations from `this' to `other'
  //: Called only by path_from_local_to_cs()
  virtual bool
  recursive_path_from_local_to_cs(const vcsl_spatial_ref &other,
                                  const double time,
                                  vcl_vector<vcsl_spatial_transformation_ref> &path,
                                  vcl_vector<bool> &sens);

  //: successive parents of `this' along the time
  vcl_vector<vcsl_spatial_ref> *_parent;

  //: Clock times 
  vcl_vector<double> *_beat;

  //: successive transformations from `this' to `parent' along the time
  vcl_vector<vcsl_spatial_transformation_ref> *_motion;

  //: List of spatial coordinate system that can be child of `this' at a time
  vcl_vector<vcsl_spatial_ref> *_potential_children;

  //: List of all the spatial coordinate system of the graph
  vcsl_graph_ref _graph;

  //: True if `this' is already reached during the search path algorithm
  bool _reached;

  // class invariants
  // (_parent.size()==_motion.size())&&(_parent.size()+1==_beats.size())
};

#endif // #ifndef VCSL_SPATIAL_H
