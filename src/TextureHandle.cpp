#include <assert.h>
#include <stdexcept>
#include <SDL_image.h>
#include <sstream>
#include <siut/simd/Vec3f.hpp>
#include "TextureHandle.h"


using siut::simd::Vec3f;
TextureHandle::TextureHandle(GLuint tex) : m_tex(tex)
{
	assert(m_tex>0);
}

TextureHandle::~TextureHandle()
{
	if (m_tex>0)
		glDeleteTextures(1, &m_tex);
}

GLuint TextureHandle::getTex() const
{
	return m_tex;
}

// Code "borrowed" from Anders Svendal, hope its OK
TextureHandle *TextureHandle::createTexture(std::string filename)
{
	SDL_Surface *texture = IMG_Load(filename.c_str());
	if(0 == texture)
	{
	    std::stringstream out;
	    out << __FILE__ << ": unable to load texture "
		<< filename;
	    throw std::runtime_error(out.str());
	}
		
	// Generate OpenGL texture
	GLuint texName;
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLenum format;
	if (texture->format->BytesPerPixel == 3)
		format = GL_RGB;
	else if (texture->format->BytesPerPixel == 4)
		format = GL_RGBA;
	else
		throw std::runtime_error( __FILE__ );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w, texture->h, 0, format, GL_UNSIGNED_BYTE, texture->pixels);
	SDL_FreeSurface(texture);
	return new TextureHandle(texName);
}

//Read height field and create normal map. Asserts If image contains more than one byte is 
TextureHandle *
TextureHandle::createNormalMapFromHeightField(std::string filename)
{
	SDL_Surface *texture = IMG_Load(filename.c_str());
	assert(texture->format->BytesPerPixel == 1 || texture->format->BytesPerPixel == 3);
	
	unsigned int stride = (texture->format->BytesPerPixel == 1) ? 1 : 3;
	GLubyte *normals = new GLubyte[(texture->w-1)*(texture->h-1)*3];

	GLubyte *p=static_cast<GLubyte*>(texture->pixels);
	for (int i=0; i<texture->h-1; i++)
		for (int j=0; j<texture->w-1; j++)
		{
			float dx=(p[texture->pitch*i+stride*(j+1)]-p[texture->pitch*i+stride*j])/255.f;
			float dy=(p[texture->pitch*(i+1)+stride*j]-p[texture->pitch*i+stride*j])/255.f;

			Vec3f n(-dx, -dy, 0.25f);
			n.normalize();
			GLubyte *np=&normals[3*((texture->w-1)*i+j)];
			np[0]=static_cast<GLubyte>((n.x()+1)*127);
			np[1]=static_cast<GLubyte>((n.y()+1)*127);
			np[2]=static_cast<GLubyte>((n.z())*255);
		}

	// Generate OpenGL texture

	GLuint texName;
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w-1, texture->h-1, 0, GL_RGB, GL_UNSIGNED_BYTE, normals);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w, texture->h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->pixels);
	delete [] normals;
	SDL_FreeSurface(texture);
	return new TextureHandle(texName);
}

