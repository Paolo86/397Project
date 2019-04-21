#version 330 core

out vec4 FragColor; 


struct Material
{
vec3 color;
};

uniform Material material;


void main()
{
   
gl_FragColor =  vec4(material.color,1.0);


} 