#include<bil/algo/bil_cedt.h>
#include<vbl/vbl_array_2d.h>
#include <vcl_cmath.h>
bil_cedt::bil_cedt()
{
}
bil_cedt::~bil_cedt()
{
}

bil_cedt::bil_cedt(vil_image_view<unsigned char> im){
    img_=im;
    ni_=img_.ni();
    nj_=img_.nj();

    dx_.resize(nj_,ni_);
    dy_.resize(nj_,ni_);
    dist_.set_size(ni_,nj_);
}

bool bil_cedt::compute_cedt()
{


    vbl_array_2d<double>        level(nj_,ni_);

    for(int j=0; j<nj_; j++) {
        for(int i=0; i<ni_; i++) {
            if(img_(i,j)==0)
                level(j,i)=0;
            else
                level(j,i)=1;
        }
    }
    bil_cedt_heap * heap=new bil_cedt_heap(nj_,ni_);

    /* initialize the heap for positive values */
    find_dist_trans(level, heap);


    for(unsigned j=0;j<dist_.nj();j++)
      for(unsigned i=0;i<dist_.ni();i++)
          dist_(i,j)=vcl_sqrt((float)(dx_(j,i)*dx_(j,i)+dy_(j,i)*dy_(j,i)));
      


    
    return true;
}


bool bil_cedt::find_dist_trans(vbl_array_2d<double> &level,
                                 bil_cedt_heap *heap)
     
{
  int yy,xx,i,ii,j,dir;
  double dist,dist_x,dist_y;

  bil_cedt_contour pc(nj_,ni_);
  bil_cedt_contour pf(nj_,ni_);
  
  

  vbl_array_2d<unsigned char>   dir_array(nj_,ni_);
  vbl_array_2d<signed char>     flag_array(nj_,ni_);
  vbl_array_2d<unsigned char>   tag_array(nj_,ni_);
  vbl_array_2d<double>          surface(nj_,ni_);

  
  pc.ptr=0;

  for(int y=0; y<nj_; y++) {
    for(int x=0; x<ni_; x++) {

      tag_array(y,x) = 2;

      if (level(y,x) == 0.0) {
        pc.x[pc.ptr] =  x;
        pc.y[pc.ptr] =  y;
        pc.ptr++;
        
        surface(y,x) = 0.0;
        dx_(y,x) = 0.0;
        dy_(y,x) = 0.0;
        tag_array(y,x) = 0;
        flag_array(y,x) = 0;
      }
      else {
        surface(y,x) = 99999.0;
        flag_array(y,x) = -1;
        dx_(y,x) = 99999.0;
        dy_(y,x) = 99999.0;
      }
      dir_array(y,x) = 17;
    }
  }
  
  

  ii = 0; pf.ptr =0;
  for(ii=0; ii<pc.ptr; ii++) {
      int y = pc.y[ii];
      int x = pc.x[ii];
      tag_array(y,x) = 0;
      for(i=-1; i<2; i++) {
          for(j=-1; j<2; j++) {
              yy = y+i; xx = x+j;
              if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
                  if (tag_array(yy,xx) != 0.0) {
                      dist_x = dx_(y,x) + (double) j;
                      dist_y = dy_(y,x) + (double) i;
                      dist = dist_x*dist_x+dist_y*dist_y;
                      if (surface(yy,xx) > dist)  {
                          surface(yy,xx) = dist;
                          dx_(yy,xx) = dist_x;
                          dy_(yy,xx) = dist_y;
                          tag_array(yy,xx) = 1;
                          dir =  initial_direction(j, i);
                          if (dir_array(yy,xx) == 17) {
                              pf.x[pf.ptr] = xx;
                              pf.y[pf.ptr] = yy;
                              pf.dir[pf.ptr] = dir;
                              pf.ptr++;
                          }
                          dir_array(yy,xx) = dir;
                      }
                  }
          }
          }
      }
  }

  


  /* correct the directions since dir_array contains right directions */
  for(int ii=0; ii<pf.ptr; ii++) {
    int y = pf.y[ii];
    int x = pf.x[ii];
    int dir = dir_array(y,x);
    pf.dir[ii] =dir;
  }


  
  /* horizontal and vertical directions */
  pc.ptr =0; heap->N=0; heap->end =1;
  for(int ii=0; ii<pf.ptr; ii++) {
    int y = pf.y[ii];
    int x = pf.x[ii];
    int dir = pf.dir[ii];
    if (tag_array(y,x) != 0.0) {
      if (dir == 0 || dir == 2 || dir == 4 || dir == 6) {
        propagate_dist(&pc, heap,surface, tag_array, dir_array,y, x, dir);
      }
    }
  }

  /* diagonal directions */
  for(int ii=0; ii<pf.ptr; ii++) {
    int y = pf.y[ii];
    int x = pf.x[ii];
    dir = pf.dir[ii];
    if (tag_array(y,x) != 0.0) {
      if (dir == 1 || dir == 3 || dir == 5 || dir == 7) {
      initial_diagonal_propagate(&pc,heap, surface, tag_array, dir_array,y, x,dir);
      }
    }
  }

  /* start the growing from the contour */

  while(heap->N > 0) {
    int x = heap->locx[heap->index[1]];
    int y = heap->locy[heap->index[1]];
    heap->remove_max();
    dir = dir_array(y,x);
    pc.ptr=0;
    //if (x > 0 && x<(ni_-1) && y>0 && y<(nj_-1)) {
      if (tag_array(y,x) != 0.0) {
        propagate_dist(&pc,heap,surface, tag_array, dir_array, y, x, dir);
      }
    //}
  }


  return true;

}


