#ifndef _TEXTUREHANDLE_H_
#define _TEXTUREHANDLE_H_
#include <GL/glew.h>
#include <string>

class TextureHandle
{
public:
	TextureHandle(GLuint tex);
	~TextureHandle();

	static TextureHandle *createTexture(std::string filename);
	static TextureHandle *createNormalMapFromHeightField(std::string filename);
	static TextureHandle *createBlinnMapFromHeightField(std::string filename);
	GLuint getTex() const;
protected:
	GLuint m_tex;
};




#endif // _TEXTUREHANDLE_H_