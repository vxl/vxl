// This is brl/bpro/core/vil_pro/processes/vil_pixelwise_roc_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <bbas_pro/bbas_1d_array_float.h>
#include <vcl_algorithm.h>

// do pixelwise sort on the image, and then
struct Pair
{
  float         change;
  unsigned char gt;
  unsigned i;
  unsigned j;
};

bool pair_sorter(Pair const& lhs, Pair const& rhs)
{
  return lhs.change < rhs.change;
}

bool pair_sorter2(Pair const& lhs, Pair const& rhs)
{
  return lhs.change > rhs.change;
}


//: Constructor
bool vil_pixelwise_roc_process_cons(bprb_func_process& pro)
{
  // this process takes 4 inputs, 2 of which are optional:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // change image
  input_types.push_back("vil_image_view_base_sptr");  // ground truth map
  input_types.push_back("vil_image_view_base_sptr");  // mask image
  input_types.push_back("bool");                      // if true use pair_sorter, otherwise use pair_sorter2, if want low values in change image to signify true classification use pair_sorter
  if (! pro.set_input_types(input_types))
    return false;

  // default arguments
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(2, empty_mask);

  brdb_value_sptr idx = new brdb_value_t<bool>(true);
  pro.set_input(3, idx);

  // this process takes 7 outputs:
  vcl_vector<vcl_string> output_types;
  output_types.push_back("bbas_1d_array_float_sptr");  // tp
  output_types.push_back("bbas_1d_array_float_sptr");  // tn
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  output_types.push_back("bbas_1d_array_float_sptr");  // tpr
  output_types.push_back("bbas_1d_array_float_sptr");  // fpr
  output_types.push_back("vil_image_view_base_sptr");  // output image with pixels given by threshold of 0.8 tpr marked red

  return pro.set_output_types(output_types);
}

