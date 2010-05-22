#version 130
	
in  vec2 in_v_TexCoord;
in  vec3 in_v_Normal;
in  vec3 in_v_Position;
in  ivec4 in_v_Bones;
in  vec4 in_v_Weights;
	
out vec2 in_f_TexCoord;
out vec3 in_f_CamSpaceNormal;
out vec3 in_f_CamSpacePosition;
out vec4 in_f_LightTexPosition;
	
uniform mat4 ModelView;
uniform mat4 ModelViewProjection;
uniform mat3 ModelViewInverseTranspose;
uniform mat4 LightTexFromObjectMatrix;
uniform mat4 boneMatrices[4];
	
void
main()
{
    in_f_TexCoord = in_v_TexCoord;
    vec4 position =  in_v_Weights[0] *  boneMatrices[in_v_Bones.x] * vec4(in_v_Position, 1.0);
    vec4 norm =   in_v_Weights[0] *  boneMatrices[in_v_Bones.x] * vec4(in_v_Normal, 0.0);
    position =  position + in_v_Weights[1] * boneMatrices[in_v_Bones.y] * vec4(in_v_Position, 1.0);
    norm = norm +  in_v_Weights[1] *  boneMatrices[in_v_Bones.y] * vec4(in_v_Normal, 0.0);
    in_f_CamSpaceNormal = ModelViewInverseTranspose * norm.xyz;
    vec4 csp = ModelView * position;
    in_f_CamSpacePosition = (1.0/csp.w)*csp.xyz;
    vec4 p =  position;
    vec4 lsp = LightTexFromObjectMatrix * p;
    in_f_LightTexPosition = lsp;
    gl_Position = ModelViewProjection * p;
}
