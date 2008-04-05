#ifndef vil_flood_fill_h_
#define vil_flood_fill_h_
//:
// \file
// \brief Fills a connected region with a given value.
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_utility.h> // for vcl_pair
#include <vil/vil_chord.h>

//: Search along i direction either side for limits of pixels matching v
//  Fills in all such pixels with new_v.  Returns limits in ilo and ihi
template<class T>
inline void vil_flood_fill_row(vil_image_view<T>& image,
                               unsigned i, unsigned j,
                               T v, T new_v,
                               unsigned& ilo, unsigned& ihi)
{
  T* row=image.top_left_ptr() + j*image.jstep();
  unsigned ni1=image.ni()-1;
  vcl_ptrdiff_t istep=image.istep();
  ilo=i;
  T* p=row+(i-1)*istep;
  while (ilo>0 && *p==v) { ilo--; *p=new_v; p-=istep; }
  ihi=i;
  p=row+(i+1)*istep;
  while (ihi<ni1 && *p==v) { ihi++; *p=new_v; p+=istep;}
}

//: Flood fill on a 4-connected region
//  Find every point in 4-connected region with values image(i,j)==v
//  containing (seed_i,seed_j), and change their values to new_v
//
//  Note, currently uses inefficient (x,y) access to image. Could be improved
//  using fast pointer access to work along the rows.
template<class T>
void vil_flood_fill4(vil_image_view<T>& image,
                     unsigned seed_i, unsigned seed_j,
                     T v, T new_v)
{
  unsigned ni1=image.ni()-1;
  unsigned nj1=image.nj()-1;
  vcl_vector<vcl_pair<unsigned,unsigned> > q;  // List of points to visit
  if (seed_i>ni1 || seed_j>nj1) return;  // Seed outside image

  // Initialise the queue with the seed
  q.push_back(vcl_pair<unsigned,unsigned>(seed_i,seed_j));

  unsigned k=0;
  while (k<q.size())
  {
    unsigned i=q[k].first, j=q[k].second;
    if (image(i,j)==v)
    {
      image(i,j)=new_v;
      // Search to left and right for limit of this line
      unsigned ilo,ihi;
      vil_flood_fill_row(image,i,j,v,new_v,ilo,ihi);

      if (j>0)
      {
        // Consider row above
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j-1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j-1));
      }
      if (j<nj1)
      {
        // Consider row below
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j+1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j+1));
      }
    }
    k++;
  }
}

//: Flood fill on a 4-connected region, and record region
//  Find every point in 4-connected region with values image(i,j)==v
//  containing (seed_i,seed_j), and change their values to new_v
//
//  On exit region is filled with a set of image chords which cover the
//  region.
//
//  Note, currently uses inefficient (x,y) access to image. Could be improved
//  using fast pointer access to work along the rows.
template<class T>
void vil_flood_fill4(vil_image_view<T>& image,
                     unsigned seed_i, unsigned seed_j,
                     T v, T new_v,
                     vcl_vector<vil_chord>& region)
{
  region.resize(0);
  unsigned ni1=image.ni()-1;
  unsigned nj1=image.nj()-1;
  vcl_vector<vcl_pair<unsigned,unsigned> > q;  // List of points to visit
  if (seed_i>ni1 || seed_j>nj1) return;  // Seed outside image

  // Initialise the queue with the seed
  q.push_back(vcl_pair<unsigned,unsigned>(seed_i,seed_j));

  unsigned k=0;
  while (k<q.size())
  {
    unsigned i=q[k].first, j=q[k].second;
    if (image(i,j)==v)
    {
      image(i,j)=new_v;
      // Search to left and right for limit of this line
      unsigned ilo,ihi;
      vil_flood_fill_row(image,i,j,v,new_v,ilo,ihi);

      region.push_back(vil_chord(ilo,ihi,j));

      if (j>0)
      {
        // Consider row above
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j-1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j-1));
      }
      if (j<nj1)
      {
        // Consider row below
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j+1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j+1));
      }
    }
    k++;
  }
}


//: Flood fill on a 8-connected region
//  Find every point in 8-connected region with values image(i,j)==v
//  containing (seed_i,seed_j), and change their values to new_v
//
//  Note, currently uses inefficient (x,y) access to image. Could be improved
//  using fast pointer access to work along the rows.
template<class T>
void vil_flood_fill8(vil_image_view<T>& image,
                     unsigned seed_i, unsigned seed_j,
                     T v, T new_v)
{
  unsigned ni1=image.ni()-1;
  unsigned nj1=image.nj()-1;
  vcl_vector<vcl_pair<unsigned,unsigned> > q;  // List of points to visit
  if (seed_i>ni1 || seed_j>nj1) return;  // Seed outside image

  // Initialise the queue with the seed
  q.push_back(vcl_pair<unsigned,unsigned>(seed_i,seed_j));

  unsigned k=0;
  while (k<q.size())
  {
    unsigned i=q[k].first, j=q[k].second;
    if (image(i,j)==v)
    {
      image(i,j)=new_v;
      // Search to left and right for limit of this line
      unsigned ilo,ihi;
      vil_flood_fill_row(image,i,j,v,new_v,ilo,ihi);

      // Expand by one to allow all 8 neighbours to be examined
      if (ilo>0) ilo--;
      if (ihi<ni1) ihi++;
      if (j>0)
      {
        // Consider row above
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j-1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j-1));
      }
      if (j<nj1)
      {
        // Consider row below
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j+1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j+1));
      }
    }
    k++;
  }
}

//: Flood fill on a 8-connected region, and record region
//  Find every point in 8-connected region with values image(i,j)==v
//  containing (seed_i,seed_j), and change their values to new_v
//
//  On exit region is filled with a set of image chords which cover the
//  region.
//
//  Note, currently uses inefficient (x,y) access to image. Could be improved
//  using fast pointer access to work along the rows.
template<class T>
void vil_flood_fill8(vil_image_view<T>& image,
                     unsigned seed_i, unsigned seed_j,
                     T v, T new_v,
                     vcl_vector<vil_chord>& region)
{
  region.resize(0);
  unsigned ni1=image.ni()-1;
  unsigned nj1=image.nj()-1;
  vcl_vector<vcl_pair<unsigned,unsigned> > q;  // List of points to visit
  if (seed_i>ni1 || seed_j>nj1) return;  // Seed outside image

  // Initialise the queue with the seed
  q.push_back(vcl_pair<unsigned,unsigned>(seed_i,seed_j));

  unsigned k=0;
  while (k<q.size())
  {
    unsigned i=q[k].first, j=q[k].second;
    if (image(i,j)==v)
    {
      image(i,j)=new_v;
      // Search to left and right for limit of this line
      unsigned ilo,ihi;
      vil_flood_fill_row(image,i,j,v,new_v,ilo,ihi);

      region.push_back(vil_chord(ilo,ihi,j));

      // Expand by one to allow all 8 neighbours to be examined
      if (ilo>0) ilo--;
      if (ihi<ni1) ihi++;
      if (j>0)
      {
        // Consider row above
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j-1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j-1));
      }
      if (j<nj1)
      {
        // Consider row below
        for (unsigned i1=ilo;i1<=ihi;++i1)
          if (image(i1,j+1)==v)
            q.push_back(vcl_pair<unsigned,unsigned>(i1,j+1));
      }
    }
    k++;
  }
}

#endif // vil_flood_fill_h_
