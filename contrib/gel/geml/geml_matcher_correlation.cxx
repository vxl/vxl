// This is gel/geml/geml_matcher_correlation.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Geoffrey Cross, Oxford RRG
// Created: ${DATE}
// .SECTION Modifications:
//   @(eval (strftime "%y%m%d")) Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>

#include <vil1/vil1_memory_image_window.h>

#include "geml_matcher_correlation.h"


#define MAX_CORNER_ERROR 1
#define CORRELATION_KERNEL 3
#define NORMALISED_CORRELATION false
#define SEARCH_WINDOW_X 50
#define SEARCH_WINDOW_Y 50
#define NO_SCORE -2

geml_matcher_correlation::geml_matcher_correlation( const vil1_memory_image_of<vxl_byte> image1,
                                                    const vil1_memory_image_of<vxl_byte> image2,
                                                    const vcl_vector< vcl_pair<float,float> > &corners1,
                                                    const vcl_vector< vcl_pair<float,float> > &corners2)
  : geml_matcher( image1, image2, corners1, corners2)
{
}


vcl_vector< vcl_pair<int,int> > geml_matcher_correlation::get_matches()
{
  // correlate each corner against each corner
  vnl_matrix<double> scores1to2(corners1_.size(),corners2_.size());
  vnl_matrix<double> scores2to1(corners2_.size(),corners1_.size());

  for (unsigned int i=0; i< corners1_.size(); ++i)
    {
      double x1= corners1_[i].first;
      double y1= corners1_[i].second;

      for (unsigned int j=0; j< corners2_.size(); ++j)
        {
          double x2= corners2_[j].first;
          double y2= corners2_[j].second;

          if (vnl_math_abs(x1-x2) < SEARCH_WINDOW_X && vnl_math_abs(y1-y2) < SEARCH_WINDOW_Y)
            {
              vcl_pair<double,double> scores= best_local_correlation_score( i, j);

              scores1to2.put( i, j, scores.first);
              scores2to1.put( j, i, scores.second);
            }
          else
            {
              scores1to2.put( i, j, NO_SCORE);
              scores2to1.put( j, i, NO_SCORE);
            }
        }
    }

  // look for best match to first image corners
  vcl_vector<int> bestimage1match( corners1_.size());
  vcl_vector<double> bestimage1score( corners1_.size());

  for (unsigned int i=0; i< corners1_.size(); ++i)
    {
      double bestscore= NO_SCORE;
      int bestmatch= -1;

      for (unsigned int j=0; j< corners2_.size(); ++j)
        {
          if (bestscore== NO_SCORE)
            {
              bestscore= scores1to2( i, j);
              bestmatch= j;
            }
          else if (bestscore>scores1to2(i,j) && scores1to2(i,j)!=NO_SCORE)
            {
              bestscore= scores1to2( i, j);
              bestmatch= j;
            }
        }

      bestimage1match[i]= bestmatch;
      bestimage1score[i]= bestscore;
    }


  // look for best match to second image corners
  vcl_vector<int> bestimage2match( corners2_.size());
  vcl_vector<double> bestimage2score( corners2_.size());

  for (unsigned int i=0; i< corners2_.size(); ++i)
    {
      double bestscore= NO_SCORE;
      int bestmatch= -1;

      for (unsigned int j=0; j< corners1_.size(); ++j)
        {
          if (bestscore== NO_SCORE)
            {
              bestscore= scores2to1( i, j);
              bestmatch= j;
            }
          else if (bestscore>scores2to1(i,j) && scores2to1(i,j)!=NO_SCORE)
            {
              bestscore= scores2to1( i, j);
              bestmatch= j;
            }
        }

      bestimage2match[i]= bestmatch;
      bestimage2score[i]= bestscore;
    }

  // and check that the best match from image 1 to 2 is the
  //  same as the best match from image 2 to 1

  vcl_vector< vcl_pair<int,int> > l;

  for (unsigned int i=0; i< corners1_.size(); i++)
    {
      int a= bestimage1match[i];
      int b= bestimage2match[a];

      if ((int)i==b)
        {
          vcl_cerr << i << ' ' << a << vcl_endl;
          vcl_cout << corners1_[i].first << ' ' << corners1_[i].second << ' '
                   << corners2_[a].first << ' ' << corners2_[a].second << vcl_endl;
          l.push_back( vcl_pair<int,int>(i,a) );
        }
    }

  //  vcl_cerr << bestimage1match << vcl_endl;
  //  vcl_cerr << bestimage2match << vcl_endl;

  return l;
}


// search in a small window (about 3x3) for the best correlation between a pair of corners
vcl_pair<double,double> geml_matcher_correlation::best_local_correlation_score( const int c1, const int c2)
{
  double x1= corners1_[c1].first;
  double y1= corners1_[c1].second;
  double x2= corners2_[c2].first;
  double y2= corners2_[c2].second;
  double bestscore1= -1;
  double bestscore2= -1;

  vil1_memory_image_window w1( im1_, int(x1), int(y1), CORRELATION_KERNEL);
  vil1_memory_image_window w2( im2_, int(x2), int(y2), CORRELATION_KERNEL);

  for (int x= -MAX_CORNER_ERROR; x<= MAX_CORNER_ERROR; x++)
    {
      for (int y= -MAX_CORNER_ERROR; y<= MAX_CORNER_ERROR; y++)
        {
          double score1, score2;

      //  score1= w1.sum_squared_differences(im2_, x2+ x, y2+ y);
      //  score2= w2.sum_squared_differences(im1_, x1+ x, y1+ y);
          score1= w1.normalised_cross_correlation(im2_, int(x2+ x), int(y2+ y));
          score2= w2.normalised_cross_correlation(im1_, int(x1+ x), int(y1+ y));

          if (score1<bestscore1 || bestscore1<0)
            bestscore1= score1;

          if (score2<bestscore2 || bestscore2<0)
            bestscore2= score2;
        }
    }

  return vcl_pair<double,double>(bestscore1,bestscore2);
}
