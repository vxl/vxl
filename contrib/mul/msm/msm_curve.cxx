#include "msm_curve.h"
//:
// \file
// \brief List of points making a curve - for defining boundaries
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_int_list.h>
#include <mbl/mbl_parse_keyword_list.h>
#include <vul/vul_string.h>
#include <vcl_iterator.h> // For vcl_back_inserter
#include <vcl_cassert.h>

// Default Constructor
msm_curve::msm_curve()
{
}

//: Define as range of indices [lo,hi]
msm_curve::msm_curve(unsigned lo, unsigned hi,
                     bool open, vcl_string name)
{
  set(lo,hi,open,name);
}

void msm_curve::set(const vcl_vector<unsigned>& index,
                    bool open, vcl_string name)
{
  index_ = index;
  open_=open;
  name_=name;
}

//: Define as range of indices [lo,hi]
void msm_curve::set(unsigned lo, unsigned hi,
                    bool open, vcl_string name)
{
  assert(hi>lo);
  index_.resize(1+hi-lo);
  for (unsigned i=lo;i<=hi;++i) index_[i-lo]=i;
  open_=open;
  name_=name;
}

//: Return the largest index value
unsigned msm_curve::max_index() const
{
  if (index_.size()==0) return 0u;
  unsigned m=index_[0];
  for (unsigned i=1;i<index_.size();++i)
    if (index_[i]>m) m=index_[i];
  return m;
}


//: Adds offset to index of every point
//  Useful when concatenating models
void msm_curve::add_index_offset(int offset)
{
  for (unsigned i=0;i<index_.size();++i)
    index_[i]=unsigned (index_[i]+offset);
}

//: Equality test
bool msm_curve::operator==(const msm_curve& c) const
{
  return (name_==c.name_) &&
         (open_==c.open_) &&
         (index_==c.index_);
}


//: Parse parameters in stream
//  Expects
// \verbatim
// { name: Chin open: true indices: { 0 1 2 3 4 5 6 } }
// \endverbatim
void msm_curve::config_from_stream(vcl_istream& is)
{
  // Cycle through stream and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  name_= props.get_optional_property("name","");
  open_=vul_string_to_bool(props.get_optional_property("open","true"));

  index_.empty();
  vcl_istringstream ss1(props.get_required_property("indices"));
  mbl_parse_int_list(ss1, vcl_back_inserter(index_), unsigned());

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "msm_curve::config_from_stream", props, mbl_read_props_type());
}

//=======================================================================
// Method: print
//=======================================================================

void msm_curve::print_summary(vcl_ostream& os) const
{
  os<<" { name: "<<name_<<" open: ";
  if (open_) os<<"true"; else os<<"false";
  os<<" indices: { ";
  for (unsigned i=0;i<index_.size();++i) os<<index_[i]<<' ';
  os<<"} } ";
}

//=======================================================================
// Method: save
//=======================================================================
void msm_curve::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,short(1)); // Version
  vsl_b_write(bfs,name_);
  vsl_b_write(bfs,open_);
  vsl_b_write(bfs,index_);
}

//=======================================================================
// Method: load
//=======================================================================
void msm_curve::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,name_);
      vsl_b_read(bfs,open_);
      vsl_b_read(bfs,index_);
      break;
    default:
      vcl_cerr << "msm_curve::b_read() :\n"
               << "Unexpected version number " << version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_curve& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_curve& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const msm_curve& b)
{
  b.print_summary(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const msm_curve& b)
{
 os << b;
}


//: Default constructor
msm_curves::msm_curves()
{
}


//: Construct as a single curve
msm_curves::msm_curves(unsigned lo, unsigned hi,
                       bool open, vcl_string name)
{
  resize(1);
  operator[](0)=msm_curve(lo,hi,open,name);
}

//: Return index of first curve with given name, or -1
int msm_curves::which_curve(const vcl_string& name) const
{
  for (unsigned i=0;i<size();++i)
    if (operator[](i).name()==name) return i;
  return -1;
}

//: Return the largest index value in any curve
unsigned msm_curves::max_index() const
{
  if (size()==0) return 0;
  unsigned m = operator[](0).max_index();
  for (unsigned j=1;j<size();++j)
    if (operator[](j).max_index()>m) m=operator[](j).max_index();
  return m;
}

//: Parse parameters in stream
//  Expects
// \verbatim
// {
//   curve: { name: Chin open: true indices: { 0 1 2 3 4 5 6 } }
//   curve: { name: Nose open: false indices: { 11 : 15 } }
// }
// \endverbatim
void msm_curves::config_from_stream(vcl_istream& is)
{
  vcl_vector<vcl_string> curve_params;
  mbl_parse_keyword_list(is,"curve:",curve_params);
  resize(curve_params.size());
  for (unsigned i=0;i<size();++i)
  {
    vcl_istringstream ss(curve_params[i]);
    operator[](i).config_from_stream(ss);
  }
}

//: Save to text file
bool msm_curves::write_text_file(const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str());
  if (!ofs) return false;
  ofs<<(*this);
  return true;
}

//: Read from text file
bool msm_curves::read_text_file(const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs) return false;
  vcl_string label;
  ifs>>label;
  if (label!="curves:")
  {
    throw mbl_exception_parse_error("msm_curves::read_text_file: Expected 'curves', got: "+label);
    return false;
  }
  config_from_stream(ifs);
  return true;
}

//: Stream output operator
vcl_ostream& operator<<(vcl_ostream& os,const msm_curves& c)
{
  os<<"curves: {\n";
  for (unsigned i=0;i<c.size();++i)
  {
    os<<"  curve: "<<c[i]<<vcl_endl;
  }
  os<<'}'<<vcl_endl;
  return os;
}

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_curves& c)
{
  vsl_b_write(bfs,short(1)); // Version number
  vsl_b_write(bfs,unsigned(c.size()));
  for (unsigned i=0;i<c.size();++i)
    vsl_b_write(bfs,c[i]);
}

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_curves& c)
{
  short version;
  unsigned n;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      c.resize(n);
      for (unsigned i=0;i<c.size();++i)
        vsl_b_read(bfs,c[i]);
      break;
    default:
      vcl_cerr << "vsl_b_read(bfs,msm_curves) :\n"
               << "Unexpected version number " << version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


