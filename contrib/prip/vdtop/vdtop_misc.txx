// This is prip/vdtop/vdtop_misc.txx
#ifndef vdtop_misc_txx_
#define vdtop_misc_txx_
//:
// \file

#include <vdtop/vdtop_veinerization_builder.h>
#include <vdtop/vdtop_8_neighborhood_mask.h>

//: The 3 in 1 function.
// It computes upper masks, removes non maximal directions, and compute the down-left version of the symmetric

template <class T>
void vdtop_compute_first_veinerization_pass_(vil_image_view<T> & img, const T& max_value,
                                             vil_image_view<vdtop_8_neighborhood_mask> & masks)
{
  unsigned ni = img.ni(),nj = img.nj(),np = img.nplanes(), nil=ni-1, njl=nj-1;
  const vxl_byte dir_order[8]={4,2,1,3,5,6,7};
  const vdtop_8_neighborhood_mask down_left_neighbors(0xF0),up_right_neighbors(0x0F) ;

  masks.set_size(ni, nj, np) ;

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

  const T* planeI = img.top_left_ptr();
  vdtop_8_neighborhood_mask* planeM=masks.top_left_ptr() ;

  for (unsigned p=0;p<np;++p,planeI += pstepI,planeM += pstepM)
  {
    const T* rowI   = planeI, *current;
    vdtop_8_neighborhood_mask* rowM   = planeM, *current_mask;

    // Sets the first row
    {
      current=rowI , current_mask=rowM ;

      // upper left corner
      *current_mask=65 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;

      // first row
      for (unsigned i=1;i<nil;i++, current_mask += istepM,current += istepI)
      {
        *current_mask=17 ;
        *current=max_value ;
      }

      // upper right corner
      *current_mask=80 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;
    }

    rowI+=jstepI , rowM+=jstepM ;

    // The general case
    for (unsigned j=1;j<njl;j++,rowI+=jstepI,rowM+=jstepM)
    {
      // first and last on row
      current=rowI;
      current_mask=rowM;
      *current_mask=64 ;
      *current=max_value ;
      current+=nil*istepI;
      current_mask+=nil*istepM;
      *current_mask=64 ;
      *current=max_value ;

      // remove non_maximal_directions for preceding line
      current=rowI-jstepI ;
      current_mask=rowM-jstepM ;
      for (unsigned i=0; i<ni; ++i, current_mask+=istepM, current+=istepI)
      {
        vdtop_8_neighborhood_mask max_mask(0) ;
        int n = current_mask->t8p();
        for (int k = 0; k<n; k++) // for each component
        {
          vdtop_8_neighborhood_mask cc = current_mask->connected_8_component(k);
          int nb_neigh=cc.nb_8_neighbors() ;

          // Computes the direction to the max neighbor
          vdtop_freeman_code m=cc.direction_8_neighbor(0) ;
          const T * m_val=current+movesI[m.code()] ;

          for (int l=1; l<nb_neigh; l++)
          {
            vdtop_freeman_code d=cc.direction_8_neighbor(l) ;
            const T * d_val=current+movesI[d.code()] ;
            if (*d_val>*m_val || (*d_val==*m_val && dir_order[d.code()]>dir_order[m.code()]))
            {
              m=d ; m_val=d_val ;
            }
          }
          max_mask.add_direction(m) ;
        }
        // pseudo symmetric
        vdtop_8_neighborhood_mask ur_mask=max_mask ;
        ur_mask&=up_right_neighbors ;
        int n = ur_mask.nb_8_neighbors() ;
        if (n!=0)
        {
          int k = 0 ;
          vdtop_freeman_code dir= ur_mask.direction_8_neighbor(k) ;
          for (; k<n; dir= ur_mask.direction_8_neighbor(++k) )
          {
            vdtop_8_neighborhood_mask* to_modif=current_mask+moves[dir.code()] ;
            to_modif->add_direction(-dir) ;
          }
        }
        // nether do it twice
        max_mask&=down_left_neighbors ;
        *current_mask=max_mask ;
      }

      // compute masks for current line
      current=rowI+istepI ;
      current_mask=rowM+istepM ;
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

        cmp=(*(current+movesI[2])<*(current+movesI[4])) ;
        *(current_mask+movesM[4])|=cmp<<1 ;
        cmp=(!cmp) ;
        *(current_mask+movesM[2])|=(cmp<<5) ;
      }
    }

    // Sets the last row
    {
      current=rowI , current_mask=rowM ;

      // lower left corner
      *current_mask=5 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;

      // last row
      for (unsigned i=1;i<nil;i++, current_mask += istepM,current += istepI)
      {
        *current_mask=17 ;
        *current=max_value ;
      }

      // lower right corner
      *current_mask=20 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;
    }
    // remove non_maximal_directions for last line
    current=rowI ;
    current_mask=rowM ;
    for (unsigned i=0; i<ni; ++i, current_mask+=istepM, current+=istepI)
    {
      vdtop_8_neighborhood_mask max_mask(0) ;
      int n = current_mask->t8p();
      for (int k = 0; k<n; k++) // for each component
      {
        vdtop_8_neighborhood_mask cc = current_mask->connected_8_component(k);
        int nb_neigh=cc.nb_8_neighbors() ;

        // Computes the direction to the max neighbor
        vdtop_freeman_code m=cc.direction_8_neighbor(0) ;
        const T * m_val=current+moves[m.code()] ;

        for (int l=1; l<nb_neigh; l++)
        {
          vdtop_freeman_code d=cc.direction_8_neighbor(l) ;
          const T * d_val=current+movesI[d.code()] ;
          if (*d_val>*m_val || (*d_val==*m_val && dir_order[d.code()]>dir_order[m.code()]))
          {
            m=d ; m_val=d_val ;
          }
        }
        max_mask.add_direction(m) ;
      }
      // pseudo symmetric
      vdtop_8_neighborhood_mask ur_mask=max_mask ;
      ur_mask&=up_right_neighbors ;
      int n = ur_mask.nb_8_neighbors() ;
      if (n!=0)
      {
        int k = 0 ;
        vdtop_freeman_code dir= ur_mask.direction_8_neighbor(k) ;
        for (; k<n; dir= ur_mask.direction_8_neighbor(++k) )
        {
          vdtop_8_neighborhood_mask* to_modif=current_mask+moves[dir.code()] ;
          to_modif->add_direction(-dir) ;
        }
      }
      // nether do it twice
      max_mask&=down_left_neighbors ;
      *current_mask=max_mask ;
    }
    //That's done for that plane !
  }
}

