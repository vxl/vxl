//: \file
//  \author Ian Scott
//  \date 12 Aug 2008
//  \brief General Purpose Reverse Polish Notation image maths program.
// To add your own operations, add a function (e.g. plus__image_3d_of_float__image_3d_of_float),
// and add a entry in operations::operations() to register your new operation. That list
// of entries must be kept sorted.


#include <vcl_iostream.h>

#if VCL_HAS_EXCEPTIONS
#include <vcl_exception.h>
#include <vcl_stdexcept.h>
#include <vcl_sstream.h>
#include <vcl_deque.h>
#include <vcl_iomanip.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vsl/vsl_deque_io.txx>
#include <vsl/vsl_stream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_ios_state.h>
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


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.image_3d_math");
  return l;
}


//: This is the return value of the program.
// It can be modified using the (as yet unimplemented) "-return" operator.
int global_retval=0;
//: load images as float types if not true
bool global_option_load_as_image_int = false;

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

bool string_to_image(const vcl_string&s, vimt3d_image_3d_of<float>&d)
{
  try
  {
    vimt3d_load(s, d, true);
  }
  catch (const vcl_exception& e)
  {
    vcl_cerr << e.what() << '\n';
    return false;
  }
  return true;
}

bool string_to_image(const vcl_string&s, vimt3d_image_3d_of<int>&d)
{
  try
  {
    vimt3d_load(s, d, true);
  }
  catch (const vcl_exception& e)
  {
    vcl_cerr << e.what() << '\n';
    return false;
  }
  return true;
}


//: A value on the stack, to be used as by an operation.
// This is a rather inefficient VARIANT-type class.
class operand
{
  vcl_string string_;
  vimt3d_image_3d_of<float> image_3d_of_float_;
  vimt3d_image_3d_of<int> image_3d_of_int_;
  double double_;

 public:
  enum operand_type_t { e_string, e_image_3d_of_float, e_image_3d_of_int, e_double };
  operand_type_t operand_type_;

  explicit operand(const vcl_string& s):
    string_(s), operand_type_(e_string) {}
  explicit operand(const vimt3d_image_3d_of<float>& i):
    image_3d_of_float_(i), operand_type_(e_image_3d_of_float) {}
  explicit operand(const vimt3d_image_3d_of<int>& i):
    image_3d_of_int_(i), operand_type_(e_image_3d_of_int) {}
  explicit operand(const double& i):
    double_(i), operand_type_(e_double) {}

  operand_type_t operand_type() const { return operand_type_; }

  const bool is_string() const { return operand_type_==e_string; }
  const vcl_string& as_string() const { assert(is_string()); return string_; }

