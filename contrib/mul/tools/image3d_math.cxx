//:
// \file
// \author Ian Scott
// \date 12 Aug 2008
// \brief General Purpose Reverse Polish Notation image maths program.
//
// To add your own operations, add a function (e.g. plus__image_3d_of_float__image_3d_of_float),
// and add an entry in operations::operations() to register your new operation.
// That list of entries must be kept sorted.


#include <vcl_iostream.h>

#if VCL_HAS_EXCEPTIONS
#include <vcl_exception.h>
#include <vcl_stdexcept.h>
#include <vcl_sstream.h>
#include <vcl_cstdio.h>
#include <vcl_deque.h>
#include <vcl_map.h>
#include <vcl_iomanip.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vsl/vsl_deque_io.txx>
#include <vsl/vsl_map_io.txx>
#include <vsl/vsl_stream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_string.h>
#include <mbl/mbl_log.h>
#include <mbl/mbl_exception.h>
#include <vil3d/file_formats/vil3d_gen_synthetic.h>
#include <vil3d/algo/vil3d_distance_transform.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_clamp.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/algo/vil3d_locally_z_normalise.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_save.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_resample_trilinear.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vul/vul_reg_exp.h>

//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.image_3d_math");
  return l;
}

//-------------------------------------------------------------------------------------
// Stack and Storage Infrastructure

//: This is the return value of the program.
// It can be modified using the (as yet unimplemented) "-return" operator.
int global_retval=0;

//: load images by default as given type.
enum global_option_load_as_image_t
{
  image3d_float_t,
  image3d_double_t,
  image3d_int_t
} global_option_load_as_image = image3d_float_t;

//: List of command arguments parsed so far
// Kept for error reporting purposes.
vcl_string args_so_far;

class operand;
vcl_ostream& operator <<( vcl_ostream&, const operand&);

bool string_to_double(const vcl_string&s, double&d)
{
  vcl_istringstream ss(s);
  ss >> d;
  if (!ss) return false;
  char dummy;
  ss >> dummy;
  if (ss) return false;
  return true;
}

//: Like string_to_image, but suppress any error messages, and report success or failure.
template <class T>
bool try_string_to_image(const vcl_string&s, vimt3d_image_3d_of<T>&d)
{
  try
  {
    vimt3d_load(s, d, true);
    if (!d.image()) return false; // LEGACY_ERROR_REPORTING
  }
  catch (const vcl_exception&)
  {
    return false;
  }
  return true;
}

//: Load filename /p s as an image.
template <class T>
void string_to_image(const vcl_string&s, vimt3d_image_3d_of<T>&d)
{
  try
  {
    vimt3d_load(s, d, true);
    if (!d.image())
    {
      vcl_cerr << "\nERROR: Failed to load image: " << s << " for unknown reason.\n"
               << "At \"" << args_so_far << "\"<-- HERE\n";
      vcl_exit(4);
    }
  }
  catch (const vcl_exception& e)
  {
    vcl_cerr << "\nERROR: " << e.what() << '\n'
             << "At \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(4);
  }
}


//: A value on the stack, to be used as by an operation.
// This is a rather inefficient VARIANT-type class.
class operand
{
  vcl_string string_;
  vimt3d_image_3d_of<double> image_3d_of_double_;
  vimt3d_image_3d_of<float> image_3d_of_float_;
  vimt3d_image_3d_of<int> image_3d_of_int_;
  double double_;


 public:
  enum operand_type_t {
    e_string,
    e_image_3d_of_double,
    e_image_3d_of_float,
    e_image_3d_of_int,
    e_double,
    e_open_brace,
    e_never };


 protected:
  operand_type_t operand_type_;


 public:
  operand(): operand_type_(e_never) {} // if used with this value - should throw.

  explicit operand(const vcl_string& s):
    string_(s), operand_type_(e_string) {}
  explicit operand(const vimt3d_image_3d_of<double>& i):
    image_3d_of_double_(i), operand_type_(e_image_3d_of_double) {}
  explicit operand(const vimt3d_image_3d_of<float>& i):
    image_3d_of_float_(i), operand_type_(e_image_3d_of_float) {}
  explicit operand(const vimt3d_image_3d_of<int>& i):
    image_3d_of_int_(i), operand_type_(e_image_3d_of_int) {}
  explicit operand(const double& i):
    double_(i), operand_type_(e_double) {}

  operand_type_t operand_type() const { return operand_type_; }

  bool is_string() const { return operand_type_==e_string; }
  const vcl_string& as_string() const { assert(is_string()); return string_; }

  bool is_double() const
  {
    double dummy;
    return (operand_type_==e_string && string_to_double(string_, dummy))
      || operand_type_==e_double;
  }
  double as_double() const
  {
    assert(is_double());
    if (operand_type_==e_double) return double_;
    else if (operand_type_==e_string)
    {
      double v;
      string_to_double(string_, v);
      return v;
    }
    vcl_cerr << "\nERROR: Tried to use unsuitable operand as a double: " << *this
             << "\nAt \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(1);
  }

