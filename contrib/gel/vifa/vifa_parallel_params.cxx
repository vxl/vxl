// This is gel/vifa/vifa_parallel_params.cxx

#include <vcl_iostream.h>
#include <vifa/vifa_parallel_params.h>


vifa_parallel_params::
vifa_parallel_params(float  angle_min,
                     float  angle_max,
                     int  buckets)
{
  min_angle = angle_min;
  max_angle = angle_max;
  nbuckets = buckets;
}

vifa_parallel_params::
vifa_parallel_params(const vifa_parallel_params& np)
  : gevd_param_mixin(), vul_timestamp(), vbl_ref_count()
{
  min_angle = np.min_angle;
  max_angle = np.max_angle;
  nbuckets = np.nbuckets;
}

vifa_parallel_params::
vifa_parallel_params(vifa_parallel_params* np)
{
  if (np)
  {
    min_angle = np->min_angle;
    max_angle = np->max_angle;
    nbuckets = np->nbuckets;
  }
  else
  {
    min_angle = DEFAULT_MIN_ANGLE;
    max_angle = DEFAULT_MAX_ANGLE;
    nbuckets = DEFAULT_NBUCKETS;
  }
}

void vifa_parallel_params::
print_info(void)
{
  vcl_cout << "vifa_parallel_params:\n"
           << "  min angle    = " << min_angle << vcl_endl
           << "  max angle    = " << max_angle << vcl_endl
           << "  # of buckets = " << nbuckets << vcl_endl;
}
