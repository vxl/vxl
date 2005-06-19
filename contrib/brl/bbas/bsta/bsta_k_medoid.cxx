//:
// \file
#include <vcl_cmath.h> //for HUGE_VAL
#include <vcl_algorithm.h> //for find
#include <bsta/bsta_k_medoid.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

bsta_k_medoid::bsta_k_medoid(const unsigned n_elements, bool verbose)
{
  n_elements_ = n_elements;
  distance_array_.resize(n_elements, n_elements);
  distance_array_.fill(0.0);
  verbose_ = verbose;
}

//------------------------------------------------
// Is a element a medoid?
bool bsta_k_medoid::is_medoid(const unsigned i) const
{
  vcl_vector<unsigned>::const_iterator result;
  result = vcl_find(medoids_.begin(), medoids_.end(), i);
  return result != medoids_.end();
}

//------------------------------------------------
// is an element in cluster k ?
bool bsta_k_medoid::in_cluster(const unsigned i, const unsigned k) const
{
  //Easy checks first
  if (k>=this->k())
    return false;
  if (is_medoid(i))
    return i==k;

  vcl_vector<unsigned>::const_iterator result;
  result = vcl_find(clusters_[k].begin(), clusters_[k].end(), i);
  return result != clusters_[k].end();
}

//------------------------------------------------
//the distance between an element and its medoid
double bsta_k_medoid::medoid_distance(const unsigned i) const
{
  double d = 0;
  for (unsigned k = 0; k<this->k(); ++k)
    if (this->in_cluster(i, k))
      d = distance(i, this->medoid(k));
  return d;
}

//---------------------------------------------------
// The total distance between elements in the cluster and the cluster medoid
double bsta_k_medoid::total_distance(const unsigned k) const
{
  assert(k<this->k());
  double d = 0;
  unsigned m = medoid(k);//the medoid corresponding to index k
  for (unsigned i = 0; i<this->size(k); ++i)
    d += distance(clusters_[k][i], m);
  return d;
}

//--------------------------------------------------------
// Compute the change in distance of swapping medoid j with
// current mediod k with respect to element i
double bsta_k_medoid::dc(const unsigned i, const unsigned j, const unsigned k)
{
  //current distance
  double d_ik = distance(i, k);

  //distance to j
  double d_ij = distance(i, j);

  // distance change if swap were done
  return d_ij - d_ik;
}

//--------------------------------------------------------
// Compute the change in distance of swapping medoid j with
// current mediod k with respect to total distance between medoids
double bsta_k_medoid::dcm(const unsigned j, const unsigned k)
{
  //iterate over medoids
  double dk = 0, dj = 0;
  unsigned kmax = this->k();
  if (!kmax)
    return 0.0;
  unsigned count = 0;
  for (unsigned nk=0; nk<kmax; ++nk, ++count)
  {
    unsigned m = this->medoid(nk);
    if (m==k)
      continue;
    dk += this->distance(k, m);
    dj += this->distance(j, m);
  }
  if (!count)
    return 0;
  //negative change if inter-medoid distance increases
  double result = (dk-dj)/count;
  return result;
}


//: Clear the cluster vectors
void bsta_k_medoid::clear_clusters()
{
  for (unsigned j=0; j<this->k(); ++j)
    clusters_[j].clear();
}


//:assign non-medoids to clusters
void bsta_k_medoid::form_clusters()
{
  this->clear_clusters();
  for (unsigned i = 0; i<n_elements_;++i)
    if (is_medoid(i))
      continue;
    else
    {
      //find closest medoid
      double dmin = HUGE_VAL;
      unsigned jmin=0;
      for (unsigned j=0; j<this->k(); ++j)
        if (distance(i,this->medoid(j))<dmin)
        {
          jmin = j;
          dmin = distance(i,this->medoid(j));
        }
      //assign i to the closest (jmin)
      clusters_[jmin].push_back(i);
    }
  //put medoids into their own clusters
  for (unsigned j=0; j<this->k(); ++j)
    clusters_[j].push_back(medoids_[j]);
}


//:replace medoid k with medoid j
bool bsta_k_medoid::replace_medoid(const unsigned j, const unsigned k)
{
  vcl_vector<unsigned>::iterator result;
  result = vcl_find(medoids_.begin(), medoids_.end(), k);
  if (result == medoids_.end())
    return false;
  (*result) = j;
  return true;
}


//: Returns false if swap is not warranted
bool bsta_k_medoid::test_medoid_swap(unsigned& mj, unsigned& mk)
{
  //Impossible values
  mj = n_elements_, mk = n_elements_;

  // for each j not a medoid
  double Sdc_min = HUGE_VAL;
  unsigned jmin=0, kmin=0;
  for ( unsigned j = 0; j<n_elements_; ++j)
    if (is_medoid(j))
      continue;
    else
      for (unsigned k = 0; k<this->k(); ++k)
      {
        if (verbose_)
        {
          vcl_cout << "\n===== Current Medoids(";
          for (unsigned m = 0; m<this->k(); ++m)
            vcl_cout << medoid(m) << ' ';
          vcl_cout << ")\n"
                   << "Checking Swap " << j << "->" << medoid(k) << '\n';
        }
        double Sdc = 0;
        unsigned count = 0;
        //Sum up the effect of swapping j for k on each non-medoid element
        for (unsigned i = 0; i<n_elements_;++i)
          if (is_medoid(i)||i==j)
            continue;
          else{
            Sdc += dc(i,j,medoid(k));
            ++count;}

        if (count>0)
          Sdc /= count;
        double med_dist=this->dcm(j, medoid(k));
        double total = Sdc + med_dist;

        if (verbose_)
        {
          vcl_cout << "Inter-element distance change " << Sdc << '\n'
                   << "Inter-medoid distance change " << med_dist << '\n'
                   << "Total change " << total << '\n';
        }

        if (total < Sdc_min)
        {
          jmin = j;
          kmin = medoid(k);
          Sdc_min = total;
        }
      }

  if (Sdc_min<0)
  {
    mj = jmin;
    mk = kmin;
    return true;
  }
  return false;
}

//-------------------------------------------------------
// Find the best set of k medoids
//
void bsta_k_medoid::do_clustering(const unsigned nk)
{
  assert(nk<=n_elements_);

  // arbitrarily select k medoids
  medoids_.clear();
  // choose first k elements as medoids
  for (unsigned i = 0; i<nk; ++i)
    medoids_.push_back(i);
  // reset the clusters
  clusters_.clear();
  clusters_.resize(nk);
  //We are done if there are no non-medoids
  if (nk==n_elements_)
  {
    for (unsigned i = 0; i<nk; ++i)
      clusters_[i].push_back(i);
    return;
  }
  //otherwise
  this->form_clusters();
  unsigned mj, mk;//potential swap for medoids( swap j for k)
  while (test_medoid_swap(mj, mk))
  {
    replace_medoid(mj, mk);
    form_clusters();
    if (verbose_)
    {
      vcl_cout << "***Swapping " << mj << "->" << mk << "***\n";
      for (unsigned k = 0; k<this->k(); ++k)
      {
        vcl_cout << "Medoid[" << k << "] = " << medoid(k) << '\n'
                 << "with cluster\n";
        for (unsigned j = 0; j<size(k); ++j)
          vcl_cout << clusters_[k][j] << ' ' ;
        vcl_cout << '\n'
                 << "Total Cluster Distance = "
                 << total_distance(k)<< '\n';
      }
    }
  }
  return;
}