  bool is_image_3d_of_float() const
  {
    vimt3d_image_3d_of<float> dummy;
    return (operand_type_==e_string && global_option_load_as_image==image3d_float_t && try_string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_float;
  }
  const vimt3d_image_3d_of<float> as_image_3d_of_float() const
  {
    if (operand_type_==e_image_3d_of_float) return image_3d_of_float_;
    else if (operand_type_==e_string && global_option_load_as_image==image3d_float_t)
    {
      vimt3d_image_3d_of<float> v;
      string_to_image(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a vimt_image_3d_of<float>: " << *this;
    mbl_exception_abort(ss.str());
    return vimt3d_image_3d_of<float>();
  }

  bool is_image_3d_of_double() const
  {
    vimt3d_image_3d_of<double> dummy;
    return (operand_type_==e_string && global_option_load_as_image==image3d_double_t && try_string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_double;
  }
  const vimt3d_image_3d_of<double> as_image_3d_of_double() const
  {
    if (operand_type_==e_image_3d_of_double) return image_3d_of_double_;
    else if (operand_type_==e_string && global_option_load_as_image==image3d_double_t)
    {
      vimt3d_image_3d_of<double> v;
      string_to_image(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a vimt_image_3d_of<double>: " << *this;
    mbl_exception_abort(ss.str());
    return vimt3d_image_3d_of<double>();
  }

  bool is_image_3d_of_int() const
  {
    vimt3d_image_3d_of<int> dummy;
    return (operand_type_==e_string && global_option_load_as_image==image3d_int_t && try_string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_int;
  }
  const vimt3d_image_3d_of<int> as_image_3d_of_int() const
  {
    if (operand_type_==e_image_3d_of_int) return image_3d_of_int_;
    else if (operand_type_==e_string && global_option_load_as_image==image3d_int_t)
    {
      vimt3d_image_3d_of<int> v;
      string_to_image(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a vimt_image_3d_of<int>: " << *this;
    mbl_exception_abort(ss.str());
    return vimt3d_image_3d_of<int>();
  }
  bool is_open_brace() const
  {
    return operand_type_==e_open_brace;
  }

  void print_summary(vcl_ostream &os) const
  {
    // Forward declaration.
    vcl_ostream& operator <<( vcl_ostream&os, const operand::operand_type_t& t);

    os << operand_type_ << ": ";
    switch (operand_type_)
    {
     case e_string:
      os << string_;
      break;
     case e_image_3d_of_double:
      os << vsl_stream_summary(image_3d_of_double_);
      break;
     case e_image_3d_of_float:
      os << vsl_stream_summary(image_3d_of_float_);
      break;
     case e_image_3d_of_int:
      os << vsl_stream_summary(image_3d_of_int_);
      break;
     case e_double:
      os << double_;
      break;
     case e_open_brace:
      break;
     default: {
      os << "Unknown operand_type: " << operand_type_;
      vcl_ostringstream ss;
      ss << "Unknown operand_type: " << operand_type_;
      throw mbl_exception_abort(ss.str()); }
    }
  }

  bool is_a(operand_type_t t) const
  {
    switch (t)
    {
     case e_string:
      return is_string();
     case e_image_3d_of_double:
      return is_image_3d_of_double();
     case e_image_3d_of_float:
      return is_image_3d_of_float();
     case e_image_3d_of_int:
      return is_image_3d_of_int();
     case e_double:
      return is_double();
     case e_open_brace:
      return is_open_brace();
     default: {
      vcl_ostringstream ss;
      ss << "Unknown operand_type: " << operand_type_;
      throw mbl_exception_abort(ss.str()); }
    }
    return false;
  }

  operand deep_copy() const
  {
    switch (operand_type_)
    {
     // Simple cases with types that deep copy by default.
     case e_string:
     case e_double:
       return *this;
     // Explicit deep copy.
     case e_image_3d_of_double:
     {
       vimt3d_image_3d_of<double> im;
       im.deep_copy(this->image_3d_of_double_);
       return operand(im);
     }
     case e_image_3d_of_float:
     {
       vimt3d_image_3d_of<float> im;
       im.deep_copy(this->image_3d_of_float_);
       return operand(im);
     }
     case e_image_3d_of_int:
     {
       vimt3d_image_3d_of<int> im;
       im.deep_copy(this->image_3d_of_int_);
       return operand(im);
     }
     default:
     {
       vcl_ostringstream ss;
       ss << "Unknown operand_type: " << operand_type_;
       throw mbl_exception_abort(ss.str()); }
    }
    return operand();
  }
};


class operand_open_brace: public operand
{
 public:
  operand_open_brace() { operand_type_ = e_open_brace; }
};

void vsl_print_summary( vcl_ostream&os, const operand& p) { p.print_summary(os); }
vcl_ostream& operator <<( vcl_ostream&os, const operand& p) { p.print_summary(os); return os; }

void vsl_print_summary(vcl_ostream& os, const vcl_deque<operand> &v)
{
  os << "Operand Stack Length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os,v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ...\n";
}

vcl_ostream& operator <<( vcl_ostream&os, const operand::operand_type_t& t)
{
  switch (t)
  {
    case operand::e_string:
      os << "string"; break;
    case operand::e_image_3d_of_double:
      os << "image_3d_of_double"; break;
    case operand::e_image_3d_of_float:
      os << "image_3d_of_float"; break;
    case operand::e_image_3d_of_int:
      os << "image_3d_of_int"; break;
    case operand::e_double:
      os << "double"; break;
    case operand::e_open_brace:
      os << "brace {"; break;
    default: {
      os << "Unknown operand_type: " << static_cast<int>(t);
      vcl_ostringstream ss;
      ss << "Unknown operand_type: " << static_cast<int>(t);
      throw mbl_exception_abort(ss.str()); }
  }
  return os;
}

class opstack_t : public vcl_deque<operand>
{
 public:
  opstack_t(): last_pop_(vcl_numeric_limits<vcl_size_t>::max()) {}

  //: Get rid of n operands on the stack.
  // The stack size after this is stored in last_pop,
  // allowing calculation of the last number of results pushed
  // onto stack.
  void pop(unsigned n)
  {
    for (; n!=0; --n)
      pop_front();
    last_pop_=size();
  }

  //: Return size of stack after last call to pop;
  unsigned last_pop() const { return last_pop_; }

  void reset_last_pop()
  {
    last_pop_=vcl_numeric_limits<vcl_size_t>::max();
  }
 private:
  vcl_size_t last_pop_;

  // Force the use of pop(unsigned) rather than pop_front();
  void pop_front() { vcl_deque<operand>::pop_front(); }
};

typedef vcl_map<vcl_string, operand> named_store_t;

static named_store_t named_store;

void print_operations(vcl_ostream&);

//-------------------------------------------------------------------------------------
// Operation implementation

void help(opstack_t& )
{
  vcl_cout <<
    "usage: image3d_math [--operation|operand] [--operation|operand] ... --operation\n"
    "Manipulate images using Reverse polish notiation.\n"
    "List of commands:\n";
  print_operations(vcl_cout);
  vcl_cout << "{ and }:  Braces - for debugging, have no side effects.\n"
    "usage: eg. { { v1 v2 --op1 } v3 --op2 }\n"
    "       The braces can be used to delineate all the operands used by the\n"
    "       operation preceeding the closing brace.\n"
    "\n"
    "+name: identical to \"name --recall\"\n";
  vcl_exit(3);
}


void close_brace__brace(opstack_t& s)
{
  static vul_reg_exp del_stack_brace_re("--del[-_]stack } $");
  static vul_reg_exp brace_brace_re("} } $");

  if (s.last_pop() > s.size())
  {
    vcl_cerr << "\nERROR: A closing brace must be preceeded by an operation.\n"
             << "At \"" << args_so_far << "\"<-- HERE";
    vcl_exit(1);
  }
  if (s.size() < 1 || s.last_pop() > s.size() ||
    s.last_pop()==0 || !s[s.size() - s.last_pop()].is_open_brace() )
  {
    vcl_cerr << "\nERROR: No matching opening brace found for closing brace.\n";
    if (del_stack_brace_re.find(args_so_far))
      vcl_cerr << "      The interaction of --del-stack and braces can be misleading. Try\n"
                  "      removing the closing brace following --del-stack, and its opening pair.\n";
    else if (brace_brace_re.find(args_so_far))
      vcl_cerr << "      You cannot have two closing braces next to each other, since closing\n"
                  "      braces check the behaviour of the immediately preceeding operation.\n";
    vcl_cerr << "At \"" << args_so_far << "\"<-- HERE\n"
      "Stack is: \n" << vsl_stream_summary(s);
    vcl_exit(1);
  }
  s.erase(s.end() - s.last_pop());
}


//: Add all voxel values together.
void voxel_sum__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());

  double result = 0;
  vil3d_math_sum(result, o1.image(), 0);

  s.pop(1);
  s.push_front(operand(result));
}

//: Add all voxel values together.
void voxel_sum__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());

  unsigned long result = 0;
  vil3d_math_sum(result, o1.image(), 0);

  s.pop(1);
  s.push_front(operand(result));
}

//: Add all voxel values together.
void voxel_product__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());
  const vil3d_image_view<int>& o1_image = o1.image();

  double result = 0.0;
  for (unsigned k=0,nk=o1_image.nk(); k<nk; ++k)
    for (unsigned j=0,nj=o1_image.nj(); j<nj; ++j)
      for (unsigned i=0,ni=o1_image.ni(); i<ni; ++i)
        result *= o1_image(i,j,k);

  s.pop(1);
  s.push_front(operand(result));
}

//: Add all voxel values together.
void voxel_product__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());
  const vil3d_image_view<int>& o1_image = o1.image();

  double result = 0;
  for (unsigned k=0,nk=o1_image.nk(); k<nk; ++k)
    for (unsigned j=0,nj=o1_image.nj(); j<nj; ++j)
      for (unsigned i=0,ni=o1_image.ni(); i<ni; ++i)
        result *= o1_image(i,j,k);

  s.pop(1);
  s.push_front(operand(static_cast<double>(result)));
}


//: Calculate the x, y and z sizes of the central voxel.
void voxel_size__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());

  vgl_vector_3d<double> voxel_size=o1.world2im().inverse().delta(
    vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0) );

  s.pop(1);
  s.push_front(operand(voxel_size.x()));
  s.push_front(operand(voxel_size.y()));
  s.push_front(operand(voxel_size.z()));
}


