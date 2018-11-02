#ifndef VBL_LOCAL_MINIMA_TXX_
#define VBL_LOCAL_MINIMA_TXX_
#include <iostream>
#include <limits>
#include "vbl_local_minima.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

template <class T>
bool local_minima(vbl_array_1d<T> const& in, vbl_array_1d<T>& minima, T thresh)
{
  const unsigned int n = (unsigned int)(in.size());
  assert(minima.size()==n);
  //special cases
  // minimum is not defined for n<3
  if (n<3)
    return false;
  bool minima_found = false;
  for (unsigned int i=0; i<n; ++i)
    minima[i] = T(0);
  //the general case
  for (unsigned int c=1; c<n-1; ++c) {
    T dm = in[c-1]-in[c], dp = in[c+1]-in[c];
    if (dm>thresh && dp > thresh) {
      T dmin = dm;
      if (dp<dmin)
        dmin = dp;
      minima[c]=dmin;
      minima_found = true;
    }
  }
  // Check the ends of the array for minima
  // left end
  if ((in[1]-in[0])>thresh) {
    minima[0]=in[1]-in[0];
    minima_found = true;
  }
  // right end
  if ((in[n-2]-in[n-1])>thresh) {
    minima[n-1]=in[n-2]-in[n-1];
    minima_found = true;
  }

  return minima_found;
}

