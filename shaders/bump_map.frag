#version 130
    
in vec2 in_f_TexCoord;
in vec3 in_f_ObjSpacePosition;
in vec3 in_f_Normal;
in vec3 in_f_Tangent;
in vec3 in_f_Bitangent;
in mat3 in_f_tnbmat;
    
out vec4 res_Color;
    
uniform vec4 color;
uniform vec3 ObjSpaceLightPos;
uniform vec3 ObjSpaceCamPos;
uniform sampler2D BumpMap;
    
void
main()
{
    vec3 init_norm = normalize(in_f_Normal);
    mat3 tnb = transpose(mat3(in_f_Tangent, in_f_Bitangent, init_norm));
    /* vec3 bump = texture(BumpMap, in_f_TexCoord).xyz; */
    /* bump.xy = bump.xy*2.f-1.f; */
    /* vec3 norm = bump.z * init_norm + bump.x*in_f_Tangent + bump.y*in_f_Bitangent; */
    vec3 norm = texture(BumpMap, in_f_TexCoord).xyz;
    norm.xy = norm.xy*2.f-1.f;
    /* norm = norm * 2.f - 1.f; */
    vec3 cam2pos = ObjSpaceCamPos - in_f_ObjSpacePosition;
    vec3 l = ObjSpaceLightPos-in_f_ObjSpacePosition;
    float d = 1.0/length(l);
    l *= d;
    l = tnb * normalize(l);
    cam2pos = tnb * cam2pos;
    vec3 n = normalize( norm );
    vec3 v = normalize( cam2pos );
    vec3 s = normalize(l+v);
    float diff = d*max( 0.1, dot( n, l ) );
    float spec = pow( max( 0.0, dot(s, n)), 20.0);
    res_Color = diff * color + 
	spec * vec4(1.0);
    // res_Color.rgb = bump;
};
