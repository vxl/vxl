#ifndef vdtop_set_structure_from_digital_graph_txx_
#define vdtop_set_structure_from_digital_graph_txx_

#include <vdtop/vdtop_8_neighborhood_mask.h>

template <class TMap>
void vdtop_set_structure_from_digital_graph(TMap & res,
                                            const vil_image_view<vdtop_8_neighborhood_mask> & mask,
                                            int nb_vertices, int nb_edges, vmap_2_map_tag)
{
  int plane = 0;

  unsigned ni = mask.ni(), nj = mask.nj();
  // Precompute steps
  vcl_ptrdiff_t istepM=mask.istep(),jstepM=mask.jstep(),pstepM = mask.planestep();

  // build res
  res.initialise_darts(nb_edges*2) ;

  int** dart = new int*[ni];
  for (unsigned int j=0; j<ni; ++j)
  {
    dart[j] = new int[4];
    dart[j][0]=dart[j][1]=dart[j][2]=dart[j][3]=-1 ;
  }

  int nb_darts=0 ;

  // Sets the links
  const vdtop_8_neighborhood_mask* planeM=mask.top_left_ptr() ;
  planeM += plane*pstepM ;
  {
    const vdtop_8_neighborhood_mask* rowM = planeM, *current_mask;

    for (unsigned j=0;j<nj;j++,rowM+=jstepM)
    {
      int dart3=-1 ;

      current_mask=rowM;
      for (unsigned int i=0; i<ni; ++i, current_mask+=istepM)
      {
        int k=0 ;
        int first=-1, last=-1 ;

        // darts toward 1-4 have already been intanciated
        while (dart[i][k]<0 && k<4) k++ ;
        if (k<4)
        {
          last=first = dart[i][k] ;
          dart[i][k]=-1 ;
          do { k++ ; } while (k<4 && dart[i][k]<0) ;
          while (k<4)
          {
            res.set_sigma(dart[i][k],last) ; // we set sigma clockwise
            last=dart[i][k];
            dart[i][k]=-1 ;
            do { k++ ; } while (k<4 && dart[i][k]<0) ;
          }
        }

        // the dart 3 from preceding iteration
        dart[i][2]=dart3 ; dart3=-1 ;

        // darts between 5,6,7,0 have to be intanciated

        vdtop_8_neighborhood_mask tmp_mask=*current_mask ;
        tmp_mask&=vdtop_8_neighborhood_mask(225) ;

        unsigned n = tmp_mask.nb_8_neighbors() ;
        if (n!=0)
        {
          unsigned k = 0;
          vdtop_freeman_code dir=tmp_mask.direction_8_neighbor(k) ;

          for (; k<n; dir=tmp_mask.direction_8_neighbor(++k) )
          {
            if (last!=-1)
              res.set_sigma(nb_darts, last) ;
            else
              first=nb_darts ;
            last=nb_darts++ ;
            res.set_alpha(nb_darts, last) ;
            if (dir.code()==7) // we do not set directly the dart 1 of i-1 for avoiding overlaps
              dart3=nb_darts++ ;
            else
              dart[i+dir.di()][(-dir).code()-1]=nb_darts++ ;
          }
        }
        if (first!=-1)
          res.set_sigma(first,last) ;
      }
    }
  }
  for (unsigned int j=0; j<ni; j++)
    delete[] dart[j];
  delete[] dart;
}

template <class TMap>
void vdtop_set_structure_from_digital_graph(TMap & res,
                                            const vil_image_view<vdtop_8_neighborhood_mask> & mask,
                                            int nb_vertices, int nb_edges, vmap_2_tmap_tag)
{
  int plane = 0 ;

  unsigned ni = mask.ni(), nj = mask.nj();
  // Precompute steps
  vcl_ptrdiff_t istepM=mask.istep(),jstepM=mask.jstep(),pstepM = mask.planestep();

  // build res

  res.initialise_darts(nb_edges*2) ;
  res.initialise_vertices(nb_vertices) ;

  int** dart = new int*[ni];
  for (unsigned int j=0; j<ni; ++j)
  {
    dart[j] = new int[4];
    dart[j][0]=dart[j][1]=dart[j][2]=dart[j][3]=-1 ;
  }

  int nb_darts=0 ;
  nb_vertices=0 ;

  // Sets the links
  const vdtop_8_neighborhood_mask* planeM=mask.top_left_ptr() ;
  planeM += plane*pstepM ;
  {
    const vdtop_8_neighborhood_mask* rowM = planeM, *current_mask;

    for (unsigned j=0;j<nj;j++,rowM+=jstepM)
    {
      int dart3=-1 ;

      current_mask=rowM;
      for (unsigned int i=0; i<ni; ++i, current_mask+=istepM)
      {
        int k=0 ;
        int first=-1, last=-1 ;

        // darts toward 1-4 have already been intanciated
        while (dart[i][k]<0 && k<4) k++ ;
        if (k<4)
        {
          last=first = dart[i][k] ;
          res.set_vertex(dart[i][k], nb_vertices) ;
          dart[i][k]=-1 ;
          do { k++ ; } while (k<4 && dart[i][k]<0) ;
          while (k<4)
          {
            res.set_sigma(dart[i][k],last) ; // we set sigma clockwise
            res.set_vertex(dart[i][k], nb_vertices) ;
            last=dart[i][k];
            dart[i][k]=-1 ;
            do { k++ ; } while (k<4 && dart[i][k]<0) ;
          }
        }

        // the dart 3 from preceding iteration
        dart[i][2]=dart3 ; dart3=-1 ;

        // darts between 5,6,7,0 have to be intanciated

        vdtop_8_neighborhood_mask tmp_mask=*current_mask ;
        tmp_mask&=vdtop_8_neighborhood_mask(225) ;

        unsigned n = tmp_mask.nb_8_neighbors() ;
        if (n!=0)
        {
          unsigned k = 0;
          vdtop_freeman_code dir=tmp_mask.direction_8_neighbor(k) ;

          for (; k<n; dir=tmp_mask.direction_8_neighbor(++k) )
          {
            if (last!=-1)
              res.set_sigma(nb_darts, last) ;
            else
              first=nb_darts ;
            res.set_vertex(nb_darts, nb_vertices) ;
            last=nb_darts++ ;
            res.set_alpha(nb_darts, last) ;
            if (dir.code()==7) // we do not set directly the dart 1 of i-1 for avoiding overlaps
              dart3=nb_darts++ ;
            else
              dart[i+dir.di()][(-dir).code()-1]=nb_darts++ ;
          }
        }
        if (first!=-1)
        {
          res.set_sigma(first,last) ;
          nb_vertices++ ;
        }
      }
    }
    res.set_edge_cycles() ;
    res.set_face_cycles() ;
  }
  for (unsigned int j=0; j<ni; j++)
    delete[] dart[j];
  delete[] dart;
}

#endif // vdtop_set_structure_from_digital_graph_txx_
