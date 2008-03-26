//:
// \file
// \brief Builder for mfpf_sad_vec_cost objects.
// \author Tim Cootes


#include <mfpf/mfpf_sad_vec_cost_builder.h>
#include <mfpf/mfpf_sad_vec_cost.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_sad_vec_cost_builder::mfpf_sad_vec_cost_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_sad_vec_cost_builder::set_defaults()
{
  min_mad_=1.0;
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_sad_vec_cost_builder::~mfpf_sad_vec_cost_builder()
{
}

//: Create new mfpf_sad_vec_cost on heap
mfpf_vec_cost* mfpf_sad_vec_cost_builder::new_cost() const
{
  return new mfpf_sad_vec_cost();
}


//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_sad_vec_cost_builder::clear(unsigned n_egs)
{
  data_.resize(0);
}

//: Add one example to the model
void mfpf_sad_vec_cost_builder::add_example(const vnl_vector<double>& v)
{
  data_.push_back(v);
}

//: dv[i] = |v1[i]-v2[i]|
inline void abs_diff(const vnl_vector<double>& v1,
              const vnl_vector<double>& v2,
              vnl_vector<double>& dv)
{
  unsigned n = v1.size();
  dv.set_size(n);
  for (unsigned i=0;i<n;++i) dv[i]=vcl_fabs(v1[i]-v2[i]);
}

//: Build this object from the data supplied in add_example()
void mfpf_sad_vec_cost_builder::build(mfpf_vec_cost& pf)
{
  assert(pf.is_a()=="mfpf_sad_vec_cost");
  mfpf_sad_vec_cost& nc = static_cast<mfpf_sad_vec_cost&>(pf);

  unsigned n = data_.size();

  if (n==1)
  {
    // Create from only one example
    vnl_vector<double> wts(data_[0].size(),1.0/min_mad_);
    nc.set(data_[0],wts);
    return;
  }

  // First compute mean
  vnl_vector<double> mean=data_[0];
  for (unsigned i=1;i<n;++i) mean+=data_[i];
  mean/=n;

  // Now compute mean absolute difference from mean
  vnl_vector<double> dv, dv_sum;
  abs_diff(mean,data_[0],dv_sum);
  for (unsigned i=1;i<n;++i) 
  {
    abs_diff(mean,data_[i],dv);
    dv_sum+=dv;
  }

  vnl_vector<double> wts(mean.size());
  for (unsigned i=0;i<mean.size();++i)
  {
    wts[i]=1.0/vcl_max(min_mad_,dv_sum[i]/n);
  }

  nc.set(mean,wts);

  // Discard data
  data_.resize(0);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_sad_vec_cost_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  if (props.find("min_mad")!=props.end())
  {
    min_mad_=vul_string_atof(props["min_mad"]);
    props.erase("min_mad");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_sad_vec_cost_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_sad_vec_cost_builder::is_a() const
{
  return vcl_string("mfpf_sad_vec_cost_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_vec_cost_builder* mfpf_sad_vec_cost_builder::clone() const
{
  return new mfpf_sad_vec_cost_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_sad_vec_cost_builder::print_summary(vcl_ostream& os) const
{
  os << "{ min_mad: " << min_mad_;
  os << " }";
}

//: Version number for I/O
short mfpf_sad_vec_cost_builder::version_no() const
{
  return 1;
}


void mfpf_sad_vec_cost_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_mad_);
  vsl_b_write(bfs,data_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_sad_vec_cost_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,min_mad_);
      vsl_b_read(bfs,data_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

