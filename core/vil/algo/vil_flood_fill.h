#ifndef vil_flood_fill_h_
#define vil_flood_fill_h_
//:
// \file
// \brief Fills a connected region with a given value.
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_utility.h> // for vcl_pair

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
      unsigned ilo=i,ihi=i;
      while (ilo>0 && image(ilo-1,j)==v) { ilo--; image(ilo,j)=new_v; }
      while (ihi<ni1 && image(ihi+1,j)==v) { ihi++; image(ihi,j)=new_v; }

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
      unsigned ilo=i,ihi=i;
      while (ilo>0 && image(ilo-1,j)==v) { ilo--; image(ilo,j)=new_v; }
      while (ihi<ni1 && image(ihi+1,j)==v) { ihi++; image(ihi,j)=new_v; }

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
