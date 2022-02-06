import os
import shutil

vf_cpp_prj="F:/prj/VoxelFrameUE4"

shutil.copy("./common.proto",vf_cpp_prj+"/ProtoPacks/common.proto")
os.system("cd "+vf_cpp_prj+" && "+vf_cpp_prj+"/proto.bat")
