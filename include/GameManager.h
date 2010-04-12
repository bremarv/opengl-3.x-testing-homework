#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include <GL/glew.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <Geometry.h>
#include <Object.h>
#include <SkinningShape.h>
#include <ShaderProgram.h>
#include <Snake.h>
#include <SkinningShader.h>
#include <RenderList.h>
#include "TextureHandle.h"

class GameManager
{
public:
    enum {WINDOW_SIZE_WIDTH  = 640, WINDOW_SIZE_HEIGHT = 480, SHADOW_MAP_SIZE=512 };
    void init();
//    void play();
    void quit();
	void reshape(int w, int h);
	void display();
protected:


    void animateFrame();

    void renderFrame();

	void recomputeNormals();

	void CreateCubeMapFrameBuffer(int width, int height);

private:
    long                 m_start_ticks;
	long m_prev_fps_ticks;
	int  m_frames;

	// textures
	boost::shared_ptr<TextureHandle> m_diffuse_tex;
	boost::shared_ptr<TextureHandle> m_bump_tex;
	boost::shared_ptr<TextureHandle> m_cubemap_tex;
	boost::shared_ptr<TextureHandle> m_depthCubemap_tex;
	boost::shared_ptr<TextureHandle> m_renderToCubemap_tex;
	
	// shader programs
    ShaderProgram*       m_phong_shader;
    ShaderProgram*       m_light_shader;
    ShaderProgram*       m_shadowcast_shader;
	ShaderProgram*       m_textured_shader;
	ShaderProgram*       m_skinning_shader;
	ShaderProgram*       m_cubemap_gen_shader;
	ShaderProgram*       m_cubemap_shader;

    // geometries
    Geometry*            m_geo_outer_cube;
    Geometry*            m_geo_cube;
    Geometry*            m_geo_light;
    Snake*            m_geo_snake;

    // objects
    Camera*              m_camera;
    Light*               m_light;
    Shape*               m_outer_box;
	
	std::vector<SkinningShape*> m_snakes;
	std::vector<Shape*>  m_boxes;

    // render passes
    RenderList           m_onscreen_pass;
    RenderList           m_cubemap_pass;
    RenderList           m_shadow_pass;

    // FBO for shadow pass
    GLuint               m_shadow_fbo;
    GLuint               m_cubemap_fbo;

    int m_window_size_width;
    int m_window_size_height;


};


#endif // _GAMEMANAGER_H_
