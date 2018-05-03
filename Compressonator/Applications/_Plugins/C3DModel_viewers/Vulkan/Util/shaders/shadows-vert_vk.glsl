#version 450

//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------

layout (std140, binding = 0) uniform perBatch 
{
	mat4 u_MVPMatrix;
} myPerBatch;

layout (std140, binding = 1) uniform perObject
{
	mat4 u_ModelMatrix;
} myPerObject;

//--------------------------------------------------------------------------------------
// I/O Structures
//--------------------------------------------------------------------------------------

layout (location = ID_POSITION) in vec4 a_Position;

#ifdef ID_TEXCOORD_0
	layout (location = ID_TEXCOORD_0) in  vec2 a_UV;
	layout (location = ID_TEXCOORD_0) out vec2 v_UV;
#endif

//--------------------------------------------------------------------------------------
// main
//--------------------------------------------------------------------------------------
void main()
{
	vec4 pos = myPerObject.u_ModelMatrix * a_Position;
	gl_Position = myPerBatch.u_MVPMatrix * pos;
}
