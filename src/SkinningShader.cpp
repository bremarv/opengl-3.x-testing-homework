#include <string>
#include <stdexcept>
#include "SkinningShader.h"
#include "SkinningShape.h"
#include "constants.h"

using std::string;

static string skinning_vs_src =
	"#version 130\n"
	"\n"
	"in  vec2 in_v_TexCoord;\n"
	"in  vec3 in_v_Normal;\n"
	"in  vec3 in_v_Position;\n"
	"in  ivec4 in_v_Bones;\n"
	"in  vec4 in_v_Weights;\n"
	"\n"
	"out vec2 in_f_TexCoord;\n"
	"out vec3 in_f_CamSpaceNormal;\n"
	"out vec3 in_f_CamSpacePosition;\n"
        "out vec4 in_f_LightTexPosition;\n"
	"\n"
	"uniform mat4 ModelView;\n"
	"uniform mat4 ModelViewProjection;\n"
	"uniform mat3 ModelViewInverseTranspose;\n"
        "uniform mat4 LightTexFromObjectMatrix;\n"
	"uniform mat4 boneMatrices[4];\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
	"    in_f_TexCoord = in_v_TexCoord;\n"
	"     vec4 position =  in_v_Weights[0] *  boneMatrices[in_v_Bones.x] * vec4(in_v_Position, 1.0);\n"
	"    vec4 norm =   in_v_Weights[0] *  boneMatrices[in_v_Bones.x] * vec4(in_v_Normal, 0.0);\n"
	"    position =  position + in_v_Weights[1] * boneMatrices[in_v_Bones.y] * vec4(in_v_Position, 1.0);\n"
	"    norm = norm +  in_v_Weights[1] *  boneMatrices[in_v_Bones.y] * vec4(in_v_Normal, 0.0);\n"
	"//position = in_v_Weights[0] * vec4(in_v_Position, 1.0);\n"
	"//position = vec4(in_v_Position, 1.0);\n"
	"norm.xyz = in_v_Normal\n;"
	"    in_f_CamSpaceNormal = ModelViewInverseTranspose * norm.xyz;\n"
	"    vec4 csp = ModelView * position;\n"
	"    in_f_CamSpacePosition = (1.0/csp.w)*csp.xyz;\n"
        "    vec4 p =  position;\n"
        "    vec4 lsp = LightTexFromObjectMatrix * p;\n"
        "    in_f_LightTexPosition = lsp;\n"
        "    gl_Position = ModelViewProjection * p;\n"
		"//if (in_v_Weights.x<0 || in_v_Weights.x>1.0)\n"
		"if (in_v_Bones.x==3)\n"
		"  in_f_CamSpaceNormal=vec3(0.0);\n"

        "}\n";

static std::string phong_fs_src =
	"#version 130\n"
	"\n"
	"in vec2 in_f_TexCoord;\n"
	"in vec3 in_f_ObjSpacePosition;\n"
	"\n"
	"out vec4 res_Color;\n"
	"\n"
	"uniform vec4 color;\n"
	"uniform vec3 ObjSpaceLightPos;\n"
	"uniform sampler2D DiffuseMap;\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
		"vec4 diffuseColor = texture(DiffuseMap, in_f_TexCoord) * color;\n"
	"    vec3 p = in_f_ObjSpacePosition;\n"
        "    vec3 l = ObjSpaceLightPos-p;\n"
	"    float d = 1.0/length(l);\n"
	"    l *= d;\n"
        "    vec3 n = normalize( in_f_CamSpaceNormal );\n"
	"    vec3 v = normalize( p );\n"
	"    vec3 r = reflect( v, n );\n"
        "    float diff = d*max( 0.1, dot( n, l ) );\n"
        "    float spec = pow( max( 0.0, dot(n, r)), 20.0);\n"
	"    res_Color = diff * diffuseColor + \n"
        "                spec * vec4(1.0);\n"
" res_Color.xyz = ObjSpaceLightPos;\n"
        "}\n";



SkinningShader::SkinningShader( ShaderType type ) :
StandardShaderProgram(type)
{
}

void SkinningShader::init()
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
		vs = skinning_vs_src;
//		vs = StandardShaderProgram::getVanillaVsSrc();
		fs = StandardShaderProgram::getPhongFsSrc();		
        break;

    case PHONG_WITH_SHADOW:
  //      vs = vanilla_vs_src;
//        fs = phong_with_shadow_fs_src;
		throw std::runtime_error( __FILE__ );
        break;

    case SHADOW_CAST:
		vs = skinning_vs_src;
		fs = StandardShaderProgram::getShadowcastFsSrc();
        break;

    default:
        throw std::runtime_error( __FILE__ );
    }
	StandardShaderProgram::init(vs, fs);

}

void SkinningShader::bindAttribLocations()
{
	StandardShaderProgram::bindAttribLocations();
    glBindAttribLocation( m_prog, SKINNINGINDEX_ATTRIB_LOCATION, "in_v_Bones" );
    glBindAttribLocation( m_prog, SKINNINGWEIGHT_ATTRIB_LOCATION, "in_v_Weights" );
}

void SkinningShader::setupUniforms()
{
	StandardShaderProgram::setupUniforms();
	m_boneMatrices_loc = 
		glGetUniformLocation( m_prog, "boneMatrices" );
}


SkinningShader*
SkinningShader::factory( ShaderType type )
{
    SkinningShader* ret = new SkinningShader(type);
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
SkinningShader::setUp( Object*  shape,
                      Viewer*  camera,
                      Light*   light )
{
	StandardShaderProgram::setUp(shape, camera, light);
	SkinningShape *sp = dynamic_cast<SkinningShape *>(shape);
	assert(sp);
	
// TODO set ben matrisene
}


SkinningShader::~SkinningShader(void)
{
}

