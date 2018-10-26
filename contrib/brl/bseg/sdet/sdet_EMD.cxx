// This is brcv/seg/sdet/algo/sdet_EMD.cxx

//:
//\file

#include "sdet_EMD.h"

// ---------------------------------------------------------------------------------//
//             GRAYSCALE DISTANCE FUNCTIONS
// ---------------------------------------------------------------------------------//

//: Compute Chi^2 distance between two histograms
double sdet_chi_sq_dist(const sdet_bin hist1[], const sdet_bin hist2[])
{
  double dist = 0.0;
  for (int i=0; i<NBINS; i++)
    dist += (hist1[i].weight - hist2[i].weight)*(hist1[i].weight - hist2[i].weight)/(hist1[i].weight + hist2[i].weight + 2e-16);

  return dist;
}

//: Compute Bhattacharya distance between two histograms
double sdet_bhat_dist(const sdet_bin hist1[], const sdet_bin hist2[])
{
  double dist = 0.0;
  for (int i=0; i<NBINS; i++)
    dist += hist1[i].weight*hist2[i].weight;

  return dist;
}

//: Compute EMD between two signatures (signature 1=dirt, signature 2 = hole)
//
// This implementation of the Earth Mover's Distance (EMD) is for one-
// dimensional datasets where the distance between clusters can be easily
// computed on the fly.
//
// implemented from the pseudocode in Ruzon's Thesis
double sdet_gray_EMD(const sdet_bin dirt[], const sdet_bin hole[])
{
  int i = -1, j = -1;
  double leftoverdirt = 0.0, leftoverhole = 0.0, work = 0.0;
  double dirt_amt, hole_amt;

  while (true) {

    // Compute the amount of mass in the lowest numbered bin that hasn't
    // been moved yet from the piles of dirt
    if (leftoverdirt == 0.0)
    {
      //advance i to the next non-empty interval
      i++;
      while(dirt[i].weight == 0.0 && i < NBINS) i++;

      //if no more intervals
      if (i == NBINS)
        return (work); //we're done
      else
        dirt_amt = dirt[i].weight;
    }
    else //use the amount that was left over from the last move
      dirt_amt = leftoverdirt;

    // Do the same for the holes
    if (leftoverhole == 0.0)
    {
      //advance j to the next non-empty interval
      j++;
      while(hole[j].weight == 0.0 && j < NBINS) j++;

      //if no more intervals
      if (j == NBINS)
        return (work); //we're done
      else
        hole_amt = hole[j].weight;
    }
    else //use the amount that was left over from the last move
      hole_amt = leftoverhole;

    // Compute the work done moving the smaller amount of mass and decide
    // how much is left over in each bin.
    double massmoved = std::min(dirt_amt, hole_amt);

    work += massmoved * (1 - std::exp(-std::fabs(hole[j].value - dirt[i].value) / GAMMA));

    leftoverdirt = dirt_amt - massmoved;
    leftoverhole = hole_amt - massmoved;
  }
}

// ---------------------------------------------------------------------------------//
//             COLOR SIGNATURE DISTANCE FUNCTIONS
// ---------------------------------------------------------------------------------//

//: Compute Chi^2 distance between two histograms
double sdet_color_chi_sq_dist(const sdet_color_sig & /*sig1*/, const sdet_color_sig & /*sig2*/)
{
  double dist = 0.0;
  //for (int i=0; i<NBINS; i++)
  //  dist += (hist1[i].weight - hist2[i].weight)*(hist1[i].weight - hist2[i].weight)/(hist1[i].weight + hist2[i].weight + 2e-16);

  return dist;
}

//: Compute Bhattacharya distance between two histograms
double sdet_color_bhat_dist(const sdet_color_sig & /*sig1*/, const sdet_color_sig & /*sig2*/)
{
  double dist = 0.0;
  //for (int i=0; i<NBINS; i++)
  //  dist += hist1[i].weight*hist2[i].weight;

  return dist;
}

double sdet_color_EMD(sdet_color_sig * /*sig1*/, sdet_color_sig * /*sig2*/, sdet_flow*  /*flow*/, int*  /*flowsize*/)
{
  //This seems to be hard

  return 0;
}
