// This is gel/vsrl/vsrl_step_diffusion.cxx
#include "vsrl_step_diffusion.h"
#include <vcl_iostream.h>

vsrl_step_diffusion::vsrl_step_diffusion(vsrl_dense_matcher *matcher):
  vsrl_diffusion(matcher)
{
}

vsrl_step_diffusion::~vsrl_step_diffusion()
{
}


void vsrl_step_diffusion::set_initial_disparity()
{
  // we want to set the initial disparity

  for (int x=0;x<get_width();x++)
  {
    for (int y=0;y<get_height();y++)
    {
      int d = matcher_->get_disparity(x,y);
      double dd =d;
      (*disparity_matrix_)(x,y)=dd;
    }
  }
}

void vsrl_step_diffusion::interpolate_disparity()
{
  // we want to be able to interpolate the disparity
  int x,y;
  double v;
  int x1,x2;
  double d,d1=0.0,d2=0.0;
  double alpha,len1,len2;
  int flag1, flag2;
  int low_val=0-99;

  // find all points do not have a defined disparity

  for (y=0;y<get_height();y++)
  {
    for (x=0;x<get_width();x++)
    {
      v = (*disparity_matrix_)(x,y);

      if (v<low_val)
      {
        // this is a non valid point
        // find the first valid point to the left of x

        flag1=0;
        x1=x-1;
        while (x1>=0 && !flag1)
        {
          d1=(*disparity_matrix_)(x1,y);
          if (d1>low_val)
          {
            flag1=1;
          }
          else
          {
            x1=x1-1;
          }
        }

        // find the first valid point to the right of x

        flag2=0;
        x2=x+1;
        while (x2<get_width() && !flag2)
        {
          d2=(*disparity_matrix_)(x2,y);
          if (d2>low_val)
          {
            flag2=1;
          }
          else
          {
            x2=x2+1;
          }
        }

        if (flag1 && flag2)
        {
          // OK we hade two points to interopolate
          // from - lets doit

          len1 = x-x1;
          len2 = x2-x1;
          alpha = len1/len2;
          d= (1.0-alpha)*d1 + (alpha)*d2;
          (*disparity_matrix_)(x,y)=d;
        }
      }
    }
  }

  for (y=0;y<get_height();y++)
  {
    for (x=0;x<get_width();x++)
    {
      v = (*disparity_matrix_)(x,y);
      if (v<low_val)
      {
        (*disparity_matrix_)(x,y)=0.0;
      }
    }
  }
}


void vsrl_step_diffusion::clear_borders(int width)
{
  // we want to put the border disparities to 0


  int x,y;

  // find all points do not have a defined disparity

  for (y=0;y<get_height();y++)
  {
    for (x=0;x<width;x++)
      (*disparity_matrix_)(x,y)=0;

    for (x=get_width()-width;x<get_width();x++)
      (*disparity_matrix_)(x,y)=0;
  }
}


void vsrl_step_diffusion::diffuse_disparity()
{
  // OK we now want to use a diffusion operator to try
  // and smooth over the steps that seem to crop up
  // all over the place.

  // OK the idea is to anchor the diffusion algorithm
  // so that steps of more than 1 pixel are maintianed.
  // We then start the diffusion process to smooth things
  // out.

  vcl_cout << "Starting to diffuse\n";

  vnl_matrix<double> mat1= (*disparity_matrix_);
  vnl_matrix<double> T1 = mat1;
  vnl_matrix<double> T2 = mat1;

  vnl_matrix<double> mat2= mat1;

  vnl_matrix<double> *mstar1 = (&mat1);
  vnl_matrix<double> *mstar2 = (&mat2);
  vnl_matrix<double> *hold;

  // compute upper and lower thresholds

  int x,y;

  for (x=0;x<get_width();x++)
  {
    for (y=0;y<get_height();y++)
    {
      T1(x,y)=T1(x,y) - 1;
      T2(x,y)=T2(x,y) + 1;
    }
  }

  double N,sum;

  // OK start to diffuse

  int dif_num;
  int i,j;
  double val;
  double low,hi;

   for (dif_num=0;dif_num<100;dif_num++)
   {
     vcl_cout << "Iteration " << dif_num << "\r";

     for (x=1;x<get_width() -1;x++)
     {
       for (y=1;y<get_height() - 1;y++)
       {
         // get the average value of mat1(x,y)'s neighborhood
         N=0;
         sum=0;

         // what are the intiail limits?

         low=T1(x,y);
         hi=T2(x,y);

         for (i=x-1;i<x+2;i++)
         {
           for (j=y-1;j<y+2;j++)
           {
             val = (*mstar1)(i,j);
             if (val>=low && val <= hi)
             {
               // this is not a major jump so
               // diffuse naturally

               sum=sum+val;
               N++;
             }
           }
         }
         sum=sum/N;
         // store the diffused value
         (*mstar2)(x,y)=sum;
       }
     }


     // OK Swap mstar1 and mstar2
     hold=mstar1;
     mstar1=mstar2;
     mstar2=hold;
   }

   // copy the new results
   (*disparity_matrix_)=(*mstar1);

   vcl_cout << vcl_endl << "Finished the diffusion" << vcl_endl;
}

void vsrl_step_diffusion::execute()
{
  // execute the disparity stuff

  // set the initial disparity
  set_initial_disparity();

  // interpolate the missing data
  interpolate_disparity();

  // run a diffusion algorithm

   diffuse_disparity();
}
