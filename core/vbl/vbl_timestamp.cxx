// This is vxl/vbl/vbl_timestamp.cxx

#include <vbl/vbl_timestamp.h>

unsigned long vbl_timestamp::mark = 0;

unsigned long vbl_timestamp::get_unique_timestamp()
{
  return mark++;
}

vbl_timestamp::vbl_timestamp()
{
  this->touch();
}

vbl_timestamp::~vbl_timestamp()
{
}

void vbl_timestamp::touch()
{
  timestamp_ = get_unique_timestamp();
}

bool vbl_timestamp::older(vbl_timestamp const& t) const
{
  // find out if this is older that t
  return timestamp_<t.get_time_stamp();
}
