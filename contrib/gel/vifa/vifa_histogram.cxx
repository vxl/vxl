// This is gel/vifa/vifa_histogram.cxx
#include <vifa/vifa_histogram.h>
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>

static int MEAN_FLAG = 1;
static int SD_FLAG = 2;

// max & min inline functions
// These functions return the max or min, respectively of the two arguments
// passed in.  If they are equal, the return value will be the first agrument.
// MPP 7/25/2003
// Replaced w/ vcl_max & vcl_min template functions
//static inline float max(float f1, float f2) {return (f1>f2) ? f1 : (f2>f1) ? f2 : f1 ;}
//static inline float min(float f1, float f2) {return (f1<f2) ? f1 : (f2<f1) ? f2 : f1 ;}

vifa_histogram::vifa_histogram()
{
  vals = new float [1];
  vals[0] = 0.0f;
  counts = new float [1];
  counts[0] = 0.0f;
  num = 0;
  vmin = 0;
  vmax = 0;
  delta = 0.0f;
  mean = 0.0f;
  standard_dev = 0.0f;
  stats_consistent = 0;
  stats_consistent |= (MEAN_FLAG | SD_FLAG);
  delimiter = ' ';
}

vifa_histogram::vifa_histogram(int xres, float val1, float val2)
{
  vals = new float [xres];
  counts = new float [xres];
  num = xres;

  // MPP 7/25/2003
  // Swapped argument order so val1 is selected if arguments are equal
  vmax = vcl_max(val2, val1);
  vmin = vcl_min(val2, val1);

  delta = (vmax - vmin) / xres;
  mean = (vmax + vmin) / 2.0f;
  standard_dev = (vmax - vmin) / float(2 * vcl_sqrt(3.0));
  stats_consistent = 0;
  stats_consistent |= (MEAN_FLAG | SD_FLAG);
  delimiter = ' ';

  if (vals == NULL || counts == NULL)
  {
    vcl_cerr << "vifa_histogram : Ran out of array memory.\n\n";
    vals = NULL;
    counts = NULL;
    num = 0;
    vmin = 0;
    vmax = 0;
    delta = 0.0f;
  }
  else
  {
    for (int i = 0; i < xres; i++)
    {
      vals[i] = vmin + (delta * (i + 0.5f));
      counts[i] = 0.0f;
    }
  }
}

vifa_histogram::vifa_histogram(float* uvals, float* ucounts, int xres)
{
  stats_consistent = 0;
  vals = uvals;
  counts = ucounts;
  num = xres;
  delta = vals[1] - vals[0]; // Changed this from delta = 1.0f
  //  vmax = GetMaxVal();
  //  vmin = GetMinVal(); JAF version
  // MPP 7/1/2002
  // JimG - inconsistent with JLM's definition!
  vmin = uvals[0]     - 0.5f * delta;
  vmax = uvals[num-1] + 0.5f * delta;

  mean = GetMean();
  standard_dev = GetStandardDev();
  stats_consistent |= (MEAN_FLAG | SD_FLAG);

  delimiter = ' ';
}
//-----------------------------------------------------------
//: Copy constructor
vifa_histogram::vifa_histogram(const vifa_histogram& h)
{
  // We know we really aren't changing h, but the array access to h isn't
  // strictly speaking const. JLM -Oct 2000
  stats_consistent = 0;
  vifa_histogram& his = (vifa_histogram&)h;

  num = his.GetRes();

  vals = new float[num];
  float* his_vals = his.GetVals();

  counts = new float[num];
  float* his_counts = his.GetCounts();

  if (vals == NULL || counts == NULL)
  {
    vcl_cerr << "vifa_histogram : Ran out of array memory.\n\n";
    vals = NULL;
    counts = NULL;
    num = 0;
    vmin = 0;
    vmax = 0;
    delta = 0.0f;
    stats_consistent = 0;
    return;
  }

  mean = his.GetMean();
  standard_dev = his.GetStandardDev();

  for (int i = 0; i < num; i++)
  {
    vals[i] = his_vals[i];
    counts[i] = his_counts[i];
  }

  // MPP 7/1/2002
  // Jim G. found an issue with this code if original histogram had
  // zero counts in end buckets.  See Cumulative() for fix.
  vmax = his.GetMaxVal();
  vmin = his.GetMinVal();

  delta = his.GetBucketSize();
  stats_consistent |= (MEAN_FLAG | SD_FLAG);
  delimiter = h.delimiter;
}

