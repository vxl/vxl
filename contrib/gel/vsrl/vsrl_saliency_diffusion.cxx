// This is gel/vsrl/vsrl_saliency_diffusion.cxx
#include "vsrl_saliency_diffusion.h"
#include <vcl_iostream.h>

vsrl_saliency_diffusion::vsrl_saliency_diffusion(vsrl_dense_matcher *matcher):
  vsrl_diffusion(matcher)
{
  _saliency=0;
  _disparity_matrix->fill(0.0);
}

vsrl_saliency_diffusion::~vsrl_saliency_diffusion()
{
}

void vsrl_saliency_diffusion::set_initial_disparity(vsrl_diffusion *step_diff)
{
  // we want to set the initial disparity

  for (int x=0;x<_width;x++)
  {
    for (int y=0;y<_height;y++)
    {
      double dd = step_diff->get_disparity(x,y);
      (*_disparity_matrix)(x,y)=dd;
    }
  }
}

void vsrl_saliency_diffusion::set_saliency(vsrl_token_saliency *saliency)
{
  // set the object that knows whether or not a pixel is salient
  _saliency = saliency;
}

void vsrl_saliency_diffusion::difuse_disparity(int num_iter)
{
  // OK we want to use diffusion so that disparity
  // will propogate from stong regions of salienct
  // to regions of low disparity

  vcl_cout << "Starting to diffuse" << vcl_endl;

  vnl_matrix<double> mat1= (*_disparity_matrix);
  vnl_matrix<double> mat2= mat1;

  vnl_matrix<double> *mstar1 = (&mat1);
  vnl_matrix<double> *mstar2 = (&mat2);
  vnl_matrix<double> *hold;

  // OK start to diffuse

   for (int dif_num=0;dif_num<num_iter;dif_num++)
   {
     vcl_cout << "Saliency Diffusion Iteration " << dif_num << vcl_endl;
     vcl_cout << " disparity for pixel 700,353 is " << (*mstar1)(700,353) << vcl_endl;

     // make an image of the current disparity

     write_image("/projects/IUP2/peter_tu/tmp/fig",dif_num,mstar1);

     // start to difuse

     int dif_range = 5; // The range that we are will to diffuse over

     for (int x=dif_range;x<_width -dif_range;x++)
     {
       for (int y=dif_range;y<_height - dif_range;y++)
       {
         // only process non salient pixels

         if (!(_saliency->get_saliency(x,y)))
         {
           // get the average value of mat1(x,y)'s neighborhood
           int N=0;
           double sum=0;

           for (int i=x-dif_range;i<x+dif_range+1;i++)
           {
             for (int j=y-dif_range;j<y+dif_range+1;j++)
             {
               double val = (*mstar1)(i,j);
               if (val!=1000)
               {
                 // this pixel has some information to offer so diffuse naturally
                 sum+=val;
                 N++;
               }
             }
           }
           if (N!=0)
           {
             // we have some information: store the difused value
             (*mstar2)(x,y)=sum/N;
           }
         }
       }
     }

     // OK Swap mstar1 and mstar2
     hold=mstar1;
     mstar1=mstar2;
     mstar2=hold;
   }

   // get rid of the pockets of unitialized data
   for (int x=0;x<_width;x++)
   {
     for (int y=0;y<_height;y++)
     {
       double val = (*mstar1)(x,y);
       if (val==1000)
         (*mstar1)(x,y)=0;
     }
   }


   // copy the new results
   (*_disparity_matrix)=(*mstar1);

   vcl_cout << "Finished the diffusion " << vcl_endl;
}

void vsrl_saliency_diffusion::execute(int num_iter)
{
  // execute the disparity stuff

  // modify the intitial disparity_matrix using the results of
  // the saliency object

  if (_saliency)
    consider_saliency();

  // run a diffusion algorithm
  difuse_disparity(num_iter);
}

void vsrl_saliency_diffusion::consider_saliency()
{
  // each pixel is marked if it is not salient

  for (int x=0;x<_width;x++)
    for (int y=0;y<_height;y++)
      if (!(_saliency->get_saliency(x,y)))
        // this pixel is not salient so mark it with 1000 disparity
        (*_disparity_matrix)(x,y)=1000; // this value will not be used for diffusion
}
