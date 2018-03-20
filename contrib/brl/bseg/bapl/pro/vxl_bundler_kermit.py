#############################
#  Exampler script to extract keypoints from an image sequence and compute tracks of correspondences across frames
#  Final output is an xml file in the bwm_video_site format
#############################

import brl_init
import boxm_batch as batch
dbvalue = brl_init.register_batch(batch)


class id_pair:

    def __init__(self, first, second):
        self.first = first
        self.second = second

python_path = "C:\\projects\\bundler\\bundler-v0.3-binary\\examples\\kermit\\vxl_bundler\\"
img_path = "C:\\projects\\bundler\\bundler-v0.3-binary\\examples\\kermit\\jpg_imgs\\"
img_name = "kermit%03d.jpg"
img_cnt = 11  # ids 0, 1, 2
output_path = python_path
out_img_name = "kermit%03d_with_keys.jpg"
out_key_name = "kermit%03d.key"
out_match_img_name = "kermit%03d_with_keys_of%03d.jpg"
out_match_img_refined_name = "kermit%03d_with_keys_of%03d_refined.jpg"
out_video_site_file = "kermit_video_site.xml"

keys_available = 0          # if keys have already been extracted, just load them

# after finding F between a pair, all matches that are off by 0.6% of
# max(image_width, image_height) pixels are considered outliers
outlier_threshold_percentage = 0.6
# for an image pair to be connected in the image connectivity graph
min_number_of_matches = 16

imgs = []
sizes = []
for i in range(0, img_cnt, 1):
    print("Loading Image")
    batch.init_process("vilLoadImageViewProcess")
    batch.set_input_string(0, img_path + img_name % i)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image = dbvalue(id, type)
    imgs.append(image)

    batch.init_process("vilImageSizeProcess")
    batch.set_input_from_db(0, image)
    batch.run_process()
    (ni_id, type) = batch.commit_output(0)
    (nj_id, type) = batch.commit_output(1)
    ni = batch.get_input_unsigned(ni_id)
    nj = batch.get_input_unsigned(nj_id)
    if ni > nj:
        sizes.append(ni)
    else:
        sizes.append(nj)

# set up the connectivity table to be used for computing tracks from the
# matches
batch.init_process("baplCreateConnTableProcess")
batch.set_input_int(0, img_cnt)
batch.run_process()
(id, type) = batch.commit_output(0)
conn_table = dbvalue(id, type)

key_sets = []
for i in range(0, img_cnt, 1):

    if keys_available:
        # warning: remove this option
        batch.init_process("baplLoadKeypointsProcess")
        batch.set_input_string(0, output_path + out_key_name % i)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        out_set = dbvalue(id, type)
        key_sets.append(out_set)
    else:
        batch.init_process("baplExtractKeypointsProcess")
        batch.set_input_from_db(0, imgs[i])
        batch.set_input_string(1, output_path + out_key_name % i)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        out_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_set = dbvalue(id, type)
        key_sets.append(out_set)

        batch.init_process("vilSaveImageViewProcess")
        batch.set_input_from_db(0, out_image)
        batch.set_input_string(1, out_img_name % i)
        batch.run_process()

    # add the data for this image to the connectivity graph to enable track
    # computation later
    batch.init_process("baplAddImageKeysProcess")
    batch.set_input_from_db(0, conn_table)
    batch.set_input_int(1, i)
    batch.set_input_from_db(2, out_set)
    batch.run_process()

# create a list of pairs of images, in the case of video these pairs may
# depend on the sequence of video, e.g. only pair with 5 images before and
# after a frame, etc.
pairs = []
outlier_thresholds = []
for i in range(0, img_cnt, 1):
    for j in range(0, i, 1):
        pairs.append(id_pair(i, j))
        if (sizes[i] > sizes[j]):
            s = sizes[i]
        else:
            s = sizes[j]
        outlier_thresholds.append(s * outlier_threshold_percentage / 100)

for ii in range(0, len(pairs), 1):
    print "outlier threshold: " + str(outlier_thresholds[ii])


# find the matches
for ii in range(0, len(pairs), 1):
    i = pairs[ii].first
    j = pairs[ii].second
    print "matching pair: " + str(i) + " " + str(j) + "\n"
    batch.init_process("baplMatchKeypointsProcess")
    batch.set_input_from_db(0, key_sets[i])
    batch.set_input_from_db(1, key_sets[j])
    batch.set_input_int(2, i)
    batch.set_input_int(3, j)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    match_set = dbvalue(id, type)

    batch.init_process("baplMatchDisplayProcess")
    batch.set_input_from_db(0, imgs[i])
    batch.set_input_from_db(1, imgs[j])
    batch.set_input_from_db(2, match_set)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_img2 = dbvalue(id, type)

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img1)
    batch.set_input_string(1, out_match_img_name % (i, j))
    batch.run_process()

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img2)
    batch.set_input_string(1, out_match_img_name % (j, i))
    batch.run_process()

    batch.init_process("baplRefineMatchProcess")
    batch.set_input_from_db(0, match_set)
    batch.set_input_float(1, outlier_thresholds[ii])
    batch.set_input_int(2, min_number_of_matches)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    refined_match_set = dbvalue(id, type)

    batch.init_process("baplMatchDisplayProcess")
    batch.set_input_from_db(0, imgs[i])
    batch.set_input_from_db(1, imgs[j])
    batch.set_input_from_db(2, refined_match_set)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_img2 = dbvalue(id, type)

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img1)
    batch.set_input_string(1, out_match_img_refined_name % (i, j))
    batch.run_process()

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img2)
    batch.set_input_string(1, out_match_img_refined_name % (j, i))
    batch.run_process()

    # add this match set to the connectivity graph
    batch.init_process("baplAddMatchSetProcess")
    batch.set_input_from_db(0, conn_table)
    batch.set_input_from_db(1, match_set)
    batch.run_process()

# compute tracks using the connectivity graph
batch.init_process("baplComputeTracksProcess")
batch.set_input_from_db(0, conn_table)
# edit file to add image folder path and site name to load with GUI
batch.set_input_string(1, out_video_site_file)
batch.run_process()
