#include <bdgl\utils.h>

#define ARCLENSAMPLE 0.1
#define SMALL_VALUE 1E-5
#define M_PI vnl_math::pi

double maxof(double a, double b,double c){
  if (a>b && a>c)
    return a;  
  else if (b>c && b>a)
    return b;
  else if (c>b && c>a)
    return c;
  else
    return a;
}
double minof(double a, double b){
  if (a<b )
    return a;  
  else 
    return b;
 
}
int almostEqual(double a, double b,double c){
  if (vcl_fabs(a-b)<c)
    return 1;
  else
    return 0;
}

int almostEqual(double a, double b){
  if (vcl_fabs(a-b)<SMALL_VALUE)
    return 1;
  else
    return 0;
}

double fixAngleMPiPi(double a){
  if (a < -M_PI)
    return a+2*M_PI;
  else if (a > M_PI)
    return a-2*M_PI;
  else
    return a;
}

double fixAngleZTPi(double a){
  if (a < 0)
    return a+2*M_PI;
  else if (a > 2*M_PI)
    return a-2*M_PI;
  else
    return a;
}

//Does a1-a2
double angleDiff(double a1, double  a2){
  a1=fixAngleMPiPi(a1);
  a2=fixAngleMPiPi(a2);
  if (a1 > a2)
    if (a1-a2 > M_PI)
      return a1-a2-2*M_PI;
    else
      return a1-a2;
  else if (a2 > a1)
    if (a1-a2 < -M_PI)
      return a1-a2+2*M_PI;
    else
      return a1-a2;
  return 0.0;
}


double angleAdd(double a1, double  a2){
  double a=a1+a2;
  
  if (a > M_PI)
    return a-2*M_PI;
  if (a < -M_PI)
    return a+2*M_PI;
  else
    return a;
}

double pointDist(vgl_point_2d<double> a ,vgl_point_2d<double> b){
  return vcl_sqrt(vcl_pow(a.x()-b.x(),2.0)+vcl_pow(a.y()-b.y(),2.0));
}


vcl_vector<double> smoothVector(vcl_vector<double> a, vcl_vector<double> kernel){
  
  int N=kernel.size();
  int M=a.size();
  int mid=(N-1)/2;
  vcl_vector<double> sa(M);

  for (int i=0;i<M;i++){
    sa[i]=0;
    for (int j=-mid;j<=mid;j++){
      double b;
      if (i-j < 0){
	b=angleFixForAdd(a[i],a[0]);
	sa[i]+=(kernel[j+mid]*b);
      }
      else if (i-j >= a.size()){
	b=angleFixForAdd(a[i],a[M-1]);
	sa[i]+=(kernel[j+mid]*b);
      }
      else{
	b=angleFixForAdd(a[i],a[i-j]);
	sa[i]+=(kernel[j+mid]*b);
      }

    }
  }
  return sa;
}

double angleFixForAdd(double ref, double  a){

  double d=a-ref;
  double b=a;

  if (d > M_PI)
    b=a-2*M_PI;
  else if (d<-M_PI)
    b=a+2*M_PI;
  else
    b=a;
  return b;
}