template <class T>
bool local_minima(vbl_array_2d<T> const& in, vbl_array_2d<T>& minima, T thresh)
{
  const unsigned int nr = (unsigned int)(in.rows()), nc = (unsigned int)(in.cols());
  assert(nr==minima.rows() && nc==minima.cols());
  //special case
  // actually a 1-d array or null
  if (nr<2||nc<2)
    return false;
  bool  minima_found = false;
  T mval = std::numeric_limits<T>::max();
  for (unsigned r = 0; r<nr; ++r)
    for (unsigned c = 0; c<nc; ++c)
      minima[r][c] = T(0);
  T ul,  um, ur;
  T lf,/*m,*/ri;
  T ll,  lm, lr;
  T dmin;
  //general case
  if (nr>2&&nc>2)
    for (unsigned r = 1; r<nr-1; ++r)
      for (unsigned c = 1; c<nc-1; ++c) {
        // xxxxxxxxxxxxxxxxxxxxxxx
        // xxxx   ul um  ur  xxxxx
        // xxxx   lf  m  ri  xxxxx
        // xxxx   ll lm  lr  xxxxx
        // xxxxxxxxxxxxxxxxxxxxxxx
        ul = in[r-1][c-1] - in[r][c];
        um = in[r-1][c]   - in[r][c];
        ur = in[r-1][c]   - in[r][c];
        lf = in[r][c-1]   - in[r][c];
        ri = in[r][c+1]   - in[r][c];
        ll = in[r+1][c-1] - in[r][c];
        lm = in[r+1][c]   - in[r][c];
        lr = in[r+1][c+1] - in[r][c];
        dmin = mval;
        if (ul<=thresh) continue; if (ul<dmin) dmin = ul;
        if (um<=thresh) continue; if (um<dmin) dmin = um;
        if (ur<=thresh) continue; if (ur<dmin) dmin = ur;
        if (lf<=thresh) continue; if (lf<dmin) dmin = lf;
        if (ri<=thresh) continue; if (ri<dmin) dmin = ri;
        if (ll<=thresh) continue; if (ll<dmin) dmin = ll;
        if (lm<=thresh) continue; if (lm<dmin) dmin = lm;
        if (lr<=thresh) continue; if (lr<dmin) dmin = lr;
        if (dmin>thresh) {
          minima[r][c] = dmin;
          minima_found = true;
        }
      }

  // special cases at the borders
  if (nc>2) {
    for (unsigned c = 1; c<nc-1; ++c) {
      // first row case
      // xxxx   lf  m  ri  xxxxx
      // xxxx   ll lm  lr  xxxxx
      // xxxxxxxxxxxxxxxxxxxxxxx
      lf = in[0][c-1]   - in[0][c];
      ri = in[0][c+1]   - in[0][c];
      ll = in[1][c-1]   - in[0][c];
      lm = in[1][c]     - in[0][c];
      lr = in[1][c+1]   - in[0][c];
      dmin = mval;
      if (lf<=thresh) continue; if (lf<dmin) dmin = lf;
      if (ri<=thresh) continue; if (ri<dmin) dmin = ri;
      if (ll<=thresh) continue; if (ll<dmin) dmin = ll;
      if (lm<=thresh) continue; if (lm<dmin) dmin = lm;
      if (lr<=thresh) continue; if (lr<dmin) dmin = lr;
      if (dmin>thresh) {
        minima[0][c] = dmin;
        minima_found = true;
      }
    }
    for (unsigned c = 1; c<nc-1; ++c) {
      // last row case
      // xxxxxxxxxxxxxxxxxxxxxxx
      // xxxx   ul um  ur  xxxxx
      // xxxx   lf  m  ri  xxxxx
      ul = in[nr-2][c-1] - in[nr-1][c];
      um = in[nr-2][c]   - in[nr-1][c];
      ur = in[nr-2][c+1] - in[nr-1][c];
      lf = in[nr-1][c-1] - in[nr-1][c];
      ri = in[nr-1][c+1] - in[nr-1][c];
      dmin = mval;
      if (ul<=thresh) continue; if (ul<dmin) dmin = ul;
      if (um<=thresh) continue; if (um<dmin) dmin = um;
      if (ur<=thresh) continue; if (ur<dmin) dmin = ur;
      if (lf<=thresh) continue; if (lf<dmin) dmin = lf;
      if (ri<=thresh) continue; if (ri<dmin) dmin = ri;
      if (dmin>thresh) {
        minima[nr-1][c] = dmin;
        minima_found = true;
      }
    }
  }
  if (nr>2) {
    //first column case
    for (unsigned r = 1; r<nr-1; ++r) {
      // um  ur  xxxxx
      //  m  ri  xxxxx
      // lm  lr  xxxxx
      um = in[r-1][0] - in[r][0];
      ur = in[r-1][1] - in[r][0];
      ri = in[r][1]   - in[r][0];
      lm = in[r+1][0] - in[r][0];
      lr = in[r+1][1] - in[r][0];
      dmin = mval;
      if (um<=thresh) continue; if (um<dmin) dmin = um;
      if (ur<=thresh) continue; if (ur<dmin) dmin = ur;
      if (ri<=thresh) continue; if (ri<dmin) dmin = ri;
      if (lm<=thresh) continue; if (lm<dmin) dmin = lm;
      if (lr<=thresh) continue; if (lr<dmin) dmin = lr;
      if (dmin>thresh) {
        minima[r][0] = dmin;
        minima_found = true;
      }
    }
    //last column case
    for (unsigned r = 1; r<nr-1; ++r) {
      //  xxxxx ul um
      //  xxxxx lf  m
      //  xxxxx ll lm
      ul = in[r-1][nc-2] - in[r][nc-1];
      um = in[r-1][nc-1] - in[r][nc-1];
      lf = in[r][nc-2]   - in[r][nc-1];
      ll = in[r+1][nc-2] - in[r][nc-1];
      lm = in[r+1][nc-1] - in[r][nc-1];
      dmin = mval;
      if (ul<=thresh) continue; if (ul<dmin) dmin = ul;
      if (um<=thresh) continue; if (um<dmin) dmin = um;
      if (lf<=thresh) continue; if (lf<dmin) dmin = lf;
      if (ll<=thresh) continue; if (ll<dmin) dmin = ll;
      if (lm<=thresh) continue; if (lm<dmin) dmin = lm;
      if (dmin>thresh) {
        minima[r][nc-1] = dmin;
        minima_found = true;
      }
    }
  }
  // check the corners for minima
  // upper left corner
  //  m  ri  xxxxx
  // lm  lr  xxxxx
  bool fail = false;
  ri = in[0][1] - in[0][0];
  lm = in[1][0] - in[0][0];
  lr = in[1][1] - in[0][0];
  dmin = mval;
  if (ri<=thresh) fail = true; if (ri<dmin) dmin = ri;
  if (lm<=thresh) fail = true; if (lm<dmin) dmin = lm;
  if (lr<=thresh) fail = true; if (lr<dmin) dmin = lr;
  if (!fail) {
    minima[0][0] = dmin;
    minima_found = true;
  }
  // upper right corner
  // xxxxx lf   m
  // xxxxx ll  lm
  fail = false;
  lf = in[0][nc-2] - in[0][nc-1];
  lm = in[1][nc-1] - in[0][nc-1];
  ll = in[1][nc-2] - in[0][nc-1];
  dmin = mval;
  if (lf<=thresh) fail = true; if (lf<dmin) dmin = lf;
  if (lm<=thresh) fail = true; if (lm<dmin) dmin = lm;
  if (ll<=thresh) fail = true; if (ll<dmin) dmin = ll;
  if (!fail) {
    minima[0][nc-1] = dmin;
    minima_found = true;
  }
  // lower right corner
  // xxxxx ul   um
  // xxxxx lf    m
  fail = false;
  ul = in[nr-2][nc-2] - in[nr-1][nc-1];
  um = in[nr-2][nc-1] - in[nr-1][nc-1];
  lf = in[nr-1][nc-2] - in[nr-1][nc-1];
  dmin = mval;
  if (ul<=thresh) fail = true; if (ul<dmin) dmin = ul;
  if (um<=thresh) fail = true; if (um<dmin) dmin = um;
  if (lf<=thresh) fail = true; if (lf<dmin) dmin = lf;
  if (!fail) {
    minima[nr-1][nc-1] = dmin;
    minima_found = true;
  }
  // lower left corner
  // xxxxx um   ur
  // xxxxx  m    ri
  fail = false;
  ur = in[nr-2][1] - in[nr-1][0];
  um = in[nr-2][0] - in[nr-1][0];
  ri = in[nr-1][1] - in[nr-1][0];
  dmin = mval;
  if (ur<=thresh) fail = true; if (ur<dmin) dmin = ur;
  if (um<=thresh) fail = true; if (um<dmin) dmin = um;
  if (ri<=thresh) fail = true; if (ri<dmin) dmin = ri;
  if (!fail) {
    minima[nr-1][0] = dmin;
    minima_found = true;
  }
  return minima_found;
}

