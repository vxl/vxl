#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif


#include <mbl/mbl_progress_composite.h>
#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>


//=======================================================================
mbl_progress_composite::mbl_progress_composite()
{
}


//=======================================================================
mbl_progress_composite::~mbl_progress_composite()
{
}


//=======================================================================
void mbl_progress_composite::add_progress(mbl_progress *progress_object)
{
  progress_objects_.push_back(progress_object);
}


//=======================================================================
short mbl_progress_composite::version_no() const
{
  return 1;
}


//=======================================================================
vcl_string mbl_progress_composite::is_a() const
{
  return vcl_string("mbl_progress_composite");
}


// =================================================
void mbl_progress_composite::on_set_estimated_iterations(const vcl_string &identifier,int total_iterations)
{
  for (unsigned int i=0;i<progress_objects_.size();++i)
  {
    progress_objects_[i]->set_estimated_iterations(identifier,total_iterations,display_text(identifier));
  }
}


// =================================================
void mbl_progress_composite::on_set_progress(const vcl_string &identifier, int progress)
{
  for (unsigned int i=0;i<progress_objects_.size();++i)
  {
    progress_objects_[i]->set_progress(identifier,progress);
  }
}


// =================================================
void mbl_progress_composite::on_end_progress(const vcl_string &identifier)
{
  for (unsigned int i=0;i<progress_objects_.size();++i)
  {
    progress_objects_[i]->end_progress(identifier);
  }
}
