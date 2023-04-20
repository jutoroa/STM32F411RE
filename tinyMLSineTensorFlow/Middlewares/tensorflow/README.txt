To generate

python3 tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py DESTINATION  --makefile_options "OPTIMIZED_KERNEL_DIR=cmsis_nn DISABLE_DOWNLOADS=true”

then on the generated code, apply patches on flatbuffer include to not include "optional.h" in case of EWARM (flexbuffers.h and stl_emulation.h)