// This is gel/vifa/vifa_group_pgram_params.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vifa/vifa_group_pgram_params.h>


vifa_group_pgram_params::
vifa_group_pgram_params(float  angle_increment,
                        int    max_suppress_radius,
                        float  length_ratio)
{
  init_params(angle_increment,
              max_suppress_radius,
              length_ratio);
}

vifa_group_pgram_params::
vifa_group_pgram_params(const vifa_group_pgram_params&  dp)
{
  init_params(dp._angle_increment,
              dp._max_suppress_radius,
              dp._length_ratio);
}

bool vifa_group_pgram_params::
SanityCheck()
{
  vcl_stringstream  msg;
  bool        valid = true;

  if ((_angle_increment <= 0) || (_angle_increment > 45))
  {
    msg << "ERROR: Value of angle increment is out of range (0, 45]" << vcl_ends;
    valid = false;
  }

  if (_max_suppress_radius <= 0)
  {
    msg << "ERROR: Maximum suppression radius must be greater than 0" << vcl_ends;
    valid = false;
  }

  if ((_length_ratio <= 0) || (_length_ratio > 1.0))
  {
    msg << "ERROR: length_ratio is out of range (0, 1.0]" << vcl_ends;
    valid = false;
  }

  SetErrorMsg(msg.str().c_str());
  return valid;
}

void vifa_group_pgram_params::
print_info(void)
{
  vcl_cout << "vifa_group_pgram_params:\n"
           << "  angle increment     = " << _angle_increment << vcl_endl
           << "  max suppress radius = " << _max_suppress_radius << vcl_endl
           << "  length ratio        = " << _length_ratio << vcl_endl;
}

void vifa_group_pgram_params::
init_params(float  angle_increment,
            int    max_suppress_radius,
            float  length_ratio)
{
  _angle_increment = angle_increment;
  _max_suppress_radius = max_suppress_radius;
  _length_ratio = length_ratio;
}
