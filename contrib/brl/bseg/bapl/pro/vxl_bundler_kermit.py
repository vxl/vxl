#############################
#  Exampler script to extract keypoints from an image sequence and compute tracks of correspondences across frames
#  Final output is an xml file in the bwm_video_site format
#############################

import boxm_batch;
boxm_batch.register_processes();
boxm_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

class id_pair:
  def __init__(self, first, second):
    self.first = first;
    self.second = second;

python_path = "C:\\projects\\bundler\\bundler-v0.3-binary\\examples\\kermit\\vxl_bundler\\";
img_path = "C:\\projects\\bundler\\bundler-v0.3-binary\\examples\\kermit\\jpg_imgs\\";
img_name="kermit%03d.jpg";
img_cnt = 11;  # ids 0, 1, 2
output_path = python_path;
out_img_name="kermit%03d_with_keys.jpg";
out_key_name="kermit%03d.key";
out_match_img_name="kermit%03d_with_keys_of%03d.jpg";
out_match_img_refined_name="kermit%03d_with_keys_of%03d_refined.jpg";
out_video_site_file="kermit_video_site.xml";

keys_available = 0;          # if keys have already been extracted, just load them

outlier_threshold_percentage = 0.6;    # after finding F between a pair, all matches that are off by 0.6% of max(image_width, image_height) pixels are considered outliers
min_number_of_matches = 16;            # for an image pair to be connected in the image connectivity graph

imgs = [];
sizes = [];
for i in range(0,img_cnt,1):
  print("Loading Image");
  boxm_batch.init_process("vilLoadImageViewProcess");
  boxm_batch.set_input_string(0,img_path + img_name % i);
  boxm_batch.run_process();
  (id,type) = boxm_batch.commit_output(0);
  image = dbvalue(id,type);
  imgs.append(image);
  
  boxm_batch.init_process("vilImageSizeProcess");
  boxm_batch.set_input_from_db(0, image);
  boxm_batch.run_process();
  (ni_id, type) = boxm_batch.commit_output(0);
  (nj_id, type) = boxm_batch.commit_output(1);
  ni=boxm_batch.get_input_unsigned(ni_id);
  nj=boxm_batch.get_input_unsigned(nj_id);
  if ni > nj:
    sizes.append(ni);
  else:
    sizes.append(nj);
    
# set up the connectivity table to be used for computing tracks from the matches
boxm_batch.init_process("baplCreateConnTableProcess");
boxm_batch.set_input_int(0, img_cnt);
boxm_batch.run_process();
(id,type) = boxm_batch.commit_output(0);
conn_table = dbvalue(id,type);

key_sets = [];
for i in range(0,img_cnt,1):

  if keys_available:
    boxm_batch.init_process("baplLoadKeypointsProcess");                              # warning: remove this option
    boxm_batch.set_input_string(0,output_path + out_key_name % i);
    boxm_batch.run_process();
    (id,type) = boxm_batch.commit_output(0);
    out_set = dbvalue(id,type);
    key_sets.append(out_set);
  else:
    boxm_batch.init_process("baplExtractKeypointsProcess");
    boxm_batch.set_input_from_db(0, imgs[i]);
    boxm_batch.set_input_string(1,output_path + out_key_name % i);
    boxm_batch.run_process();
    (id,type) = boxm_batch.commit_output(0);
    out_image = dbvalue(id,type);
    (id,type) = boxm_batch.commit_output(1);
    out_set = dbvalue(id,type);
    key_sets.append(out_set);

    boxm_batch.init_process("vilSaveImageViewProcess");
    boxm_batch.set_input_from_db(0,out_image);
    boxm_batch.set_input_string(1,out_img_name%i);
    boxm_batch.run_process();
    
  # add the data for this image to the connectivity graph to enable track computation later
  boxm_batch.init_process("baplAddImageKeysProcess");
  boxm_batch.set_input_from_db(0, conn_table);
  boxm_batch.set_input_int(1, i);
  boxm_batch.set_input_from_db(2, out_set);
  boxm_batch.run_process();

