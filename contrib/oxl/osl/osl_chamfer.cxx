// This is oxl/osl/osl_chamfer.cxx
//:
// \file
// \author fsm

#include "osl_chamfer.h"

//: Determines the minimum of five ints.
int osl_Minimum5(int a, int b, int c, int d, int e)
{
  if ( (a<=b) && (a<=c) && (a<=d) && (a<=e) )
    return 1;
  else if ( (b<=c) && (b<=d) && (b<=e) )
    return 2;
  else if ( (c<=d) && (c<=e) )
    return 3;
  else if ( d<=e )
    return 4;
  else
    return 5;
}

//: Determines the minimum of five ints.
int osl_Minimum4(int a, int b, int c, int d)
{
  if ( (a<=b) && (a<=c) && (a<=d) )
    return 1;
  else if ( (b<=c) && (b<=d) )
    return 2;
  else if ( (c<=d) )
    return 3;
  else
    return 4;
}

//-----------------------------------------------------------------------------
//
//:
// Performs a forward chamfer convolution on the dist image and associates
// a send image (param) that reports on some parameter of the nearest pixel.
// The image sizes are mxn.
void osl_chamfer_Forward (int m, int n, int **dist, float **param)
{
  for (int i=1; i<m-1; ++i) {
    for (int j=1; j<n-1; ++j)  {

      int val = osl_Minimum5(dist[i-1][j-1]+4,
                             dist[i-1][j  ]+3,
                             dist[i-1][j+1]+4,
                             dist[i  ][j-1]+3,
                             dist[i  ][j  ]);
      switch (val) {
      case 1:
        dist[i][j] = dist[i-1][j-1]+4;
        param[i][j] = param[i-1][j-1];
        break;

      case 2:
        dist[i][j] = dist[i-1][j]+3;
        param[i][j] = param[i-1][j];
        break;

      case 3:
        dist[i][j] = dist[i-1][j+1]+4;
        param[i][j] = param[i-1][j+1];
        break;

      case 4:
        dist[i][j] = dist[i][j-1]+3;
        param[i][j] = param[i][j-1];
        break;

      case 5:
      default:
        break;
      }
    }
  }
}

//: Performs a backward chamfer convolution on the dist and param images.
void osl_chamfer_Backward(int m, int n, int **dist, float **param)
{
  for (int i=m-2; i>0; --i) {
    for (int j=n-2; j>0; --j)  {

      int val = osl_Minimum5(dist[i  ][j],
                             dist[i  ][j+1]+3,
                             dist[i+1][j-1]+4,
                             dist[i+1][j  ]+3,
                             dist[i+1][j+1]+4 );
      switch (val) {
      case 1:
        break;

      case 2:
        dist[i][j] = dist[i][j+1]+3;
        param[i][j] = param[i][j+1];
        break;

      case 3:
        dist[i][j] = dist[i+1][j-1]+4;
        param[i][j] = param[i+1][j-1];
        break;

      case 4:
        dist[i][j] = dist[i+1][j]+3;
        param[i][j] = param[i+1][j];
        break;

      case 5:
        dist[i][j] = dist[i+1][j+1]+4;
        param[i][j] = param[i+1][j+1];
        break;

      default:
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
//
//:
// Performs a chamfer convolution starting from (minx,maxy) on the dist image
// and associates a send image (param) that reports on some parameter of the
// nearest pixel. The image sizes are mxn.
//
void osl_chamfer_Alt1(int m, int n, int **dist, float **param)
{
  for (int i=1; i<m-1; ++i) {
    for (int j=n-2; j>0; --j)  {

      int val = osl_Minimum5(dist[i-1][j+1]+4,
                             dist[i-1][j]+3,
                             dist[i-1][j-1]+4,
                             dist[i  ][j+1]+3,
                             dist[i  ][j  ]);
      switch (val) {
      case 1:
        dist[i][j] = dist[i-1][j+1]+4;
        param[i][j] = param[i-1][j+1];
        break;

      case 2:
        dist[i][j] = dist[i-1][j]+3;
        param[i][j] = param[i-1][j];
        break;

      case 3:
        dist[i][j] = dist[i-1][j-1]+4;
        param[i][j] = param[i-1][j-1];
        break;

      case 4:
        dist[i][j] = dist[i][j+1]+3;
        param[i][j] = param[i][j+1];
        break;

      case 5:
      default:
        break;
      }
    }
  }
}


//-----------------------------------------------------------------------------
//
//:
// Performs a chamfer convolution starting from (maxx,miny) on the dist image
// and associates a send image (param) that reports on some parameter of the
// nearest pixel. The image sizes are mxn.
//
void osl_chamfer_Alt2(int m, int n, int **dist, float **param)
{
  for (int i=m-2; i>0; --i) {
    for (int j=1; j<n-1; ++j)
    {
      int val = osl_Minimum5(dist[i  ][j  ],
                             dist[i  ][j+1]+3,
                             dist[i+1][j-1]+4,
                             dist[i+1][j  ]+3,
                             dist[i+1][j+1]+4 );
      switch (val) {
      case 1:
        break;

      case 2:
        dist[i][j] = dist[i][j+1]+3;
        param[i][j] = param[i][j+1];
        break;

      case 3:
        dist[i][j] = dist[i+1][j-1]+4;
        param[i][j] = param[i+1][j-1];
        break;

      case 4:
        dist[i][j] = dist[i+1][j]+3;
        param[i][j] = param[i+1][j];
        break;

      case 5:
        dist[i][j] = dist[i+1][j+1]+4;
        param[i][j] = param[i+1][j+1];
        break;

      default:
        break;
      }
    }
  }
}
