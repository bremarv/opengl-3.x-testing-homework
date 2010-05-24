#version 130

in vec2 in_v_TexCoord;
in vec3 in_v_Normal;
in vec4 in_v_Tangent;
in vec3 in_v_Position;
in ivec4 in_v_Bones;
in vec4 in_v_Weights;

out vec2 in_f_TexCoord;
out vec4 in_f_LightTexPosition;
out vec3 in_f_lightdir;
out vec3 in_f_lookdir;

uniform mat4 ModelView;
uniform mat4 ModelViewProjection;
uniform mat3 ModelViewInverseTranspose;
uniform mat4 LightTexFromObjectMatrix;
uniform mat4 boneMatrices[4];
uniform vec3 CamSpaceLightPos;
uniform vec3 ObjSpaceLightPos;
uniform vec3 ObjSpaceCamPos;

void main()
{
    vec4 position = in_v_Weights[0] * boneMatrices[in_v_Bones.x] *
	vec4(in_v_Position, 1.0);
    vec4 norm = in_v_Weights[0] * boneMatrices[in_v_Bones.x] *
	vec4(in_v_Normal, 0.0);
    vec4 tangent = in_v_Weights[0] * boneMatrices[in_v_Bones.x] *
	vec4(in_v_Tangent.xyz, 0.0);
    position = position + in_v_Weights[1] * boneMatrices[in_v_Bones.y] *
	vec4(in_v_Position, 1.0);
    norm = norm + in_v_Weights[1] * boneMatrices[in_v_Bones.y] *
	vec4(in_v_Normal, 0.0);
    tangent = tangent + in_v_Weights[1] * boneMatrices[in_v_Bones.y] *
	vec4(in_v_Tangent.xyz, 0.0);
    
    vec4 lsp = LightTexFromObjectMatrix * position;
    in_f_LightTexPosition = lsp;
    in_f_TexCoord = in_v_TexCoord;
    gl_Position = ModelViewProjection * position;
    vec3 bitangent = cross(normalize(norm.xyz), normalize(in_v_Tangent.xyz)) * in_v_Tangent.w;
    mat3 tnbmat = transpose(mat3(normalize(in_v_Tangent.xyz), normalize(bitangent), normalize(in_v_Normal)));
    in_f_lookdir = tnbmat * (ObjSpaceCamPos - position.xyz);
    in_f_lightdir = tnbmat * (ObjSpaceLightPos - position.xyz);
}