template <class T>
void vdtop_compute_8_upper_masks(vil_image_view<T> & img, const T& max_value, vil_image_view<vdtop_8_neighborhood_mask> & masks)
{
  unsigned ni = img.ni(),nj = img.nj(),np = img.nplanes(), nil=ni-1, njl=nj-1;

  masks.set_size(ni, nj, np) ;

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

  const T* planeI = img.top_left_ptr();
  vdtop_8_neighborhood_mask* planeM=masks.top_left_ptr() ;

  for (unsigned p=0;p<np;++p,planeI += pstepI,planeM += pstepM)
  {
    const T* rowI   = planeI, *current;
    vdtop_8_neighborhood_mask* rowM   = planeM, *current_mask;

    // Sets the first row
    {
      current=rowI , current_mask=rowM ;

      // upper left corner
      *current_mask=65 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;

      // first row
      for (unsigned i=1;i<nil;i++, current_mask += istepM,current += istepI)
      {
        *current_mask=17 ;
        *current=max_value ;
      }

      // upper right corner
      *current_mask=80 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;
    }

    rowI+=jstepI , rowM+=jstepM ;

    // The general case
    for (unsigned j=1;j<njl;j++,rowI+=jstepI,rowM+=jstepM)
    {
      // first and last on row
      current=rowI;
      current_mask=rowM;
      *current_mask=64 ;
      *current=max_value ;
      current+=nil*istepI;
      current_mask+=nil*istepM;
      *current_mask=64 ;
      *current=max_value ;

      // compute masks for current line
      current=rowI+istepI ;
      current_mask=rowM+istepM ;
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

        cmp=(*(current+movesI[2])<*(current+movesI[4])) ;
        *(current_mask+movesM[4])|=cmp<<1 ;
        cmp=(!cmp) ;
        *(current_mask+movesM[2])|=(cmp<<5) ;
      }
    }

    // Sets the last row
    {
      current=rowI , current_mask=rowM ;

      // lower left corner
      *current_mask=5 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;

      // last row
      for (unsigned i=1;i<nil;i++, current_mask += istepM,current += istepI)
      {
        *current_mask=17 ;
        *current=max_value ;
      }

      // lower right corner
      *current_mask=20 ;
      *current=max_value ;
      current+=istepI, current_mask+=istepM ;
    }
    //That's done for that plane !
  }
}

