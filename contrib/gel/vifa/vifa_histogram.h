// This is vifa/vifa_histogram.h
#ifndef VIFA_HISTOGRAM_H
#define VIFA_HISTOGRAM_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief General Purpose Histogram
//
//  This histogram is mainly a port from the old histogram class in TargetJr
//  which resided in GeneralUtility/Basics.  In this initial version, not all
//  the functionality is ported but it is intended to be useful.
//  It is placed in vsrl right now because it is not determined yet where it
//  should permanently live.
//
// \author Glen W. Brooksby
// \date   14 April, 2003
//
// \verbatim
//  Modifications
//   2003/04/14 Initial Version
//   2003/06/02 MPP Moved to vifa, since it's the only user for now...
//
// \endverbatim
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vul/vul_timestamp.h>
#include <vnl/vnl_vector.h>

class vifa_histogram : public vul_timestamp,
					   public vbl_ref_count
{
  enum histogram_type{HISTOGRAM=0, CONTRAST_HIST, NUM_TYPES};
protected:
  char* delimiter;  // text delimeter for writing out histograms.
  float* vals;      // histogram of x array (value = midpoint of the buckets.
  float* counts;    // histogram of y array; counts[i] is the # of pixels
                    // with value within range of bucket i.
  int num;          // Number of indicies.
  float delta;      // Width of the bins
  float vmax, vmin; // Max & Min plots on the histogram.
  float mean;       // Mean value of the distribution
  float standard_dev;      // Standard Deviation of the distribution

  virtual int GetIndex(float i);

public:
  // Constructors
  vifa_histogram();
  vifa_histogram(int, float, float);
  vifa_histogram(float*, float*, int);
  vifa_histogram(const vifa_histogram& h); // copy X-tor
  // Resampling Constructor
  vifa_histogram(vifa_histogram*, float width, bool preserveCounts=false);
  // Destructor
  virtual ~vifa_histogram();

  // Formative function(s)
  vifa_histogram* Scale(float scale_factor);
  vifa_histogram* Cumulative();
  vifa_histogram* NonMaximumSupress(int radius = 1, bool cyclic = false);

  // Misc. Methods
  void RemoveFlatPeaks(int nbins, float* cnts, bool cyclic);
  float CompareToHistogram(vifa_histogram* h);

  // Attribute accessors
  void  UpCount(float newval);
  void  UpCount(float newval,bool useNewIndexMethod);
  int   GetNumSamples();
  float GetCount(float uval);
  float SetCount(float pixelval, float count);

  float GetMinVal();
  float GetMaxVal();
  float GetMaxCount();
  float GetMean();
  float GetStandardDev();
  float GetMedian();
  int GetValIndex(float val);
  
  float* GetVals()
  {
    stats_consistent = 0; // Values might change.
    return vals; 
  }

  float* GetCounts()
  { 
    stats_consistent = 0; // Counts might change.
    return counts; 
  }
  
  int GetRes()
  { return num; }

  float GetBucketSize() { return delta; }
  
  float* GetMinValAddr()
  { return vals+GetIndex(GetMinVal());  }
  
  float* GetMinCountAddr()
  { return counts+GetIndex(GetMinVal());  }
  
  // Other usefule functions
  char* GetDelimiter() {return delimiter;}
  void SetDelimiter(char* d) {delimiter = d;}
  float ComputeArea(float low, float high);// bounded area
  float ComputeArea();//total area
  
  //Find bounds that clip off a given percent of the area
  float LowClipVal(float clip_fraction);
  float HighClipVal(float clip_fraction);
  
  void Print();
  void Dump(char *);
  int  WritePlot(const char* fname);

private:
  int stats_consistent;  // A 2 bit state flag  Mean = 1 | StandDev = 2
};

typedef vbl_smart_ptr<vifa_histogram>	vifa_histogram_sptr;


#endif	// VIFA_HISTOGRAM_H
