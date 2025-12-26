#type fragment
#version 330 core

out vec4 FragColor;

uniform vec3 u_Color;
uniform vec3 u_Opacity;

void main()
{
    FragColor = vec4(u_Color, u_Opacity);
}

#type vertex
#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ProjectionView * u_Model * vec4(aPosition, 1.0);
}