int bil_cedt:: initial_direction(int x, int y)
     
{
  if (x == 1 &&  y == 0) 
    return 0;
  else if (x == 1 &&  y == -1) 
    return 1;
  else if (x == 0 &&  y == -1) 
    return 2;
  else if (x == -1 &&  y == -1) 
    return 3;
  else if (x == -1 &&  y == 0) 
    return 4;
  else if (x == -1 &&  y == 1) 
    return 5;
  else if (x == 0 &&  y == 1) 
    return 6;
  else if (x == 1 &&  y == 1) 
    return 7;
  return -1;
    
}


 
//: function to propagate distance
void bil_cedt::propagate_dist(bil_cedt_contour *pf, bil_cedt_heap *heap, vbl_array_2d<double> &surface, 
                    vbl_array_2d<unsigned char> &tag_array, vbl_array_2d<unsigned char> &dir_array, 
                    int y, int x, int dir)

{
  double dist_x, dist_y,dist;
  int position,yy,xx;

  
  switch (dir) { 
  case (0): {
    yy = y; xx =x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (1): {
    yy = y-1; xx =x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (2): {
    yy = y-1; xx =x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (3): {
    yy = y-1; xx =x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (4): {
    yy = y; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (5): {
    yy = y+1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
     add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (6): {
    yy = y+1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist) 
     add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (7): {
    yy = y+1; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)  
  add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (8): {
    yy = y; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)   
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    yy = y-1; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)    
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (9): {
    yy = y-1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)    
     add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    yy = y-1; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)    
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (10): {
    yy = y-1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)    
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }

    yy = y-1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)    
     add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (11): {
    yy = y-1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)     
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    yy = y; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)     
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (12): {
    yy = y; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)      
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    yy = y+1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)       
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (13): {
    yy = y+1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)      
     add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    yy = y+1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)       
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);   
    }
    break;
  }
  case (14): {
    yy = y+1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)       
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    yy = y+1; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) > dist)        
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, dir, position);
    }
    break;
  }
  case (15): {
      yy = y; xx = x+1;
      if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
      position = yy*ni_+xx;
      dist_x = dx_(y,x)+1.0;
      dist_y = dy_(y,x);
      dist = dist_x*dist_x+dist_y*dist_y;
      if (surface(yy,xx) > dist)       
          add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist,yy, xx, dir, position);
      }
      yy = y+1; xx = x+1;
      if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
      position = yy*ni_+xx;
      dist_x = dx_(y,x)+1.0;
      dist_y = dy_(y,x)+1.0;
      dist = dist_x*dist_x+dist_y*dist_y;
      if (surface(yy,xx) > dist)       
          add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist,yy, xx, dir, position);
      }
      break;
             }
  }
  

}

