#include <string>
#include <stdexcept>
#include "BumpShader.h"
#include "constants.h"

// TODO: oppdater shaderne til å gjøre 
// lyssettingen i TNB space
//static std::string bump_map_vs_src = "shaders/bump_map.vert\0";
// "#version 130\n"
// "\n"
// "in  vec2 in_v_TexCoord;\n"
// "in  vec3 in_v_Normal;\n"
// "in  vec3 in_v_Position;\n"
// "in vec3 in_v_Tangent;\n"
// "in vec3 in_v_Bitangent;\n"
// "\n"
// "out vec3 in_f_Normal;\n"
// "out vec3 in_f_Tangent;\n"
// "out vec3 in_f_Bitangent;\n"
// "out vec2 in_f_TexCoord;\n"
// "out vec4 in_f_LightTexPosition;\n"
// "out vec3 in_f_ObjSpacePosition;\n"
// "out mat3 in_f_tnbmat;\n"

// "\n"
// "uniform mat4 ModelView;\n"
// "uniform mat4 ModelViewProjection;\n"
// "uniform mat3 ModelViewInverseTranspose;\n"
// "uniform mat4 LightTexFromObjectMatrix;\n"
// "uniform vec3 CamSpaceLightPos;\n"

// "\n"
// "void\n"
// "main()\n"
// "{\n"
// "    in_f_TexCoord = in_v_TexCoord;\n"
// "    vec4 csp = ModelView * vec4( in_v_Position, 1.0 );\n"
// "    in_f_ObjSpacePosition=in_v_Position;\n"
// "    vec4 p =  vec4( in_v_Position, 1.0 );\n"
// "    vec4 lsp = LightTexFromObjectMatrix * p;\n"
// "    in_f_LightTexPosition = lsp;\n"
// " in_f_Normal = in_v_Normal;\n"
// " in_f_Tangent = in_v_Tangent;\n"
// " in_f_Bitangent = in_v_Bitangent;\n"
// "    gl_Position = ModelViewProjection * p;\n"
// " in_f_tnbmat = mat3(in_v_Tangent, in_v_Bitangent, normalize(in_v_Normal));\n"
// "}\n";

// static std::string phong_fs_src =
// 	"#version 130\n"
// 	"\n"
// 	"in vec2 in_f_TexCoord;\n"
// 	"in vec3 in_f_ObjSpacePosition;\n"
// 	"\n"
// 	"out vec4 res_Color;\n"
// 	"\n"
// 	"uniform vec4 color;\n"
// 	"uniform vec3 ObjSpaceLightPos;\n"
// 	"uniform sampler2D BumpMap;\n"
// 	"\n"
//         "void\n"
//         "main()\n"
//         "{\n"
// 		"vec4 diffuseColor = texture(DiffuseMap, in_f_TexCoord) * color;\n"
// 	"    vec3 p = in_f_ObjSpacePosition;\n"
//         "    vec3 l = ObjSpaceLightPos-p;\n"
// 	"    float d = 1.0/length(l);\n"
// 	"    l *= d;\n"
//         "    vec3 n = normalize( in_f_CamSpaceNormal );\n"
// 	"    vec3 v = normalize( p );\n"
// 	"    vec3 r = reflect( v, n );\n"
//         "    float diff = d*max( 0.1, dot( n, l ) );\n"
//         "    float spec = pow( max( 0.0, dot(n, r)), 20.0);\n"
// 	"    res_Color = diff * diffuseColor + \n"
//         "                spec * vec4(1.0);\n"
// " res_Color.xyz = ObjSpaceLightPos;\n"
//         "}\n";


//static std::string bumpMap_fs_src = "bump_map.frag\0";
//     "#version 130\n"
//     "\n"
//     "in vec2 in_f_TexCoord;\n"
//     "in vec3 in_f_ObjSpacePosition;\n"
//     "in vec3 in_f_Normal;\n"
//     "in  vec3 in_f_Tangent;\n"
//     "in  vec3 in_f_Bitangent;\n"
//     "in mat3 in_f_tnbmat;\n"
//     "\n"
//     "out vec4 res_Color;\n"
//     "\n"
//     "uniform vec4 color;\n"
//     "uniform vec3 ObjSpaceLightPos;\n"
//     "uniform vec3 ObjSpaceCamPos;\n"
//     "uniform sampler2D BumpMap;\n"
//     "\n"
//     "void\n"
//     "main()\n"
//     "{\n"
//     " vec3 init_norm = normalize(in_f_Normal);\n"
//     " mat3 tnb = transpose(mat3(in_f_Tangent, in_f_Bitangent, init_norm));\n"
//     // " vec3 bump = texture(BumpMap, in_f_TexCoord).xyz;\n"
//     // " bump.xy = bump.xy*2.f-1.f;\n"
//     // " vec3 norm = bump.z * init_norm + bump.x*in_f_Tangent + bump.y*in_f_Bitangent;\n"
//     " vec3 norm = texture(BumpMap, in_f_TexCoord).xyz;\n"
//     " norm.xy = norm.xy*2.f-1.f;\n"
// //    " norm = norm * 2.f - 1.f;\n"
//     " vec3 cam2pos = ObjSpaceCamPos - in_f_ObjSpacePosition;\n"
//     " vec3 l = ObjSpaceLightPos-in_f_ObjSpacePosition;\n"
//     " float d = 1.0/length(l);\n"
//     " l *= d;\n"
//     " l = tnb * normalize(l);\n"
//     " cam2pos = tnb * cam2pos;\n"
//     " vec3 n = normalize( norm );\n"
//     " vec3 v = normalize( cam2pos );\n"
//     " vec3 s = normalize(l+v);\n"
//     " float diff = d*max( 0.1, dot( n, l ) );\n"
//     " float spec = pow( max( 0.0, dot(s, n)), 20.0);\n"
//     " res_Color = diff * color + \n"
//     "             spec * vec4(1.0);\n"
//     " // res_Color.rgb = bump;\n"
//     "}\n";

