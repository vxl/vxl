// This is vxl/vul/vul_timestamp.cxx

#include "vul_timestamp.h"

unsigned long vul_timestamp::mark = 0;

unsigned long vul_timestamp::get_unique_timestamp()
{
  return mark++;
}

vul_timestamp::vul_timestamp()
{
  this->touch();
}

vul_timestamp::~vul_timestamp()
{
}

void vul_timestamp::touch()
{
  timestamp_ = get_unique_timestamp();
}

bool vul_timestamp::older(vul_timestamp const& t) const
{
  // find out if this is older that t
  return timestamp_<t.get_time_stamp();
}