# create a list of pairs of images, in the case of video these pairs may depend on the sequence of video, e.g. only pair with 5 images before and after a frame, etc.
pairs = [];
outlier_thresholds = [];
for i in range(0,img_cnt,1):
  for j in range(0,i,1):
    pairs.append(id_pair(i,j));
    if (sizes[i] > sizes[j]):
      s = sizes[i];
    else:
      s = sizes[j];
    outlier_thresholds.append(s*outlier_threshold_percentage/100);

for ii in range(0,len(pairs),1):
  print "outlier threshold: "+str(outlier_thresholds[ii]);


# find the matches
for ii in range(0, len(pairs), 1):
  i = pairs[ii].first;
  j = pairs[ii].second;
  print "matching pair: "+str(i)+" "+str(j)+"\n";
  boxm_batch.init_process("baplMatchKeypointsProcess");
  boxm_batch.set_input_from_db(0, key_sets[i]);
  boxm_batch.set_input_from_db(1, key_sets[j]);
  boxm_batch.set_input_int(2, i);
  boxm_batch.set_input_int(3, j);
  boxm_batch.run_process();
  (id,type) = boxm_batch.commit_output(0);
  match_set = dbvalue(id,type);
  
  boxm_batch.init_process("baplMatchDisplayProcess");
  boxm_batch.set_input_from_db(0, imgs[i]);
  boxm_batch.set_input_from_db(1, imgs[j]);
  boxm_batch.set_input_from_db(2, match_set);
  boxm_batch.run_process();
  (id,type) = boxm_batch.commit_output(0);
  out_img1 = dbvalue(id,type);
  (id,type) = boxm_batch.commit_output(1);
  out_img2 = dbvalue(id,type);
  
  boxm_batch.init_process("vilSaveImageViewProcess");
  boxm_batch.set_input_from_db(0,out_img1);
  boxm_batch.set_input_string(1,out_match_img_name%(i,j));
  boxm_batch.run_process();

  boxm_batch.init_process("vilSaveImageViewProcess");
  boxm_batch.set_input_from_db(0,out_img2);
  boxm_batch.set_input_string(1,out_match_img_name%(j,i));
  boxm_batch.run_process();
  
  boxm_batch.init_process("baplRefineMatchProcess");
  boxm_batch.set_input_from_db(0, match_set);
  boxm_batch.set_input_float(1, outlier_thresholds[ii]);
  boxm_batch.set_input_int(2, min_number_of_matches);
  boxm_batch.run_process();
  (id,type) = boxm_batch.commit_output(0);
  refined_match_set = dbvalue(id,type);
  
  boxm_batch.init_process("baplMatchDisplayProcess");
  boxm_batch.set_input_from_db(0, imgs[i]);
  boxm_batch.set_input_from_db(1, imgs[j]);
  boxm_batch.set_input_from_db(2, refined_match_set);
  boxm_batch.run_process();
  (id,type) = boxm_batch.commit_output(0);
  out_img1 = dbvalue(id,type);
  (id,type) = boxm_batch.commit_output(1);
  out_img2 = dbvalue(id,type);

  boxm_batch.init_process("vilSaveImageViewProcess");
  boxm_batch.set_input_from_db(0,out_img1);
  boxm_batch.set_input_string(1,out_match_img_refined_name%(i,j));
  boxm_batch.run_process();

  boxm_batch.init_process("vilSaveImageViewProcess");
  boxm_batch.set_input_from_db(0,out_img2);
  boxm_batch.set_input_string(1,out_match_img_refined_name%(j,i));
  boxm_batch.run_process();
  
  # add this match set to the connectivity graph
  boxm_batch.init_process("baplAddMatchSetProcess");
  boxm_batch.set_input_from_db(0, conn_table);
  boxm_batch.set_input_from_db(1, match_set);
  boxm_batch.run_process();

# compute tracks using the connectivity graph
boxm_batch.init_process("baplComputeTracksProcess");
boxm_batch.set_input_from_db(0, conn_table);
boxm_batch.set_input_string(1, out_video_site_file);      # edit file to add image folder path and site name to load with GUI
boxm_batch.run_process();


  
  

