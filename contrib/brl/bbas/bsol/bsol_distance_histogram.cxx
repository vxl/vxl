//:
// \file
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_distance_histogram.h>

//: Constructors
bsol_distance_histogram::bsol_distance_histogram()
{
  delta_ = 0;
}

//------------------------------------------------------------------------
//: set up the histogram with bin spacing defined by max_val and nbins.
//------------------------------------------------------------------------
bsol_distance_histogram::bsol_distance_histogram(int nbins, double max_val)
{
  if (!nbins)
  {
    delta_=0;
    return;
  }
  bin_counts_.resize(nbins, 0.0);
  bin_values_.resize(nbins, 0.0);
  weights_.resize(nbins, 0.0);
  delta_ = max_val/nbins;
}

bsol_distance_histogram::
bsol_distance_histogram(const int nbins,
                        vcl_vector<vsol_line_2d_sptr> const& lines)
{
  if (!nbins)
  {
    delta_=0;
    return;
  }
  bin_counts_.resize(nbins, 0.0);
  bin_values_.resize(nbins, 0.0);
  weights_.resize(nbins, 0.0);

  int Nlines = lines.size();
  vcl_vector<vgl_homg_line_2d<double> > hlines;
  double dmin = vnl_numeric_traits<double>::maxval, dmax = -dmin;
  for (int i = 0; i<Nlines; i++)
  {
    hlines.push_back(lines[i]->vgl_hline_2d());
    hlines[i].normalize();
    double d = hlines[i].c();
    dmin = vnl_math_min(dmin, d);
    dmax = vnl_math_max(dmax, d);
  }
  delta_ = (dmax-dmin)/nbins;

  for (int i = 0; i<Nlines; i++)
  {
    double ci = hlines[i].c();
    double length_i = lines[i]->length();
    for (int j = i+1; j<Nlines; j++)
    {
      double cj = hlines[j].c();
      double length = lines[j]->length()+length_i;
      double D = vcl_fabs(ci-cj);
      this->up_count(D, length, length);
    }
  }
  this->normalize_distance();
}

//: Destructor
bsol_distance_histogram::~bsol_distance_histogram()
{
}


//---------------------------------------------------------------------
//: normalized bin distance = (Sum_i length_i*dist_i)/(Sum_i length_i)
//  Note distances can't be negative so the -1 flag indicates there
//  was no count for that bin.
//---------------------------------------------------------------------
void bsol_distance_histogram::normalize_distance()
{
  for (unsigned int k = 0; k<bin_counts_.size(); ++k)
  {
    if (weights_[k])
    {
      double val = bin_values_[k];
      double w = weights_[k];
      bin_values_[k]=val/w;
    }
    else
      bin_values_[k]=-1;
  }
}

void bsol_distance_histogram::up_count(const double value, const double count,
                                       const double weight)
{
  double val = 0;
  bool inserted = false;
  for (unsigned int k = 0; k<bin_counts_.size()&&!inserted; ++k, val+=delta_)
    if (val>=value)
    {
      bin_counts_[k]+=count;
      bin_values_[k]+=value*weight;
      weights_[k]+=weight;
      inserted = true;
    }
}

//---------------------------------------------------------------
//: refine the peak location using parabolic interpolation
//
double bsol_distance_histogram::interpolate_peak(int initial_peak)
{
  //boundary conditions
  if (initial_peak<0)
    return 0;
  if (initial_peak==0)
    return bin_values_[0];
  int n = bin_values_.size();
  if (initial_peak==(n-1))
    return bin_values_[n-1];
  if (initial_peak>=n)
    return 0;
  double fminus = bin_counts_[initial_peak-1];
  double fzero = bin_counts_[initial_peak];
  double fplus = bin_counts_[initial_peak+1];

  double df = 0.5*(fplus-fminus);//first derivative
  double d2f = 0.5*(fplus+fminus-2.0*fzero);//second derivative
  if (vcl_fabs(d2f)<1.0e-8)
    return bin_values_[initial_peak];

  double root = -0.5*df/d2f;

  //interpolate the bin values within the appropriate interval
  double dminus = bin_values_[initial_peak-1];
  double dzero = bin_values_[initial_peak];
  double dplus = bin_values_[initial_peak+1];

  double result;
  if (root<0)
   result = (dzero*(1+root)-dminus*root);
  else
    result =  (dzero*(1-root)+dplus*root);
  //  vcl_cout << "interpolated distance " << result << '\n';
  return result;
}

