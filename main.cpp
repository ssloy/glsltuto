#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <cmath>

const int SCREEN_WIDTH  = 1024;
const int SCREEN_HEIGHT = 1024;
const float camera[]           = {.6,0,1};
const float light0_position[4] = {1,1,1,0};

void render_scene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(camera[0], camera[1], camera[2], 0,  0, 0, 0, 1, 0);
	glColor3f(.8, 0., 0.);
	glutSolidTeapot(.7);
	glutSwapBuffers();
}

void process_keys(unsigned char key, int x, int y) {
	if (27==key) {
		exit(0);
	}
}

void change_size(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	glOrtho(-1,1,-1,1,-1,8);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("GLSL tutorial");
	glClearColor(0.0,0.0,1.0,1.0);

	glutDisplayFunc(render_scene);
	glutReshapeFunc(change_size);
	glutKeyboardFunc(process_keys);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glutMainLoop();
	return 0;
}