//: Calculate the x, y and z sizes of the central voxel.
void voxel_size__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());

  vgl_vector_3d<double> voxel_size=o1.world2im().inverse().delta(
    vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0) );

  s.pop(1);
  s.push_front(operand(voxel_size.x()));
  s.push_front(operand(voxel_size.y()));
  s.push_front(operand(voxel_size.z()));
}

//: Add voxels of two images together
void sum__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());

  vimt3d_image_3d_of<float> result;
  vil3d_math_image_sum(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(2);
  s.push_front(operand(result));
}

//: Add voxels of two images together
void sum__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());

  vimt3d_image_3d_of<int> result;
  vil3d_math_image_sum(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(2);
  s.push_front(operand(result));
}

//: And voxels of two images together
void and__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  const vil3d_image_view<int>& o1_image = o1.image();
  const vil3d_image_view<int>& o2_image = o2.image();

  vimt3d_image_3d_of<int> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<int> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) =
          (o1_image(i,j,k) && o2_image(i,j,k)) ? 1 : 0;

  s.pop(2);
  s.push_front(operand(result));
}


//: Or voxels of two images together
void or__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  const vil3d_image_view<int>& o1_image = o1.image();
  const vil3d_image_view<int>& o2_image = o2.image();

  vimt3d_image_3d_of<int> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<int> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) =
          (o1_image(i,j,k) || o2_image(i,j,k)) ? 1 : 0;

  s.pop(2);
  s.push_front(operand(result));
}


//: Find the minimum of corresponding voxel pairs between two images
void min__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  const vil3d_image_view<int>& o1_image = o1.image();
  const vil3d_image_view<int>& o2_image = o2.image();

  vimt3d_image_3d_of<int> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<int> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) = vcl_min(o1_image(i,j,k), o2_image(i,j,k));

  s.pop(2);
  s.push_front(operand(result));
}


//: Find the minimum of corresponding voxel pairs between two images
void min__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  const vil3d_image_view<float>& o1_image = o1.image();
  const vil3d_image_view<float>& o2_image = o2.image();

  vimt3d_image_3d_of<float> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<float> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) = vcl_min(o1_image(i,j,k), o2_image(i,j,k));

  s.pop(2);
  s.push_front(operand(result));
}

//: Find the maximum of corresponding voxel pairs between two images
void max__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  const vil3d_image_view<int>& o1_image = o1.image();
  const vil3d_image_view<int>& o2_image = o2.image();

  vimt3d_image_3d_of<int> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<int> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) = vcl_max(o1_image(i,j,k), o2_image(i,j,k));

  s.pop(2);
  s.push_front(operand(result));
}


//: Find the maximum of corresponding voxel pairs between two images
void max__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  const vil3d_image_view<float>& o1_image = o1.image();
  const vil3d_image_view<float>& o2_image = o2.image();

  vimt3d_image_3d_of<float> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<float> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) = vcl_max(o1_image(i,j,k), o2_image(i,j,k));

  s.pop(2);
  s.push_front(operand(result));
}

//: Find the difference between voxels of two images
void diff__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());

  vimt3d_image_3d_of<float> result;
  vil3d_math_image_difference(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(2);
  s.push_front(operand(result));
}

//: Find the difference between voxels of two images
void diff__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());

  vimt3d_image_3d_of<int> result;
  vil3d_math_image_difference(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(2);
  s.push_front(operand(result));
}

//: Resample one image so that it looks like another.
void resample__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());

  vimt3d_image_3d_of<float> result;

  vimt3d_transform_3d i2w = o2.world2im().inverse();
  const vgl_point_3d<double> zero(0,0,0);
  typedef vgl_vector_3d<double> gvec3;
  vimt3d_resample_trilinear(o1, result,
                            i2w.origin(),
                            i2w.delta(zero, gvec3(1, 0, 0)),
                            i2w.delta(zero, gvec3(0, 1, 0)),
                            i2w.delta(zero, gvec3(0, 0, 1)),
                            o2.image().ni(), o2.image().nj(), o2.image().nk() );

  result.world2im() = o2.world2im();

  s.pop(2);
  s.push_front(operand(result));
}


void store__poly__string(opstack_t& s)
{
  assert(s.size() >= 2);

  named_store[s[0].as_string()] = s[1];

  s.pop(2);
}


void recall__string(opstack_t& s)
{
  assert(s.size() >= 1);

  const vcl_string& o1 = s[0].as_string();
  named_store_t::const_iterator it = named_store.find(o1);
  if (it == named_store.end())
  {
    vcl_cerr << "\nERROR: --recall could not find a store called " << o1
             << "\n Store is :\n" << vsl_stream_summary(named_store);
    vcl_exit(5);
  }
  s.pop(1);
  s.push_front(it->second.deep_copy());
}


//: Save a float image using vil3d saver
void save__image_3d_of_float__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_save(s[0].as_string(), s[1].as_image_3d_of_float(), true);

  s.pop(2);
}

//: Save an int image using vil3d saver
void save__image_3d_of_int__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_save(s[0].as_string(), s[1].as_image_3d_of_int(), true);

  s.pop(2);
}


//: Load an image from an ASCII matlab format - according to
void load_from_mat__string(opstack_t& s)
{
  assert(s.size() >= 1);

  vcl_string o1(s[0].as_string());

  vcl_ifstream input(o1.c_str());

  if (!input)
  {
    vcl_cerr << "\nERROR: --load_from_mat: ";
    vcl_perror(o1.c_str());
    vcl_cerr << "At \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(4);
  }

  char c;
  vcl_string dummy;
  while (true)
  {
    input >> vcl_ws >> c;
    if (c=='#')
      vcl_getline(input, dummy);
    else
      break;
  }
  input.putback(c);
  unsigned ni, nj, nk;
  input >> ni >> nj >> nk;
  if (!input)
  {
    vcl_cerr << "\nERROR: Unable to parse " << o1
             << "\nAt \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(1);
  }

  s.pop(1);

  switch (global_option_load_as_image)
  {
    case image3d_int_t:
    {
      vimt3d_image_3d_of<int> result(ni, nj, nk);
      for (unsigned k=0;k<nk;++k)
      {
        for (unsigned j=0;j<nj;++j)
          for (unsigned i=0;i<ni;++i)
            input >> result.image()(i,j,k);
        if (!input)
        {
          vcl_cerr << "\nERROR: Unable to parse " << o1
                   << "\nAt \"" << args_so_far << "\"<-- HERE\n";
          vcl_exit(1);
        }
      }
      s.push_front(operand(result));
      break;
    }
    case image3d_double_t:
    {
      vimt3d_image_3d_of<double> result(ni, nj, nk);
      for (unsigned k=0;k<nk;++k)
      {
        for (unsigned j=0;j<nj;++j)
          for (unsigned i=0;i<ni;++i)
            input >> result.image()(i,j,k);
        if (!input)
        {
          vcl_cerr << "\nERROR: Unable to parse " << o1
                   << "\nAt \"" << args_so_far << "\"<-- HERE\n";
          vcl_exit(1);
        }
      }
      s.push_front(operand(result));
      break;
    }
    case image3d_float_t:
    {
      vimt3d_image_3d_of<float> result(ni, nj, nk);
      for (unsigned k=0;k<nk;++k)
      {
        for (unsigned j=0;j<nj;++j)
          for (unsigned i=0;i<ni;++i)
            input >> result.image()(i,j,k);
        if (!input)
        {
          vcl_cerr << "\nERROR: Unable to parse " << o1
                   << "\nAt \"" << args_so_far << "\"<-- HERE\n";
          vcl_exit(1);
        }
      }
      s.push_front(operand(result));
      break;
    }
    default: // do nothing
      break;
  }
}


