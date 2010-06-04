#ifndef raa_run_length_encoding_h
#define raa_run_length_encoding_h
//:
// \file
// \author Graham Vincent

#include <vcl_vector.h>
#include <vsl/vsl_vector_io.h>
#include <vimt3d/vimt3d_image_3d_of.h>

const unsigned version_no = 1;


// Encode vimt3d image using run lengths
// \note decode with vimt3d_run_length_decode
template<class T>
void vimt3d_run_length_encode(vsl_b_ostream& bfs,const vimt3d_image_3d_of<T> &image)
{
  vcl_vector<vcl_pair<unsigned,T> > run_length_and_values;

  typename vil3d_image_view<T>::const_iterator it, itp1;
  for (it=image.image().begin();it != image.image().end();++it)
  {
    itp1=it;
    itp1++;
    unsigned run_length=1;

    while (itp1!=image.image().end() && *itp1 == *it ) // move through image until value changes or reach end
    {
      it++;
      itp1++;
      run_length++;
    } // coming out of this loop (*it) has the value for the run just ended

    run_length_and_values.push_back( vcl_make_pair(run_length, *it) );
  }

  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,image.world2im());
  vsl_b_write(bfs,image.image().ni());
  vsl_b_write(bfs,image.image().nj());
  vsl_b_write(bfs,image.image().nk());
  vsl_b_write(bfs,run_length_and_values);
}


template<class T>
// Decode vimt3d image encoded by vimt3d_run_length_encode
void vimt3d_run_length_decode(vsl_b_istream& bfs,vimt3d_image_3d_of<T> &image)
{
  vimt3d_transform_3d world2im;
  unsigned ni, nj, nk;
  vcl_vector<vcl_pair<unsigned,T> > run_length_and_values;

  unsigned v;
  vsl_b_read(bfs,v);
  vsl_b_read(bfs,world2im);
  vsl_b_read(bfs,ni);
  vsl_b_read(bfs,nj);
  vsl_b_read(bfs,nk);
  vsl_b_read(bfs,run_length_and_values);

  image.image().set_size(ni,nj,nk);
  image.set_world2im(world2im);
  typename vil3d_image_view<T>::iterator it=image.image().begin();

  for (unsigned i=0;i<run_length_and_values.size();++i)
  {
    for (unsigned j=0;j<run_length_and_values[i].first;++j)
    {
      *it=run_length_and_values[i].second;
      ++it;
    }
  }
}


#endif

