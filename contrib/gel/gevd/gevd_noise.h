// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifndef _gevd_noise_h_
#define _gevd_noise_h_

// .NAME gevd_noise - Estimation of noise from histogram of weak responses
// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/gevd_noise.h
// .FILE Detection/gevd_noise.h
// .FILE Detection/gevd_noise.C

// .SECTION Description
// Fit a Raleigh distribution to the histogram curve of all 
// edgels with low magnitudes, h(x), to estimate the sensor noise, 
// as would be zero-crossing of dh(x), and texture noise as the dominant 
// peak in h(x). 
// Setting the threshold at 3 times the sensor/texture noise response 
// found would eliminate 99% of all noisy edges. 
// Knowing the filter, the standard deviation of noise in the original
// image can be deduced from the above response values.
// Assume no intensity truncation, if not, the sensor/texture noise has
// been already artificially removed from the image, and so can not
// be estimated.

// Input: edgels in a typical ROI.
// Ouput: histogram of low response edgels, and 
//        estimate of sensor/texture responses.
// Complexity: O(|data|) time and space.

// .SECTION Example

// .SECTION Authors
//  Harry Voorhees  (1987) SM Thesis
//  Van-Duc Nguyen  (1989) CLOS implementation
//  Van-Duc Nguyen  (1996) C++ implementation

class gevd_bufferxy;

class gevd_noise
{
public:
  friend class DetectionUI;
  gevd_noise(const float* data, const int n, // data in a typical region
	const int number_of_bins=200);	// granularity of histogram
  ~gevd_noise();	  

  static float* EdgelsInCenteredROI(const gevd_bufferxy& magnitude,
				    const gevd_bufferxy& dirx, 
				    const gevd_bufferxy& diry,
				    int& nedgel,
				    const int roiArea=250*250); // ROI
  bool EstimateSensorTexture(float& sensor, // sensor noise is would-be zc
			     float& texture) const; // texture is real zc

  const float* Histogram(int& n) const { n = nbin; return hist; }
  float BinSize() const { return binsize; } // size of bin in data unit

protected:
  float* hist;			// histogram curve of low responses only
  int nbin;			// number of bins
  float binsize;		// size of bin in data unit

protected:
  static bool WouldBeZeroCrossing(const float* dhist, const int nbin, 
				  float& index);
  static bool RealZeroCrossing(const float* dhist, const int nbin, 
			       float& index);
};

#endif
