//:
// \file
// \brief Builder for mfpf_profile_pdf objects.
// \author Tim Cootes


#include <mfpf/mfpf_profile_pdf_builder.h>
#include <mfpf/mfpf_profile_pdf.h>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vcl_cassert.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_data_array_wrapper.h>

#include <vimt/vimt_bilin_interp.h>
#include <vimt/vimt_sample_profile_bilin.h>
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_vector_io.h>

#include <mfpf/mfpf_norm_vec.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_profile_pdf_builder::mfpf_profile_pdf_builder()
{
  set_defaults();
}

//: Define default values
void mfpf_profile_pdf_builder::set_defaults()
{
  step_size_=1.0;
  ilo_=-4; ihi_=4;
  search_ni_=5;
}

void mfpf_profile_pdf_builder::set_step_size(double s)
{
  step_size_=s;
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_profile_pdf_builder::~mfpf_profile_pdf_builder()
{
}

//: Create new mfpf_profile_pdf on heap
mfpf_point_finder* mfpf_profile_pdf_builder::new_finder() const
{
  return new mfpf_profile_pdf();
}

void mfpf_profile_pdf_builder::set(int ilo, int ihi,
                               const vpdfl_builder_base& builder)
{
  ilo_=ilo;
  ihi_=ihi;
  pdf_builder_ = builder.clone();
}


//: Initialise building
// Must be called before any calls to add_example(...)
void mfpf_profile_pdf_builder::clear(unsigned n_egs)
{
  data_.resize(0);
}


//: Add one example to the model
void mfpf_profile_pdf_builder::add_example(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)
{
  int n=1+ihi_-ilo_;
  unsigned np=image.image().nplanes();
  vnl_vector<double> v(n*np);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p+ilo_*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n);
  mfpf_norm_vec(v);
  data_.push_back(v);
}

//: Build this object from the data supplied in add_example()
void mfpf_profile_pdf_builder::build(mfpf_point_finder& pf)
{
  assert(pf.is_a()=="mfpf_profile_pdf");
  mfpf_profile_pdf& nc = static_cast<mfpf_profile_pdf&>(pf);
  nc.set_search_area(search_ni_,0);

  vcl_cout<<"Building from "<<data_.size()<<" examples."<<vcl_endl;

  vpdfl_pdf_base *pdf = pdf_builder().new_model();
  mbl_data_array_wrapper<vnl_vector<double> > data(&data_[0],data_.size());

  pdf_builder().build(*pdf,data);

  nc.set(ilo_,ihi_,*pdf);

  // Tidy up
  delete pdf;
  data_.resize(0);
}

//=======================================================================
// Method: set_from_stream
//=======================================================================

//: Initialise from a string stream
bool mfpf_profile_pdf_builder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  // Extract the properties
  if (props.find("ilo")!=props.end())
  {
    ilo_=vul_string_atoi(props["ilo"]);
    props.erase("ilo");
  }
  if (props.find("ihi")!=props.end())
  {
    ihi_=vul_string_atoi(props["ihi"]);
    props.erase("ihi");
  }

  if (props.find("search_ni")!=props.end())
  {
    search_ni_=vul_string_atoi(props["search_ni"]);
    props.erase("search_ni");
  }

  if (props.find("pdf_builder")!=props.end())
  {
    vcl_istringstream b_ss(props["pdf_builder"]);
    vcl_auto_ptr<vpdfl_builder_base> bb =
         vpdfl_builder_base::new_pdf_builder_from_stream(b_ss);
    pdf_builder_ = bb->clone();
    props.erase("pdf_builder");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_profile_pdf_builder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_profile_pdf_builder::is_a() const
{
  return vcl_string("mfpf_profile_pdf_builder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder_builder* mfpf_profile_pdf_builder::clone() const
{
  return new mfpf_profile_pdf_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_profile_pdf_builder::print_summary(vcl_ostream& os) const
{
  os << "{ step_size: " << step_size_
     << " size: [" << ilo_ << ',' << ihi_ << "] "
     << " search_ni: " << search_ni_
     << " pdf_builder: " << pdf_builder_
     << '}';
}

void mfpf_profile_pdf_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_);
  vsl_b_write(bfs,ilo_);
  vsl_b_write(bfs,ihi_);
  vsl_b_write(bfs,search_ni_);
  vsl_b_write(bfs,pdf_builder_);
  vsl_b_write(bfs,data_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_profile_pdf_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,ilo_);
      vsl_b_read(bfs,ihi_);
      vsl_b_read(bfs,search_ni_);
      vsl_b_read(bfs,pdf_builder_);
      vsl_b_read(bfs,data_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

