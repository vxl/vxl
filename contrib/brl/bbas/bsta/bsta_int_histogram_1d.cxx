// This is bsta/bsta_int_histogram_1d.cxx
// see bsta/bsta_int_histogram_1d.h for description of class

//:
// \file

#include <bsta/bsta_int_histogram_1d.h>
// for gausian parzan window filter
#include <bsta/bsta_gauss.h>

// constructor
  bsta_int_histogram_1d::bsta_int_histogram_1d(unsigned int bins)		// const??
  {
    nbins_ = bins;
    counts_.resize(nbins_, 0);      	// create the array for histogram nbins wide
  }

// destructor
  bsta_int_histogram_1d::~bsta_int_histogram_1d() {}

// -----------------------------------------------------------

// get min and max non-zero bins in histogram 
  unsigned int bsta_int_histogram_1d::get_min_bin()
  {
    unsigned int i = 0;
    while (i<(nbins_-1) && !counts_[i])   	 // search for the first non-zero bin
      i++;
    if (i >= nbins_)							 // this shouldn't happen unless hist is empty
      return 0;								 //   (the same answer as if bin[0] has counts_)
    return i;
  }

  unsigned int bsta_int_histogram_1d::get_max_bin() 
  {
    unsigned int i = nbins_-1;
    while (i>0 && !counts_[i])    			// search for the first non-zero bin		
      i--;
    if (i < 0)								// this shouldn't happen unless hist is empty
      return 0;
    return i;
  }

// get total counts_ in entire histogram 
  unsigned long int bsta_int_histogram_1d::get_area()
  {
    long int area = 0;
    for (unsigned int i=0; i<nbins_; i++)
			{area = area + counts_[i];}
    return area;
  }
 
  //: get the count in a given bin
  long int bsta_int_histogram_1d::get_count(unsigned int bin)						// const???
  {
	if(bin < nbins_) 
	  return counts_[bin]; 
	else 
	  return 0;
  }
	
  //: set the count for a given bin
  void bsta_int_histogram_1d::set_count(unsigned int bin, long int val)			// const???
    {if(bin>=0 && bin<nbins_) counts_[bin] = val;}
    
// get highest bin value in histogram; returns max value; index of max is available in imax
  unsigned long int bsta_int_histogram_1d::get_max_val(unsigned int &imax)
  {
    register long int max = 0;
    for (unsigned int i=0; i<nbins_; i++)
	if( counts_[i] > max ) {
	  max = counts_[i];
	  imax = i;
	}
  return max;
  }
  
// trim off a fraction of the histogram at top and bottom ends.  Note that fractions
//    should be less than 0.5, usually considerably less.
  void bsta_int_histogram_1d::trim(float low_fract, float high_fract)
  {
    long int total = this->get_area();		// total counts_ in histogram
	long int low_count = static_cast<long int>(((float)total * low_fract) + 0.5);	  		// # of counts_ to trim
	long int high_count = static_cast<long int>(((float)total * high_fract) + 0.5);	  

	// trim low end
	unsigned int i = 0;
	while(i<nbins_ && low_count > 0) {
	  if(counts_[i] > 0) {
		if(low_count >= counts_[i]) {
			low_count = low_count - counts_[i];
			counts_[i] = 0;
			i++;
		}
		else {		
			counts_[i] = counts_[i] - low_count;
			low_count = 0;
			i++;
		}
	  }				
    }

	// trim high end
	i = nbins_ - 1;
	while(i>=0 && high_count > 0) {
	  if(counts_[i] > 0) {
		if(high_count >= counts_[i]) {
			high_count = high_count - counts_[i];
			counts_[i] = 0;
			i--;
		}
		else {		
			counts_[i] = counts_[i] - high_count;
			high_count = 0;
			i--;
		}
	  }				
    }
    return;
  }

// Zero out bottom n bins.  This is sometimes necessary if image has a black area due
//   to a sensor's failed detectors.
  void bsta_int_histogram_1d::zero_low(unsigned int n)
  {
    for(unsigned int i=0; i<n; i++) counts_[i] = 0;
  }
  
// Zero out top n bins.  This is sometimes necessary if the sensor doesn't handle too
//    bright pixels gracefully (like from a sunlight glint off a shiney surface).
  void bsta_int_histogram_1d::zero_high(unsigned int n)
  {
    for(unsigned int i=nbins_-1; i>nbins_-n; i--) counts_[i] = 0;
  }
  
