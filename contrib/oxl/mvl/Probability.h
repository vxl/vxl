#ifndef Probability_h_
#define Probability_h_

#include <vcl_vector.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>
#include <mvl/PairMatchSetCorner.h>

vcl_vector<int> Monte_Carlo(vcl_vector<HomgPoint2D> points, vcl_vector<int> index, int buckets, int samples);
HomgPoint2D Taubins_MLE(HomgPoint2D x1, HomgPoint2D x2, FMatrix *F);
double Sampsons_MLE(HomgPoint2D x1, HomgPoint2D x2, FMatrix *F);

#endif