template <class T>
bool local_minima(vbl_array_3d<T> const& in, vbl_array_3d<T>& minima, T thresh)
{
  const unsigned int n1=(unsigned int)(in.get_row1_count()),
                     n2=(unsigned int)(in.get_row2_count()),
                     n3=(unsigned int)(in.get_row3_count());
  assert(n3==minima.get_row3_count() &&
         n2==minima.get_row2_count() &&
         n1==minima.get_row1_count() );
  //special case
  // actually a 2-d array or null
  if (n3<2||n2<2||n1<2)
    return false;
  bool minima_found = false;
  unsigned int x3=0, x2=0, x1=0;
  for (x3 = 0; x3<n3; ++x3)
    for (x2 = 0; x2<n2; ++x2)
      for (x1 = 0; x1<n1; ++x1)
        minima[x1][x2][x3] = T(0);
  T v, d, mind;
  bool fail;
  const T mval = std::numeric_limits<T>::max();
  // general case
  if (n3>2||n2>2||n1>2)
    for (x3 = 1; x3<n3-1; ++x3)
      for (x2 = 1; x2<n2-1; ++x2)
        for (x1 = 1; x1<n1-1; ++x1) {
          mind = mval;
          fail = false;
          v = in[x1][x2][x3];
          for (int k3 = -1; k3<=1;++k3)
            for (int k2 = -1; k2<=1;++k2)
              for (int k1 = -1; k1<=1;++k1)
                if (k1!=0||k2!=0||k3!=0) {
                  d = in[x1+k1][x2+k2][x3+k3]-v;
                  if (d<=thresh) {fail = true; break;}
                  if (d<mind) mind = d;
                }
          if (!fail) { // local min found
            minima[x1][x2][x3]=mind;
            minima_found = true;
          }
        }
  //face border cases six in total
  if (n3>2&&n2>2) { // top and bottom faces of the array vary x3, x2
    //top face
    x1 = 0;
    for (x3 = 1; x3<n3-1; ++x3)
      for (x2 = 1; x2<n2-1; ++x2) {
        mind = mval; fail = false;
        v = in[x1][x2][x3];
        for (int k3 = -1; k3<=1;++k3)
          for (int k2 = -1; k2<=1;++k2)
            for (int k1 = 0; k1<=1;++k1)
              if (k3!=0||k2!=0||k1!=0) {
                d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
                if (d<mind) mind = d;
              }
        if (!fail) { // local min found
          minima[x1][x2][x3]=mind;
          minima_found = true;
        }
      }
    //bottom face
    x1 = n1-1;
    for (x3 = 1; x3<n3-1; ++x3)
      for (x2 = 1; x2<n2-1; ++x2) {
        mind = mval; fail = false;
        v = in[x1][x2][x3];
        for (int k3 = -1; k3<=1;++k3)
          for (int k2 = -1; k2<=1;++k2)
            for (int k1 = -1; k1<=0;++k1)
              if (k3!=0||k2!=0||k1!=0) {
                d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
                if (d<mind) mind = d;
              }
        if (!fail) { // local min found
          minima[x1][x2][x3]=mind;
          minima_found = true;
        }
      }
  }
  if (n3>2&&n1>2) { // front and back faces of the array vary x3, x1
    //front face
    x2 = 0;
    for (x3 = 1; x3<n3-1; ++x3)
      for (x1 = 1; x1<n1-1; ++x1) {
        mind = mval; fail = false;
        v = in[x1][x2][x3];
        for (int k3 = -1; k3<=1;++k3)
          for (int k2 = 0; k2<=1;++k2)
            for (int k1 = -1; k1<=1;++k1)
              if (k3!=0||k2!=0||k1!=0) {
                d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
                if (d<mind) mind = d;
              }
        if (!fail) { // local min found
          minima[x1][x2][x3]=mind;
          minima_found = true;
        }
      }
    //back face
    x2 = n2-1;
    for (x3 = 1; x3<n3-1; ++x3)
      for (x1 = 1; x1<n1-1; ++x1) {
        mind = mval; fail = false;
        v = in[x1][x2][x3];
        for (int k3 = -1; k3<=1;++k3)
          for (int k2 = -1; k2<=0;++k2)
            for (int k1 = -1; k1<=1;++k1)
              if (k3!=0||k2!=0||k1!=0) {
                d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
                if (d<mind) mind = d;
              }
        if (!fail) { // local min found
          minima[x1][x2][x3]=mind;
          minima_found = true;
        }
      }
  }

  if (n2>2&&n1>2) { // left and right faces of the array vary x2, x1
    //left face
    x3 = 0;
    for (x2 = 1; x2<n2-1; ++x2)
      for (x1 = 1; x1<n1-1; ++x1) {
        mind = mval; fail = false;
        v = in[x1][x2][x3];
        for (int k3 = 0; k3<=1;++k3)
          for (int k2 = -1; k2<=1;++k2)
            for (int k1 = -1; k1<=1;++k1)
              if (k3!=0||k2!=0||k1!=0) {
                d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
                if (d<mind) mind = d;
              }
        if (!fail) { // local min found
          minima[x1][x2][x3]=mind;
          minima_found = true;
        }
      }
    //right face
    x3 = n3-1;
    for (x2 = 1; x2<n2-1; ++x2)
      for (x1 = 1; x1<n1-1; ++x1) {
        mind = mval; fail = false;
        v = in[x1][x2][x3];
        for (int k3 = -1; k3<=0;++k3)
          for (int k2 = -1; k2<=1;++k2)
            for (int k1 = -1; k1<=1;++k1)
              if (k3!=0||k2!=0||k1!=0) {
                d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
                if (d<mind) mind = d;
              }
        if (!fail) { // local min found
          minima[x1][x2][x3]=mind;
          minima_found = true;
        }
      }
  }

  //edge border cases, 12 in total
  if (x1>2)
  {
    //edges along x1
    x3 = 0, x2 = 0;     //vary x1
    for (x1 = 1; x1<n1-1; ++x1) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = 0; k3<=1;++k3)
        for (int k2 = 0; k2<=1;++k2)
          for (int k1 = -1; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x3 = n3-1; x2 = 0;     //vary x1
    for (x1 = 1; x1<n1-1; ++x1) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = 0; k3<=1;++k3)
        for (int k2 = 0; k2<=1;++k2)
          for (int k1 = -1; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x3 = 0; x2 = n2-1;     //vary x1
    for (x1 = 1; x1<n1-1; ++x1) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = 0; k3<=1;++k3)
        for (int k2 = -1; k2<=0;++k2)
          for (int k1 = -1; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x3 = n3-1; x2 = n2-1;     //vary x1
    for (x1 = 1; x1<n1-1; ++x1) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=0;++k3)
        for (int k2 = -1; k2<=0;++k2)
          for (int k1 = -1; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
  }
  if (x2>2)
  {
    // edges along x2
    x3 = 0,    x1 = 0;     //vary x2
    for (x2 = 1; x2<n2-1; ++x2) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = 0; k3<=1;++k3)
        for (int k2 = -1; k2<=1;++k2)
          for (int k1 = 0; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x3 = n3-1; x1 = 0;     //vary x2
    for (x2 = 1; x2<n2-1; ++x2) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=0;++k3)
        for (int k2 = -1; k2<=1;++k2)
          for (int k1 = 0; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x3 = 0;    x1 = n1-1;     //vary x2
    for (x2 = 1; x2<n2-1; ++x2) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = 0; k3<=1;++k3)
        for (int k2 = -1; k2<=1;++k2)
          for (int k1 = -1; k1<=0;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x3 = n3-1; x1 = n1-1;     //vary x2
    for (x2 = 1; x2<n2-1; ++x2) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=0;++k3)
        for (int k2 = -1; k2<=1;++k2)
          for (int k1 = -1; k1<=0;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
  }
  if (x3>2)
  {
    // edges along x3
    x2 = 0,    x1 = 0;     //vary x3
    for (x3 = 1; x3<n3-1; ++x3) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=1;++k3)
        for (int k2 = 0; k2<=1;++k2)
          for (int k1 = 0; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x2 = n2-1; x1 = 0;     //vary x3
    for (x3 = 1; x3<n3-1; ++x3) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=1;++k3)
        for (int k2 = -1; k2<=0;++k2)
          for (int k1 = 0; k1<=1;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x2 = 0;    x1 = n1-1;     //vary x3
    for (x3 = 1; x3<n3-1; ++x3) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=1;++k3)
        for (int k2 = 0; k2<=1;++k2)
          for (int k1 = -1; k1<=0;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
    x2 = n2-1; x1 = n1-1;     //vary x3
    for (x3 = 1; x3<n3-1; ++x3) {
      mind = mval; fail = false;
      v = in[x1][x2][x3];
      for (int k3 = -1; k3<=1;++k3)
        for (int k2 = -1; k2<=0;++k2)
          for (int k1 = -1; k1<=0;++k1)
            if (k3!=0||k2!=0||k1!=0) {
              d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
              if (d<mind) mind = d;
            }
      if (!fail) { // local min found
        minima[x1][x2][x3]=mind;
        minima_found = true;
      }
    }
  }
  // corner border cases 8 in total
  // corner 000
  x3=0; x2=0; x1=0;
  v = in[x1][x2][x3];
  mind = mval; fail = false;
  for (int k3 = 0; k3<=1;++k3)
    for (int k2 = 0; k2<=1;++k2)
      for (int k1 = 0; k1<=1;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }
  // corner 001
  x3=n3-1; x2=0; x1=0;
  v = in[x1][x2][x3];
  mind = mval; fail = false;
  for (int k3 = -1; k3<=0;++k3)
    for (int k2 = 0; k2<=1;++k2)
      for (int k1 = 0; k1<=1;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }
  // corner 010
  x3=0; x2=n2-1; x1=0;
  v = in[x1][x2][x3];
  mind = mval; fail = false;
  for (int k3 = 0; k3<=1;++k3)
    for (int k2 = -1; k2<=0;++k2)
      for (int k1 = 0; k1<=1;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }
  // corner 011
  x3=n3-1; x2=n2-1; x1=0;
  v = in[x1][x2][x3];
  mind = mval; fail = false;
  for (int k3 = -1; k3<=0;++k3)
    for (int k2 = -1; k2<=0;++k2)
      for (int k1 = 0; k1<=1;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }
  // corner 100
  x3=0; x2=0; x1=n1-1;
  v = in[x1][x2][x3];
  mind = mval; fail = false;
  for (int k3 = 0; k3<=1;++k3)
    for (int k2 = 0; k2<=1;++k2)
      for (int k1 = -1; k1<=0;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }
  // corner 101
  x3=n3-1; x2=0; x1=n1-1;
  v = in[x1][x2][x3];
  mind = mval; fail = false;
  for (int k3 = -1; k3<=0;++k3)
    for (int k2 = 0; k2<=1;++k2)
      for (int k1 = -1; k1<=0;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }  // corner 110
  mind = mval; fail = false;
  x3=0; x2=n2-1; x1=n1-1;
  v = in[x1][x2][x3];
  for (int k3 = 0; k3<=1;++k3)
    for (int k2 = -1; k2<=0;++k2)
      for (int k1 = -1; k1<=0;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }  // corner 111
  mind = mval; fail = false;
  x3=n3-1; x2=n2-1; x1=n1-1;
  v = in[x1][x2][x3];
  for (int k3 = -1; k3<=0;++k3)
    for (int k2 = -1; k2<=0;++k2)
      for (int k1 = -1; k1<=0;++k1)
        if (k3!=0||k2!=0||k1!=0) {
          d = in[x1+k1][x2+k2][x3+k3]-v; if (d<=thresh) {fail=true; break;}
          if (d<mind) mind = d;
        }
  if (!fail) {
    minima[x1][x2][x3]=mind;
    minima_found = true;
  }
  return minima_found;
}

#define VBL_LOCAL_MINIMA_INSTANTIATE(T) \
template vbl_array_1d<T > vbl_local_minima(vbl_array_1d<T >const&, T); \
template vbl_array_2d<T > vbl_local_minima(vbl_array_2d<T >const&, T); \
template vbl_array_3d<T > vbl_local_minima(vbl_array_3d<T >const&, T); \
template bool local_minima(vbl_array_1d<T >const&, vbl_array_1d<T >&, T); \
template bool local_minima(vbl_array_2d<T >const&, vbl_array_2d<T >&, T); \
template bool local_minima(vbl_array_3d<T >const&, vbl_array_3d<T >&, T)

#endif // VBL_LOCAL_MINIMA_TXX_
