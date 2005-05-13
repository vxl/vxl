//:
// \file
// \brief Exceptions thrown by mbl, and a mechanism for turning them off.
// \author Ian Scott.

#include <vcl_sstream.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_read_props.h>

//: Throw error if there are any keys in props that aren't in ignore.
void mbl_exception_look_for_unused_props(
  const vcl_string & function_name,
  const mbl_read_props_type &props,
  const mbl_read_props_type &ignore)
{
  mbl_read_props_type p2(props);
  
  // Remove ignoreable properties
  for (mbl_read_props_type::const_iterator it=ignore.begin();
         it != ignore.end(); ++it)
    p2.erase(it->first);

  if (!p2.empty())
  {

    vcl_ostringstream ss;
    mbl_read_props_print(ss, p2);
    mbl_exception_error(mbl_exception_unused_props(function_name, ss.str()));
  }
}