template <class T>
void vdtop_remove_non_maximal_direction(const vil_image_view<T> & img, vil_image_view<vdtop_8_neighborhood_mask> & mask)
{
  unsigned ni = img.ni(),nj = img.nj(),np = img.nplanes();
  const vxl_byte dir_order[8]={4,2,1,3,5,6,7};

  // Precompute steps
  vcl_ptrdiff_t istepI=img.istep(),jstepI=img.jstep(),pstepI = img.planestep();
  vcl_ptrdiff_t istepM=masks.istep(),jstepM=masks.jstep(),pstepM = masks.planestep();
  vcl_ptrdiff_t movesI[8] ;
  movesI[0]=istepI ;
  movesI[1]=-jstepI+istepI ;
  movesI[2]=-jstepI ;
  movesI[3]=-jstepI-istepI ;
  movesI[4]=-istepI ;
  movesI[5]=jstepI-istepI ;
  movesI[6]=jstepI ;
  movesI[7]=jstepI+istepI ;

  const T* planeI = img.top_left_ptr();
  vdtop_8_neighborhood_mask* planeM=masks.top_left_ptr() ;

  for (unsigned p=0;p<np;++p,planeI += pstepI,planeM += pstepM)
  {
    const T* rowI   = planeI, *current;
    vdtop_8_neighborhood_mask* rowM   = planeM, *current_mask;

    for (unsigned j=0;j<nj;j++,rowI+=jstepI,rowM+=jstepM)
    {
      // remove non_maximal_directions for preceding line
      current=rowI ;
      current_mask=rowM ;
      for (unsigned i=0; i<ni; ++i, current_mask+=istepM, current+=istepI)
      {
        vdtop_8_neighborhood_mask max_mask(0) ;
        int n = current_mask->t8p();
        for (int k = 0; k<n; k++) // for each component
        {
          vdtop_8_neighborhood_mask cc = current_mask->connected_8_component(k);
          int nb_neigh=cc.nb_8_neighbors() ;

          // Computes the direction to the max neighbor
          vdtop_freeman_code m=cc.direction_8_neighbor(0) ;
          const T * m_val=current+moves[m.code()] ;

          for (int l=1; l<nb_neigh; l++)
          {
            vdtop_freeman_code d=cc.direction_8_neighbor(l) ;
            const T * d_val=current+movesI[d.code()] ;
            if (*d_val>*m_val || (*d_val==*m_val && dir_order[d.code()]>dir_order[m.code()]))
            {
              m=d ; m_val=d_val ;
            }
          }
          max_mask.add_direction(m) ;
        }
        *current_mask=max_mask ;
      }
    }
  }
}

