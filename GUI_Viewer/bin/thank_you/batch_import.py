import os
import bpy

# location to the directory where the objs are located
# if you are using windows style backslashes, you need to cancel one with another
path_to_obj_dir = os.path.join('C:\\', 'Users\\a0309')

# get list of all files in directory
file_list = sorted(os.listdir(path_to_obj_dir))

# get a list of files ending in 'obj'
obj_list = [item for item in file_list if item[-3:] == 'obj']

# loop through the strings in obj_list and add the files to the scene
for item in obj_list:
    path_to_file = os.path.join(path_to_obj_dir, item)
    bpy.ops.import_scene.obj(filepath = path_to_file)