  const bool is_double() const
  {
    double dummy;
    return (operand_type_==e_string && string_to_double(string_, dummy))
      || operand_type_==e_double;
  }
  const double as_double() const
  {
    assert(is_double());
    if (operand_type_==e_double) return double_;
    else if (operand_type_==e_string)
    {
      double v;
      string_to_double(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a double: " << *this;
    throw mbl_exception_abort(ss.str());
  }

  const bool is_image_3d_of_float() const
  {
    vimt3d_image_3d_of<float> dummy;
    return (operand_type_==e_string && !global_option_load_as_image_int && string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_float;
  }
  const vimt3d_image_3d_of<float> as_image_3d_of_float() const
  {
    if (operand_type_==e_image_3d_of_float) return image_3d_of_float_;
    else if (operand_type_==e_string && !global_option_load_as_image_int)
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

  const bool is_image_3d_of_int() const
  {
    vimt3d_image_3d_of<int> dummy;
    return (operand_type_==e_string && global_option_load_as_image_int && string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_int;
  }
  const vimt3d_image_3d_of<int> as_image_3d_of_int() const
  {
    if (operand_type_==e_image_3d_of_int) return image_3d_of_int_;
    else if (operand_type_==e_string && global_option_load_as_image_int)
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


  void print_summary(vcl_ostream &ss) const
  {
    // Forward declaration.
    vcl_ostream& operator <<( vcl_ostream&ss, const operand::operand_type_t& t);

    ss << operand_type_ << ": ";
    switch (operand_type_)
    {
    case e_string:
      ss << string_;
      break;
    case e_image_3d_of_float:
      ss << vsl_stream_summary(image_3d_of_float_);
      break;
    case e_image_3d_of_int:
      ss << vsl_stream_summary(image_3d_of_int_);
      break;
    case e_double:
      ss << double_;
      break;
    default:
      {
        vcl_ostringstream ss;
        ss << "Unknown operand_type: " << operand_type_;
        throw mbl_exception_abort(ss.str());
      }
    }
  }
  bool is_a(operand_type_t t) const
  {
    switch (t)
    {
    case e_string:
      return is_string();
    case e_image_3d_of_float:
      return is_image_3d_of_float();
    case e_image_3d_of_int:
      return is_image_3d_of_int();
    case e_double:
      return is_double();
    default:
      {
        vcl_ostringstream ss;
        ss << "Unknown operand_type: " << operand_type_;
        throw mbl_exception_abort(ss.str());
      }
    }
    return false;
  }
};

void vsl_print_summary( vcl_ostream&ss, const operand& p) { p.print_summary(ss); }
vcl_ostream& operator <<( vcl_ostream&ss, const operand& p) { p.print_summary(ss); return ss; }

void vsl_print_summary(vcl_ostream& os, const vcl_deque<operand> &v)
{
  os << "Deque length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os,v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ...\n";
}

vcl_ostream& operator <<( vcl_ostream&ss, const operand::operand_type_t& t)
{
  switch (t)
  {
    case operand::e_string:
      ss << "string"; break;
    case operand::e_image_3d_of_float:
      ss << "image_3d_of_float"; break;
    case operand::e_image_3d_of_int:
      ss << "image_3d_of_int"; break;
    case operand::e_double:
      ss << "double"; break;
    default:
      {
        vcl_ostringstream ss;
        ss << "Unknown operand_type: " << static_cast<int>(t);
        throw mbl_exception_abort(ss.str());
      }
  }
  return ss;
}

typedef vcl_deque<operand> opstack_t;


void print_operations(vcl_ostream&);

//: Operation implementation
void help(opstack_t& s)
{
  vcl_cerr <<
    "usage: image3d_math [--operand | operation] [--operand | operation] ... --operand\n"
    "Manipulate images using Reverse polish notiation.\n"
    "List of commands:\n";
  print_operations(vcl_cerr);
  vcl_exit(3);
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

  s.pop_front();
  s.pop_front();
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
    i2w.origin(), i2w.delta(zero, gvec3(1, 0, 0)),
    i2w.delta(zero, gvec3(0, 1, 0)), i2w.delta(zero, gvec3(0, 0, 1)),
    o2.image().ni(), o2.image().nj(), o2.image().nk() );

  result.world2im() = o2.world2im();

  s.pop_front();
  s.pop_front();
  s.push_front(operand(result));
}


//: Save a float image using vil3d saver
void save__image_3d_of_float__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_save(s[0].as_string(), s[1].as_image_3d_of_float(), true);

  s.pop_front();
  s.pop_front();
}

//: Save an int image using vil3d saver
void save__image_3d_of_int__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_save(s[0].as_string(), s[1].as_image_3d_of_int(), true);

  s.pop_front();
  s.pop_front();
}

//: Save a float image to a ascii matlab format
void save_to_mat__image_3d_of_float__string(opstack_t& s)
{
  assert(s.size() >= 2);

  vcl_string o1(s[0].as_string());
  vimt3d_image_3d_of<float> o2(s[1].as_image_3d_of_float());
  const vil3d_image_view<float>& o2_image = o2.image();;

  vcl_ofstream output(o1.c_str());

  if (!output)
    mbl_exception_throw_os_error(o1);

  output <<
    "# Created by vxl/image3d_math\n"
    "# name: image3d\n"
    "# type: matrix\n"
    "# ndims: 3\n"
    << o2.image().ni() << ' ' << o2.image().nj() << ' ' << o2.image().nk() << '\n';

  for (unsigned k=0;k<o2_image.nk();++k)
    for (unsigned j=0;j<o2_image.nj();++j)
      for (unsigned i=0;i<o2_image.ni();++i)
        output <<  o2_image(i,j,k) << '\n';

  s.pop_front();
  s.pop_front();
}

//: Force an image to load - replace the filename in the operand stack with the image.
// Uses global_option_load_as_image_int to decide whether to load as float or int.
void load__string(opstack_t& s)
{
  assert(s.size() >= 1);
  if (global_option_load_as_image_int)
  {
    vimt3d_image_3d_of<int> v;
    string_to_image(s[0].as_string(), v);
    s.pop_front();
    s.push_front(operand(v));
  }
  else
  {
    vimt3d_image_3d_of<float> v;
    string_to_image(s[0].as_string(), v);
    s.pop_front();
    s.push_front(operand(v));
  }
}

void scale_and_offset__image_3d_of_float__double__double(opstack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<float> o1=s[2].as_image_3d_of_float();

  vil3d_math_scale_and_offset_values(o1.image(),
    s[1].as_double(), static_cast<float>(s[0].as_double()) );

  s.pop_front();
  s.pop_front();
  s.pop_front();
  s.push_front(operand(o1));
}

void convert_to_int__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1=s[0].as_image_3d_of_float();
  vimt3d_image_3d_of<int> result;

  vil3d_convert_round(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop_front();
  s.push_front(operand(result));
}

void option_load_as_image_int(opstack_t& s)
{
  global_option_load_as_image_int = true;
}

void option_load_as_image_float(opstack_t& s)
{
  global_option_load_as_image_int = false;
}

void convert_to_float__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1=s[0].as_image_3d_of_int();
  vimt3d_image_3d_of<float> result;

  vil3d_convert_cast(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop_front();
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

  s.pop_front();
  s.pop_front();
  s.push_front(operand(result));
}

void copy__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> result;
  result.deep_copy(o1);

  s.push_front(operand(result));
}

void copy__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());
  vimt3d_image_3d_of<int> result;
  result.deep_copy(o1);