//---------------------------------------------------------------------
// Destructor
vifa_histogram::~vifa_histogram()
{
  if (vals)
    delete [] vals;
  if (counts)
    delete [] counts;
}


//---------------------------------------
//: Resample a histogram
vifa_histogram::vifa_histogram(vifa_histogram const* his, float width, bool preserveCounts)
{
  stats_consistent = 0;

  // Attributes of original histogram
  // Width of buckets
  float org_delta = his->GetBucketSize();

  // Last bucket index
  int max_index = his->GetRes() - 1;

  // Range start
  float minvalue = his->GetVals()[0] - (org_delta * 0.5f);

  // Range end
  float maxvalue = his->GetVals()[max_index] + (org_delta * 0.5f);

  // Initialize a new histogram
  if (width == org_delta)
  {
    num = his->GetRes();
  }
  else if (!(width == 0.0f))
  {
    // Any degenerate cases?
    num = (int)vcl_ceil((maxvalue - minvalue) / width);
  }
  else
  {
    // This shouldn't happen anyway.
    num = 1;
  }

  vals = new float [num];
  counts = new float [num];
  delta = width;

  // Entire range of x-values, including endpoints of first & last bin
  float mean_val = (maxvalue + minvalue) * 0.5f;
  float half_range = (num * delta) * 0.5f;

  // Endpoint to endpoint (inconsistent w/ JLM's definition!)
  vmax =  mean_val + half_range;
  vmin =  mean_val - half_range;

  if (vals == NULL || counts == NULL)
  {
    vcl_cerr << "vifa_histogram : Ran out of array memory.\n\n";
    vals = NULL;
    counts = NULL;
    num = 0;
    vmin = 0;
    vmax = 0;
    delta = 0.0f;
    mean = 0.0f;
    standard_dev = 0.0f;
    stats_consistent |= (MEAN_FLAG | SD_FLAG);
    return;
  }
  else
  {
    for (int i = 0; i < num; i++)
    {
      // Inconsistent with JLM's definition!
      vals[i] = vmin + (delta * (i + 0.5f));
      counts[i] = 0.0f;
    }
  }

  // Cases:

  if (delta == org_delta)    // Then just copy his
  {
    float* his_counts = his->GetCounts();
    for (int i = 0; i < num; i++)
      counts[i] = his_counts[i];
    mean = GetMean();
    standard_dev = GetStandardDev();
    stats_consistent |= (MEAN_FLAG | SD_FLAG);
    return;
  }

  if (org_delta > delta)     // Then interpolate his counts.
  {
    // Boundary conditions:
    //    Start

    // Midpoint of old first bucket
    float his_start = minvalue + (0.5f * org_delta);

    // Inconsistent with JLM's definition!
    float start = vmin + (0.5f * delta);

    // Contents of first old bucket
    float c0 = his->GetCount(his_start);

    // Contents of second old bucket
    float c1 = his->GetCount(his_start + org_delta);

    // Slope between first & second old buckets
    float s0 = (c1 - c0) / org_delta;

    // Start at first new bucket.  Loop until <= one new delta beyond
    // old 2nd bucket; put counts into new histogram buckets.
    for (float x = start; x <= (his_start + org_delta + delta);)
    {
      float x_diff = x - his_start;
      float interp = c0 + (s0 * x_diff);

      // MPP 7/1/2002
      // JimG - Should never be negative!
      if (interp < 0)
        interp = 0; // Can be negative

      SetCount(x, interp);
      x += delta;
    }

    //    End
    // Midpoint of last old bucket
    float his_end = maxvalue - (0.5f * org_delta);

    // Midpoint of last new bucket
    float end = vmax - (0.5f * delta);

    // Contents of last old bucket
    float cn = his->GetCount(his_end);

    // Contents of next-to-last old bucket
    float cn_1 = his->GetCount(his_end - org_delta);

    // Slope between next-to-last & last buckets
    float sn = (cn_1 - cn) / org_delta;

    // MPP 7/1/2002
    // JimG - Is "+ delta" an error?  Inconsistent w/ JLM's definition!
    for (float y = end; y >= (his_end - org_delta + delta);)
    {
      float x_diff = his_end - y;
      float interp = cn + (sn * x_diff);
      if (interp < 0)
        interp = 0; // Can be negative

      SetCount(y, interp);
      y -= delta;
    }

    // Interior Loop
    for (float z = his_start + org_delta; z <= (his_end - org_delta);)
    {
      // Old bucket contents
      float ci = his->GetCount(z);
      float ci_1 = his->GetCount(z - org_delta);
      float cip1 = his->GetCount(z + org_delta);

      // Old bucket first derivative
      float deriv = (cip1 - ci_1)/(2.0f * org_delta);

      // Old bucket second derivative
      float second_drv =
        (((cip1 + ci_1) / 2.0f) - ci) / (org_delta * org_delta);

      // X-index in new histogram, based on old histogram value
      int fine_x_index = GetIndex(z);

      // Clamp the index to the allowed bucket range
      if (fine_x_index < 0)
      {
        if (z < vmin)
          fine_x_index = 0;
        else
          fine_x_index = num - 1;
      }

      // X-value in new histogram (center of bucket)
      float fine_x = vals[fine_x_index];

      // Fill in all of the new histogram buckets within the range
      // of the old histogram bucket [z, z+org_delta)
      for (float xfine = fine_x; xfine < z + org_delta;)
      {
        float interp = ci + (deriv * (xfine - z)) +
          second_drv * (xfine - z) * (xfine - z);
        if (interp < 0)
          interp = 0; // Can be negative ???

        SetCount(xfine, interp);
        xfine += delta;
      }

      z += org_delta;
    }

    // MPP 7/2/2002
    // If needed, rescale the new histogram's
    // counts to preserve overall bucket counts
    if (preserveCounts)
    {
      float scaleFactor = delta / org_delta;
      for (int i = 0; i < num; i++)
        counts[i] *= scaleFactor;
    }
  }


  if (org_delta < delta)
  {
    // Just accumulate samples from his into larger bins
    if (org_delta != 0.0f)
    {
      float his_start = minvalue + (0.5f * org_delta);
      float his_end = maxvalue - (0.5f * org_delta);
      for (float x = his_start; x <= his_end;)
      {
        SetCount(x, GetCount(x) + his->GetCount(x));
        x += org_delta;
      }
    }
  }

  mean = GetMean();
  standard_dev = GetStandardDev();
  stats_consistent = (MEAN_FLAG | SD_FLAG);
  delimiter = his->delimiter;
}

