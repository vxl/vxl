// This is bbas/bvgl/bvgl_eulerspiral_base.h
#ifndef bvgl_eulerspiral_base_h_
#define bvgl_eulerspiral_base_h_
//:
// \file
// \brief A base to derive Euler Spiral and Arc classes
// \author Ozge Can Ozcanli (ozge@lems.brown.edu)
// \date 02/09/05
//
// \verbatim
// Modifications
//
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <bvgl/bvgl_param_curve.h>

class bvgl_eulerspiral_base : public bvgl_param_curve
{
 public:

  //: default constructor
  bvgl_eulerspiral_base() : bvgl_param_curve() {}

  static const std::type_info& type_id()
  { return typeid(bvgl_eulerspiral_base); }

  bool is_type( const std::type_info& type ) const override
  { return (typeid(bvgl_eulerspiral_base) == type)!=0 ||
            this->bvgl_param_curve::is_type(type);
  }

  bvgl_param_curve *clone() const override = 0;
};

//: Write "<bvgl_param_curve of type euler spiral> to stream"
// \relates bvgl_param_curve
//std::ostream&  operator<<(std::ostream& s, bvgl_eulerspiral_base const& c);

//: Read parameters from stream
// \relates dbvgl_param_curve
//std::istream&  operator>>(std::istream& s, bvgl_eulerspiral_base const& c);

#endif // bvgl_eulerspiral_base_h_
