#ifndef vdtop_set_8_veinerization_structure_txx_
#define vdtop_set_8_veinerization_structure_txx_

#include "vcl_limits.h"

template <class T>
void vdtop_compute_8_veinerization_mask(vil_image_view<T> & img,
                                        vil_image_view<vdtop_8_neighborhood_mask> & masks,
                                        int & nb_vertices, int & nb_edges)
{
  unsigned ni = img.ni(),nj = img.nj(),np = img.nplanes(), nil=ni-1, njl=nj-1;
  //const vxl_byte dir_order[8]={7,6,4,1,2,3,5,8} ;
  const vxl_byte dir_order[8]={5,3,2,1,4,6,7,8} ;
  T max_value=vcl_numeric_limits<T>::max();
  masks.set_size(ni, nj, np) ;
  masks.fill(vdtop_8_neighborhood_mask(0)) ;
  nb_vertices=nb_edges=0 ;

  // Precompute steps
  vcl_ptrdiff_t istepI=img.istep(),jstepI=img.jstep(),pstepI = img.planestep();
  vcl_ptrdiff_t istepM=masks.istep(),jstepM=masks.jstep(),pstepM = masks.planestep();
  vcl_ptrdiff_t movesI[8], movesM[8] ;
  movesI[0]=istepI ;
  movesI[1]=-jstepI+istepI ;
  movesI[2]=-jstepI ;
  movesI[3]=-jstepI-istepI ;
  movesI[4]=-istepI ;
  movesI[5]=jstepI-istepI ;
  movesI[6]=jstepI ;
  movesI[7]=jstepI+istepI ;
  movesM[0]=istepM ;
  movesM[1]=-jstepM+istepM ;
  movesM[2]=-jstepM ;
  movesM[3]=-jstepM-istepM ;
  movesM[4]=-istepM ;
  movesM[5]=jstepM-istepM ;
  movesM[6]=jstepM ;
  movesM[7]=jstepM+istepM ;

  T* planeI = img.top_left_ptr();
  vdtop_8_neighborhood_mask* planeM=masks.top_left_ptr() ;

  for (unsigned p=0;p<np;++p,planeI += pstepI,planeM += pstepM)
  {
    T* rowI   = planeI, *current;
    vdtop_8_neighborhood_mask* rowM = planeM, *current_mask;

    // Sets the first row
    {
      current=rowI ; current_mask=rowM ;

      // upper left corner
      *current_mask=65 ;
      *current=max_value ;
      current+=istepI ; current_mask+=istepM ;

      // first row
      for (unsigned i=1;i<nil;i++, current_mask+=istepM, current+=istepI)
      {
        *current_mask=1;//17 ;
        *current=max_value ;
      }

      // upper right corner
      *current_mask=64;//80 ;
      *current=max_value ;
    }

    rowI+=jstepI ; rowM+=jstepM ;
    // The general case
    unsigned j;
    for (j=1;j<njl;j++,rowI+=jstepI,rowM+=jstepM)
    {
      current=rowI;
      current_mask=rowM;

      // first column
      *current_mask=64;//68 ;
      *current=max_value ;
      current=rowI+istepI;
      current_mask=rowM+istepM;

      // compute masks for current and preceding line
      for (unsigned i=1;i<nil;++i, current_mask+=istepM, current+=istepI)
      {
        int cmp=(*current<*(current+movesI[2])) ;
        *current_mask=cmp<<2 ;
        cmp=!cmp ;
        *(current_mask+movesM[2])|=(cmp<<6) ;

        cmp=(*current<*(current+movesI[3])) ;
        *(current_mask)|=cmp<<3 ;
        cmp=!cmp ;
        *(current_mask+movesM[3])|=(cmp<<7) ;

        cmp=(*current<*(current+movesI[4])) ;
        *current_mask|=cmp<<4 ;
        cmp=!cmp ;
        *(current_mask+movesM[4])|=(cmp) ;

        cmp=(*(current+movesI[2])>*(current+movesI[4])) ;
        *(current_mask+movesM[4])|=cmp<<1 ;
        cmp=(!cmp) ;
        *(current_mask+movesM[2])|=(cmp<<5) ;
      }


      // last column
      *(current_mask-istepM)|=131 ;
      *current_mask=64;//68 ;
      *current=max_value ;

      // remove non_maximal_directions for preceding line
      current=rowI-jstepI ;
      current_mask=rowM-jstepM ;
      for (unsigned i=0; i<ni; ++i, current_mask+=istepM, current+=istepI)
      {
        vdtop_8_neighborhood_mask max_mask(0) ;
        // for each component
        int n = current_mask->t8p();
        if (n>0)
        {
          for (int k = 0; k<n; k++)
          {
            vdtop_8_neighborhood_mask cc = current_mask->connected_8_component(k);
            int nb_neigh=cc.nb_8_neighbors() ;

            // Computes the direction to the max neighbor
            vdtop_freeman_code m=cc.direction_8_neighbor(0) ;
            T * m_val=current+movesI[m.code()] ;

            for (int l=1; l<nb_neigh; l++)
            {
              vdtop_freeman_code d=cc.direction_8_neighbor(l) ;
              T * d_val=current+movesI[d.code()] ;

              if (*d_val>*m_val || (*d_val==*m_val && dir_order[d.code()]>dir_order[m.code()]))
              {
                m=d ; m_val=d_val ;
              }
            }
            max_mask.add_direction(m) ;
          }

          // upper symmetric
          nb_vertices+=(!max_mask.empty()) ;
          nb_edges+=max_mask.nb_8_neighbors() ;
          vdtop_8_neighborhood_mask tmp=max_mask ;
          tmp&=vdtop_8_neighborhood_mask(30) ;
          int nb = tmp.nb_8_neighbors();
          if (nb>0)
          {
            vdtop_freeman_code dir= tmp.direction_8_neighbor(0) ;
            for (int k = 0 ; k<nb; dir= tmp.direction_8_neighbor(++k) )
            {
              vdtop_8_neighborhood_mask* to_modif=current_mask+movesM[dir.code()] ;
              nb_vertices+=(to_modif->empty()) ;
              to_modif->add_direction(-dir) ;
            }
          }

          (*current_mask)=max_mask ;
        }
        else
          (*current_mask)=0 ;
      }
    }

    // Sets the last row
    {
      current=rowI ; current_mask=rowM ;

      // lower left corner
      *current_mask=1;//5 ;
      *current=max_value ;
      current+=istepI ; current_mask+=istepM ;

      // last row
      for (unsigned i=1;i<nil;i++, current_mask += istepM,current += istepI)
      {
        *(current_mask-jstepM)|=224 ;
        *current_mask=1;//17 ;
        *current=max_value ;
      }

      // lower right corner
      *current_mask=0;//20 ;
      *current=max_value ;
    }

    // remove non_maximal_directions for 2 last line
    rowI=planeI+(nj-2)*jstepI ; rowM=planeM+(nj-2)*jstepM ;
    for (j=0;j<2;j++,rowI+=jstepI,rowM+=jstepM)
    {
      // remove non_maximal_directions for preceding line
      current=rowI ; current_mask=rowM ;
      for (unsigned i=0; i<ni; ++i, current_mask+=istepM, current+=istepI)
      {
        vdtop_8_neighborhood_mask max_mask(0) ;
        int n = current_mask->t8p();
        if (n>0)
        {
          // for each component
          for (int k = 0; k<n; k++)
          {
            vdtop_8_neighborhood_mask cc = current_mask->connected_8_component(k);
            int nb_neigh=cc.nb_8_neighbors() ;

            // Computes the direction to the max neighbor
            vdtop_freeman_code m=cc.direction_8_neighbor(0) ;
            T * m_val=current+movesI[m.code()] ;

            for (int l=1; l<nb_neigh; l++)
            {
              vdtop_freeman_code d=cc.direction_8_neighbor(l) ;
              T * d_val=current+movesI[d.code()] ;
              if (*d_val>*m_val || (*d_val==*m_val && dir_order[d.code()]>dir_order[m.code()]))
              {
                m=d ; m_val=d_val ;
              }
            }
            max_mask.add_direction(m) ;
          }

          // upper symmetric
          nb_vertices+=(!max_mask.empty()) ;
          nb_edges+=max_mask.nb_8_neighbors() ;
          vdtop_8_neighborhood_mask tmp=max_mask ;
          tmp&=vdtop_8_neighborhood_mask(30) ;
          int nb = tmp.nb_8_neighbors();
          if (nb>0)
          {
            vdtop_freeman_code dir= tmp.direction_8_neighbor(0) ;
            for (int k = 0 ; k<nb; dir= tmp.direction_8_neighbor(++k) )
            {
              vdtop_8_neighborhood_mask* to_modif=current_mask+movesM[dir.code()] ;
              nb_vertices+=(to_modif->empty()) ;
              to_modif->add_direction(-dir) ;
            }
          }
          (*current_mask)=max_mask ;
        }
        else
          (*current_mask)=0 ;
      }
    }
    // lower symmetric
    rowM = planeM;
    for (j=0;j<nj;j++,rowM+=jstepM)
    {
      current_mask=rowM;
      for (unsigned i=0; i<ni; ++i, current_mask+=istepM)
      {
        vdtop_8_neighborhood_mask tmp=*current_mask ;
        tmp&=vdtop_8_neighborhood_mask(225) ;
        int nb = tmp.nb_8_neighbors();
        if (nb>0)
        {
          vdtop_freeman_code dir= tmp.direction_8_neighbor(0) ;
          for (int k = 0 ; k<nb; dir= tmp.direction_8_neighbor(++k) )
          {
            vdtop_8_neighborhood_mask* to_modif=current_mask+movesM[dir.code()] ;
            nb_vertices+=(to_modif->empty()) ;
            to_modif->add_direction(-dir) ;
          }
        }
      }
    }
    //That's done for that plane !
  }
}

#endif // vdtop_set_8_veinerization_structure_txx_
