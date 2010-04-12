#include "GameManager.h"
#include <iostream>
#include <GL/freeglut.h>

GameManager game;

void
reshape(int w, int h)
{
	game.reshape(w, h);
    glutPostRedisplay();
}

void
idle()
{
    glutPostRedisplay();
}

void
display()
{
	game.display();
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize( GameManager::WINDOW_SIZE_WIDTH, GameManager::WINDOW_SIZE_HEIGHT );
    glutCreateWindow( "PG611 Framework" );
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutIdleFunc( idle );

	try
	{
		game.init();
		glutMainLoop();
	} catch (std::exception &e)
	{
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
	}
	return 0;
}