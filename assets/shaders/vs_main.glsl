layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

uniform mat4 u_model;

void main() {
    gl_Position = u_model * vec4(in_position, 1.0);
}