template <class T, class TMap>
void vdtop_set_veinerization_structure(TMap & res, const vil_image_view<T> & arg, vmap_2_map_tag)
{
  vil_image_view<vdtop_8_neighborhood_mask> mask ;
  // build upper mask from grey map
  compute_upper_masks(arg, mask) ;
  remove_non_maximal_direction(arg, mask) ;

  int moves[8] ;
  moves[0]=mask.istep() ;
  moves[1]=-mask.jstep()+mask.istep() ;
  moves[2]=-mask.jstep() ;
  moves[3]=-mask.jstep()-mask.istep() ;
  moves[4]=-mask.istep() ;
  moves[5]=mask.jstep()-mask.istep() ;
  moves[6]=mask.jstep() ;
  moves[7]=mask.jstep()+mask.istep() ;

  // symmetrize
  down_masks(mask) ;

  // build res
  int nb_vertices , nb_darts ;
  count_non_empty_masks(mask, nb_vertices, nb_darts);
  nb_darts*=2 ;
  res.initialise_darts(nb_darts) ;
  int dart[mask.ni()][4] ;
  for (int j=0; j<mask.ni(); j++)
  {
    dart[j][0]=dart[j][1]=dart[j][2]=dart[j][3]=-1 ;
  }
  nb_darts=nb_vertices=0 ;
  for (int j=0; j<mask.nj(); j++)
  {
    for (int i=mask.ni()-1; i>=0; --i)
    {
      int k=0 ;
      int first=-1, last=-1 ;
      while (dart[i][k]<0 && k<4) k++ ;
      if (k<4)
      {
        last=first = dart[i][k] ;
        dart[i][k]=-1 ;
        do { k++ ; } while (k<4 && dart[i][k]<0) ;
        while (k<4)
        {
          res.set_sigma(dart[i][k],last) ;
          last=dart[i][k];
          dart[i][k]=-1 ;
          do { k++ ; } while (k<4 && dart[i][k]<0) ;
        }
      }

      int n = mask(i,j).nb_8_neighbors() ;
      if (n!=0)
      {
        nb_vertices++ ;
        int k = 0 ;
        vdtop_freeman_code dir=mask(i,j).direction_8_neighbor(k) ;
        for (k = 0; k<n; dir=mask(i,j).direction_8_neighbor(++k) )
        {
          if (last!=-1)
          {
            res.set_sigma(nb_darts, last) ;
          }
          else
          {
            first=nb_darts ;
          }
          last=nb_darts++ ;
          res.set_alpha(nb_darts, last) ;
          dart[i+dir.di()][(-dir).code()]=nb_darts++ ;
        }
      }
      if (first!=-1)
      {
        res.set_sigma(last, first) ;
      }
    }
  }
}

template <class T, class TMap>
void vdtop_set_structure_from_masks(TMap & res, const vil_image_view<vdtop_8_neighborhood_mask> & mask,int plane, vmap_2_map_tag)
{
  unsigned ni = mask.ni(),nj = mask.nj(),np = mask.nplanes(), nil=ni-1, njl=nj-1;
  // Precompute steps
  vcl_ptrdiff_t istepM=masks.istep(),jstepM=masks.jstep(),pstepM = masks.planestep();
  vcl_ptrdiff_t movesM[8] ;
  movesM[0]=istepM ;
  movesM[1]=-jstepM+istepM ;
  movesM[2]=-jstepM ;
  movesM[3]=-jstepM-istepM ;
  movesM[4]=-istepM ;
  movesM[5]=jstepM-istepM ;
  movesM[6]=jstepM ;
  movesM[7]=jstepM+istepM ;

  // build res
  int nb_vertices , nb_darts ;
  count_non_empty_masks(mask, nb_vertices, nb_darts);
  nb_darts*=2 ;
  res.initialise_darts(nb_darts) ;
  int dart[mask.ni()][4] ;
  for (int j=0; j<mask.ni(); j++)
  {
    dart[j][0]=dart[j][1]=dart[j][2]=dart[j][3]=-1 ;
  }
  nb_darts=nb_vertices=0 ;

  // Sets the links
  vdtop_8_neighborhood_mask* planeM=masks.top_left_ptr() ;
  planeM += plane*pstepM ;
  {
    vdtop_8_neighborhood_mask* rowM   = planeM, *current_mask;

    for (unsigned j=0;j<nj;j++,rowM+=jstepM)
    {
      // starts from the end
      current_mask=rowM+(ni-1)*istep;
      for (int i=ni-1; i>=0; --i, current_mask-=istepM)
      {
        int k=0 ;
        int first=-1, last=-1 ;

        // darts toward 0-4 have already been intanciated
        while (dart[i][k]<0 && k<4) k++ ;
        if (k<4)
        {
          last=first = dart[i][k] ;
          dart[i][k]=-1 ;
          do { k++ ; } while (k<4 && dart[i][k]<0) ;
          while (k<4)
          {
            res.set_sigma(dart[i][k],last) ;
            last=dart[i][k];
            dart[i][k]=-1 ;
            do { k++ ; } while (k<4 && dart[i][k]<0) ;
          }
        }

        // darts between 5-7 have to be intanciated
        const vdtop_8_neighborhood_mask down_left_neighbors(0xF0) ;
        vdtop_8_neighborhood_mask tmp_mask=*current_mask ;
        tmp_mask&=down_left_neighbors ;
        int n = tmp_mask.nb_8_neighbors() ;
        if (n!=0)
        {
          nb_vertices++ ;
          int k = 0 ;
          vdtop_freeman_code dir=tmp_mask.direction_8_neighbor(k) ;
          for (k = 0; k<n; dir=tmp_mask.direction_8_neighbor(++k) )
          {
            if (last!=-1)
            {
              res.set_sigma(nb_darts, last) ;
            }
            else
            {
              first=nb_darts ;
            }
            last=nb_darts++ ;
            res.set_alpha(nb_darts, last) ;
            dart[i+dir.di()][(-dir).code()]=nb_darts++ ;
          }
        }
        if (first!=-1)
        {
          res.set_sigma(last, first) ;
        }
      }
    }
  }
}

