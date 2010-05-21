#version 130
    
in vec2 in_f_TexCoord;
in vec3 in_f_lightdir;
in vec3 in_f_lookdir;
    
out vec4 res_Color;
    
uniform vec4 color;
uniform sampler2D BumpMap;
    
void
main()
{
    vec3 norm = texture(BumpMap, in_f_TexCoord).xyz;
    norm.xy = norm.xy*2.f-1.f;
    float d = 1.0/length(in_f_lightdir);
    vec3 l = in_f_lightdir * d;
    vec3 n = normalize( norm );
    vec3 v = normalize( in_f_lookdir );
    vec3 s = normalize(l+v);
    float diff = d*max( 0.1, dot( n, l ) );
    float spec = pow( max( 0.0, dot(s, n)), 20.0);
    res_Color = diff * color +
    	spec * vec4(1.0);
};