  s.push_front(operand(result));
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
  vul_ios_state_saver saved(vcl_cout);
  vcl_cout.precision(9);


  vcl_nth_element(storage.begin(), storage.begin() + vnl_math_rnd(step), storage.end());

  vcl_cout << "     0%: " << vcl_setw(16) << *vcl_min_element(storage.begin(), storage.begin() + vnl_math_rnd(step))
           << '\n' << vcl_setw(6) << 100.0/nsteps << "%: "
    << vcl_setw(16) << *(storage.begin() + vnl_math_rnd(step)) << '\n';
  for (unsigned i=1; i+1<nsteps; ++i)
  {
    vcl_nth_element(storage.begin() + vnl_math_rnd(i*step),
      storage.begin() + vnl_math_rnd((i+1)*step), storage.end());
    vcl_cout << vcl_setw(6) << (i+1)*100.0/nsteps << "%: " << vcl_setw(16)
             << *(storage.begin() + vnl_math_rnd((i+1)*step)) << '\n';
  }
  vcl_cout << "   100%: " << vcl_setw(16)
           << *vcl_max_element(storage.begin() + vnl_math_rnd((nsteps-1)*step), storage.end()) << vcl_endl;
  s.pop_front();
  s.pop_front();
}

void print_quantiles__image_3d_of_int__double(opstack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());
  vil3d_image_view<int> storage(o1.image().ni(), o1.image().nj(),
    o1.image().nk(), o1.image().nplanes());
  vil3d_copy_reformat(o1.image(), storage);

  double nsteps = vnl_math_floor(s[0].as_double());
  double step = storage.size() / nsteps;
  vul_ios_state_saver saved(vcl_cout);
  vcl_nth_element(storage.begin(), storage.begin() + vnl_math_rnd(step), storage.end());

  vcl_cout << "   0%: " << vcl_setw(16) << *vcl_min_element(storage.begin(), storage.begin() + vnl_math_rnd(step))
           << '\n' << vcl_setw(6) << 100.0/nsteps << "%: " << *(storage.begin() + vnl_math_rnd(step)) << '\n';
  for (unsigned i=1; i <9; ++i)
  {
    vcl_nth_element(storage.begin() + vnl_math_rnd(i*step),
      storage.begin() + vnl_math_rnd((i+1)*step), storage.end());
    vcl_cout << vcl_setw(6) << (i+1)*100.0/nsteps << "%: " << vcl_setw(16)
             << *(storage.begin() + vnl_math_rnd((i+1)*step)) << '\n';
  }
  vcl_cout << " 100%: " << vcl_setw(16)
           << *vcl_max_element(storage.begin() + vnl_math_rnd((nsteps-1)*step), storage.end()) << vcl_endl;
  s.pop_front();
  s.pop_front();
}