//--------------------------------------------------
//: Transform the value axis of a histogram by a translation, transl, and a scale factor, scale.
//    The new histogram has the same resolution as his.

vifa_histogram* vifa_histogram::Scale(float scale_factor)
{
  stats_consistent = 0;
  // Extract attributes of self

  //    float lowvalue = vals[0];
  float highvalue = vals[num-1];

  // Construct a new histogram

  vifa_histogram* scaled_his = new vifa_histogram(this, delta);
  float* new_counts = scaled_his->GetCounts();
  for (int i = 0; i < num; i++)  // Initialize
    new_counts[i] = 0.0f;

  // Compute scaled values
  // We assume that the new histogram is to be scaled down from his

  float scale = scale_factor;
  if (scale_factor > 1.0f)
    scale = 1.0f;

  for (float x = highvalue; x > vmin;)
  {
    float trans_x = (x - vmin) * scale + vmin; // Scaled x.
    int index = GetIndex(trans_x);
    if (index < 0)
    {
      if (trans_x < vmin)
        index = 0;
      else
        index = num - 1;
    }

    float fraction = (trans_x - vals[index])/delta;
    float abs_fraction = (float)vcl_fabs(fraction);
    int x_index = GetIndex(x);
    if (x_index < 0)
    {
      if (x < vmin)
        x_index = 0;
      else
        x_index = num - 1;
    }

    // Distribute the counts in proportion

    new_counts[index] += (1.0f - abs_fraction) * counts[x_index];
    if (fraction > 0)
    {
      if (index < (num - 1))
        new_counts[index + 1] += abs_fraction * counts[x_index];
      else
        new_counts[index] += abs_fraction * counts[x_index];
    }
    else
    {
      if (index > 0)
        new_counts[index - 1] += abs_fraction * counts[x_index];
      else
        new_counts[index] += abs_fraction * counts[x_index];
    }

    x -= delta;
  }

  // Compute new histogram attributes

  mean = scaled_his->GetMean();
  standard_dev = scaled_his->GetStandardDev();
  stats_consistent |= (MEAN_FLAG | SD_FLAG);
  return scaled_his;
}