//---------------------------------------------------------------
//: There will typically be a large distance peak at small distances
//  The second distance peak will correspond to periodic line segments
//
bool bsol_distance_histogram::
distance_peaks(double& peak1, double& peak2, double min_peak_height_ratio)
{
  int nbins = bin_counts_.size();
  //Peak search states
  int init = 0, start = 1, down =2 , up=3, s_peak1= 4, down2 = 5, up2 = 6,
    s_peak2 = 7, fail=8;
  //Initial state assignment
  int state=init;
  int upi1=0, upi2=0;
  double v=0;
  double tr=0, peak1_bin_counts=0;
  double start_distance = 0;
  for (int i = 0; i<nbins&&state!=fail&&state!=s_peak2; i++)
  {
#ifdef DEBUG
    vcl_cout << "State[" << state << "], D = " << bin_values_[i]
             << " C = "<< bin_counts_[i] << " srt_d = " << start_distance
             << " v = "<< v << '\n';
#endif
    //Begin the scan look for a value above 0
    if (state==init)
      if (bin_counts_[i]>0)
      {
        v = bin_counts_[i];
        state = start;
        start_distance = 0;
        continue;
      }
    //If we are in the start state set the threshold and move down.
    if (state==start)
    {
      //        if (bin_counts_[i]>0)
      if (bin_counts_[i]<=v)
      {
        state = down;
        //second peak should be a significant ratio of the starting value
        tr = min_peak_height_ratio*v;
        v=bin_counts_[i];    //population
        continue;
      }
      else
      {
        state = start;
        v=bin_counts_[i];
        start_distance = bin_values_[i];
        continue;
      }
    }

    // we are in the down state looking for a value above the
    // ratio threshold and is above the current v value.
    if (state==down)
    {
      if (bin_counts_[i]>tr)
      {
        if (bin_counts_[i]<=v)
          state = down;
        else
        {
          state = up;
          //upi1 is the location of the up state
          upi1 = i;
        }
        v = bin_counts_[i];
        continue;
      }
    }
    // we are moving up, waiting to capture a peak above the threshold
    if (state==up)
    {
      //        if (bin_counts_[i]>tr)
      if (bin_counts_[i]<=v)
      {
        state = s_peak1;
        peak1_bin_counts = bin_counts_[upi1];
        tr = min_peak_height_ratio*peak1_bin_counts;
      }
      else
      {
        upi1 = i;
        state = up;
      }
      v = bin_counts_[i];
      continue;
    }
    // we have just passed  peak 1
    if (state==s_peak1)
    {
      if (bin_counts_[i]<=peak1_bin_counts)
        state = down2;
      else
        state = fail;
      v = bin_counts_[i];
      continue;
    }
    // we are in the second down state looking for a value above the
    // ratio threshold and is above the current v value.
    if (state==down2)
      if (bin_counts_[i]>tr)
      {
        if (bin_counts_[i]<=v)
          state = down2;
        else
        {
          state = up2;
          //upi2 is the location of the up state
          upi2 = i;
        }
        v = bin_counts_[i];
        continue;
      }

    // we are moving up a second time, waiting to capture a peak
    // above the threshold
    if (state==up2)
    {
      //        if (bin_counts_[i]>tr)
      if (bin_counts_[i]<=v)
      {
        state = s_peak2;
      }
      else
      {
        upi2 = i;
        state = up2;
      }
      v = bin_counts_[i];
      continue;
    }
  }
  if (state==up2||state==s_peak2)
  {
    peak1 = this->interpolate_peak(upi1)-start_distance;
    peak2 = this->interpolate_peak(upi2)-start_distance;
    return true;
  }
  return false;
}

double bsol_distance_histogram::min_val() const
{
  int nbins = bin_values_.size();
  if (!nbins)
    return 0;
  return bin_values_[0];
}

double bsol_distance_histogram::max_val() const
{
  int nbins = bin_values_.size();
  if (!nbins)
    return 0;
  return bin_values_[nbins-1];
}

double bsol_distance_histogram::min_count() const
{
  int nbins = bin_counts_.size();
  if (!nbins)
    return 0;
  double min_cnt = bin_counts_[0];
  for (int i = 1; i<nbins; i++)
    if (bin_counts_[i]<min_cnt)
      min_cnt = bin_counts_[i];
  return min_cnt;
}

double bsol_distance_histogram::max_count() const
{
  int nbins = bin_counts_.size();
  if (!nbins)
    return 0;
  double max_cnt = bin_counts_[0];
  for (int i = 1; i<nbins; i++)
    if (bin_counts_[i]>max_cnt)
      max_cnt = bin_counts_[i];
  return max_cnt;
}

vcl_ostream& operator << (vcl_ostream& os, const bsol_distance_histogram& h)
{
  int nchars = 25;//display resolution
  vcl_cout << "Distance Histogram\n";
  //get the maximum bin value
  double max_cnt = h.max_count();
  if (!max_cnt)
    return os;
  // forced to cast away const
  bsol_distance_histogram & hh = (bsol_distance_histogram&)h;
  vcl_vector<double>& vals = hh.values();
  vcl_vector<double>& cnts = hh.counts();
  for (unsigned int i=0; i<vals.size(); i++)
  {
    double val = vals[i];
    vcl_cout << val << '|';
    double cnt = cnts[i];
    int c = int(cnt*nchars/max_cnt);
    for (int k = 0; k<c; k++)
      vcl_cout << '*';
    vcl_cout << '\n';
  }
  return os;
}