void print_stats__image_3d_of_float(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());

  double mean=0, var=0;
  vil3d_math_mean_and_variance(mean, var, o1.image(), 0);
  vcl_cout << "Mean: " << mean << " Std: " << vcl_sqrt(var) << vcl_endl;

  s.pop_front();
}

void print_stats__image_3d_of_int(opstack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1(s[0].as_image_3d_of_int());

  double mean=0, var=0;
  vil3d_math_mean_and_variance(mean, var, o1.image(), 0);
  vcl_cout << "Mean: " << mean << " Std: " << vcl_sqrt(var) << vcl_endl;

  s.pop_front();
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

  s.pop_front();
  s.push_front(operand(result));
}

void local_z_normalise__image_3d_of_float__double(opstack_t& s)
{
  assert(s.size() >= 1);
  double o1(s[0].as_double());
  vimt3d_image_3d_of<float> o2(s[1].as_image_3d_of_float());


  vgl_vector_3d<double> voxel_size=o2.world2im().inverse().delta(
    vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0) );

  vimt3d_image_3d_of<float> result;
  result.world2im() = o2.world2im();

  vil3d_locally_z_normalise(o2.image(),
    o1/voxel_size.x(), o1/voxel_size.y(), o1/voxel_size.z(),
    result.image());

  s.pop_front();
  s.pop_front();
  s.push_front(operand(result));
}

void clamp_above__image_3d_of_float__double__double(opstack_t& s)
{
  vimt3d_image_3d_of<float> o1(s[2].as_image_3d_of_float());

  vil3d_clamp_above(o1.image(), static_cast<float>(s[1].as_double()),
    static_cast<float>(s[0].as_double()) );

  s.pop_front();
  s.pop_front();
  s.pop_front();
  s.push_front(operand(o1));
}

void clamp_below__image_3d_of_float__double__double(opstack_t& s)
{
  vimt3d_image_3d_of<float> o1(s[2].as_image_3d_of_float());

  vil3d_clamp_below(o1.image(), static_cast<float>(s[1].as_double()),
    static_cast<float>(s[0].as_double()) );

  s.pop_front();
  s.pop_front();
  s.pop_front();
  s.push_front(operand(o1));
}

void clamp_above__image_3d_of_int__double__double(opstack_t& s)
{
  vimt3d_image_3d_of<int> o1(s[2].as_image_3d_of_int());

  vil3d_clamp_above(o1.image(), static_cast<int>(s[1].as_double()),
    static_cast<int>(s[0].as_double()) );

  s.pop_front();
  s.pop_front();
  s.pop_front();
  s.push_front(operand(o1));
}

