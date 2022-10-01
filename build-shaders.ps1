mkdir -p build/debug/shaders/spv/
mkdir -p build/release/shaders/spv/

glslc --target-env=vulkan1.3 src/Graphics/Shaders/main.vert -o build/debug/shaders/spv/main.vert.spv
glslc --target-env=vulkan1.3 src/Graphics/Shaders/main.frag -o build/debug/shaders/spv/main.frag.spv

glslc --target-env=vulkan1.3 src/Graphics/Shaders/main.vert -o build/release/shaders/spv/main.vert.spv
glslc --target-env=vulkan1.3 src/Graphics/Shaders/main.frag -o build/release/shaders/spv/main.frag.spv