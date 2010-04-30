#version 130

in vec2 in_f_TexCoord;
in vec3 in_f_CamSpaceNormal;
in vec3 in_f_CamSpacePosition;

out vec4 res_Color;

uniform vec4 color;
uniform vec3 CamSpaceLightPos;

void
main()
{
    vec3 p = in_f_CamSpacePosition;
    vec3 l = CamSpaceLightPos-p;
    float d = 1.0/length(l);
    l *= d;
    vec3 n = normalize( in_f_CamSpaceNormal );
    vec3 v = normalize( p );
    vec3 r = reflect( v, n );
    float diff = d*max( 0.1, dot( n, l ) );
    float spec = pow( max( 0.0, dot(n, r)), 20.0);
    res_Color = diff * color + 
                spec * vec4(1.0);
}