//---------------------------------------------------------------------
//: Assuming that "this" is a histogram of population density, construct a new histogram which is the cumulative distribution.
//    Each bin, xi, in his is assumed to represent a density, i.e.,
//            {x | (xi - .5*delta) < x <= (xi + .5*delta)}
//    Each bin, xi, in the result represents a cumulative distribution, i.e.,
//            {x | x <= (xi + .5*delta)}
vifa_histogram* vifa_histogram::Cumulative()
{
  vifa_histogram* cum_his = new vifa_histogram(*this);
  float* density_counts = this->GetCounts();
  int res = this->GetRes();

  // Initialize cumulative counts
  float* cum_counts = cum_his->GetCounts();
  for (int j=0; j < res; j++)
    cum_counts[j] = 0;

  // Accumulate counts
  for (int i = (res - 1); i >= 0; i--) // RWMC: cumulative ignored lowest bin
    for (int j = i; j >= 0; j--)
      cum_counts[j] += density_counts[i];

  // MPP 7/1/2002
  // JimG - Another bug: if the original histogram had zero counts in the
  //   lower bucket, vmin for cumulative histogram was incorrectly set by
  //   copy constructor above.  Let's reset vmin & vmax after cumulative
  //   counts have been filled in!
  cum_his->vmin = cum_his->GetMinVal() - 0.5f * cum_his->GetBucketSize();
  cum_his->vmax = cum_his->GetMaxVal() + 0.5f * cum_his->GetBucketSize();

  return cum_his;
}

//:
// Provides the correct values for histogram counts when the bin index
// extends outside the valid range of the counts array.  This function
// permits easy array access logic for the NonMaximumSuppression algorithm.
// The cyclic flag indicates that the counts array index is circular, i.e,
// cnts[0] equivalent to cnts[n_bins]
inline float GetExtendedCount(int bin, int n_bins, float* cnts, bool cyclic)
{
  int nbm = n_bins-1;
  if (!cyclic)
  {
    if (bin < 0)
      return cnts[0];
    if (bin >= n_bins)
      return cnts[nbm];
  }
  else
  {
    if (bin<0)
      return cnts[nbm+bin];
    if (bin >= n_bins)
      return cnts[bin-n_bins];
  }
  return cnts[bin];
}

