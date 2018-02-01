#############################
#  Exampler script to extract keypoints from a video sequence and compute tracks of correspondences across frames
#  Final output is an xml file in the bwm_video_site format
#############################


import brl_init
import boxm_batch as batch
dbvalue = brl_init.register_batch(batch)


class id_pair:

    def __init__(self, first, second):
        self.first = first
        self.second = second

python_path = "C:\\projects\\capitol_sfm\\vxl_bundler_subset_wide\\"
img_path = "C:\\projects\\capitol_sfm\\video_grey\\"
img_name = "frame_%05d.png"
img_cnt = 6  # ids 0, 1, 2
output_path = python_path
out_img_name = "frame_%05d_with_keys.jpg"
out_key_name = "frame_%05d.key"
out_match_img_name = "frame_%05d_with_keys_of_%05d.jpg"
out_match_img_refined_name = "frame_%05d_with_keys_of_%05d_refined.jpg"
out_video_site_file = "tracks.xml"
every_nth = 16  # use every nth image of the video sequence to guarantee view disparity

keypoints_available = 0

# after finding F between a pair, all matches that are off by 9.0 pixels
# are considered outliers
outlier_threshold = 9.0
# for an image pair to be connected in the image connectivity graph
min_number_of_matches = 16

sizes = []
for i in range(0, img_cnt, 1):
    print("Loading Image")
    batch.init_process("vilLoadImageViewProcess")
    batch.set_input_string(0, img_path + img_name % (i * every_nth + 1))
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image = dbvalue(id, type)

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

    batch.remove_data(image.id)

# set up the connectivity table to be used for computing tracks from the
# matches
batch.init_process("baplCreateConnTableProcess")
batch.set_input_int(0, img_cnt)
batch.run_process()
(id, type) = batch.commit_output(0)
conn_table = dbvalue(id, type)

for i in range(0, img_cnt, 1):
    print("Loading Image")
    batch.init_process("vilLoadImageViewProcess")
    batch.set_input_string(0, img_path + img_name % (i * every_nth + 1))
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image = dbvalue(id, type)

    if keypoints_available:
        batch.init_process("baplLoadKeypointsProcess")
        batch.set_input_string(
            0, output_path + out_key_name % (i * every_nth + 1))
        batch.run_process()
        (id, type) = batch.commit_output(0)
        out_set = dbvalue(id, type)

        batch.init_process("baplDrawKeypointsProcess")
        batch.set_input_from_db(0, image)
        batch.set_input_from_db(1, out_set)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        out_image = dbvalue(id, type)
    else:
        batch.init_process("baplExtractKeypointsProcess")
        batch.set_input_from_db(0, image)
        batch.set_input_string(
            1, output_path + out_key_name % (i * every_nth + 1))
        batch.run_process()
        (id, type) = batch.commit_output(0)
        out_image = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_set = dbvalue(id, type)

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_image)
    batch.set_input_string(1, output_path + out_img_name % (i * every_nth + 1))
    batch.run_process()

    # to enable destruction of pyramid images in the keypoint representation
    # and thus avoid memory crash
    print("Removing Data")
    batch.remove_data(image.id)
    batch.remove_data(out_image.id)
    batch.remove_data(out_set.id)

pairs = []
outlier_thresholds = []
# create a list of pairs of images
for i in range(0, img_cnt, 1):
    for j in range(0, i, 1):
        pairs.append(id_pair(i, j))
        outlier_thresholds.append(outlier_threshold)

# re-load the keys and add to the table later to enable destruction of
# pyramid images in the keypoint representation and thus avoid memory
# overload in the case of large images
keys = []
for i in range(0, img_cnt, 1):
    batch.init_process("baplLoadKeypointsProcess")
    batch.set_input_string(0, output_path + out_key_name % (i * every_nth + 1))
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_set = dbvalue(id, type)
    keys.append(out_set)

    # add the data for this image to the connectivity graph to enable track
    # computation later
    batch.init_process("baplAddImageKeysProcess")
    batch.set_input_from_db(0, conn_table)
    # image ids start from 0 in the match table
    batch.set_input_int(1, i)
    batch.set_input_from_db(2, out_set)
    batch.run_process()


# find the matches
for ii in range(0, len(pairs), 1):
    i = pairs[ii].first
    j = pairs[ii].second

    print "matching pair: " + str(i * every_nth + 1) + " " + str(j * every_nth + 1) + "\n"
    batch.init_process("baplMatchKeypointsProcess")
    batch.set_input_from_db(0, keys[i])
    batch.set_input_from_db(1, keys[j])
    batch.set_input_int(2, i)
    batch.set_input_int(3, j)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    match_set = dbvalue(id, type)

    print("Loading Image")
    batch.init_process("vilLoadImageViewProcess")
    batch.set_input_string(0, img_path + img_name % (i * every_nth + 1))
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image_i = dbvalue(id, type)

    print("Loading Image")
    batch.init_process("vilLoadImageViewProcess")
    batch.set_input_string(0, img_path + img_name % (j * every_nth + 1))
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image_j = dbvalue(id, type)

    batch.init_process("baplMatchDisplayProcess")
    batch.set_input_from_db(0, image_i)
    batch.set_input_from_db(1, image_j)
    batch.set_input_from_db(2, match_set)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_img2 = dbvalue(id, type)

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img1)
    batch.set_input_string(1,
                           output_path + out_match_img_name % ((i * every_nth + 1),
                                                               (j * every_nth + 1)))
    batch.run_process()

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img2)
    batch.set_input_string(1,
                           output_path + out_match_img_name % ((j * every_nth + 1),
                                                               (i * every_nth + 1)))
    batch.run_process()

    batch.remove_data(out_img1.id)
    batch.remove_data(out_img2.id)

    batch.init_process("baplRefineMatchProcess")
    batch.set_input_from_db(0, match_set)
    batch.set_input_float(1, outlier_thresholds[ii])
    batch.set_input_int(2, min_number_of_matches)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    refined_match_set = dbvalue(id, type)

    batch.init_process("baplMatchDisplayProcess")
    batch.set_input_from_db(0, image_i)
    batch.set_input_from_db(1, image_j)
    batch.set_input_from_db(2, refined_match_set)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_img2 = dbvalue(id, type)

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img1)
    batch.set_input_string(1,
                           output_path + out_match_img_refined_name % (i * every_nth + 1,
                                                                       j * every_nth + 1))
    batch.run_process()

    batch.init_process("vilSaveImageViewProcess")
    batch.set_input_from_db(0, out_img2)
    batch.set_input_string(1,
                           output_path + out_match_img_refined_name % (j * every_nth + 1,
                                                                       i * every_nth + 1))
    batch.run_process()

    # add this match set to the connectivity graph
    batch.init_process("baplAddMatchSetProcess")
    batch.set_input_from_db(0, conn_table)
    batch.set_input_from_db(1, match_set)
    batch.run_process()

    batch.remove_data(image_i.id)
    batch.remove_data(image_j.id)

# compute tracks using the connectivity graph
batch.init_process("baplComputeTracksProcess")
batch.set_input_from_db(0, conn_table)
batch.set_input_string(1, output_path + out_video_site_file)
batch.run_process()
