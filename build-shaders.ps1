mkdir build/shaders/spv/
glslc --target-env=vulkan1.3 src/Graphics/Shaders/main.vert -o build/shaders/spv/main.vert.spv
glslc --target-env=vulkan1.3 src/Graphics/Shaders/main.frag -o build/shaders/spv/main.frag.spv