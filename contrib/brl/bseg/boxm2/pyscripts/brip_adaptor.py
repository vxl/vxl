from boxm2_register import boxm2_batch, dbvalue;

###########################################################
# Calculate mutual information per blob between two images
###########################################################
def blobwise_mutual_info(in_img, exp_img, blob_img) :
  boxm2_batch.init_process("bripBlobwiseMutualInfoProcess")
  boxm2_batch.set_input_from_db(0, in_img);
  boxm2_batch.set_input_from_db(1, exp_img);
  boxm2_batch.set_input_from_db(2, blob_img);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  vis_img = dbvalue(id,type);
  return vis_img;
  
