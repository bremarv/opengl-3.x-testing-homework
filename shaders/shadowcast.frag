#version 140
        
out vec4 res_Color;
        
void
main()
{
    res_Color = vec4( vec3( 1.0-2.5*gl_FragCoord.w), 1.0 );
};
