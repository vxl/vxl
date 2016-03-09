#ifndef Probability_h_
#define Probability_h_

#include <vcl_compiler.h>
#include <iostream>
#include <vector>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>
#include <mvl/PairMatchSetCorner.h>

std::vector<int> Monte_Carlo(std::vector<HomgPoint2D> points, std::vector<int> index, int buckets, int samples);
HomgPoint2D Taubins_MLE(HomgPoint2D x1, HomgPoint2D x2, FMatrix *F);
double Sampsons_MLE(HomgPoint2D x1, HomgPoint2D x2, FMatrix *F);

#endif
