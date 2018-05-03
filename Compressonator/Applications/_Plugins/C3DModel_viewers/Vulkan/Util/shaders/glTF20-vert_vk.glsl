#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = ID_POSITION) in vec4 a_Position;

#ifdef ID_TEXCOORD_0
	layout (location = ID_TEXCOORD_0) in  vec2 a_UV;
	layout (location = ID_TEXCOORD_0) out vec2 v_UV;
#endif

#ifdef ID_NORMAL
	#ifdef ID_TANGENT
		layout (location = ID_NORMAL)  in  vec3 a_Normal;
		layout (location = ID_TANGENT) in  vec4 a_Tangent;
		layout (location = ID_TANGENT) out mat3 v_TBN;
	#else
		layout (location = ID_NORMAL) in  vec3 a_Normal;
		layout (location = ID_NORMAL) out vec3 v_Normal;
	#endif
#endif

layout (location = ID_WORLDPOS) out vec3 v_Position;

layout (std140, binding = 0) uniform perBatch 
{
	mat4 u_MVPMatrix;
} myPerBatch;

layout (std140, binding = 1) uniform perObject
{
	mat4 u_ModelMatrix;
} myPerObject;

void main()
{
  vec4 pos = myPerObject.u_ModelMatrix * a_Position;
  vec3 v_Position = vec3(pos.xyz) / pos.w;

  #ifdef ID_NORMAL
  #ifdef ID_TANGENT
  vec3 normalW = normalize(vec3(myPerObject.u_ModelMatrix * vec4(a_Normal.xyz, 0.0)));
  vec3 tangentW = normalize(vec3(myPerObject.u_ModelMatrix * vec4(a_Tangent.xyz, 0.0)));
  vec3 bitangentW = cross(normalW, tangentW) * a_Tangent.w;
  
  v_TBN = mat3(tangentW, bitangentW, normalW);
  #else // HAS_TANGENTS != 1
  v_Normal = normalize(vec3(myPerObject.u_ModelMatrix * vec4(a_Normal.xyz, 0.0)));
  #endif
  #endif

  #ifdef ID_TEXCOORD_0
  v_UV = a_UV;
  #else
  v_UV = vec2(0.,0.);
  #endif

  gl_Position = myPerBatch.u_MVPMatrix * pos; // needs w for proper perspective correction
}

