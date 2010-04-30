#version 130

in vec2 in_f_TexCoord;
in vec3 in_f_ObjSpacePosition;
in vec3 in_f_Normal;
in vec3 in_f_Tangent;
in vec3 in_f_Bitangent;

out vec4 res_color;

uniform vec4 color;
uniform vec3 ObjSpaceLightPos;
uniform vec3 ObjSpaceCamPos;
uniform sampler2D BumpMap;
uniform float Depth;

float ray_intersect_rm(vec2 dp, vec2 ds)
{
    const int linear_search_steps = 15;

    float size = 1.0 / linear_search_steps;
    float depth = 0.0;

    for(int i = 0; i < linear_search_steps - 1; ++i)
    {
	vec4 t = texture(BumpMap, dp+ds+depth);
	if(depth < t.w)
	    depth += size;
    }

    const int binary_search_steps = 5;

    for(int ii = 0; ii < binary_search_steps; ++ii)
    {
	size *= 0.5;
	vec4 t = texture(BumpMap, dp+ds+depth);
	if(depth < t.w)
	    depth += (2*size);

	depth -= size;
    }

    return depth;
}

float ray_intersect_rm_lin( vec2 dp, vec2 ds)
{
    const int linear_search_steps = 15;
    float size = 1.0 / linear_search_steps;
    float depth = 0.0;
    for(int i = 0; i < linear_search_steps -1; ++i)
    {
	vec4 t = texture(BumpMap, dp+ds*depth);
	if(depth < t.w)
	    depth += size;
    }
    return depth;
}

void
main()
{
    vec3 initnorm = normalize(in_f_Normal);
    mat3 tnb = transpose(mat3(in_f_Tangent, in_f_Bitangent, initnorm));
    vec3 viewdirorg = tnb * (ObjSpaceCamPos - in_f_ObjSpacePosition);
    vec3 viewdir = normalize(viewdirorg);
    float a = dot(initnorm, -viewdir);
    vec3 s = vec3(dot(viewdir, in_f_Tangent), dot(viewdir, in_f_Bitangent), a);
    s *= Depth/a;

    vec2 ds = s.xy;
    vec2 dp = in_f_TexCoord;
    float d = ray_intersect_rm(dp, ds);
    vec2 uv = dp+ds*d;
    vec3 norm = texture(BumpMap, uv).xyz - vec3(0.5,0.5,0.5);

    // compute light
    viewdirorg += viewdir * d / (a * Depth);
    vec3 ln = normalize(viewdirorg - ObjSpaceLightPos);
    float att = clamp(dot(-ln, initnorm), 0.0, 1.0);
    float diff = clamp(dot(-ln, norm), 0.0, 1.0);
    float spec = clamp(dot(normalize(-ln-viewdir), norm),
			  0.0, 1.0);

    dp += ds * d;
    a = dot(initnorm, -ln);
    s = vec3(dot(ln, in_f_Tangent), dot(ln, in_f_Bitangent), a);
    s *= Depth / a;
    ds = s.xy;
    dp -= ds * d;
    float dl = ray_intersect_rm_lin(dp, ds);
    if( dl < d - 0.05) //in shadow
    {
	diff *= dot(color, vec4(1.0)) * 0.3333333;
	spec = 0;
    }

    spec = pow(spec, 20.0);

    res_color = color * diff + spec * vec4(1.0);
};
