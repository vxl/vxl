// This is brcv/seg/sdet/algo/sdet_EMD.h
#ifndef sdet_EMD_h
#define sdet_EMD_h
//:
//\file
//\brief An implementation of the EMD algorithm to compute distance between two signatures
//\author Amir Tamrakar
//\date 08/26/07
//
//\verbatim
//  Modifications
//\endverbatim

#include <cmath>
#include <iostream>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//#include <sdet/sdet_BS.h>

//for grayscale
#define NBINS 8
#define GAMMA 14.0
#define MAXCLUSTERS 30

typedef struct /*bin_struct*/{
  double wsum;
  double weight;
  double value;
} sdet_bin;

//for color
typedef struct
{
  int from;             /* Feature number in signature 1 */
  int to;               /* Feature number in signature 2 */
  double amount;        /* Amount of flow from "from" to "to" */
} sdet_flow;

//forward declaration
class sdet_signature;
class sdet_color_sig;

//------------------------------------------------------------------------------------------
// Grayscale histogram distance functions
double sdet_gray_EMD(const sdet_bin dirt[], const sdet_bin hole[]);
double sdet_chi_sq_dist(const sdet_bin hist1[], const sdet_bin hist2[]);
double sdet_bhat_dist(const sdet_bin hist1[], const sdet_bin hist2[]);
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//Color Histogram distance functions
double sdet_color_EMD(sdet_color_sig *sig1, sdet_color_sig *sig2, sdet_flow* flow=nullptr, int* flowsize=nullptr);
double sdet_color_chi_sq_dist(const sdet_color_sig &sig1, const sdet_color_sig &sig2);
double sdet_color_bhat_dist(const sdet_color_sig &sig1, const sdet_color_sig &sig2);
//------------------------------------------------------------------------------------------

//: This code implements the basic Binary Splitting algorithm described in
// the 1991 IEEE Trans. on Sig. Proc. article "Color Quantization of Images"
// by Michael Orchard and Charles Bouman, pp. 2677-90.
//
// The input is a 1-D array of data points.  The #define'd constant CDIM
// holds the number of dimensions.  The output is a set of cluster centers
// and is also a 1-D array.  The number of clusters is also returned.
void bs(float *, int, int, int *, float **, int *);

//: This class represents an intensity distribution as a signature (an adaptive histogram)
//  The bin centers of this adaptive histogram are dynamically determined. The weight parameter stores
//  mass of that bin.
class sdet_signature
{
public:
  sdet_bin bins[NBINS]; //fixed number of bins for now (this is not strictly required)

  sdet_signature()
  {
    for (auto & bin : bins){
      bin.value=0;
      bin.weight=0;
      bin.wsum=0;
    }
  }
  ~sdet_signature()= default;

  //: The EMD dist is the default distance between two signatures
  double operator-(const sdet_signature& sig) const
  {
    //return sdet_chi_sq_dist(bins, sig.bins);
    return sdet_gray_EMD(bins, sig.bins);
  }

  //: This operator sums two signatures
  void operator +=(sdet_signature const& sig)
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

//: This class represents the Lab color distribution as a signature (an adaptive histogram)
//  The bin centers of this adaptive histogram are dynamically determined. The weight parameter stores
//  mass of that bin.
class sdet_color_sig
{
public:
  int n; //number of features
  double Features[MAXCLUSTERS]; //fixed number of features for now (this is not strictly required)
  double Weights[MAXCLUSTERS];

  sdet_color_sig()= default;
  ~sdet_color_sig()= default;

  //: The EMD dist is the default distance between two signatures
  double operator-(const sdet_color_sig& /*sig1*/) const
  {
    //return sdet_chi_sq_dist(bins, sig1.bins);
    //return sdet_color_EMD(*this, sig1, NULL, NULL);
    return 0;
  }

};


#endif // sdet_EMD_h
