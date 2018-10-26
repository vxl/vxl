#include "msm_aligner.h"
//:
// \file
// \author Tim Cootes
// \brief Base for functions which calculate and apply 2D transformations

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>

//: Compute mean of points after transforming with pose
void msm_aligner::mean_of_transformed(
                         const std::vector<msm_points>& points,
                         const std::vector<vnl_vector<double> >& pose,
                         msm_points& mean) const
{
  mean.vector().set_size(points[0].vector().size());
  mean.vector().fill(0.0);
  msm_points posed_points;
  for (unsigned i=0;i<points.size();++i)
  {
    apply_transform(points[i],pose[i],posed_points);
    mean.vector()+=posed_points.vector();
  }
  mean.vector()/=points.size();
}

//: Print class to os
void msm_aligner::print_summary(std::ostream& os) const
{
  os<<" { } ";
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void msm_aligner::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no);
}


//: Load class from binary file stream
void msm_aligner::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
}

//=======================================================================

void vsl_add_to_binary_loader(const msm_aligner& b)
{
  vsl_binary_loader<msm_aligner>::instance().add(b);
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_aligner& b)
{
  b.b_write(bfs);
}

//=======================================================================
//: Initialise from a text stream.
// The default implementation is for attribute-less normalisers,
// and throws if it finds any data in the stream.
void msm_aligner::config_from_stream(std::istream &is)
{
  std::string s = mbl_parse_block(is);
  if (s.empty() || s=="{}") return;

  mbl_exception_parse_error x(
    this->is_a() + " expects no properties in initialisation,\n"
    "But the following properties were given:\n" + s);
  mbl_exception_error(x);
}


//=======================================================================
//: Create a concrete msm_aligner-derived object, from a text specification.
std::unique_ptr<msm_aligner> msm_aligner::create_from_stream(std::istream &is)
{
  std::string name;
  is >> name;

  std::unique_ptr<msm_aligner> ps =
    mbl_cloneables_factory<msm_aligner>::get_clone(name);

  ps -> config_from_stream(is);
  return ps;
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_aligner& b)
{
  b.b_read(bfs);
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_aligner& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_aligner* b)
{
  if (b)
    return os << *b;
  else
    return os << "No msm_aligner defined.";
}

//=======================================================================
//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_aligner& b)
{
  os << b;
}

//=======================================================================
//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_aligner* b)
{
  if (b)
    os << *b;
  else
    os << vsl_indent() << "No msm_aligner defined.";
}