//: Execute the process
bool vil_pixelwise_roc_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "vil_pixelwise_roc_process: The number of inputs should be 2 (with optional 3rd (num thresh) and 4th (mask image))\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr detection_map_sptr    = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr ground_truth_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_map_sptr         = pro.get_input<vil_image_view_base_sptr>(i++);
  bool use_pair_sorter = pro.get_input<bool>(i++);

  // catch a "null" mask (not really null because that throws an error)
  bool use_mask = true;
  if (mask_map_sptr->ni()==1 && mask_map_sptr->nj()==1) {
    vcl_cout<<"USE mask = false"<<vcl_endl;
    use_mask = false;
  }

  // true positive, true negative, false positive, false negative
  const unsigned int numPoints = 10000;
  bbas_1d_array_float * tp=new bbas_1d_array_float(numPoints);
  bbas_1d_array_float * tn=new bbas_1d_array_float(numPoints);
  bbas_1d_array_float * fp=new bbas_1d_array_float(numPoints);
  bbas_1d_array_float * fn=new bbas_1d_array_float(numPoints);
  vil_image_view<float> * detection_map;

  // check bounds to make sure they match
  if (detection_map_sptr->ni() != ground_truth_map_sptr->ni() ||
      detection_map_sptr->nj() != ground_truth_map_sptr->nj() ) {
    vcl_cout<<"vil_pixelwise_roc_process:: detection map doesn't match ground truth map"<<vcl_endl;
    return false;
  }
  if (use_mask) {
    if (detection_map_sptr->ni()!=mask_map_sptr->ni() ||
        detection_map_sptr->nj()!=mask_map_sptr->nj() ) {
      vcl_cout<<"vil_pixelwise_roc_process:: detection map doesn't match mask map"<<vcl_endl;
      return false;
    }
  }

  // convert detection map to [0,1] float
  if (vil_image_view<unsigned char> * detection_map_uchar=dynamic_cast<vil_image_view<unsigned char> *>(detection_map_sptr.ptr()))
  {
    detection_map =new vil_image_view<float>(detection_map_uchar->ni(),detection_map_uchar->nj());
    vil_convert_stretch_range_limited<unsigned char>(*detection_map_uchar,*detection_map,0,255,0.0f,1.0f);
  }
  else if (dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr()))
  {
    detection_map=dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr());
    vcl_cout << "detection map is float!\n";
  }
  else
  {
    vcl_cout<<"Detection Map cannot be converted to float image"<<vcl_endl;
    return false;
  }

  // cast to usable image views
  vil_image_view<unsigned char> * ground_truth_map = dynamic_cast<vil_image_view<unsigned char> *>(ground_truth_map_sptr.ptr());
  if ( !ground_truth_map )
  {
    vcl_cout<<"vil_pixelwise_roc_process:: gt map is not an unsigned char map"<<vcl_endl;
    return false;
  }
  vil_image_view<unsigned char> * mask_map=dynamic_cast<vil_image_view<unsigned char> *>(mask_map_sptr.ptr());
  if (!mask_map)
  {
    vcl_cout<<"vil_pixelwise_roc_process:: mask map is not an unsigned char map"<<vcl_endl;
    return false;
  }

  // sort pixel/gt pairs
  //Pair* pairs = new Pair[ detection_map->ni() * detection_map->nj() ];
  vcl_vector<Pair> pairs;
  unsigned c = 0;
  for (unsigned j=0; j<detection_map->nj(); ++j) {
    for (unsigned i=0; i<detection_map->ni(); ++i) {
      if (use_mask && (*mask_map)(i,j) == 0)
        continue;
      if(  (*ground_truth_map)(i,j) == 0 || (*ground_truth_map)(i,j) == 255)
      {
      Pair p;
      p.change = (*detection_map)(i,j);
      p.gt     = (*ground_truth_map)(i,j);
      p.i = i;
      p.j = j;
      //pairs[c] = p;
      pairs.push_back(p);
      ++c;
      }
    }
  }

  //
  //unsigned totPix = detection_map->ni() * detection_map->nj();
  unsigned totPix = pairs.size();
  //vcl_sort(pairs, pairs + totPix, &pair_sorter);
  if (use_pair_sorter)
    vcl_sort(pairs.begin(), pairs.end(), &pair_sorter);
  else
    vcl_sort(pairs.begin(), pairs.end(), &pair_sorter2);

  // grab 100 points for the ROC curve
  unsigned int incr = totPix / numPoints;
  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
     if (pnt%100 == 0 )
         vcl_cout<<".";
    tp->data_array[pnt]=0.0f;
    fp->data_array[pnt]=0.0f;
    tn->data_array[pnt]=0.0f;
    fn->data_array[pnt]=0.0f;

    unsigned int exampleIdx = pnt*incr;

    // all classified examples in this loop are negative
    for (unsigned int i=0; i<exampleIdx; ++i) {
      bool truth = (pairs[i].gt == 255);
      bool ignore= (pairs[i].gt > 0 && pairs[i].gt < 255);
      if(!ignore)
      if (truth)
        fn->data_array[pnt]++; // gt=true, class=false => false neg
      else
        tn->data_array[pnt]++; // gt=false, class=false => true neg
    }

    // all classified examples in this loop are positive
    for (unsigned int i=exampleIdx; i<totPix; ++i) {
      bool truth = (pairs[i].gt == 255);
      bool ignore= (pairs[i].gt > 0 && pairs[i].gt < 255);
      if(!ignore)
      if (truth )
        tp->data_array[pnt]++; // gt = true, class = true => true pos
      else
        fp->data_array[pnt]++; // gt = false, class = true => false pos
    }
  } 

  bbas_1d_array_float * tpr=new bbas_1d_array_float(numPoints);
  bbas_1d_array_float * fpr=new bbas_1d_array_float(numPoints);

  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
    tpr->data_array[pnt]= tp->data_array[pnt] / (tp->data_array[pnt] + fn->data_array[pnt]);
    fpr->data_array[pnt]= fp->data_array[pnt] / (fp->data_array[pnt] + tn->data_array[pnt]);
  }
  //: find the point when tpr > 0.8
  //float change = 0.0f;
  //for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
  //  if (tpr->data_array[pnt] > 0.8f) {
  //    unsigned int exampleIdx = pnt*incr;
  //    change = pairs[exampleIdx].change;
  //    break;
  //  }
  //}

  vil_image_view<vxl_byte>* temp = new vil_image_view<vxl_byte>(detection_map->ni(), detection_map->nj(), 3);
  temp->fill(0);

  //for (unsigned k = 0; k < totPix; ++k) {
  //  unsigned i = pairs[k].i;
  //  unsigned j = pairs[k].j;
  //  if (pairs[k].change >= change) {
  //    (*temp)(i,j,1) = 0;
  //    (*temp)(i,j,2) = 0;
  //    (*temp)(i,j,0) = 255;
  //  }
  //  else {
  //    (*temp)(i,j,1) = 255*(*detection_map)(i,j) > 255 ? 255 : (vxl_byte)(255*(*detection_map)(i,j));
  //    (*temp)(i,j,2) = 255*(*detection_map)(i,j) > 255 ? 255 : (vxl_byte)(255*(*detection_map)(i,j));
  //    (*temp)(i,j,0) = 255*(*detection_map)(i,j) > 255 ? 255 : (vxl_byte)(255*(*detection_map)(i,j));
  //  }
  //}

  pro.set_output_val<bbas_1d_array_float_sptr>(0, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fn);
  pro.set_output_val<bbas_1d_array_float_sptr>(4, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(5, fpr);
  pro.set_output_val<vil_image_view_base_sptr>(6, temp);

  return true;
}

