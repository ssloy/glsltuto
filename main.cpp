#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#define ROTATE 0
#define RENDER_SPHERES_INSTEAD_OF_VERTICES 0
GLuint  prog_hdlr;
GLint location_attribute_0, location_viewport;

const int NATOMS        = 1000;
const int SCREEN_WIDTH  = 1024;
const int SCREEN_HEIGHT = 1024;
const float camera[]           = {.6,0,1};
const float light0_position[4] = {1,1,1,0};
std::vector<std::vector<float> > atoms;
float angle = 0.72;

float rand_minus_one_one() {
	return (float)rand()/(float)RAND_MAX*(rand()>RAND_MAX/2?1:-1);
}

float rand_zero_one() {
	return (float)rand()/(float)RAND_MAX;
}

float cur_camera[] = {0,0,0};

void render_scene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	cur_camera[0] = cos(angle)*camera[0]+sin(angle)*camera[2];
	cur_camera[1] = camera[1];
	cur_camera[2] = cos(angle)*camera[2]-sin(angle)*camera[0];
#if ROTATE
	angle+=0.01;
#endif
	gluLookAt(cur_camera[0], cur_camera[1], cur_camera[2], 0,  0, 0, 0, 1, 0);

#if RENDER_SPHERES_INSTEAD_OF_VERTICES
	for (int i=0; i<NATOMS; i++) {
		glColor3f(atoms[i][4], atoms[i][5], atoms[i][6]);
		glPushMatrix();
		glTranslatef(atoms[i][0], atoms[i][1], atoms[i][2]);
		glutSolidSphere(atoms[i][3], 16, 16);
		glPopMatrix();
	}
#else
	glUseProgram(prog_hdlr);
	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	glUniform4fv(location_viewport, 1, viewport);
	glBegin(GL_POINTS);
	for (int i=0; i<NATOMS; i++) {
		glColor3f(atoms[i][4], atoms[i][5], atoms[i][6]);
		glVertexAttrib1f(location_attribute_0, atoms[i][3]);
		glVertex3f(atoms[i][0], atoms[i][1], atoms[i][2]);
	}
	glEnd();
	glUseProgram(0);
#endif

	glutSwapBuffers();
}

void process_keys(unsigned char key, int x, int y) {
	if (27==key) {
		exit(0);
	}
}

void change_size(int w, int h) {
    float ratio = (1.0*w)/(!h?1:h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, ratio, .1, 100);
//    glOrtho(-1,1,-1,1,-2,2);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

#if !RENDER_SPHERES_INSTEAD_OF_VERTICES
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
	for (int i=0; i<NATOMS; i++) {
		std::vector<float> tmp;
		for (int c=0; c<3; c++) {
			tmp.push_back(rand_minus_one_one()/2);      // xyz
		}
		tmp.push_back(rand_zero_one()/8.0); // radius
		for (int c=0; c<3; c++) {
			tmp.push_back(rand_zero_one()); // rgb
		}
		atoms.push_back(tmp);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("GLSL tutorial");
	glClearColor(0.0,0.0,1.0,1.0);

	glutDisplayFunc(render_scene);
	glutReshapeFunc(change_size);
	glutKeyboardFunc(process_keys);
#if ROTATE
	glutIdleFunc(render_scene);
#endif

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

#if !RENDER_SPHERES_INSTEAD_OF_VERTICES
	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		std::cout << "Ready for GLSL - vertex, fragment, and geometry units" << std::endl;
	else {
		std::cout << "No GLSL support" << std::endl;
		exit(1);
	}
	setShaders(prog_hdlr, "shaders/vert_shader.glsl", "shaders/frag_shader.glsl");

	location_attribute_0   = glGetAttribLocation(prog_hdlr, "R");          // radius
	location_viewport = glGetUniformLocation(prog_hdlr, "viewport"); // viewport

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
	glutMainLoop();
	return 0;
}

