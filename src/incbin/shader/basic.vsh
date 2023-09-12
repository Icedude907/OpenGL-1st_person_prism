#version 330 core

layout(location = 0) in  vec3 pos;
layout(location = 1) in  vec3 vertexColour;
                     out vec3 colour;
layout(location = 2) in  vec2 tex;
                     out vec2 texCoord;

uniform mat4 model;
uniform mat4 projview;

void main(){
    // gl_Position is hard coded. Order of mul matters
    gl_Position = projview * model * vec4(pos.xyz, 1.0f);
    colour = vertexColour;
    texCoord = tex;
}