template <class T, class TMap>
void vdtop_set_veinerization_structure(TMap & res, vil_image_view<T> & img, const T& max_value, vmap_2_tmap_tag)
{
  vil_image_view<vdtop_8_neighborhood_mask> mask ;

  // build upper mask from grey map
  vdtop_compute_8_upper_directions(img, max_value, mask) ;

  int moves[8] ;
  moves[0]=mask.istep() ;
  moves[1]=-mask.jstep()+mask.istep() ;
  moves[2]=-mask.jstep() ;
  moves[3]=-mask.jstep()-mask.istep() ;
  moves[4]=-mask.istep() ;
  moves[5]=mask.jstep()-mask.istep() ;
  moves[6]=mask.jstep() ;
  moves[7]=mask.jstep()+mask.istep() ;

  // symmetrize
  down_masks(mask) ;

  // build res
  int nb_vertices , nb_darts ;
  count_non_empty_masks(mask, nb_vertices, nb_darts);
  nb_darts*=2 ;
  res.initialise_darts(nb_darts) ;
  int dart[mask.ni()][4] ;
  int vertex_first_dart[mask.ni()] ;
  for (int j=0; j<mask.ni(); j++)
  {
    dart[j][0]=dart[j][1]=dart[j][2]=dart[j][3]=-1 ;
  }
  nb_darts=nb_vertices=0 ;
  for (int j=0; j<mask.nj(); j++)
  {
    for (int i=mask.ni()-1; i>=0; --i)
    {
      int k=0 ;
      int first=-1, last=-1 ;
      while (dart[i][k]<0 && k<4) k++ ;
      if (k<4)
      {
        last=first = dart[i][k] ;
        dart[i][k]=-1 ;
        do { k++ ; } while (k<4 && dart[i][k]<0) ;
        while (k<4)
        {
          res.set_sigma(dart[i][k],last) ;
          last=dart[i][k];
          dart[i][k]=-1 ;
          do { k++ ; } while (k<4 && dart[i][k]<0) ;
        }
      }

      int n = mask(i,j).nb_8_neighbors() ;
      if (n!=0)
      {
        int k = 0 ;
        vdtop_freeman_code dir=mask(i,j).direction_8_neighbor(k) ;
        for (k = 0; k<n; dir=mask(i,j).direction_8_neighbor(++k) )
        {
          if (last!=-1)
          {
            res.set_sigma(nb_darts, last) ;
          }
          else
          {
            first=nb_darts ;
          }
          last=nb_darts++ ;
          res.set_alpha(nb_darts, last) ;
          dart[i+dir.di()][(-dir).code()]=nb_darts++ ;
        }
      }
      if (first!=-1)
      {
        res.set_sigma(last, first) ;
      }
      vertex_first_dart[i]=first ;
    }
    for (int i=0; i<mask.ni(); ++i)
    {
      if (vertex_first_dart[i]>=0)
      {
        res.set_vertex(vertex_first_dart[i], nb_vertices) ;
        nb_vertices++ ;
      }
    }
  }
  vcl_cout<<"V:"<<arg.ni()*arg.nj()<<'/'<<nb_vertices<<vcl_endl ;
}

#endif // vdtop_misc_txx_