BumpShader::BumpShader( ShaderType type ) :
    StandardShaderProgram(type)
{
}

void BumpShader::init()
{
    std::string vs;
    std::string fs;

    switch( m_type ) {

    case LIGHT_SHADER:
//        vs = vanilla_vs_src;
//        fs = lightshape_fs_src;
	throw std::runtime_error( __FILE__ );
        break;

    case PHONG_SHADER:
	vs = StandardShaderProgram::getVanillaVsSrc();
	fs = StandardShaderProgram::getPhongFsSrc();		
//        fs = phong_fs_src;
	vs = readshaderfromfile( "shaders/bump_map.vert");
	fs = readshaderfromfile( "shaders/bump_map.frag");
//	fs = readshaderfromfile( "shaders/bump_map_relief.frag");
//		throw std::runtime_error( __FILE__ );
        break;

    case PHONG_WITH_SHADOW:
	throw std::runtime_error( __FILE__ );
        break;

    case SHADOW_CAST:
	vs = StandardShaderProgram::getVanillaVsSrc();
	fs = StandardShaderProgram::getShadowcastFsSrc();
        break;

    default:
        throw std::runtime_error( __FILE__ );
    }
    init(vs, fs);
}

void BumpShader::init(const std::string &vs, const std::string &fs)
{
    StandardShaderProgram::init(vs, fs);
    glBindAttribLocation( m_prog, TANGENT_ATTRIB_LOCATION, "in_v_Tangent" );
    //glBindAttribLocation( m_prog, BITANGENT_ATTRIB_LOCATION, "in_v_Bitangent" );

    m_cam_pos_in_objspace_loc =
	glGetUniformLocation( m_prog, "ObjSpaceCamPos");
    m_lightpos_in_objspace_loc =
	glGetUniformLocation( m_prog, "ObjSpaceLightPos");
    m_bumpmap_sampler_loc =
	glGetUniformLocation( m_prog, "BumpMap" );
    m_relief_depth_loc =
	glGetUniformLocation( m_prog, "Depth");
}


BumpShader*
BumpShader::factory( ShaderType type )
{
    BumpShader* ret = new BumpShader(type);
    try
    {
	ret->init();
    } catch (...)
    {
	delete ret;
	throw;
    }

    return ret;
}

void 
BumpShader::setBumpMapTexture(boost::shared_ptr<TextureHandle> tex)
{
    m_bump_tex = tex;
}

void
BumpShader::setUp( Object*  shape,
		   Viewer*  camera,
		   Light*   light )
{
    StandardShaderProgram::setUp(shape, camera, light);

    if( m_bumpmap_sampler_loc != -1 ) {
	glUniform1i( m_bumpmap_sampler_loc, BUMP_MAP_TEXTURE_UNIT );
	glActiveTexture( GL_TEXTURE0 + BUMP_MAP_TEXTURE_UNIT);
	if (m_bump_tex)
	    glBindTexture( GL_TEXTURE_2D, m_bump_tex->getTex() );
    }

    if(m_relief_depth_loc != -1)
    {
	glUniform1f( m_relief_depth_loc, 1.f);
    }

    HasGeometry* hg = dynamic_cast<HasGeometry*>( shape );

    if (hg)
    {
	if( m_cam_pos_in_objspace_loc != -1 ) {
	    siut::simd::Vec4f lp = shape->getObjectFromWorldMatrix() *
		camera->getWorldFromObjectMatrix() *
		siut::simd::Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
	    float s = 1.0f/lp[3];
	    glUniform3f( m_cam_pos_in_objspace_loc, s*lp[0], s*lp[1], s*lp[2] );
	}

	if( light != NULL ) {
	    if( m_lightpos_in_objspace_loc != -1 ) {
		siut::simd::Vec4f lp = shape->getObjectFromWorldMatrix() *
		    light->getWorldFromObjectMatrix() *
		    siut::simd::Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
		float s = 1.0f/lp[3];
		glUniform3f( m_lightpos_in_objspace_loc, s*lp[0], s*lp[1], s*lp[2] );
	    }
	}
    }
}


BumpShader::~BumpShader(void)
{
}

