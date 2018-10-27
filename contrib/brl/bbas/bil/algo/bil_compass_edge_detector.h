// This is brl/bbas/bil/algo/bil_compass_edge_detector.h
#ifndef bil_compass_edge_detector_h
#define bil_compass_edge_detector_h
//:
// \file
// \brief An implementation of the Compass Edge detector By Ruzon and Tomasi, PAMI 01
// For Grayscale edge detection
// \author Amir Tamrakar
// \date November 7, 2009
//
// \verbatim
//  Modifications
//   Vishal Jain moved the code from lemsvxl to vxl
// \endverbatim

#include <vil/vil_image_view.h>

#define NBINS 8
#define GAMMA 14.0

//: Function to find edges using a compass operator
vil_image_view<float> bil_detect_compass_edges(vil_image_view<vxl_byte>& image,
                              int n_wedges,double sigma,
                              double threshold,vil_image_view<float>& hist_grad);

//supporting functions
inline double CArea(double Xhigh, double Xlow, double Y, double r);

double* MakeQtrMask(double r, int n_wedges);

//: compute the optimal orientation and edge strength from the vector of edge strengths computed at discrete orientations
void compute_strength_and_orient(const double* dist, int n_orient, float& strength, double& orientation);

typedef struct bin_struct
{
  double wsum;
  double weight;
  double value;
} bil_bin;

//forward declaration
class bil_signature;

//------------------------------------------------------------------------------------------
// Grayscale histogram distance functions
double bil_gray_EMD(const bil_bin dirt[], const bil_bin hole[]);
double bil_chi_sq_dist(const bil_bin hist1[], const bil_bin hist2[]);
double bil_bhat_dist(const bil_bin hist1[], const bil_bin hist2[]);


//:
// This code implements the basic Binary Splitting algorithm described in
// the 1991 IEEE Trans. on Sig. Proc. article "Color Quantization of Images"
// by Michael Orchard and Charles Bouman, pp. 2677-90.
//
// The input is a 1-D array of data points.  The #define'd constant CDIM
// holds the number of dimensions.  The output is a set of cluster centers
// and is also a 1-D array.  The number of clusters is also returned.
void bs(float *, int, int, int *, float **, int *);

//: This class represents an intensity distribution as a signature (an adaptive histogram)
//  The bin centers of this adaptive histogram are dynamically determined.
//  The weight parameter stores mass of that bin.
class bil_signature
{
 public:
  bil_bin bins[NBINS]; //fixed number of bins for now (this is not strictly required)

  bil_signature()
  {
    for (auto & bin : bins){
      bin.value=0;
      bin.weight=0;
      bin.wsum=0;
    }
  }

  ~bil_signature() = default;

  //: The EMD dist is the default distance between two signatures
  double operator-(const bil_signature& sig) const
  {
    //return bil_chi_sq_dist(bins, sig.bins);
    return bil_gray_EMD(bins, sig.bins);
  }

  //: This operator sums two signatures
  void operator +=(bil_signature const& sig)
  {
    for (int i=0; i<NBINS; i++){
      bins[i].weight += sig.bins[i].weight;
      bins[i].weight /= 2.0;
      bins[i].wsum += sig.bins[i].wsum;
      //renormalize
      bins[i].value = sig.bins[i].wsum/bins[i].weight;
    }
  }
};


#endif // bil_compass_edge_detector_h