//: Save an int image to an ASCII matlab format
void save_to_mat__image_3d_of_int__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vcl_string o1(s[0].as_string());
  vimt3d_image_3d_of<int> o2(s[1].as_image_3d_of_int());
  const vil3d_image_view<int>& o2_image = o2.image();;

  vcl_ofstream output_file;
  vcl_ostream *output;

  if (o1 == "-")
    output = &vcl_cout;
  else
  {
    output_file.open(o1.c_str());
    output = &output_file;
  }

  if (!*output)
  {
    vcl_cerr << "\nERROR: --save_to_mat: ";
    vcl_perror(o1.c_str());
    vcl_cerr << "\nAt \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(4);
  }

  // copy precision length from console to output file
  output->precision(vcl_cout.precision());

  (*output) <<
    "# Created by vxl/image3d_math\n"
    "# name: image3d\n"
    "# type: matrix\n"
    "# ndims: 3\n"
    << o2.image().ni() << ' ' << o2.image().nj() << ' ' << o2.image().nk() << '\n';

  for (unsigned k=0;k<o2_image.nk();++k)
    for (unsigned j=0;j<o2_image.nj();++j)
      for (unsigned i=0;i<o2_image.ni();++i)
        (*output) <<  o2_image(i,j,k) << '\n';

  s.pop(2);
}

//: Save a float image to an ASCII matlab format
void save_to_mat__image_3d_of_float__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vcl_string o1(s[0].as_string());
  vimt3d_image_3d_of<float> o2(s[1].as_image_3d_of_float());
  const vil3d_image_view<float>& o2_image = o2.image();;


  vcl_ofstream output_file;
  vcl_ostream *output;

  if (o1 == "-")
    output = &vcl_cout;
  else
  {
    output_file.open(o1.c_str());
    output = &output_file;
  }

  if (!*output)
  {
    vcl_cerr << "\nERROR: --save_to_mat: ";
    vcl_perror(o1.c_str());
    vcl_cerr << "\nAt \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(4);
  }

  // copy precision length from console to output file
  output->precision(vcl_cout.precision());

  (*output) <<
    "# Created by vxl/image3d_math\n"
    "# name: image3d\n"
    "# type: matrix\n"
    "# ndims: 3\n"
    << o2.image().ni() << ' ' << o2.image().nj() << ' ' << o2.image().nk() << '\n';

  for (unsigned k=0;k<o2_image.nk();++k)
    for (unsigned j=0;j<o2_image.nj();++j)
      for (unsigned i=0;i<o2_image.ni();++i)
        (*output) <<  o2_image(i,j,k) << '\n';

  s.pop(2);
}


//: Save a double image to an ASCII matlab format
void save_to_mat__image_3d_of_double__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vcl_string o1(s[0].as_string());
  vimt3d_image_3d_of<double> o2(s[1].as_image_3d_of_double());
  const vil3d_image_view<double>& o2_image = o2.image();;


  vcl_ofstream output_file;
  vcl_ostream *output;

  if (o1 == "-")
    output = &vcl_cout;
  else
  {
    output_file.open(o1.c_str());
    output = &output_file;
  }

  if (!*output)
  {
    vcl_cerr << "\nERROR: --save_to_mat: ";
    vcl_perror(o1.c_str());
    vcl_cerr << "\nAt \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(4);
  }

  // copy precision length from console to output file
  output->precision(vcl_cout.precision());

  (*output) <<
    "# Created by vxl/image3d_math\n"
    "# name: image3d\n"
    "# type: matrix\n"
    "# ndims: 3\n"
    << o2.image().ni() << ' ' << o2.image().nj() << ' ' << o2.image().nk() << '\n';

  for (unsigned k=0;k<o2_image.nk();++k)
    for (unsigned j=0;j<o2_image.nj();++j)
      for (unsigned i=0;i<o2_image.ni();++i)
        (*output) <<  o2_image(i,j,k) << '\n';

  s.pop(2);
}

//: Force an image to load as double - replace the filename in the operand stack with the image.
void load_image_double__string(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<double> v;
  string_to_image(s[0].as_string(), v);
  s.pop(1);
  s.push_front(operand(v));
}

//: Force an image to load as float - replace the filename in the operand stack with the image.
void load_image_float__string(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> v;
  string_to_image(s[0].as_string(), v);
  s.pop(1);
  s.push_front(operand(v));
}

//: Force an image to load as int - replace the filename in the operand stack with the image.
void load_image_int__string(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> v;
  string_to_image(s[0].as_string(), v);
  s.pop(1);
  s.push_front(operand(v));
}

void scale_and_offset__image_3d_of_float__double__double(opstack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<float> o1=s[2].as_image_3d_of_float();

  vil3d_math_scale_and_offset_values(o1.image(),
                                     s[1].as_double(),
                                     static_cast<float>(s[0].as_double()) );

  s.pop(3);
  s.push_front(operand(o1));
}


void not__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1=s[0].as_image_3d_of_int();
  const vil3d_image_view<int>& o1_image = o1.image();;

  vimt3d_image_3d_of<int> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<int> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) = o1_image(i,j,k) ? 0 : 1;

  s.pop(1);
  s.push_front(operand(result));
}


void not__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1=s[0].as_image_3d_of_float();
  const vil3d_image_view<float>& o1_image = o1.image();

  vimt3d_image_3d_of<int> result(o1_image.ni(), o1_image.nj(), o1_image.nk(),
                                 1, o1.world2im());
  vil3d_image_view<int> &result_image = result.image();

  for (unsigned k=0,nk=o1_image.nk();k<nk;++k)
    for (unsigned j=0,nj=o1_image.nj();j<nj;++j)
      for (unsigned i=0,ni=o1_image.ni();i<ni;++i)
        result_image(i,j,k) = o1_image(i,j,k) ? 0 : 1;

  s.pop(1);
  s.push_front(operand(result));
}

void convert_to_int__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1=s[0].as_image_3d_of_float();
  vimt3d_image_3d_of<int> result;

  vil3d_convert_round(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(1);
  s.push_front(operand(result));
}

