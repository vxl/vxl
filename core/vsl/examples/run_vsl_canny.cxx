/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>
#include <vbl/vbl_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_byte.h>
#include <vil/vil_memory_image_of.h>

#include <vsl/vsl_easy_canny.h>
#include <vsl/vsl_save_topology.h>

// runs canny on the given input image and outputs
// the segmentation to the given output file.

int main(int argc, char **argv) {
  //cerr << "this is " __FILE__ << endl;
  vbl_arg<int>        canny("-canny", "which canny? (0:oxford, 1:rothwell1, 2:rothwell2)", 0);
  vbl_arg<vcl_string> in   ("-in", "input image", "/users/fsm/images/pig-grey.jpg");
  vbl_arg<vcl_string> out  ("-out", "output file (default is stdout)", "");
  vbl_arg_parse(argc, argv);

  vil_image image = vil_load(in().c_str());
  if (!image)
    return 1;
  cerr << in() << " : " << image << endl;

  vcl_list<vsl_Edge*> edges;
  vsl_easy_canny(canny(), image, &edges);
  
  if (out() == "")
    vsl_save_topology(cout, edges, vcl_list<vsl_Vertex*>());
  else
    vsl_save_topology(out().c_str(), edges, vcl_list<vsl_Vertex*>());
  //   for (vcl_list<vsl_Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
  //     cerr << "# edge " << (void*) (*i) << endl;
  //     vsl_EdgelChain const &c = **i;
  //     cout << c.size() << endl;
  //     for (unsigned j=0; j<c.size(); ++j)
  //       cout << c.GetX(j) << ' ' << c.GetY(j) << endl;
  //     //cout << "-1 -1" << endl;
  //     cout << endl;
  //     (*i)->unref();
  //   }
  
  return 0;
}
