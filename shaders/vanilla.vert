#version 130

in vec2 in_v_TexCoord;
in vec3 in_v_Normal;
in vec3 in_v_Position;

out vec2 in_f_TexCoord;
out vec3 in_f_CamSpaceNormal;
out vec3 in_f_CamSpacePosition;
out vec4 in_f_LightTexPosition;

uniform mat4 ModelView;\n
uniform mat4 ModelViewProjection;
uniform mat3 ModelViewInverseTranspose;
uniform mat4 LightTexFromObjectMatrix;

void
main()
{
    in_f_TexCoord = in_v_TexCoord;
    in_f_CamSpaceNormal = ModelViewInverseTranspose * in_v_Normal;
    vec4 csp = ModelView * vec4( in_v_Position, 1.0 );
    in_f_CamSpacePosition = (1.0/csp.w)*csp.xyz;
    vec4 p =  vec4( in_v_Position, 1.0 );
    vec4 lsp = LightTexFromObjectMatrix * p;
    in_f_LightTexPosition = lsp;
    gl_Position = ModelViewProjection * p;
}