void clamp_below__image_3d_of_int__double__double(opstack_t& s)
{
  vimt3d_image_3d_of<int> o1(s[2].as_image_3d_of_int());

  vil3d_clamp_below(o1.image(), static_cast<int>(s[1].as_double()),
    static_cast<int>(s[0].as_double()) );

  s.pop_front();
  s.pop_front();
  s.pop_front();
  s.push_front(operand(o1));
}


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

    add_operation("--clamp_above", &clamp_above__image_3d_of_float__double__double,
      function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double,
      "image threshold value", "image", "Set all voxels in image at or above threshold to value");
    add_operation("--clamp_above", &clamp_above__image_3d_of_int__double__double,
      function_type_t() << operand::e_image_3d_of_int << operand::e_double << operand::e_double,
      "image threshold value", "image", "Set all voxels in image at or above threshold to value");
    add_operation("--clamp_below", &clamp_below__image_3d_of_float__double__double,
      function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double,
      "image threshold value", "image", "Set all voxels in image at or below threshold to value");
    add_operation("--clamp_below", &clamp_below__image_3d_of_int__double__double,
      function_type_t() << operand::e_image_3d_of_int << operand::e_double << operand::e_double,
      "image threshold value", "image", "Set all voxels in image at or below threshold to value");
    add_operation("--convert_to_float", &convert_to_float__image_3d_of_int,
      function_type_t() << operand::e_image_3d_of_int,
      "image", "image", "Convert voxel type of image from int to float");
    add_operation("--convert_to_int", &convert_to_int__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float,
      "image", "image", "Round voxel values from float to int");
    add_operation("--copy", &copy__image_3d_of_int,
      function_type_t() << operand::e_image_3d_of_int,
      "image", "image image", "Duplicate image onto stack");
    add_operation("--copy", &copy__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float,
      "image", "image image", "Duplicate image onto stack");
    add_operation("--help", &help,
      no_operands,
      "", "", "Display help");
    add_operation("--load", &load__string,
      function_type_t() << operand::e_string,
      "filename", "image", "Explicitly load image using current option_load_as_image_type");
    add_operation("--local_z_normalise", &local_z_normalise__image_3d_of_float__double,
      function_type_t() << operand::e_image_3d_of_float << operand::e_double,
      "image half_radius", "image", "Normalise each voxel by mean and stddev measured over half_radius window");
    add_operation("--option_load_as_image_float", &option_load_as_image_float,
      no_operands,
      "", "", "Load future image files as float voxel-type (default)");
    add_operation("--option_load_as_image_int", &option_load_as_image_int,
      no_operands,
      "", "", "Load future image files as int voxel-type");
    add_operation("--print_quantiles", &print_quantiles__image_3d_of_int__double,
      function_type_t() << operand::e_image_3d_of_int << operand::e_double,
      "image n", "", "Print n evenly space quantiles of image's voxel values");
    add_operation("--print_quantiles", &print_quantiles__image_3d_of_float__double,
      function_type_t() << operand::e_image_3d_of_float << operand::e_double,
      "image n", "", "Print n evenly space quantiles of image's voxel values");
    add_operation("--print_stats", &print_stats__image_3d_of_int,
      function_type_t() << operand::e_image_3d_of_int,
      "image", "", "Print basic statistics of image's voxel values");
    add_operation("--print_stats", &print_stats__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float,
      "image", "", "Print basic statistics of image's voxel values");
    add_operation("--product", &product__image_3d_of_float__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
      "im_A im_B", "im_A*B", "Multiply corresponding voxels of im_A and im_B");
    add_operation("--resample", &resample__image_3d_of_float__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
      "image im_template", "image", "Resample image to same size and transform as im_template");
    add_operation("--save", &save__image_3d_of_float__string,
      function_type_t() << operand::e_image_3d_of_float << operand::e_string,
      "filename image", "", "Save image to filename");
    add_operation("--save", &save__image_3d_of_int__string,
      function_type_t() << operand::e_image_3d_of_int << operand::e_string,
      "filename image", "", "Save image to filename");
    add_operation("--save_to_mat", &save_to_mat__image_3d_of_float__string,
      function_type_t() << operand::e_image_3d_of_float << operand::e_string,
      "filename image", "", "Save image to text 3D array Matlab format");
    add_operation("--scale_and_offset", &scale_and_offset__image_3d_of_float__double__double,
      function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double,
      "image scale offset", "image", "multiply images's voxels by scale and add offset");
    add_operation("--signed_distance_transform", &signed_distance_transform__image_3d_of_int,
      function_type_t() << operand::e_image_3d_of_int,
      "image", "image", "Calculate SDT from zero/non-zero boundary in image");
    add_operation("--sum", &sum__image_3d_of_float__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float,
      "im_A im_B", "im_A+B", "Add corresponding voxels of im_A and im_B");

    // Check they are correctly sorted.
    vcl_vector<vcl_string>::iterator it=names_.begin(), end=names_.end();
    while ((it+1)!=end)
    {
      vcl_string &left = *it;
      if (left > *++it)
        throw mbl_exception_abort(vcl_string("\"") + left + "\" is not in correct order");
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
  static void run(vcl_string &name, opstack_t& stack)
  {
    typedef vcl_pair<vcl_vector<vcl_string>::iterator, vcl_vector<vcl_string>::iterator> range_t;
    range_t range =
      vcl_equal_range(singleton_.names_.begin(), singleton_.names_.end(), name);
    for (unsigned i = distance(singleton_.names_.begin(), range.first),
      end = distance(singleton_.names_.begin(), range.second); i!=end; ++i)
    {
      if (operation_type_matches(singleton_.function_types_[i], stack))
      {
        (*singleton_.functions_[i])(stack);
        return ;
      }
    }
    vcl_ostringstream ss;
    ss << "Unable to match command \"" << name << '"'
       << "\nStack is :\n" << vsl_stream_summary(stack);
    throw vcl_runtime_error(ss.str());
  }
  static void print(vcl_ostream& ss)
  {
    unsigned n = singleton_.names_.size();

    for (unsigned i=0; i<n; ++i)
    {
      vcl_string name = singleton_.names_[i].substr(2);
      ss << vul_string_upcase(name) << ":  " <<
        singleton_.help_desc_[i] << "\n    usage: " <<
        singleton_.help_input_[i] << ' ' << singleton_.names_[i];
      if (!singleton_.help_output_[i].empty())
        ss << " => " << singleton_.help_output_[i];
      ss << "\n    types: " ;
      if (singleton_.function_types_[i].empty())
        ss << "null";
      else for (unsigned j=0, nj=singleton_.function_types_[i].size(); j<nj; ++j)
        ss << singleton_.function_types_[i][j] << ' ';
      ss << "\n\n";
    }
  }
};

operations operations::singleton_ = operations();
void print_operations(vcl_ostream&ss) { operations::print(ss); }

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

    if (option.empty()) continue;
    if (option.substr(0, 2) == "--")
    {
      operations::run(option, stack);
    }
    else
    {
      stack.push_front(operand(option));
    }
  }

  if (!stack.empty())
    MBL_LOG(WARN, logger(), "Stack not empty after all operations." <<
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

    main2(argc, argv);
  }
  catch (vcl_exception& e)
  {
    vcl_cout << "caught exception " << e.what() << vcl_endl;
    return 3;
  }
  catch (...)
  {
    vcl_cout << "caught unknown exception" << vcl_endl;
    return 3;
  }

  return 0;
}
#else // VCL_HAS_EXCEPTIONS
int main(int argc, char*argv[])
{
  vcl_cerr << "ERROR: image3d_math needs exception support to compile properly.\n";
}
#endif
