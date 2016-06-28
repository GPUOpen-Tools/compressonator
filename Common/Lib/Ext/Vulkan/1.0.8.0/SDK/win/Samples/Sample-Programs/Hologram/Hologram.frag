#version 310 es

precision highp float;

in vec3 color;
in float alpha;

layout(location = 0) out vec4 fragcolor;

void main()
{
	fragcolor = vec4(color, alpha);
}
