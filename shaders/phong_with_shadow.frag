#version 130

in vec2 in_f_TexCoord;
in vec3 in_f_CamSpaceNormal;
in vec3 in_f_CamSpacePosition;
in vec4 in_f_LightTexPosition;

out vec4 res_Color;

uniform vec4 color;
uniform vec3 CamSpaceLightPos;
uniform mat4 CamSpaceToLightTex;
uniform sampler2DShadow ShadowMap;

float
getIllumination( vec4 lph )
{
    return textureProj( ShadowMap, lph )*float(lph.z > 0.0);
    return float( all(greaterThan(lph.xyz, vec3(0))) && all(lessThan(lph.xy,lph.ww)) ) ;
}
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
    float i = getIllumination( in_f_LightTexPosition );
    float amb = 0.2*max(0.0, dot(n,-v ) );
    float diff = i*max( 0.0, dot( n, l ) );
    float spec = i*pow( max( 0.0, dot(n, r)), 50.0);
    res_Color = (amb+diff) * color + 
  spec * vec4(1.0);
}