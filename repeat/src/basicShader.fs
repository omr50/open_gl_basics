#version 330 core 

out vec4 FragmentColor;
uniform  vec4 ourColor;

void main() {
    // gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    FragmentColor = ourColor;
}