void convert_to_int__image_3d_of_double(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<double> o1=s[0].as_image_3d_of_double();
  vimt3d_image_3d_of<int> result;

  vil3d_convert_round(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(1);
  s.push_front(operand(result));
}

void option_load_as_image_int(opstack_t& s)
{
  global_option_load_as_image = image3d_int_t;
  s.pop(0);
}

void option_load_as_image_double(opstack_t& s)
{
  global_option_load_as_image = image3d_double_t;
  s.pop(0);
}

void option_load_as_image_float(opstack_t& s)
{
  global_option_load_as_image = image3d_float_t;
  s.pop(0);
}

void option_precision__double(opstack_t& s)
{
  assert(s.size() >= 1);

  int prec = vnl_math_rnd(s[0].as_double());
  if (prec < 0 || prec > 20)
  {
    vcl_cerr << "\nERROR: option_precision takes an integer between 0 and 20.\n"
             << "At \"" << args_so_far << "\"<-- HERE\n";
    vcl_exit(1);
  }
  vcl_cout.precision(prec);
  s.pop(1);
}


void convert_to_float__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1=s[0].as_image_3d_of_int();
  vimt3d_image_3d_of<float> result;

  vil3d_convert_cast(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(1);
  s.push_front(operand(result));
}

void convert_to_float__image_3d_of_double(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<double> o1=s[0].as_image_3d_of_double();
  vimt3d_image_3d_of<float> result;

  vil3d_convert_cast(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(1);
  s.push_front(operand(result));
}


void product__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  vimt3d_image_3d_of<float> result;
  vil3d_math_image_product(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop(2);
  s.push_front(operand(result));
}

void copy__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  operand op = s[0];
  vimt3d_image_3d_of<float> result;
  result.deep_copy(op.as_image_3d_of_float());

  s.pop(1);
  s.push_front(op);
  s.push_front(operand(result));
}

void copy__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  operand op = s[0];
  vimt3d_image_3d_of<int> result;
  result.deep_copy(op.as_image_3d_of_int());

  s.pop(1);
  s.push_front(op);
  s.push_front(operand(result));
}

void copy__double(opstack_t& s)
{
  assert(s.size() >= 1);
  operand op = s[0];

  s.pop(1);
  s.push_front(op);
  s.push_front(op);
}

void copy_coordinate_frame__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  const vil3d_image_view<float>& i_vox = o1.image();
  const vimt3d_transform_3d& i_w2i = o2.world2im();

  vimt3d_image_3d_of<float> result;
  result.image().deep_copy(i_vox);
  result.world2im() = i_w2i;

  s.pop(2);

  s.push_front(operand(result));
}

void copy_coordinate_frame__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int());
  const vil3d_image_view<int>& i_vox = o1.image();
  const vimt3d_transform_3d& i_w2i = o2.world2im();

  vimt3d_image_3d_of<int> result;
  result.image().deep_copy(i_vox);
  result.world2im() = i_w2i;

  s.pop(2);
  s.push_front(operand(result));
}

void print_overlap__image_3d_of_float__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  const vil3d_image_view<float>& i1 = o1.image();
  const vil3d_image_view<float>& i2 = o2.image();

  // Partial volume generalised Tanamoto, as per
  // "Generalized Overlap Measures for Evaluation and Validation in Medical Image Analysis"
  // Crum et al. , IEEE Trans Medical Imaging, Vol 25, Iss 11,  Nov. 2006, 1451 - 1461

  double gTanamoto_num=0, gTanamoto_den=0;
  double sum1=0, sum2=0;
  bool dodgy=false; // True if image isn't really a partial volume/label mask.
  for (unsigned k=0, nk=o1.image().nk(); k<nk; ++k)
    for (unsigned j=0, nj=o1.image().nj(); j<nj; ++j)
      for (unsigned i=0, ni=o1.image().ni(); i<ni; ++i)
      {
        double p1 = i1(i,j,k);
        double p2 = i2(i,j,k);
        if (p1<0.0 || p1>1.0 || p2<0.0 || p2>1.0)
          dodgy=true;
        gTanamoto_num += vcl_min(p1, p2);
        gTanamoto_den += vcl_max(p1, p2);
        sum1 += p1;
        sum2 += p2;
      }

  vcl_cout << "Tanamoto: " << gTanamoto_num/gTanamoto_den <<
    " Volume Change: " << (sum2-sum1)/sum1 << vcl_endl;

  if (dodgy)
    vcl_cerr << "WARNING: PRINT_OVERLAP: At least some voxels were outside the range [0,1].\n";

  s.pop(2);
}

void print_overlap__image_3d_of_int__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int()); // golden
  vimt3d_image_3d_of<int> o2(s[0].as_image_3d_of_int()); // predicted
  const vil3d_image_view<int>& i1 = o1.image();
  const vil3d_image_view<int>& i2 = o2.image();

  // Partial volume generalised Tanamoto, as per
  // "Generalized Overlap Measures for Evaluation and Validation in Medical Image Analysis"
  // Crum et al. , IEEE Trans Medical Imaging, Vol 25, Iss 11,  Nov. 2006, 1451 - 1461

  unsigned long Tanamoto_num=0, Tanamoto_den=0;
  unsigned long sum1=0, sum2=0;
  for (unsigned k=0, nk=o1.image().nk(); k<nk; ++k)
    for (unsigned j=0, nj=o1.image().nj(); j<nj; ++j)
      for (unsigned i=0, ni=o1.image().ni(); i<ni; ++i)
      {
        int p1 = i1(i,j,k);
        int p2 = i2(i,j,k);
        Tanamoto_num += (p1 && p2)?1:0;
        Tanamoto_den += (p1 || p2)?1:0;
        sum1 += p1;
        sum2 += p2;
      }

  vcl_cout << "Tanamoto: " << static_cast<double>(Tanamoto_num)/Tanamoto_den
           << " Volume Change: " << (static_cast<double>(sum2)-sum1)/sum1 << vcl_endl;

  s.pop(2);
}

void print_quantiles__image_3d_of_float__double(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  vil3d_image_view<float> storage(o1.image().ni(), o1.image().nj(),
                                  o1.image().nk(), o1.image().nplanes());
  vil3d_copy_reformat(o1.image(), storage);

  double nsteps = vnl_math_floor(s[0].as_double());
  double step = storage.size() / nsteps;

  vcl_nth_element(storage.begin(), storage.begin() + vnl_math_rnd(step), storage.end());

  vcl_cout << "     0%: " << vcl_setw(20) << *vcl_min_element(storage.begin(), storage.begin() + vnl_math_rnd(step))
           << '\n' << vcl_setw(6) << 100.0/nsteps << "%: "
           << vcl_setw(20) << *(storage.begin() + vnl_math_rnd(step)) << '\n';
  for (unsigned i=1; i+1<nsteps; ++i)
  {
    vcl_nth_element(storage.begin() + vnl_math_rnd(i*step),
                    storage.begin() + vnl_math_rnd((i+1)*step), storage.end());
    vcl_cout << vcl_setw(6) << (i+1)*100.0/nsteps << "%: " << vcl_setw(20)
             << *(storage.begin() + vnl_math_rnd((i+1)*step)) << '\n';
  }
  vcl_cout << "   100%: " << vcl_setw(20)
           << *vcl_max_element(storage.begin() + vnl_math_rnd((nsteps-1)*step), storage.end()) << vcl_endl;
  s.pop(2);
}

