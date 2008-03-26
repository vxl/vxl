//:
// \file
// \brief Cost functions to evaluate vector of pixel samples
// \author Tim Cootes


#include "mfpf_vec_cost.h"
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vcl_cassert.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_vec_cost::mfpf_vec_cost()
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_vec_cost::~mfpf_vec_cost()
{
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_vec_cost::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_vec_cost::is_a() const
{
  return vcl_string("mfpf_vec_cost");
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_vec_cost& b)
{
  vsl_binary_loader<mfpf_vec_cost>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_vec_cost& b)
{
    b.b_write(bfs);
    
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_vec_cost& b)
{
    b.b_read(bfs);
    
}

//=======================================================================
// Associated function: operator<< 
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_vec_cost& b)
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

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_vec_cost* b)
{
    if (b)  
    return os << *b;
    else      
    return os << "No mfpf_vec_cost defined.";
}
