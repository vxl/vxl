#include <boxm2/class/boxm2_class_generate_samples.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>

#include <vcl_cstdlib.h> // for std::exit()

//check dirs, exit if not exist
void check_dir(vcl_string& dir)
{
  if (!vul_file::exists(dir)) {
    vcl_cout<<"Directory of file does not exist: "<<dir<<vcl_endl;
    vcl_exit(-1);
  }
}

//push back vector onto vector
template <class T>
void push_back(vcl_vector<T>& dest, vcl_vector<T>& src)
{
  for (unsigned int i=0; i<src.size(); ++i)
    dest.push_back(src[i]);
}


int main(int argc, char ** argv)
{
  vul_arg<vcl_string> eoDir("-eoImgs", "EO Image input directory", "");
  vul_arg<vcl_string> irDir("-irImgs", "IR Image input directory", "");
  vul_arg<vcl_string> xmlDir("-a", "Annotations directory (label me xml files per image)");
  vul_arg<bool>       sparse("-sparse", "Make the sampling of pixels sparse", false);
  vul_arg_parse(argc, argv);

  //check diarectories
  check_dir(eoDir());
  check_dir(irDir());
  check_dir(xmlDir());

  //grab images and xml files from directory
  vcl_vector<vcl_string> eoImgs;
  vcl_vector<vcl_string> irImgs;
  vcl_vector<vcl_string> xmlFiles;
  if (vul_file::is_directory(eoDir())) {
    eoImgs = boxm2_util::images_from_directory(eoDir());
    irImgs = boxm2_util::images_from_directory(irDir());
    xmlFiles = boxm2_util::files_from_dir(xmlDir(), "xml");
  }
  else {
    eoImgs.push_back(eoDir());
    irImgs.push_back(irDir());
    xmlFiles.push_back(xmlDir());
  }
  vcl_cout<<"Generating samples for "<<eoDir()<<','<<irDir()<<','<<" using "<<xmlDir()<<'\n'
          <<"  num images: "<<xmlFiles.size()<<'\n'
          <<"  num eo: "<<eoImgs.size()<<'\n'
          <<"  num ir: "<<irImgs.size()<<vcl_endl;

  //set the number of samples K to take from each image
  vcl_size_t K = vcl_size_t(sparse() ? 10000 : 0x7fffffff);

  //boxm2_class_generate_samples sampler(;a
  vcl_vector<vnl_vector_fixed<float,4> > allInts;
  vcl_vector<vcl_string> allClasses;
  for (unsigned int i=0; i<xmlFiles.size(); ++i) {
    //generate IR samples
    boxm2_class_generate_samples sampler(xmlFiles[i], eoImgs[i], irImgs[i], K);
    vcl_vector<vcl_string> classes = sampler.classes();
    vcl_vector<float> ints = sampler.intensities();
    vcl_vector<float> r = sampler.r();
    vcl_vector<float> g = sampler.g();
    vcl_vector<float> b = sampler.b();

    vcl_vector<vnl_vector_fixed<float,4> > feats;
    for (unsigned int i=0; i<r.size(); ++i)
      feats.push_back( vnl_vector_fixed<float,4>(ints[i],r[i],g[i],b[i]) );

    //stack
    push_back(allInts, feats);
    push_back(allClasses, classes);
  }

  //print samples/write to file
  for (unsigned int i=0; i<allInts.size(); ++i) {
    vcl_cout<<allClasses[i]<<"  "<<allInts[i]<<vcl_endl;
  }
}