void print_quantiles__image_3d_of_int__double(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  vil3d_image_view<int> storage(o1.image().ni(), o1.image().nj(),
                                o1.image().nk(), o1.image().nplanes());
  vil3d_copy_reformat(o1.image(), storage);

  double nsteps = vnl_math_floor(s[0].as_double());
  double step = storage.size() / nsteps;
  vcl_nth_element(storage.begin(), storage.begin() + vnl_math_rnd(step), storage.end());

  vcl_cout << "     0%: " << vcl_setw(20) << *vcl_min_element(storage.begin(), storage.begin() + vnl_math_rnd(step))
           << '\n' << vcl_setw(6) << 100.0/nsteps << "%: " << vcl_setw(20) << *(storage.begin() + vnl_math_rnd(step))
           << '\n';
  for (unsigned i=1; i+1<nsteps; ++i)
  {
    vcl_nth_element(storage.begin() + vnl_math_rnd(i*step),
                    storage.begin() + vnl_math_rnd((i+1)*step), storage.end());
    vcl_cout << vcl_setw(6) << (i+1)*100.0/nsteps << "%: " << vcl_setw(20)
             << *(storage.begin() + vnl_math_rnd((i+1)*step)) << '\n';
  }
  vcl_cout << "   100%: " << vcl_setw(20)
           << *vcl_max_element(storage.begin() + vnl_math_rnd((nsteps-1)*step), storage.end()) << vcl_endl;
  s.pop(2);
}

void print_stats__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());

  double mean=0, var=0;
  vil3d_math_mean_and_variance(mean, var, o1.image(), 0);
  vcl_cout << "Mean: " << mean << " Std: " << vcl_sqrt(var) << vcl_endl;

  s.pop(1);
}

void print_stats__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());

  double mean=0, var=0;
  vil3d_math_mean_and_variance(mean, var, o1.image(), 0);
  vcl_cout << "Mean: " << mean << " Std: " << vcl_sqrt(var) << vcl_endl;

  s.pop(1);
}

void print__double(opstack_t& s)
{
  assert(s.size() >= 1);
  unsigned n = static_cast<unsigned>(s[0].as_double());
  // variable number of parameters, so check types now

  if (s.size() < n+1)
  {
    vcl_cerr << "\nERROR: --print command could not find " << n << " doubles or strings to print.\n"
             << "At \"" << args_so_far << "\"<-- HERE\n"
             << "Stack is :\n" << vsl_stream_summary(s);
    vcl_exit(1);
  }

  if (n)
  {
    for (unsigned i=1;i<=n; ++i)
    {
      if (!s[i].is_string() && !s[i].is_double())
      {
        vcl_ostringstream ss;
        vcl_cerr << "\nERROR: --print command could not find " << n << " doubles or strings to print.\n"
                 << "At \"" << args_so_far << "\"<-- HERE\n"
                 << "Stack is :\n" << vsl_stream_summary(s);
        vcl_exit(1);
      }
    }

    if (s[n].is_string())
      vcl_cout << s[n].as_string();
    else
      vcl_cout << s[n].as_double();

    for (unsigned i=n-1;i>=1; --i)
    {
      if (s[i].is_string())
        vcl_cout << ' ' << s[i].as_string();
      else
        vcl_cout << ' ' << s[i].as_double();
    }
  }
  vcl_cout << vcl_endl;

  s.pop(n+1);
}

void signed_distance_transform__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());
  vil3d_image_view<bool> mask;
  vil3d_convert_cast(o1.image(), mask);

  vgl_vector_3d<double> voxel_size=o1.world2im().inverse().delta(
    vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0) );

  vimt3d_image_3d_of<float> result;
  result.world2im() = o1.world2im();

  vil3d_signed_distance_transform(mask, result.image(), 1000.f,
                                  static_cast<float>(vcl_abs(voxel_size.x())),
                                  static_cast<float>(vcl_abs(voxel_size.y())),
                                  static_cast<float>(vcl_abs(voxel_size.z())) );

  s.pop(1);
  s.push_front(operand(result));
}


void sum__double__double(opstack_t& s)
{
  assert(s.size() >= 2);
  double o1(s[1].as_double());
  double o2(s[0].as_double());

  s.pop(2);
  s.push_front(operand(o1+o2));
}

void diff__double__double(opstack_t& s)
{
  assert(s.size() >= 2);
  double o1(s[1].as_double());
  double o2(s[0].as_double());

  s.pop(2);
  s.push_front(operand(o1-o2));
}

void product__double__double(opstack_t& s)
{
  assert(s.size() >= 2);
  double o1(s[1].as_double());
  double o2(s[0].as_double());

  s.pop(2);
  s.push_front(operand(o1*o2));
}

void quotient__double__double(opstack_t& s)
{
  assert(s.size() >= 2);
  double o1(s[1].as_double());
  double o2(s[0].as_double());

  s.pop(2);
  s.push_front(operand(o1/o2));
}


void fill__image_3d_of_float__double(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  double o2(s[0].as_double());

  o1.image().fill(static_cast<float>(o2));
  s.pop(2);
  s.push_front(operand(o1));
}


void fill__image_3d_of_int__double(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o1(s[1].as_image_3d_of_int());
  double o2(s[0].as_double());

  o1.image().fill(static_cast<int>(o2));
  s.pop(2);
  s.push_front(operand(o1));
}

void local_z_normalise__image_3d_of_float__double(opstack_t& s)
{
  assert(s.size() >= 2);
  double o1(s[0].as_double());
  vimt3d_image_3d_of<float> o2(s[1].as_image_3d_of_float());

  vgl_vector_3d<double> voxel_size=o2.world2im().inverse().delta(
    vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0) );

  vimt3d_image_3d_of<float> result;
  result.world2im() = o2.world2im();

  vil3d_locally_z_normalise(o2.image(),
                            o1/voxel_size.x(), o1/voxel_size.y(), o1/voxel_size.z(),
                            result.image());

  s.pop(2);
  s.push_front(operand(result));
}

void clamp_above__image_3d_of_float__double__double(opstack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<float> o1(s[2].as_image_3d_of_float());

  vil3d_clamp_above(o1.image(), static_cast<float>(s[1].as_double()),
    static_cast<float>(s[0].as_double()) );

  s.pop(3);
  s.push_front(operand(o1));
}

void clamp_below__image_3d_of_float__double__double(opstack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<float> o1(s[2].as_image_3d_of_float());

  vil3d_clamp_below(o1.image(), static_cast<float>(s[1].as_double()),
    static_cast<float>(s[0].as_double()) );

  s.pop(3);
  s.push_front(operand(o1));
}

void clamp_above__image_3d_of_int__double__double(opstack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<int> o1(s[2].as_image_3d_of_int());

  vil3d_clamp_above(o1.image(), static_cast<int>(s[1].as_double()),
    static_cast<int>(s[0].as_double()) );

  s.pop(3);
  s.push_front(operand(o1));
}

void clamp_below__image_3d_of_int__double__double(opstack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<int> o1(s[2].as_image_3d_of_int());

  vil3d_clamp_below(o1.image(), static_cast<int>(s[1].as_double()),
    static_cast<int>(s[0].as_double()) );

  s.pop(3);
  s.push_front(operand(o1));
}

//-------------------------------------------------------------------------------------
// Execution infrastructure

class operations
{
 private:
  //: Syntax sugar to allow
  class function_type_t: public vcl_vector<operand::operand_type_t>
  {
   public:
    function_type_t& operator << (operand::operand_type_t t)
    {
      this->push_back(t);
      return *this;
    }
  };

  typedef void(*function_t)(opstack_t& stack);
  vcl_vector<vcl_string> names_;
  vcl_vector<function_t> functions_;
  vcl_vector<function_type_t > function_types_;
  vcl_vector<vcl_string> help_input_;
  vcl_vector<vcl_string> help_output_;
  vcl_vector<vcl_string> help_desc_;

  static operations singleton_;

