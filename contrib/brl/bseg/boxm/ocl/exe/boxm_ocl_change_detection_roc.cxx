// Main to run opencl implementation of online update

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>


static void roc_numbers(vil_image_view<float> detection,
                        vil_image_view<unsigned char> gt,
                        vil_image_view<unsigned char> mask,
                        float threshold,
                        int& tp, int & tn, int & fp , int & fn)
{
    vil_image_view<float>::iterator detiter=detection.begin();
    vil_image_view<unsigned char>::iterator gtiter=gt.begin();
    vil_image_view<unsigned char>::iterator maskiter=mask.begin();

    tp=0;tn=0;fp=0;fn=0;

    bool pos=false;
    bool truth=false;
    for (;detiter!=detection.end();detiter++,gtiter++,maskiter++)
    {
        if ((*maskiter)!=255)
        {
            if ((*detiter)>=threshold)
                pos=true;
            else
                pos=false;
            if ((*gtiter)>0)
                truth=true;
            else
                truth=false;

            if (pos && truth)    tp++;
            if ((!pos) && truth) fn++;
            if ((!pos) && (!truth))  tn++;
            if (pos && (!truth))  fp++;
        }
    }
}


int main(int argc,  char** argv)
{
  vul_arg<vcl_string> detdir("-detdir", "directory for change detections", "");
  vul_arg<vcl_string> gtdir("-gtdir", "directory for change detection ground truth", "");
  vul_arg<vcl_string> maskimg("-maskimg", "Universal Mask Image", "");
  vul_arg<vcl_string> rocoutfile("-roc", "Output file for Roc", "");
  vul_arg_parse(argc, argv);

  vcl_string detglob=detdir()+"/*.tiff";
  vcl_string gtglob=gtdir()+"/*.tiff";

  vul_file_iterator det_it(detglob.c_str());
  vul_file_iterator gt_it(gtglob.c_str());

  vcl_vector<vcl_string> det_files;
  vcl_vector<vcl_string> gt_files;
  while (det_it && gt_it) {
      vcl_string detname(det_it());
      vcl_string gtname(gt_it());
      det_files.push_back(detname);
      gt_files.push_back(gtname);
      ++det_it; ++gt_it;
  }
  vcl_sort(det_files.begin(), det_files.end());
  vcl_sort(gt_files.begin(), gt_files.end());
  if (det_files.size() != gt_files.size()) {
      vcl_cerr<<"Image files and cam files not one to one\n";
      return -1;
  }

  vil_image_view_base_sptr mask_ptr=vil_load(maskimg().c_str());

  if (vil_image_view<unsigned char> * mask=dynamic_cast<vil_image_view<unsigned char> *> (mask_ptr.ptr()))
  {
      float tpcum[10]={0};float tncum[10]={0};
      float fpcum[10]={0};float fncum[10]={0};

      for (unsigned i=0;i<det_files.size();i++)
      {
          vil_image_view_base_sptr det_ptr=vil_load(det_files[i].c_str());
          if (vil_image_view<float> * det=dynamic_cast<vil_image_view<float> *> (det_ptr.ptr()))
          {
              vil_image_view_base_sptr gt_ptr=vil_load(gt_files[i].c_str());
              if (vil_image_view<unsigned char> * gt=dynamic_cast<vil_image_view<unsigned char> *> (gt_ptr.ptr()))
              {
                  int cnt=0;
                  for (float t=0.1f;t<=1.05f;++cnt,t+=0.1f)
                  {
                      int tp=0,tn=0,fn=0,fp=0;
                      roc_numbers(*det,*gt,*mask,t,tp,tn,fp,fn);
                      //vcl_cout<<'['<<tp<<','<<tn<<','<<fp<<','<<fn<<"] ";
                      tpcum[cnt]+=(float)tp;
                      fpcum[cnt]+=(float)fp;
                      tncum[cnt]+=(float)tn;
                      fncum[cnt]+=(float)fn;
                  }
              }
          }
      }

      for (unsigned l=0;l<10;l++)
          vcl_cout<<tpcum[l]/(tpcum[l]+fncum[l])<<' '<<1-tncum[l]/(fpcum[l]+tncum[l])<<vcl_endl;
        //vcl_cout<<'['<<tpcum[l]<<','<<tncum[l]<<','<<fpcum[l]<<','<<fncum[l]<<']'<<vcl_endl;
  }
  int a=0;
  vcl_cin>>a;
  return 0;
}