//: Prune any sequences of more than one maximum value
// That is, it is possible to have a "flat" top peak with an arbitrarily
// long sequence of equal, but maximum values. The cyclic flag indicates
// that the sequence wraps around, i.e. cnts[0] equivalent to cnts[nbins]
void vifa_histogram::RemoveFlatPeaks(int nbins, float* cnts, bool cyclic)
{
  int nbm = nbins-1;

  //Here we define a small state machine - parsing for runs of peaks
  //init is the state corresponding to an initial run (starting at i ==0)
  float init=GetExtendedCount(0, nbins, cnts, cyclic);
  int init_end =0;

  //start is the state corresponding to any other run of peaks
  bool start=false;
  int start_index=0;

  //The scan of the state machine
  for (int i = 0; i < nbins; i++)
  {
    float v = GetExtendedCount(i, nbins, cnts, cyclic);

    //State init: a string of non-zeroes at the beginning.
    if (init&&v!=0)
      continue;

    if (init&&v==0)
    {
      init_end = i;
      // fix to eliminate compiler warning.
      // init used to be bool, but now is float.  It should still work.
      //init = false;
      init = 0;
      continue;
    }

    //State !init&&!start: a string of "0s"
    if (!start&&v==0)
      continue;

    //State !init&&start: the first non-zero value
    if (!start&&v!=0)
    {
      start_index = i;
      start = true;
      continue;
    }
    //State ending flat peak: encountered a subsequent zero after starting
    if (start&&v==0)
    {
      int peak_location = (start_index+i-1)/2;//The middle of the run
      for (int k = start_index; k<=(i-1); k++)
        if (k!=peak_location)
          cnts[k] = 0;
      start = false;
    }
  }
  //Now handle the boundary conditions
  //The non-cyclic case
  if (!cyclic)
  {
    if (init_end!=0)  //Was there an initial run of peaks?
    {
      int init_location = (init_end-1)/2;
      for (int k = 0; k<init_end; k++)
        if (k!=init_location)
          cnts[k] = 0;
    }
    if (start)       // Did we reach the end of the array in a run of pks?
    {
      int end_location = (start_index + nbm)/2;
      for (int k = start_index; k<nbins; k++)
        if (k!=end_location)
          cnts[k] = 0;
    }
  }
  else  //The cyclic case
  {
    if (init_end!=0)  //Is there a run which crosses the cyclic cut?
      if (start)
      { //Yes, so define the peak location accordingly
        int peak_location = (start_index + init_end - nbm -1)/2;
        int k;
        if (peak_location < 0) //Is the peak to the left of the cut?
        {// Yes, to the left
          peak_location += nbm;
          for ( k = 0; k< init_end; k++)
            cnts[k]=0;
          for ( k= start_index; k <nbins; k++)
            if (k!=peak_location)
              cnts[k] = 0;
        }
        else
        {//No, on the right.
          for ( k = start_index; k< nbins; k++)
            cnts[k]=0;
          for ( k= 0; k < init_end; k++)
            if (k!=peak_location)
              cnts[k] = 0;
        }
      }
      else
      {//There wasn't a final run so just clean up the initial run
        int init_location = (init_end-1)/2;
        for (int k = 0; k<=init_end; k++)
          if (k!=init_location)
            cnts[k] = 0;
      }
  }
}

//----------------------------------------------------------
//: Suppress values in the histogram which are not locally
//    The neighborhood for computing the local maximum a maximum.
//    is [radius X radius], e.g. for radius =1 the neighborhood
//    is [-X-], for radius = 2, the neighborhood is [--X--], etc.
//    If the cyclic flag is true then the index space is assumed to
//    be equivalent to a circle. That is, elements "0" and "n_buckets"
//    are in correspondence.
vifa_histogram* vifa_histogram::NonMaximumSupress(int radius, bool cyclic)
{
  if ((2*radius +1)> num/2)
  {
    vcl_cerr << "In vifa_histogram::NonMaximumSupress(): radius is too large\n";
    return NULL;
  }
  //Get the counts array of "this"
  vifa_histogram* h_new = new vifa_histogram(*this);
  int n_buckets = h_new->GetRes();
  float* counts_old = this->GetCounts();

  //Make a new histogram for the suppressed version
  float* counts_new = h_new->GetCounts();
  int i;
  for ( i =0; i < n_buckets; i++)
    counts_new[i] = 0;

  //Find local maxima
  for ( i = 0; i<  n_buckets; i++)
  {
    //find the maximum value in the current kernel
    float max_count = counts_old[i];
    for (int k = -radius; k <= radius ;k++)
    {
      int index = i+k;
      float c = GetExtendedCount(index, n_buckets, counts_old, cyclic);
      if ( c > max_count)
        max_count = c;
    }
    //Is position i a local maximum?
    if (max_count == counts_old[i])
      counts_new[i] = max_count;//Yes. So set the counts to the max value
  }
  RemoveFlatPeaks(n_buckets, counts_new, cyclic);
  return h_new;
}


