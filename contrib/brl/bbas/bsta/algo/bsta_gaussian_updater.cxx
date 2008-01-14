
#include<bsta/algo/bsta_gaussian_updater.h>
#include<bsta/bsta_gaussian_angles_1d.h>
void
bsta_update_gaussian(bsta_gaussian_angles_1d & gaussian, float rho,
                       const float& sample,float min_var )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  float rho_comp = 1.0f - rho;
  // compute the updated mean
  const float& old_mean = gaussian.mean();

  float diff=gaussian.difference(sample,old_mean);
  float newmean=gaussian.sum(old_mean,rho * diff);
  float new_var=rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * vcl_max(diff*diff,min_var);


  if(new_var<min_var)
      gaussian.set_var(min_var);
  else
      gaussian.set_var(new_var);
  gaussian.set_mean(newmean);

  //float rho_comp = 1.0 - rho;
  //// compute the updated mean
  //const float& old_mean = gaussian.mean();

  //float diff=sample - old_mean;

  //float new_covar=rho_comp * gaussian.var();
  //new_covar += (rho * rho_comp) * diff*diff;

  //gaussian.set_var(new_covar);
  //gaussian.set_mean((rho_comp * old_mean) +  (rho * sample));
};
void
bsta_update_gaussian(bsta_gaussian_angles_1d & gaussian, float rho,
                       const float& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  float rho_comp = 1.0f - rho;
  // compute the updated mean
  const float& old_mean = gaussian.mean();

  float diff=gaussian.difference(sample,old_mean);
  float newmean=gaussian.sum(rho_comp * old_mean,rho * sample);
  float new_var=rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * diff*diff;

  gaussian.set_var(new_var);
  gaussian.set_mean(newmean);

};

void
bsta_update_gaussian(bsta_gaussian_x_y_theta & gaussian, float rho,
                       const vnl_vector_fixed<float,3>& sample,float min_var )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  float rho_comp = 1.0f - rho;
  // compute the updated mean
  const vnl_vector_fixed<float,3>& old_mean = gaussian.mean();

  vnl_vector_fixed<float,3> diff(0.0);
  diff[0]=(sample[0] - old_mean[0]);
  diff[1]=(sample[1] - old_mean[1]);
  diff[2]=gaussian.difference(sample[2], old_mean[2]);

  vnl_matrix_fixed<float,3,3> new_covar(rho_comp * gaussian.covar());

  new_covar+=rho*outer_product<float,3>(diff,diff);
  //new_covar(0,0)+=(rho )*vcl_max(diff[0]*diff[0],min_var);
  //new_covar(0,1)+=(rho )*vcl_max(diff[0]*diff[1],min_var);
  //new_covar(1,0)+=(rho )*vcl_max(diff[0]*diff[1],min_var);
  //new_covar(1,1)+=(rho )*vcl_max(diff[1]*diff[1],min_var);


  //new_covar(2,2)+=(rho )*vcl_max(diff[2]*diff[2],min_var);

  //new_covar += (rho * rho_comp) * outer_product(diff,diff);
  gaussian.set_covar(new_covar);
  vnl_vector_fixed<float,3> newmean(0.0);
  newmean[0]=(old_mean[0]) +  (rho * diff[0]);
  newmean[1]=(old_mean[1]) +  (rho * diff[1]);

  newmean[2]=gaussian.sum(old_mean[2] , (rho * diff[2]));
  gaussian.set_mean(newmean);
};
void
bsta_update_gaussian(bsta_gaussian_x_y_theta & gaussian, float rho,
                       const vnl_vector_fixed<float,3>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  float rho_comp = 1.0f - rho;
  // compute the updated mean
  const vnl_vector_fixed<float,3>& old_mean = gaussian.mean();

  vnl_vector_fixed<float,3> diff(0.0);
  diff[0]=(sample[0] - old_mean[0]);
  diff[1]=(sample[1] - old_mean[1]);
  diff[2]=gaussian.difference(sample[2], old_mean[2]);

  vnl_matrix_fixed<float,3,3> new_covar(rho_comp * gaussian.covar());
  new_covar(0,0)+=(rho * rho_comp)*diff[0]*diff[0];
  new_covar(0,1)+=(rho * rho_comp)*diff[0]*diff[1];
   new_covar(1,0)+=(rho * rho_comp)*diff[0]*diff[1];
   new_covar(1,1)+=(rho * rho_comp)*diff[1]*diff[1];

  
   new_covar(2,2)=(rho * rho_comp)*diff[2]*diff[2];

   gaussian.set_covar(new_covar);
   vnl_vector_fixed<float,3> newmean(0.0);
   newmean[0]=(old_mean[0]) +  (rho * diff[0]);
   newmean[1]=(old_mean[1]) +  (rho * diff[1]);

   newmean[2]=gaussian.sum(old_mean[2] , (rho * diff[2]));
   gaussian.set_mean(newmean);


};
