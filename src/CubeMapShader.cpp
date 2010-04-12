#include <string>
#include "CubeMapShader.h"
#include "GlTools.h"
#include "constants.h"

using std::string;

static string vanilla_vs_src =
	"#version 130\n"
	"\n"
	"in  vec3 in_v_Normal;\n"
	"in  vec3 in_v_Position;\n"
	"\n"
	"out vec3 in_f_CamSpaceNormal;\n"
	"out vec3 in_f_CamSpacePosition;\n"
	"out vec3 in_f_ObjSpacePosition;\n"
	"\n"
	"uniform mat4 ModelView;\n"
	"uniform mat4 ModelViewProjection;\n"
	"uniform mat3 ModelViewInverseTranspose;\n"
        "uniform mat4 LightTexFromObjectMatrix;\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
		"in_f_ObjSpacePosition = in_v_Position;\n"
	"    in_f_CamSpaceNormal = ModelViewInverseTranspose * in_v_Normal;\n"
	"    vec4 csp = ModelView * vec4( in_v_Position, 1.0 );\n"
	"    in_f_CamSpacePosition = (1.0/csp.w)*csp.xyz;\n"
        "    vec4 p =  vec4( in_v_Position, 1.0 );\n"
        "    gl_Position = ModelViewProjection * p;\n"
        "}\n";

string geometry_src = 
	"#version 130\n"
	"#extension GL_EXT_geometry_shader4 : enable\n"
"\n"
	"out vec4 in_f_Color;\n"
"	void main(void)\n"
"	{\n"
"	\n"
"		//increment variable\n"
"		int i;\n"
"\n"
"		/////////////////////////////////////////////////////////////\n"
"		//This example has two parts\n"
"		//	step a) draw the primitive pushed down the pipeline\n"
"		//		 there are gl_Vertices # of vertices\n"
"		//		 put the vertex value into gl_Position\n"
"		//		 use EmitVertex => 'create' a new vertex\n"
"		// 		use EndPrimitive to signal that you are done creating a primitive!\n"
"		//	step b) create a new piece of geometry (I.E. WHY WE ARE USING A GEOMETRY SHADER!)\n"
"		//		I just do the same loop, but swizzle the x and y values\n"
"		//	result => the line we want to draw, and the same line, but along the other axis\n"
" \n"
"		//Pass-thru!\n"
"		for(i=0; i< gl_VerticesIn; i++){\n"
"			gl_Position = gl_PositionIn[i];\n"
"           in_f_Color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"			EmitVertex();\n"
"		}\n"
"		EndPrimitive();	\n"
"	}\n";

static string red_fs_src = 
	"#version 130\n"
	"\n"
	"in vec4 in_f_Color;\n"
	"out vec4 res_Color;\n"
	"\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
//	"res_Color = vec4(0.5, 0.5, 0.5, 1.0);\n"
	"res_Color = in_f_Color;\n"
        "}\n";

static string phong_fs_src =
	"#version 130\n"
	"\n"
	"in vec3 in_f_CamSpaceNormal;\n"
	"in vec3 in_f_CamSpacePosition;\n"
	"in vec3 in_f_ObjSpacePosition;\n"
	"\n"
	"uniform samplerCube CubeMap;\n"
	"out vec4 res_Color;\n"
	"\n"
	"uniform vec4 color;\n"
	"uniform vec3 CamSpaceLightPos;\n"
	"\n"
        "void\n"
        "main()\n"
        "{\n"
		"vec3 incident = in_f_CamSpacePosition;\n"
		"vec3 reflected = reflect(incident, in_f_CamSpaceNormal);\n"
	"    res_Color = texture(CubeMap, reflected);\n"
	" //   res_Color = texture(CubeMap, in_f_ObjSpacePosition*2-1);\n"
	"//    res_Color.xyz = in_f_ObjSpacePosition*2-1;\n"
	"//res_Color = vec4(0.5, 0.5, 0.5, 1.0);\n"
        "}\n";

CubeMapShader::CubeMapShader( ShaderType type ) :
StandardShaderProgram(type)
{
}

void CubeMapShader::init()
{
	std::string vs;
	std::string gs;
    std::string fs;

    switch( m_type ) {

    case PHONG_SHADER:
		vs = StandardShaderProgram::getVanillaVsSrc();
		fs = StandardShaderProgram::getPhongFsSrc();		
        vs = vanilla_vs_src;
//		gs = geometry_src;
//		fs =  red_fs_src;
		fs = phong_fs_src;
//		throw std::runtime_error( __FILE__ );
        break;
    default:
        throw std::runtime_error( __FILE__ );
    }
	StandardShaderProgram::init(vs, gs, fs);
	glProgramParameteriEXT(m_prog, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES); 
    ASSERT_GL;
	glProgramParameteriEXT(m_prog, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP); 
    ASSERT_GL;
	glProgramParameteriEXT(m_prog, GL_GEOMETRY_VERTICES_OUT_EXT, 3); 
    ASSERT_GL;
}

void CubeMapShader::setupUniforms()
{
	StandardShaderProgram::setupUniforms();
	m_cubeMap_loc =
		glGetUniformLocation( m_prog, "CubeMap");
    ASSERT_GL;
}


CubeMapShader*
CubeMapShader::factory( ShaderType type )
{
    CubeMapShader* ret = new CubeMapShader(type);
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
CubeMapShader::setUp( Object*  shape,
                      Viewer*  camera,
                      Light*   light )
{
    ASSERT_GL;
	StandardShaderProgram::setUp(shape, camera, light);

	if (m_cubeMap_loc >=0)
		glUniform1i(m_cubeMap_loc, ENVIRONMENT_MAP_TEXTURE_UNIT);
    ASSERT_GL;
}


CubeMapShader::~CubeMapShader(void)
{
}