//----------------------------------------------------------
//: Compute the mean of the histogram population
float vifa_histogram::GetMean() const
{
  float xsum = 0.0f;
  float minv = this->GetMinVal();
  float maxv = this->GetMaxVal();
  float bucket_size = this->GetBucketSize();

  if (MEAN_FLAG & stats_consistent)
    return mean;
  else
  {
    if (bucket_size > 0.0f)
    {
      for (float x=minv; x<=maxv; x+=bucket_size)
        xsum += x*GetCount(x);
    }
    else
    {
      stats_consistent |= MEAN_FLAG;
      mean = (maxv+minv)/2.0f;
      return mean;
    }

    float area = ComputeArea(vmin, vmax);
    if (area <= 0.0f)
    {
#ifdef DEBUG
      vcl_cerr << "vifa_histogram::GetMean() : Area <= 0.0\n\n";
#endif
      return 0.0f;
    }
    else
    {
      stats_consistent |= MEAN_FLAG;
      mean = xsum/area;
      return mean;
    }
  }
}

float vifa_histogram::GetStandardDev() const
{
  float sum = 0.0f;
  float bucket_size = this->GetBucketSize();

  if (SD_FLAG & stats_consistent)
    return standard_dev;
  else
  {
    float xm = this -> GetMean(); // Force an Update of Mean

    if ( this->GetBucketSize() > 0.0f)
    {
      for (float x=this->GetMinVal(); x<= this->GetMaxVal(); x += bucket_size)
        sum += (x-xm)*(x-xm)*GetCount(x);
    }
    else
    {
      stats_consistent |= SD_FLAG;
      standard_dev = 0.0f;
      return standard_dev;
    }

    float area = ComputeArea(vmin, vmax);
    if (area <= 0.0f)
    {
#ifdef DEBUG
      vcl_cerr << "vifa_histogram::GetStandardDev() : Area <= 0.0\n\n";
#endif
      return 0.0f;
    }
    else
    {
      stats_consistent |= SD_FLAG;
      standard_dev = (float)vcl_sqrt(sum/area);
      return standard_dev;
    }
  }
}

float vifa_histogram::GetMedian() const
{
  // step through each bin until num_so_far > num_samps / 2
  int i = 0;  // bin number
  float num_samps_2 = 0.5f * GetNumSamples();
  float num_so_far  = 0;
  while (num_so_far < num_samps_2)
  {
    num_so_far += counts[i];
    i++;
  }

  return vals[i];
}

int vifa_histogram::GetIndex(float pixelval) const
{
  // Done by binary search.  This was taking far too long
  // Since there is an implication that the distance between
  // bins is near constant, we could probably do even better
  // by linear interpolation.

  // The present routine will find the least i such that
  // pixelval <= vals[i] + 0.5*delta;

  // MPP 7/1/2002
  // JimG - If vmin/vmax defined incorrectly, this test fails & returns -1!
  if ((pixelval > vmax) || (pixelval < vmin))
    return -1;

  // The required solution must lie between low and high inclusive
  // Thus, low-1 is in the no set and high is in the yes set.
  int high = num-1;
  int low = 0;

  while (high > low)
  {
    // Get a mid point.  mid will lie strictly in the range [low, high-1]
    int mid = (high + low - 1) / 2;

    if (pixelval <= vals[mid] + 0.5f*delta)
    {
      // mid is in the yes set
      high = mid;
    }
    else
    {
      // mid is in the no set
      low = mid + 1;
    }
  }

  // Now, low=high, so they are the required solution
  return low;
}

int vifa_histogram::GetValIndex(float pixelval) const
{
  if ((pixelval > vmax) || (pixelval < vmin))
    return -1;

  int idx = 0;

  for (int i = 0; i < num; i++)
  {
    if ((pixelval > (vals[i] - 0.5f * delta)) &&
        (pixelval <= (vals[i] + 0.5f * delta)))
    {
      idx = i;
      break;
    }
  }

  return idx;
}

float vifa_histogram::GetCount(float pixelval) const
{
  int index = GetIndex(pixelval);
  if (index < 0)
    return -1;
  else
    return counts[index];
}

