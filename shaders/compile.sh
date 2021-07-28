#! /bin/sh
set -e 

SHADER_DIR="${HOME}/projects/vulkan-tutorial/HelloTriangle/shaders/"
    
${HOME}/.local/vulkan/1.2.182.0/x86_64/bin/glslc ${SHADER_DIR}shader.vert -o ${SHADER_DIR}vert.spv
${HOME}/.local/vulkan/1.2.182.0/x86_64/bin/glslc ${SHADER_DIR}shader.frag -o ${SHADER_DIR}frag.spv