  //: Add a single operation to the list.
  void add_operation(const char* name, function_t func, function_type_t& t,
                     const char * help_input, const char * help_output, const char * help_desc)
  {
    names_.push_back(name);
    functions_.push_back(func);
    function_types_.push_back(t);
    help_input_.push_back(help_input);
    help_output_.push_back(help_output);
    help_desc_.push_back(help_desc);
  }

  //: Add all operations to the list.
  operations()
  {
    function_type_t no_operands;

    add_operation("--and",  &and__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "im_A im_B", "image", "Logical AND over corresponding voxels in im_B and im_B");
    add_operation("--clamp-above", &clamp_above__image_3d_of_float__double__double,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double,
                  "image threshold value", "image", "Set all voxels in image at or above threshold to value");
    add_operation("--clamp-above", &clamp_above__image_3d_of_int__double__double,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_double << operand::e_double,
                  "image threshold value", "image", "Set all voxels in image at or above threshold to value");
    add_operation("--clamp-below", &clamp_below__image_3d_of_float__double__double,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double,
                  "image threshold value", "image", "Set all voxels in image at or below threshold to value");
    add_operation("--clamp-below", &clamp_below__image_3d_of_int__double__double,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_double << operand::e_double,
                  "image threshold value", "image", "Set all voxels in image at or below threshold to value");
    add_operation("--convert-to-float", &convert_to_float__image_3d_of_double,
                  function_type_t() << operand::e_image_3d_of_double,
                  "image", "image", "Convert voxel type of image from double to float");
    add_operation("--convert-to-float", &convert_to_float__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "image", "Convert voxel type of image from int to float");
    add_operation("--convert-to-int", &convert_to_int__image_3d_of_double,
                  function_type_t() << operand::e_image_3d_of_double,
                  "image", "image", "Round voxel values from double to int");
    add_operation("--convert-to-int", &convert_to_int__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "image", "Round voxel values from float to int");
    add_operation("--copy", &copy__double,
                  function_type_t() << operand::e_double,
                  "value", "value value", "Duplicate value onto stack");
    add_operation("--copy", &copy__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "image image", "Duplicate image onto stack");
    add_operation("--copy", &copy__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "image image", "Duplicate image onto stack");
    add_operation("--copy-coordinate-frame", &copy_coordinate_frame__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "image_v image_c", "image", "create image that is voxels of image_v and the world to image transform of image_c");
    add_operation("--copy-coordinate-frame", &copy_coordinate_frame__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "image_v image_c", "image", "create image that is voxels of image_v and the world to image transform of image_c");
    add_operation("--diff", &diff__double__double,
                  function_type_t() << operand::e_double << operand::e_double,
                  "A B", "A-B", "Subtract values A minus B");
    add_operation("--diff", &diff__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "im_A im_B", "im_A-B", "Subtract corresponding voxels of im_B from im_A");
    add_operation("--diff", &diff__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "im_A im_B", "im_A-B", "Subtract corresponding voxels of im_B from im_A");
    add_operation("--fill", &fill__image_3d_of_float__double,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_double,
                  "image v", "image", "Fill all voxels with value v");
    add_operation("--fill", &fill__image_3d_of_int__double,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_double,
                  "image n", "image", "Fill all voxels with value v");
    add_operation("--help", &help,
                  no_operands,
                  "", "", "Display help");
    add_operation("--load-from-mat", &load_from_mat__string,
                  function_type_t() << operand::e_string,
                  "filename", "image", "Explicitly load image from ASCII Matlab file ");
    add_operation("--load-image-double", &load_image_double__string,
                  function_type_t() << operand::e_string,
                  "filename", "image", "Explicitly load into double image.");
    add_operation("--load-image-float", &load_image_float__string,
                  function_type_t() << operand::e_string,
                  "filename", "image", "Explicitly load into float image.");
    add_operation("--load-image-int", &load_image_int__string,
                  function_type_t() << operand::e_string,
                  "filename", "image", "Explicitly load into integer image");
    add_operation("--local-z-normalise", &local_z_normalise__image_3d_of_float__double,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_double,
                  "image half_radius", "image", "Normalise each voxel by mean and stddev measured over half_radius (mm) window");
    add_operation("--max",  &max__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "im_A im_B", "image", "max over corresponding voxels in im_B and im_B");
    add_operation("--max",  &max__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "im_A im_B", "image", "max over corresponding voxels in im_B and im_B");
    add_operation("--min",  &min__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "im_A im_B", "image", "min over corresponding voxels in im_B and im_B");
    add_operation("--min",  &min__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "im_A im_B", "image", "min over corresponding voxels in im_B and im_B");
    add_operation("--not", &not__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "image", "Apply logical NOT to voxels");
    add_operation("--not", &not__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "image", "Apply logical NOT to voxels");
    add_operation("--option-load-as-image-double", &option_load_as_image_double,
                  no_operands,
                  "", "", "Load future image files as double voxel-type");
    add_operation("--option-load-as-image-float", &option_load_as_image_float,
                  no_operands,
                  "", "", "Load future image files as float voxel-type (default)");
    add_operation("--option-load-as-image-int", &option_load_as_image_int,
                  no_operands,
                  "", "", "Load future image files as int voxel-type");
    add_operation("--option-precision", &option_precision__double,
                  function_type_t() << operand::e_double,
                  "n", "", "Set precision of floating point numbers on the console to n digits");
    add_operation("--or",  &or__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "im_A im_B", "image", "Logical OR over corresponding voxels in im_B and im_B");
    add_operation("--print", &print__double,
                  function_type_t() <<operand::e_double,
                  "[value value ...] n", "", "Print the previous n values");
    add_operation("--print-overlap", &print_overlap__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "image image", "", "Print overlap measures and volumes of two binary mask images. Assuming the first image is the golden data, it prints the error rates.");
    add_operation("--print-overlap", &print_overlap__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "image image", "", "Print generalised overlap measures and volumes of two partial volume mask images.");
    add_operation("--print-quantiles", &print_quantiles__image_3d_of_int__double,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_double,
                  "image n", "", "Print n evenly space quantiles of image's voxel values");
    add_operation("--print-quantiles", &print_quantiles__image_3d_of_float__double,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_double,
                  "image n", "", "Print n evenly space quantiles of image's voxel values");
    add_operation("--print-stats", &print_stats__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "", "Print basic statistics of image's voxel values");
    add_operation("--print-stats", &print_stats__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "", "Print basic statistics of image's voxel values");
    add_operation("--product", &product__double__double,
                  function_type_t() << operand::e_double << operand::e_double,
                  "A B", "A*B", "Multiply values A plus B");
    add_operation("--product", &product__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "im_A im_B", "im_A*B", "Multiply corresponding voxels of im_A and im_B");
    add_operation("--quotient", &quotient__double__double,
                  function_type_t() << operand::e_double << operand::e_double,
                  "A B", "A/B", "Divide value A by B");
    add_operation("--recall", &recall__string,
                  function_type_t() << operand::e_string,
                  "image name", "", "Recall image or value from local store \"name\"");
    add_operation("--resample", &resample__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "image im_template", "image", "Resample image to same size and transform as im_template");
    add_operation("--save", &save__image_3d_of_float__string,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_string,
                  "image filename", "", "Save image to filename");
    add_operation("--save", &save__image_3d_of_int__string,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_string,
                  "image filename", "", "Save image to filename");
    add_operation("--save-to-mat", &save_to_mat__image_3d_of_int__string,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_string,
                  "image filename", "", "Save image to text 3D array Matlab format");
    add_operation("--save-to-mat", &save_to_mat__image_3d_of_float__string,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_string,
                  "image filename", "", "Save image to text 3D array Matlab format");
    add_operation("--save-to-mat", &save_to_mat__image_3d_of_double__string,
                  function_type_t() << operand::e_image_3d_of_double << operand::e_string,
                  "image filename", "", "Save image to text 3D array Matlab format");
    add_operation("--scale-and-offset", &scale_and_offset__image_3d_of_float__double__double,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double,
                  "image scale offset", "image", "multiply images's voxels by scale and add offset");
    add_operation("--signed-distance-transform", &signed_distance_transform__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "image", "Calculate SDT from zero/non-zero boundary in image");
    add_operation("--store", &store__poly__string,
                  function_type_t() << operand::e_double << operand::e_string,
                  "value name", "", "Store value locally as variable \"name\"");
    add_operation("--store", &store__poly__string,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_string,
                  "image name", "", "Store image locally as variable \"name\"");
    add_operation("--store", &store__poly__string,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_string,
                  "image name", "", "Store image locally as variable \"name\"");
    add_operation("--sum", &sum__double__double,
                  function_type_t() << operand::e_double << operand::e_double,
                  "A B", "A+B", "Add values A plus B");
    add_operation("--sum", &sum__image_3d_of_float__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
                  "im_A im_B", "im_A+B", "Add corresponding voxels of im_A and im_B");
    add_operation("--sum", &sum__image_3d_of_int__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int << operand::e_image_3d_of_int,
                  "im_A im_B", "im_A+B", "Add corresponding voxels of im_A and im_B");
    add_operation("--voxel-product", &voxel_product__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "value", "Product over all the voxel values.");
    add_operation("--voxel-product", &voxel_product__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "value", "Product over all the voxel values.");
    add_operation("--voxel-size", &voxel_size__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "x_length y_length z_length", "Length of each side of a voxel.");
    add_operation("--voxel-size", &voxel_size__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "x_length y_length z_length", "Length of each side of a voxel.");
    add_operation("--voxel-sum", &voxel_sum__image_3d_of_float,
                  function_type_t() << operand::e_image_3d_of_float,
                  "image", "value", "Sum over all the voxel values.");
    add_operation("--voxel-sum", &voxel_sum__image_3d_of_int,
                  function_type_t() << operand::e_image_3d_of_int,
                  "image", "value", "Sum over all the voxel values.");

    // Check they are correctly sorted.
    vcl_vector<vcl_string>::iterator it=names_.begin(), end=names_.end();
    while ((it+1)!=end)
    {
      vcl_string &left = *it;
      if (left > *++it)
      {
        vcl_cerr << "INTERNAL ERROR: \"" << left << "\" is not in correct order\n";
        vcl_abort();
      }
    }
    for (it=names_.begin(); it!=end; ++it)
      if (it->find('_')!=vcl_string::npos)
      {
        vcl_cerr << "INTERNAL ERROR: \"" << *it << "\" contains an underscore\n";
        vcl_abort();
      }
  }