float vifa_histogram::GetMinVal() const
{
  register int i=0;
  while (i<num-1 && !counts[i])
    i++;
  return vals[i];
}

float vifa_histogram::GetMaxVal() const
{
  register int i=num-1;
  while (i>0 && !counts[i])
    i--;
  if (i < 0)
    return 0.0f;
  return vals[i];
}

float vifa_histogram::GetMaxCount() const
{
  register int i;
  float max = 0.0f;

  for (i=0; i < num; i++)
  {
    if (counts[i] > max)
    {
      max = counts[i];
    }
  }

  return max;
}

float vifa_histogram::SetCount(float pixelval, float count)
{
  stats_consistent = 0;

  int index = GetIndex(pixelval);

  if (index < 0)
    return -1;
  else
  {
    counts[index] = count;
    return count;
  }
}

void vifa_histogram::UpCount(float pixelval, bool useNewIndexMethod)
{
  stats_consistent = 0;
  int index = -1;
  if (useNewIndexMethod)
    index = GetValIndex(pixelval);
  else
    UpCount(pixelval);

  if (index >= 0)
    counts[index] += 1.0f;
}

void vifa_histogram::UpCount(float pixelval)
{
  stats_consistent = 0;
  int index = GetIndex(pixelval);
  if (index >= 0)  // Originally (index > 0)
  {
    counts[index] += 1.0f;
  }
}

int vifa_histogram::GetNumSamples() const
{
  float num_samps = 0;
  for (int i=0; i < num; i++)
  {
    num_samps += counts[i];
  }

  return (int)num_samps;
}

float vifa_histogram::ComputeArea(float low, float high) const
{
  float maxval = GetMaxVal();
  float minval = GetMinVal();

  if (low < minval)
    low = minval;
  if (high > maxval)
    high = maxval;

  if (low <= high)
  {
    int indexlow, indexhigh;
    indexlow = (int) GetIndex(low);
    if (indexlow < 0)
    {
      if (low < vmin)
        indexlow = 0;
      else
        indexlow = num-1;
    }

    indexhigh = (int) GetIndex(high);
    if (indexhigh < 0)
    {
      if (high < vmin)
        indexhigh = 0;
      else
        indexhigh = num-1;
    }

    register int i=indexlow;
    float sum = 0.0f;
    while (i <= indexhigh)
    {
      sum += counts[i];
      i++;
    }

    return sum;
  }
  else
  {
    return 0.0f;
  }
}
//----------------------------------------------------------------------
//: Compute the total area under the histogram
//
float vifa_histogram::ComputeArea() const
{
  float vmin = this->GetMinVal();
  float vmax = this->GetMaxVal();

  if (vmin>vmax)
  {
    float temp = vmin;
    vmin = vmax;
    vmax = temp;
  }

  return this->ComputeArea(vmin, vmax);
}
//----------------------------------------------------------------------
//: Finds the lower bound value which eliminates a given fraction of histogram area.
//
float vifa_histogram::LowClipVal(float clip_fraction)
{
  if (clip_fraction < 0)
    clip_fraction = 0.0f;
  if (clip_fraction > 1.0f)
    clip_fraction = 1.0f;

  float area = this->ComputeArea();
  if (area == 0.0f)
    return this->GetMinVal();

  if (clip_fraction == 0.0f)
    return this->GetMinVal();
  if (clip_fraction == 1.0f)
    return this->GetMaxVal();

  float clip_area = area*clip_fraction;
  float* counts = this->GetCounts();
  float* vals = this->GetVals();
  int res = this->GetRes();
  float sum = 0;
  int i=0;
  for (; i < res; i++)
  {
    sum += counts[i];
    if (sum >= clip_area)
      break;
  }

  return vals[i];
}
//----------------------------------------------------------------------
//: Finds the lower bound value which eliminates a given fraction of histogram area.
//
float vifa_histogram::HighClipVal(float clip_fraction)
{
  if (clip_fraction < 0)
    clip_fraction = 0.0f;
  if (clip_fraction > 1.0f)
    clip_fraction = 1.0f;

  float area = this->ComputeArea();
  if (area==0.0f)
    return this->GetMaxVal();

  if (clip_fraction == 0.0f)
    return this->GetMaxVal();
  if (clip_fraction == 1.0f)
    return this->GetMinVal();

  float clip_area = area * clip_fraction;
  float* counts = this->GetCounts();
  float* vals = this->GetVals();
  int res = this->GetRes();
  float sum = 0;
  int i = (res - 1);
  for (; i >= 0; i--)
  {
    sum += counts[i];
    if (sum >= clip_area)
      break;
  }

  return vals[i];
}
//--------------------------------------------------------------------------
//: Prints histogram counts onto vcl_cout
void vifa_histogram::Print()
{
  float* vals = this->GetVals();
  float* counts = this->GetCounts();
  int res = this->GetRes();
  int width = 0;
  for (int j = 0; j < res; j++)
  {
    if (width >= 5)
    {
      width = 0;
      vcl_cout << vcl_endl;
    }
    vcl_printf("%6.1f %5.0f |", vals[j], counts[j]);
    width++;
  }

  vcl_cout << vcl_endl << " MaxVal " << this->GetMaxVal() << vcl_endl
           << " MinVal " << this->GetMinVal() << vcl_endl
           << " BucketSize " << this->GetBucketSize() << vcl_endl
           << " Resolution " << this->GetRes() << vcl_endl
           << " Area " << this->ComputeArea(this->GetMinVal(),this->GetMaxVal()) << vcl_endl
           << "------------------------------------------------\n\n";
}
//---------------------------------------------------------------------------
//: dumps histogram  values  to file.

