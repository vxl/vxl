//:
// \file
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_algs.h>
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
  vbl_bounding_box<double, 2> b = bsol_algs::bounding_box(lines);
  double dx = b.xmax()-b.xmin();
  double dy = b.ymax()-b.ymin();
  double max_distance = dx;
  if (dy>dx)
    max_distance = dy;
  bin_counts_.resize(nbins, 0.0);
  bin_values_.resize(nbins, 0.0);
  weights_.resize(nbins, 0.0);
  delta_ = max_distance/nbins;
  int Nlines = lines.size();
  vcl_vector<vgl_homg_line_2d<double> > hlines;
  for (int i = 0; i<Nlines; i++)
    {
      hlines.push_back(lines[i]->vgl_hline_2d());
      hlines[i].normalize();
    }
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
//: There will typically be a large distance peak at small distances
//  The second distance peak will correspond to periodic line segments
//
double bsol_distance_histogram::
second_distance_peak(double min_peak_height_ratio)
{
  int nbins = bin_counts_.size();
  //Peak search states
  int init = 0, start = 1, down =2 , up=3, finish= 4, fail=5;
  //Initial state assignment
  int state=init;
  int i=0,upi=0;
  double v=0;
  double tr=0;
  for (i = 0; i<nbins&&state!=fail&&state!=finish; i++)
    {
//       vcl_cout << "State[" << state << "], D = " << bin_values_[i]
//                << " C = "<< bin_counts_[i] << " tr = " << tr 
//                << " v = "<< v << "\n";
      //Begin the scan look for a value above 0
      if (state==init)
        if (bin_counts_[i]>0)
          {
            v = bin_counts_[i];
            state = start;
            continue;
          }
      //If we are in the start state set the threshold and move down.
      if(state==start)
        //        if(bin_counts_[i]>0)
          if(bin_counts_[i]<=v)
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
                continue;
              }

      // we are in the down state looking for a value above the
      // ratio threshold and is above the current v value.
      if (state==down)
        if (bin_counts_[i]>tr)
          {
            if (bin_counts_[i]<=v)
              state = down;
            else
              {
                state = up;
                //upi is the location of the up state
                upi = i;
              }
            v = bin_counts_[i];
            continue;
          }
      // we are moving up, waiting to capture a peak above the threshold
      if(state==up)
        //        if(bin_counts_[i]>tr)
          {
            if (bin_counts_[i]<=v)
              state = finish;
            else
              {
                upi = i;
                state = up;
              }
            v = bin_counts_[i];
            continue;
          }
    }
  if (state==up||state==finish)
    return bin_values_[upi];
  return -1;
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
      vcl_cout << val << "|";
      double cnt = cnts[i];
      int c = int(cnt*nchars/max_cnt);
      for (int k = 0; k<c; k++)
        vcl_cout << "*";
      vcl_cout << "\n";
    }
  return os;
}
