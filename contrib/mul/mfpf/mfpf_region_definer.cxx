#include <iostream>
#include <sstream>
#include "mfpf_region_definer.h"
//:
// \file
// \brief Base for objects which generate regions from sets of points
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_region_definer::mfpf_region_definer() = default;

//=======================================================================
// Destructor
//=======================================================================

mfpf_region_definer::~mfpf_region_definer() = default;

//: Initialise from a string stream
bool mfpf_region_definer::set_from_stream(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  if (props.size()!=0)
  {
    std::cerr<<is_a()<<" does not expect any extra arguments.\n";
    mbl_read_props_look_for_unused_props(
      "mfpf_region_definer::set_from_stream", props, mbl_read_props_type());
  }
  return true;
}

//: Create a concrete object, from a text specification.
std::unique_ptr<mfpf_region_definer> mfpf_region_definer::
  create_from_stream(std::istream &is)
{
  std::string name;
  is >> name;
  std::unique_ptr<mfpf_region_definer> vcb;
  try {
    vcb = mbl_cloneables_factory<mfpf_region_definer>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
    throw (mbl_exception_parse_error( e.what() ));
  }
  vcb->set_from_stream(is);
  return vcb;
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_region_definer::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_region_definer::is_a() const
{
  return std::string("mfpf_region_definer");
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_region_definer& b)
{
  vsl_binary_loader<mfpf_region_definer>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_region_definer& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_region_definer& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_region_definer& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_region_definer* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mfpf_region_definer defined.";
}

//: Generate a new set of points from pts0 using set of definers
void mfpf_points_from_definers(
              const std::vector<mfpf_region_definer*>& definer,
              const std::vector<vgl_point_2d<double> >& pts0,
              std::vector<vgl_point_2d<double> >& new_pts)
{
  unsigned n=definer.size();
  new_pts.resize(n);
  for (unsigned i=0;i<n;++i)
    new_pts[i]=definer[i]->get_ref_point(pts0);
}

//: Change indices in definers to refer to points generated
//  Suppose definer is used to generate a set of n=definer.size()
//  regions/pts (say pts1), by referring to some other set of m points.
//  This sets up self_definer to generate an identical set of
//  regions/pts by using the originally generated points (pts1).
//  This can only be done if there is a region centred on each
//  of the original points used in the definer.
//  The function tests for this case, and returns false if it fails.
//  In particular consider the following
//  \verbatim
//  std::vector<vgl_point_2d<double> > pts0,pts1,pts2;
//  // Set up pts0
//  ...
//  // Generate pts1 from pts0
//  mfpf_points_from_definers(definer,pts0,pts1);
//  mfpf_renumber_to_self(definer,pts0.size())
//  // Now generate pts2 from pts1
//  mfpf_points_from_definers(self_definer,pts1,pts2);
//  // pts2 should be the same as pts1
//  \endverbatim
//  Note that objects pointed to by definer are changed.
//  They may be left in an invalid state if this returns false,
//  so caller should ensure a backup retained.
bool mfpf_renumber_to_self(
                  std::vector<mfpf_region_definer*>& definer,
                  unsigned n_pts0)
{
  std::vector<unsigned> new_index(n_pts0,mfpf_invalid_index);
  for (unsigned i=0;i<definer.size();++i)
  {
    if (definer[i]->is_centred_on_pt())
    {
      unsigned ri = definer[i]->ref_point_index();
      if (ri>=n_pts0)
      {
        std::cerr<<"Index out of range:"<<ri<<std::endl;
        return false;
      }
      new_index[ri]=i;
    }
  }

  for (auto & i : definer)
  {
    if (!i->replace_index(new_index))
    {
      std::cerr<<"Failed to update indices in "<<i<<std::endl;
      return false;
    }
  }

  return true;
}
