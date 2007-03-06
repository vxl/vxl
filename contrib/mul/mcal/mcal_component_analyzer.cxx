//:
// \file
#include "mcal_component_analyzer.h"
#include <vcl_cstdlib.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_block.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

mcal_component_analyzer::mcal_component_analyzer()
{
}

//=======================================================================

mcal_component_analyzer::~mcal_component_analyzer()
{
}

//: Compute the mean of the supplied data
void mcal_component_analyzer::compute_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                     vnl_vector<double>& mean)
{
  if (data.size()==0)
  {
    mean.set_size(0);
    return;
  }
  data.reset();
  mean = data.current();
  while (data.next()) mean += data.current();
  mean/=data.size();
}

//: Compute mean and modes from the supplied data
//  Computes the mean, then calls build_about_mean
void mcal_component_analyzer::build(mbl_data_wrapper<vnl_vector<double> >& data,
                     vnl_vector<double>& mean,
                     vnl_matrix<double>& modes,
                     vnl_vector<double>& mode_var)
{
  compute_mean(data,mean);
  build_about_mean(data,mean,modes,mode_var);
}


void mcal_component_analyzer::build_from_array(const vnl_vector<double>* data, int n,
                                vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                                vnl_vector<double>& mode_var)
{
  mbl_data_array_wrapper<vnl_vector<double> > data_array(data,n);
  build(data_array,mean,modes,mode_var);
}

//=======================================================================

short mcal_component_analyzer::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const mcal_component_analyzer& b)
{
  vsl_binary_loader<mcal_component_analyzer>::instance().add(b);
}

//=======================================================================

vcl_string  mcal_component_analyzer::is_a() const
{
  return vcl_string("mcal_component_analyzer");
}


//: Create a concrete mcal_component_analyzer object, from a text specification.
vcl_auto_ptr<mcal_component_analyzer>
  mcal_component_analyzer::new_component_analyzer_from_stream(vcl_istream &is)
{
  vcl_string name;
  is >> name;

  vcl_auto_ptr<mcal_component_analyzer> pvmb;
  try
  {
    pvmb = mbl_cloneables_factory<mcal_component_analyzer>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
      vcl_cerr<<"ERROR in mcal_component_analyzer::new_vm_builder_from_stream\n"
              <<"\tRequired vector model builder of "<<name<<" is not in the factory. Further exception details follow:\n"
              <<'\t'<<e.what()<<vcl_endl;
      vcl_abort();
  }
  pvmb->config_from_stream(is);
  return pvmb;
}

//: Read initialisation settings from a stream.
// The default implementation merely checks that no properties have
// been specified.
void mcal_component_analyzer::config_from_stream(vcl_istream& is)
{
  vcl_string s = mbl_parse_block(is);
  if (s.empty() || s=="{}") return;

  throw mbl_exception_parse_error(
    this->is_a() + " expects no properties in initialisation,\n"
    "But the following properties were given:\n" + s);
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mcal_component_analyzer& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mcal_component_analyzer& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mcal_component_analyzer& b)
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

vcl_ostream& operator<<(vcl_ostream& os,const mcal_component_analyzer* b)
{
    if (b)
    return os << *b;
    else
    return os << "No mcal_component_analyzer defined.";
}