//: function to propagate the intial  diagonal elements
void bil_cedt::initial_diagonal_propagate(bil_cedt_contour *pf, bil_cedt_heap *heap, vbl_array_2d<double> &surface, 
                                           vbl_array_2d<unsigned char> &tag_array, vbl_array_2d<unsigned char> &dir_array, 
                                           int y, int x, int dir)
{
  double dist_x, dist_y,dist;
  int position,yy,xx;

  switch (dir) { 
  case (1): {
    yy = y; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)       
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 8, position);
    }
    yy = y-1; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)       
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 1, position);
    }
    yy = y-1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)       
      add_to_contour(pf, heap, surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 9, position);
    }
    break;
  }
  case (3): {
    yy = y-1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)        
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 10, position);
    }
    yy = y-1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)-1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)         
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 3, position);
    }
    yy = y; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)         
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 11, position);
    }
    break;
  }
  case (5): {
    yy = y; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)          
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 12, position);
    }
    yy = y+1; xx = x-1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x)-1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)           
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 5, position);
    }
    yy = y+1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)            
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 13, position);
    }
    break;
  }
  case (7): {
    yy = y+1; xx = x;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;
    dist_x = dx_(y,x);
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)          
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 14, position);
    }
    yy = y+1; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx;  
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x)+1.0;
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)          
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 7, position);
    }
    yy = y; xx = x+1;
    if(yy>=0 && yy<nj_ && xx>=0 && xx<ni_){
    position = yy*ni_+xx; 
    dist_x = dx_(y,x)+1.0;
    dist_y = dy_(y,x);
    dist = dist_x*dist_x+dist_y*dist_y;
    if (surface(yy,xx) >= dist)           
      add_to_contour(pf, heap,surface, tag_array, dir_array,dist_x, dist_y,dist, 
                     yy, xx, 15, position);
    }
    break;
  }
  }
}



void bil_cedt::add_to_contour(bil_cedt_contour *pf, bil_cedt_heap *heap, vbl_array_2d<double> &surface, 
                               vbl_array_2d<unsigned char> &tag_array, vbl_array_2d<unsigned char> &dir_array,
                               double dist_x, double dist_y, double dist,int y, int x, int dir, int position)

{
  
 
  surface(y,x) = dist;
  dx_(y,x) = dist_x;
  dy_(y,x) = dist_y;
  tag_array(y,x) = 1; 
  pf->x[pf->ptr] =  x;
  pf->y[pf->ptr] =  y;
  pf->dir[pf->ptr] = dir;
  dir_array(y,x) = dir;
  pf->ptr++;
  heap->locx[heap->end] =  x;
  heap->locy[heap->end] =  y;
  heap->insert(heap->end,position,dist);
}






//: heap to keep track of updated contour
bil_cedt_heap::bil_cedt_heap(int nj,int ni)
{
    nj_=nj;
    ni_=ni;
    
    data = new double[2*nj*ni];
    index = new int[2*nj*ni];
    rank = new int[2*nj*ni];
    loc = new int[2*nj*ni];
    locx= new int[2*nj*ni];
    locy= new int[2*nj*ni];
    
    for(int i=0;i<2*nj*ni;i++)
    {
        data[i]=0;
        index[i]=0;
        rank[i]=0;
        loc[i]=0;
        locx[i]=0;
        locy[i]=0;
    }

    N=0;

}
bil_cedt_heap::~bil_cedt_heap()
{

}
void bil_cedt_heap::print_heap()
     
{
    for(int i=1; i<=N; i++) {
      int x = locx[index[i]];
      int y = locy[index[i]];
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
      int pos = y*ni_+x;
#endif

      vcl_cout<<"i ="<<i<<" index[i]= "<<index[i]<< " x= "<<x<<" y= "<<y<<"\n";
      
    }
  
}






void bil_cedt_heap::upheap( int k) 
     
{ 
  
  int v = index[k];   
  data[0] = 0.0;

  while (data[index[(int) (k/2.0)]] >= data[v]) {
    index[k] = index[(int)(k/2.0)]; k = (int) (k/2.0);
  }
  index[k] = v; 
 
}

void bil_cedt_heap::insert( int pos, int location, double item)
     
{
  
  N++;
  data[pos] = item; 
  index[N] = pos; 
  loc[location] = pos;
  upheap(N);
  end++;
  
}

void bil_cedt_heap::downheap(int k) 
     
{ 
  int j; 
  
  int v = index[k];

  while (k <= (int) N/2.0) {
    j = k+k;
    if (j < N && data[index[j]] > data[index[j+1]])
        j++;
    if (data[v] <= data[index[j]]) 
        break;
    index[k] = index[j]; 
    k = j;
  }
  index[k] = v; 
}

void bil_cedt_heap::remove_max()
{
  
  int v = index[1];
  data[v] = data[index[N]];
  locx[v] = locx[index[N]];
  locy[v] = locy[index[N]];
  
  int y = locy[v];
  int x = locx[v];
  loc[y*ni_+x] = v;
  N--;
  downheap(1);
}
//: updated contour
bil_cedt_contour::bil_cedt_contour(int nj,int ni)

{
  x= new int[nj*ni];
  y= new int[nj*ni];
  dir= new int[nj*ni];
  
}



bil_cedt_contour::~bil_cedt_contour()
{
    delete[] x;
    delete[] y;
    delete[] dir;

}


