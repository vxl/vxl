// This is vxl/vcsl/vcsl_spatial.h
#ifndef vcsl_spatial_h
#define vcsl_spatial_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author François BERTEL
//
// \verbatim
// Modifications
// 2000/07/10 François BERTEL Creation
// 2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
// 2002/01/22 Peter Vanroose - return type of from_local_to_cs() changed from ptr to non-ptr
// 2002/01/28 Peter Vanroose - vcl_vector members changed from ptr to non-ptr
// \endverbatim

#include <vcsl/vcsl_spatial_sptr.h>

#include <vcsl/vcsl_coordinate_system.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>
#include <vcsl/vcsl_spatial_transformation_sptr.h>
#include <vcsl/vcsl_graph_sptr.h>

// Because VXL does not use dynamic_cast<> :-(

class vcsl_cartesian_2d;
class vcsl_polar;

class vcsl_cartesian_3d;
class vcsl_cylindrical;
class vcsl_spherical;

//: A spatial coordinate system
// class invariants:
// (parent_.size()==motion_.size())&&(parent_.size()+1==beat_.size())

class vcsl_spatial
  :public vcsl_coordinate_system
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_spatial(void) {}

  //: Destructor
  virtual ~vcsl_spatial();

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_spatial *cast_to_spatial(void) const { return this; }
  virtual const vcsl_cartesian_2d *cast_to_cartesian_2d(void) const {return 0; }
  virtual const vcsl_polar *cast_to_polar(void) const {return 0; }
  virtual const vcsl_cartesian_3d *cast_to_cartesian_3d(void) const {return 0; }
  virtual const vcsl_cylindrical *cast_to_cylindrical(void) const {return 0; }
  virtual const vcsl_spherical *cast_to_spherical(void) const {return 0; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the list of time clocks
  virtual vcl_vector<double> beat(void) const { return beat_; }

  //: Return the list of parent coordinate system along the time
  virtual vcl_vector<vcsl_spatial_sptr> parent(void) const { return parent_; }

  //: Return the list of transformations along the time
  virtual vcl_vector<vcsl_spatial_transformation_sptr> motion(void) const
  { return motion_; }

  //: Is `time' between the two time bounds ?
  virtual bool valid_time(double time) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the list of time clocks
  virtual void set_beat(vcl_vector<double> const& new_beat);

  //: Set the list of parent coordinate system along the time
  virtual void set_parent(vcl_vector<vcsl_spatial_sptr> const& new_parent);

  //: Set the list of transformations along the time
  virtual void set_motion(vcl_vector<vcsl_spatial_transformation_sptr> const& new_motion);

  //
  // Can be called to initialize a CS for the first time.  Dynamically
  // allocates space for parent_ and motion_.  Makes new_parent and new_motion
  // the first element of each, respectively.
  //
  // Does not set beat_.  Effectively, new_parent and new_motion are valid for
  // all times.  However, a subsequent call to set_beat() could add a list of
  // clocks with a time corresponding to (*parent_)[0] and (*motion_)[0].
  //
  // \param new_parent CS that 'this' is derived from.
  // \param new_motion The transformation from new_parent to 'this'.
  virtual void set_unique(const vcsl_spatial_sptr &new_parent,
                          const vcsl_spatial_transformation_sptr &new_motion);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Return the index of the beat inferior or equal to `time'
  //  REQUIRE: parent().size()!=0
  //  REQUIRE: valid_time(time)
  virtual int matching_interval(double time) const;

  //: Does a path from `this' to `other' exist ?
  virtual bool path_from_local_to_cs_exists(const vcsl_spatial_sptr &other,
                                            double time);

  //: Is `this' an absolute spatial coordinate system at time `time'?
  //  REQUIRE: valid_time(time)
  virtual bool is_absolute(double time) const;


  //: Return v but exprimed in the spatial coordinate system `other'
  //  REQUIRE: path_from_local_to_cs_exists(other,time)
  virtual vnl_vector<double> from_local_to_cs(const vnl_vector<double> &v,
                                              const vcsl_spatial_sptr &other,
                                              double time);

  virtual void set_graph(const vcsl_graph_sptr &new_graph);

  //: WARNING: Only used by vcsl_spatial and vcsl_graph
  //  DO NOT USE IT DIRECTLY
  virtual void set_reached(const bool &new_reached);

  virtual bool reached(void) const;

  //***************************************************************************
  // Implementation
  //***************************************************************************
 protected:
  //: Does a path from `this' to `other' exist ?
  // Called only by path_to_cs_exists()
  virtual bool
  recursive_path_from_local_to_cs_exists(const vcsl_spatial_sptr &other,
                                         double time);
  //: Find the sequence of transformations from `this' to `other'
  //  REQUIRE: path.size()==0 and sens.size()==0
  //  REQUIRE: path_from_local_to_cs_exists()
  virtual void
  path_from_local_to_cs(const vcsl_spatial_sptr &other,
                        double time,
                        vcl_vector<vcsl_spatial_transformation_sptr> &path,
                        vcl_vector<bool> &sens);

  //: Find the sequence of transformations from `this' to `other'
  //  Called only by path_from_local_to_cs()
  virtual bool
  recursive_path_from_local_to_cs(const vcsl_spatial_sptr &other,
                                  double time,
                                  vcl_vector<vcsl_spatial_transformation_sptr> &path,
                                  vcl_vector<bool> &sens);

  //: successive parents of `this' along the time
  vcl_vector<vcsl_spatial_sptr> parent_;

  //: Clock times
  vcl_vector<double> beat_;

  //: successive transformations from `this' to `parent' along the time
  vcl_vector<vcsl_spatial_transformation_sptr> motion_;

  //: List of spatial coordinate system that can be child of `this' at a time
  vcl_vector<vcsl_spatial_sptr> potential_children_;

  //: List of all the spatial coordinate system of the graph
  vcsl_graph_sptr graph_;

  //: True if `this' is already reached during the search path algorithm
  bool reached_;
};

#endif // vcsl_spatial_h
