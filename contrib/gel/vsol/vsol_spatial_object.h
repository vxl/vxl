// This is gel/vsol/vsol_spatial_object.h
#ifndef vsol_spatial_object_h_
#define vsol_spatial_object_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Abstract base class for vsol_spatial_object_(n)d, n=2,3,...
//
// \author
//     H.Can Aras
//
//-----------------------------------------------------------------------------

#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_flags_id.h>

class vsol_spatial_object : public vul_timestamp, public vbl_ref_count, public vsol_flags_id
{
 public:
  // Constructors/Destructor---------------------------------------------------
  virtual ~vsol_spatial_object();
 protected:
  vsol_spatial_object();
};

#endif // vsol_spatial_object_h_
