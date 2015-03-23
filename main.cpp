#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#define USE_SHADERS 1
GLuint  prog_hdlr;

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

#if USE_SHADERS
void printInfoLog(GLuint obj) {
	int log_size = 0;
	int bytes_written = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &log_size);
	if (!log_size) return;
	char *infoLog = new char[log_size];
	glGetProgramInfoLog(obj, log_size, &bytes_written, infoLog);
	std::cerr << infoLog << std::endl;
	delete [] infoLog;
}

bool read_n_compile_shader(const char *filename, GLuint &hdlr, GLenum shaderType) {
	std::ifstream is(filename, std::ios::in|std::ios::binary|std::ios::ate);
	if (!is.is_open()) {
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}
	long size = is.tellg();
	char *buffer = new char[size+1];
	is.seekg(0, std::ios::beg);
	is.read (buffer, size);
	is.close();
	buffer[size] = 0;

	hdlr = glCreateShader(shaderType);
	glShaderSource(hdlr, 1, (const GLchar**)&buffer, NULL);
	glCompileShader(hdlr);
	std::cerr << "info log for " << filename << std::endl;
	printInfoLog(hdlr);
	delete [] buffer;
	return true;
}

void setShaders(GLuint &prog_hdlr, const char *vsfile, const char *fsfile) {
	GLuint vert_hdlr, frag_hdlr;
	read_n_compile_shader(vsfile, vert_hdlr, GL_VERTEX_SHADER);
	read_n_compile_shader(fsfile, frag_hdlr, GL_FRAGMENT_SHADER);

	prog_hdlr = glCreateProgram();
	glAttachShader(prog_hdlr, frag_hdlr);
	glAttachShader(prog_hdlr, vert_hdlr);

	glLinkProgram(prog_hdlr);
	std::cerr << "info log for the linked program" << std::endl;
	printInfoLog(prog_hdlr);
}
#endif


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

#if USE_SHADERS
	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		std::cout << "Ready for GLSL - vertex, fragment, and geometry units" << std::endl;
	else {
		std::cout << "No GLSL support" << std::endl;
		exit(1);
	}
	setShaders(prog_hdlr, "shaders/vert_shader.glsl", "shaders/frag_shader.glsl");
	glUseProgram(prog_hdlr);
#endif
	glutMainLoop();
	return 0;
}