void vifa_histogram::Dump(char *dumpfile)
{
  vcl_ofstream dumpfp(dumpfile, vcl_ios::out);

  if (!dumpfp)
  {
    vcl_cerr << "Error opening histogram data file.\n";
    return;
  }

  for (int i = 0; i < num; i++)
    dumpfp << vals[i] << ' ' << counts[i] << vcl_endl;
}

//---------------------------------------------------------------------------
//: Writes histogram in format suitable for plotting tools like Gnuplot.
int vifa_histogram::WritePlot(const char *fname)
{
  vcl_ofstream fp(fname, vcl_ios::out); // open the file...

  if (!fp)
  {
    vcl_cerr << "Error opening histogram plot file.\n";
    return 0;
  }

  int stat_res = this->GetRes();

  float * x = new float[2*stat_res];
  float * y = new float[2*stat_res];

  float * temp_x = this->GetVals();
  float * temp_y = this->GetCounts();
  float     delt = this->GetBucketSize();

  for (register int i=0; i < stat_res ;i++)
  {
    x[2*i] = temp_x[i] - 0.5f * delt;
    x[2*i+1] = temp_x[i] + 0.5f * delt;
    y[2*i] = temp_y[i];
    y[2*i+1] = temp_y[i];
  }

  for (register int j = 0; j < 2*stat_res; j++)
    fp << x[j] << delimiter << y[j] << vcl_endl;

  delete [] x;
  delete [] y;
  fp.close();

  return 1;
}
//---------------------------------------------------------------------------
//: Compare 'this' histogram to another (passed in).
//    Taken from the old TargetJr class HistEntropy.
//
float vifa_histogram::CompareToHistogram(vifa_histogram* h)
{
  // h1 = 'this'
  float m1 = this->GetMean();
  float m2 = h->GetMean();

  float v1 = this->GetStandardDev();
  float v2 = h->GetStandardDev();

  //We don't like singular situations
  if ( vcl_fabs(v1) < 1e-6 || vcl_fabs(v2) < 1e-6 ) return 0.0f;
  if (m1==0||m2==0) return 0.0f; //means exactly 0 indicate singular histogram

  // scale factor ln(2)/2 = 0.347 means M = 2 at exp = 0.5
  return (float)vcl_exp(- vcl_fabs( 0.693 * (m1 - m2) * vcl_sqrt(1.0/(v1*v1) + 1.0/(v2*v2))));
}