// smooth the histogram with a 1D parzan window (which is a gaussian filter)
  void bsta_int_histogram_1d::parzen(const float sigma)
  {
    if (sigma<=0)
      return;
    double sd = (double)sigma;
    vcl_vector<double> in(nbins_), out(nbins_);
    for (unsigned int i=0; i<nbins_; i++)
      in[i] = counts_[i];
    bsta_gauss::bsta_1d_gaussian(sd, in, out);
    for (unsigned int i=0; i<nbins_; i++)
      counts_[i] = static_cast<long int>(out[i] +0.5);	// as we are going back to a long int, round off
  }

  // Find the "significant" peaks & vallwys in a histogram.  Here "significant" means there is
  //   a specified difference in height between the peak and the previous valley, or vice versa.
  bool bsta_int_histogram_1d::find_peaks( float perct, int &n_peaks, vcl_vector<unsigned int> &peaks)
  {
	  bool success = false;

	  unsigned int peak_index = 0;
	  long int ymax = get_max_val(peak_index);			// get highest peak & index
	  long int hysteresis = static_cast<long int>(ymax*perct);	// thresh for setting peaks
	  n_peaks = 0;

	  // The assumption is that 1st "peak" is a valley.  If it isn't, MAJOR ERROR!!
	  if (counts_[0] > 0) 
		  vcl_cerr << "Warning, counts_[0] in diagonal hist != 0, Major Error!!\n";
	  if (counts_[0] > hysteresis) 
		  return success;					// if > hysteresis, return failure

	  // Set up some indices for algorithm
	  bool direction = true;				// true = look for peak, false = look for valley
	  bool past_p_thresh = false;			// is delta from last valley large enough for new peak?
	  bool past_v_thresh = false;			// is delta from last peak large enough for new valley?
	  long int last_p_val = 0;				// height of last peak
	  unsigned int last_p_index = 0;		// index of last peak
	  long int last_v_val = 0;				// height of last valley
	  unsigned int last_v_index = 0;		// index of last valley

	  // At start, we assume that 1st "peak" is a valley stored in peaks[0] and the next
	  //   is a peak stored in peaks[1].  So we start at index 0 and find the first bucket
	  //   where the hist is non-zero and set the 1st valley at the last zero bucket.
	  unsigned int j = 0;					// index of peaks[] array
	  unsigned int istart = 0;
	  for (unsigned int i=1; i<nbins_; i++)		// start at i=1, not i=0
	  {
		  if (counts_[i] > 0)
		  {
			  peaks[j] = i-1;				// last zero bucket
			  n_peaks = 1;
			  last_v_index = i-1;
			  istart = i;
			  j = 1;
			  break;						// when we find it exit for loop
		  }
	  }

	  // Step through remaining histogram buckets
	  for (unsigned int i=istart; i<nbins_; i++)
	  {
		  long int delta = counts_[i] - counts_[i-1];
		  if (delta > 0) direction = true;
		  if (delta < 0) direction = false;
		  // delta = 0, leave direction the same

		  if (direction == true)			// we are looking for next peak
		  {
			if (counts_[i] - last_v_val > hysteresis)
			{
			  // Only add new valley if past tentative valley has not yet been added
			  //   to peaks[] array and AND peak hysteresis has been exceeded.  j=1
			  //   will skip 1st valley because it is already set.
			  if (j>1 && last_v_index > peaks[j-1])
			  {
			    peaks[j] = last_v_index;	// here j is index to "next" p/v
				n_peaks++;
				j++;						// now j is index to p/v after this one

				// This is tricky.  Set new value of last_p_val to this valley so it
				//   will track up.
				last_p_val = last_v_val;
			  }
			  // Found new tentative peak; this also moves peak along if it's increasing.  
			  //   Only do this if peak height > past peak height (last_p_val).
			  if (counts_[i] > last_p_val)
			  {
			    last_p_val = counts_[i];	// found new tentative peak, set tentative max
				last_p_index = i;			//   also set tentative index
			  }
			}
		  }
		  else								// we are looking for next valley
		  {
		    // Is delta from last peak large enough to have a new valley?
		    if (last_p_val - counts_[i] > hysteresis)
			{
			  // Only add new peak if past tentative peak has not yet been added to peaks[]
			  //   array and valley delta has been exceeded
			  if (last_p_index > peaks[j-1])
			  {
			    peaks[j] = last_p_index;
				n_peaks++;
				j++;
				//  The tricky part again
				last_v_val = last_p_val;
			  }
			  // Finding new tentative valley; this also moves tentative valley along when hist
			  //   is decreasing.
			  if (counts_[i] < last_v_val)
			  {
			    last_v_val = counts_[i];
				last_v_index = i;
			  }
			}
		  }
	  }

	  // When we get through the histogram, set the last valley if not already set
	  if (last_v_index > peaks[j-1])
	  {
		  peaks[j] = last_v_index;
		  n_peaks++;
	  }
	  // If we get to this point, we have succeeded
	  success = true;
	  return success;
  }