  static bool operation_type_matches(const function_type_t& type, const opstack_t& stack)
  {
    if (stack.size() < type.size()) return false;
    for (unsigned i=0; i<type.size(); ++i)
    {
      if (!stack[type.size()-i-1].is_a(type[i]))
        return false;
    }
    return true;
  }
 public:
  static void run(vcl_string name, opstack_t& stack)
  {
    // Handle underscores or dashes in name.
    vcl_replace(name.begin(), name.end(), '_', '-');

    typedef vcl_pair<vcl_vector<vcl_string>::iterator, vcl_vector<vcl_string>::iterator> range_t;
    range_t range =
      vcl_equal_range(singleton_.names_.begin(), singleton_.names_.end(), name);

    if (range.first == range.second)
    {
      vcl_cerr << "\nERROR: No such operation \"" << name << "\"\n."
               << "At \"" << args_so_far << "\"<-- HERE\n";
      vcl_exit(1);
    }
    for (vcl_ptrdiff_t i = distance(singleton_.names_.begin(), range.first),
         end = distance(singleton_.names_.begin(), range.second); i!=end; ++i)
    {
      if (operation_type_matches(singleton_.function_types_[i], stack))
      {
        (*singleton_.functions_[i])(stack);
        return ;
      }
    }
    vcl_cerr << "\nERROR: Unable to match operands for operation \"" << name << '"'
             << "\nAt \"" << args_so_far << "\"<-- HERE\n"
             << "\nStack is :\n" << vsl_stream_summary(stack)
             << "\nPossible \"" << name << "\" operations are:\n";
    print(vcl_cerr, distance(singleton_.names_.begin(), range.first),
      distance(range.first, range.second) );
    vcl_exit(1);
  }
  static void print(vcl_ostream& os, vcl_size_t j, vcl_size_t n)
  {
    n+=j;
    for (unsigned i=j; i<n; ++i)
    {
      vcl_string name = singleton_.names_[i].substr(2);
      os << vul_string_upcase(name) << ":  "
         << singleton_.help_desc_[i] << "\n    usage: "
         << singleton_.help_input_[i] << ' ' << singleton_.names_[i];
      if (!singleton_.help_output_[i].empty())
        os << " => " << singleton_.help_output_[i];
      os << "\n    types: " ;
      if (singleton_.function_types_[i].empty())
        os << "null";
      else for (unsigned j=0, nj=singleton_.function_types_[i].size(); j<nj; ++j)
        os << singleton_.function_types_[i][j] << ' ';
      os << "\n\n";
    }
  }
  static void print(vcl_ostream& os)
  {
    print(os, 0, singleton_.names_.size());
  }
};

operations operations::singleton_ = operations();
void print_operations(vcl_ostream&os) { operations::print(os); }

//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  opstack_t stack;

  if (argc==1)
    help(stack);

  for (int i=1; i<argc; ++i)
  {
    vcl_string option = argv[i];

    args_so_far += option + ' ';

    if (option.empty()) continue;
    if (option.substr(0, 2) == "--")
    {
      operations::run(option, stack);
      continue; // skip reset_last_pop at end of loop.
    }

    // Special cases
    if (option == "{")
      stack.push_front(operand_open_brace());
    else if (option == "}")
      close_brace__brace(stack);
    else if (option.length()>1 && option[0]=='+')
    {
      stack.push_front(operand(option.substr(1, vcl_string::npos)));
      recall__string(stack);
    }
    else // operand case
      stack.push_front(operand(option));

    stack.reset_last_pop();
  }

  if (!stack.empty())
    MBL_LOG(WARN, logger(), "Stack not empty after all operations.\n" <<
            vsl_stream_summary(stack));

  return global_retval;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  try
  {
    // Initialize the logger
    mbl_logger::root().load_log_config_file();

    vimt3d_add_all_loaders();
    vil3d_file_format::add_format(new vil3d_gen_synthetic_format);

    return main2(argc, argv);
  }
  catch (vcl_exception& e)
  {
    vcl_cerr << "caught exception " << e.what() << '\n';
    return 3;
  }
  catch (...)
  {
    vcl_cerr << "caught unknown exception\n";
    return 3;
  }
}

#else // VCL_HAS_EXCEPTIONS
int main(int argc, char*argv[])
{
  vcl_cerr << "\nERROR: image3d_math needs exception support to compile properly.\n";
}
#endif // VCL_HAS_EXCEPTIONS
