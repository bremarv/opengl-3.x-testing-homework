#version 130
   
in vec2 in_v_TexCoord;
in vec3 in_v_Normal;
in vec3 in_v_Position;
in vec3 in_v_Tangent;
in vec3 in_v_Bitangent;
   
/* out vec3 in_f_Normal; */
/* out vec3 in_f_Tangent; */
/* out vec3 in_f_Bitangent; */
out vec2 in_f_TexCoord;
/* out vec3 in_f_ObjSpacePosition; */
/* out mat3 in_f_tnbmat; */
out vec4 in_f_LightTexPosition;
out vec3 in_f_lightdir;
out vec3 in_f_lookdir;
   
uniform mat4 ModelView;
uniform mat4 ModelViewProjection;
uniform mat3 ModelViewInverseTranspose;
uniform mat4 LightTexFromObjectMatrix;
uniform vec3 CamSpaceLightPos;
uniform vec3 ObjSpaceLightPos;
uniform vec3 ObjSpaceCamPos;
    
void
main()
{

    /* vec4 csp = ModelView * vec4( in_v_Position, 1.0 ); */
    /* in_f_ObjSpacePosition=in_v_Position; */

    /* in_f_Normal = in_v_Normal; */
    /* in_f_Tangent = in_v_Tangent; */
    /* in_f_Bitangent = in_v_Bitangent; */

    vec4 p =  vec4( in_v_Position, 1.0 );
    vec4 lsp = LightTexFromObjectMatrix * p;
    in_f_LightTexPosition = lsp;
    in_f_TexCoord = in_v_TexCoord;
    gl_Position = ModelViewProjection * p;
    mat3 tnbmat = transpose(mat3(in_v_Tangent, in_v_Bitangent, normalize(in_v_Normal)));
    in_f_lookdir = tnbmat * (ObjSpaceCamPos - in_v_Position);
    in_f_lightdir = tnbmat * (ObjSpaceLightPos - in_v_Position);
}
