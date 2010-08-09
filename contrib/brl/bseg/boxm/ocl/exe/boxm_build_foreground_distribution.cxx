// Main to run opencl implementation of onlineupdate
#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <bsta/bsta_histogram.h>


int main(int argc,  char** argv)
{
  vul_arg<vcl_string> neighborhoodfile("-file", "neighborhoods filename", "");

  vul_arg<vcl_string> outfile("-ofile", "output filename", "");
  vul_arg_parse(argc, argv);
  // load the camera;
  vcl_ifstream ifs(neighborhoodfile().c_str());
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << neighborhoodfile() << vcl_endl;
    return -1;
  }

  vcl_string tag;
  int ni,nj;
  ifs>>tag>>ni>>nj;

  if (tag!="dim:")
    return -1;
  int ncorrs;
  ifs>>tag>>ncorrs;
  if (tag!="n_tracks:")
    return -1;


  bsta_histogram<float> hist(0.0f,1.0f,20,0.0);
  char buffer[1024];
  int corr_count=0;
  while (corr_count<ncorrs)
  {
    int ntracks;
    ifs>>tag>>ntracks;
    if (tag!="n_i:")
      return -1;

    int temp;
    for (int i=0;i<ntracks;++i)
    {
      for (int j=0;j<ni*nj;++j)
      {
        ifs>>temp;
        hist.upcount((float)temp/255,1.0f);
      }
    }
    ifs.getline(buffer,1024);
    corr_count++;
  }

  ifs.close();
  vcl_ofstream ofile(outfile().c_str());

  if (!ofile)
  {
    vcl_cerr << "Failed to open file " << outfile() << vcl_endl;
    return -1;
  }
  hist.write(ofile);
  ofile.close();

  return 0;
}

