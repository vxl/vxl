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
  _timestamp = get_unique_timestamp();
}
