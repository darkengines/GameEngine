#version 460
#extension GL_EXT_nonuniform_qualifier: enable

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(0, 1, 0, 1);
}