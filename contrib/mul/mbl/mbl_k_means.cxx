#include "mbl_k_means.h"
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

//: Find k cluster centres
// Uses batch k-means clustering.
// If you provide parameter partition, it will return the
// cluster index for each data sample. The number of iterations
// performed is returned.
// 
// \par Initial Cluster Centres
// If centres contain the correct number of centres, they will
// be used as the initial centres, If not, and if partition is
// given, and it is the correct size, then this will be used
// to find the initial centres.
//
// \par Degenerate Cases
// If at any point the one of the centres has no data points allocated to it
// the number of centres will be reduced below k. This is most likely to
// happen if you start the function with one or more centre identical, or
// if some of the centres start off outside the convex hull of the data set.
// In particular if you let the function initialise the centres, it will
// occur if any of the first k data samples are identical.
unsigned mbl_k_means(mbl_data_wrapper<vnl_vector<double> > &data, unsigned k,
                 vcl_vector<vnl_vector<double> >* cluster_centres,
                 vcl_vector<unsigned> * partition //=0
                 )
{
  vcl_vector<vnl_vector<double> > & centres = *cluster_centres;
  
  vcl_vector<unsigned> * p_partition;
  data.reset();
  unsigned  dims = data.current().size();
  vcl_vector<vnl_vector<double> > sums(k, vnl_vector<double>(dims, 0.0));
  vcl_vector<unsigned> nNearest(k,0);
  unsigned i;
  unsigned iterations =0;

  bool initialise_from_clusters = false;

  // set up p_partition to point to something sensible
  if (partition)
  {
    p_partition = partition;
    if (p_partition->size() != data.size())
    {
      p_partition->resize(data.size());
      vcl_fill(p_partition->begin(), p_partition->end(), 0);
    }
    else initialise_from_clusters = true;
  }
  else
    p_partition = new vcl_vector<unsigned>(data.size(), 0);
  

  

// Calculate initial centres

  if (centres.size() != k) // use first k data items as centres
  {
    centres.resize(k);
    for (i=0; i<k; ++i)
    {
      centres[i] = data.current();
      sums[i] += data.current();
      nNearest[i]++;
      data.next();
    }
  }
  else if (initialise_from_clusters)
  {                         // calculate centres fro existing 
  
    do
    {
      sums[(*p_partition)[data.index()] ] += data.current();
      nNearest[(*p_partition)[data.index()] ]++;
    } while (data.next());
    // Calculate centres
    for (i=0; i<k; ++i)
      centres[i] = sums[i]/nNearest[i];
    data.reset();
    vcl_fill(sums.begin(), sums.end(), vnl_vector<double>(dims, 0.0));
    vcl_fill(nNearest.begin(), nNearest.end(), 0);
  }

  bool changed;


  do
  {
    changed = false;
    do
    {
      unsigned bestCentre = 0;
      double bestDist = vnl_vector_ssd(centres[0], data.current());
      for (i=1; i<k; ++i)
      {
        double dist = vnl_vector_ssd(centres[i], data.current());
        if (dist < bestDist)
        {
            bestDist = dist;
            bestCentre = i;
        }
      }
      sums[bestCentre] += data.current();
      nNearest[bestCentre] ++;
      if (bestCentre != (*p_partition)[data.index()])
      {
        changed = true;
        (*p_partition)[data.index()] = bestCentre;
      }
    } while (data.next());


    // reduce k if any centres have no data items assigned to its cluster.
    for (i=0; i<k; ++i)
    {
      if ( nNearest[i] == 0)
      {
        k--;
        centres.erase(centres.begin()+i);
        sums.erase(sums.begin()+i);
        
        for (unsigned j=0; j<p_partition->size(); ++j)
        {
          assert ((*p_partition)[j] = i);
          if ((*p_partition)[j] > i) (*p_partition)[j]--;
        }
      }
    }
      
  
  // Calculate new centres
  for (i=0; i<k; ++i)
    centres[i] = sums[i]/nNearest[i];

  // and repeat
  data.reset();
  vcl_fill(sums.begin(), sums.end(), vnl_vector<double>(dims, 0.0));
  vcl_fill(nNearest.begin(), nNearest.end(), 0);
  iterations ++;
  } while(changed);


  if (!partition)
    delete p_partition;
  return iterations;
}