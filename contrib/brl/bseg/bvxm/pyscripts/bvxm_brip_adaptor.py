from bvxm_register import bvxm_batch, dbvalue;

# truncate 16 bits NITF image by ignoring certain bits
# note the original NITF image will be truncated to a byte image by default
def truncate_nitf_image(img, is_byte = True, is_scale = False):
  bvxm_batch.init_process("bripTruncateNITFBitProcess");
  bvxm_batch.set_input_from_db(0, img);
  bvxm_batch.set_input_bool(1, is_byte);
  bvxm_batch.set_input_bool(2, is_scale);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(id, type);
  return out_img;
