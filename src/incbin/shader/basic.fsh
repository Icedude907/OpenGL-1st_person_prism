#version 330 core

in vec3 colour; // Same name
in vec2 texCoord;

uniform float colourScale; // See the uniforms in the main function
uniform sampler2D tex0;

out vec4 FragColour; // A "pixel" on the canvas (before scaling, etc?)

void main(){
    // Implicitly samples colour between vertices
    // FragColour = vec4(colour * colourScale, 1.0f);
    FragColour = texture(tex0, texCoord) * vec4(colour * colourScale, 1.0f);
}