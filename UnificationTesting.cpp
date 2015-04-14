
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdlib.h>

using namespace std;


using glm::vec4;
using glm::vec3;
using glm::mat4;

float red = 0.0;
float green = 0.0;
float blue = 1.0;

int object_count = 0;

class VertexData {
public:
	//GLubyte color[4];
	//glm::vec4 color;
	glm::vec3 normal;
	glm::vec4 position;
	//glm::vec3 normal;
	glm::vec3 Kd;
	glm::vec3 Ka;
	VertexData()
	{
		position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

};

class object {
public:
	VertexData *_shape_vertices;
	glm::vec3 *_vertex_normal;
	glm::vec3 *kd;
	glm::vec3 *ka;
	glm::vec4 dominating_color;
	int vertex_counts;
	GLushort *indices;
	GLuint object_program_shader;
	GLint dominating_color_uniform_loc;
	object()
	{
		vertex_counts = 0;
		_shape_vertices = NULL;
	}

	void init_vertex()
	{
		_shape_vertices = new VertexData[vertex_counts];

		indices = new GLushort[((vertex_counts / 2) * 3) * 2];
	}

	void init_indices()
	{
		int j = 0;
		for (int i = 0; i < ((vertex_counts / 2) * 3); i = i + 3)
		{
			indices[i] = j;

			indices[i + 1] = j + 1;
			if ((j + 1) == (vertex_counts / 2))
				indices[i + 1] = 0;

			indices[i + 2] = j + (vertex_counts / 2);

			j++;


		}
		for (int i = ((vertex_counts / 2) * 3); i < (2 * ((vertex_counts / 2) * 3)); i = i + 3)
		{
			indices[i] = j;

			indices[i + 1] = j + 1;
			if ((j + 1) == vertex_counts)
				indices[i + 1] = vertex_counts / 2;

			indices[i + 2] = j - (vertex_counts / 2) + 1;
			if (j - (vertex_counts / 2) + 1 == (vertex_counts / 2))
				indices[i + 2] = 0;

			j++;

		}
	}


	void init_normal()
	{
		_vertex_normal = new glm::vec3[vertex_counts];
	}
	void init_color()
	{
		kd = new glm::vec3[vertex_counts];
		ka = new glm::vec3[vertex_counts];
		for (int i = 0; i < vertex_counts; i++)
		{
			kd[i] = glm::vec3(0.9f, 0.9f, 0.9f);
			ka[i] = glm::vec3(0.1f, 0.1f, 0.1f);

		}


	}


	void obj_init()
	{




	}

};

struct branch {
	glm::vec3 startPoint;
	glm::vec3 endPoint;

	bool branchGrowing;

	vector<object> branchGeometry;
	vector<struct branch*> branches;

	int steps;
	float angle;
	float z_angle;

	float getLength() {
		vec3 branchVector = endPoint - startPoint;
		float length = branchVector.length();
		return length;
	}
};





object *punk;

glm::mat4  s[4];

struct branch* mainBranch;

/*camera vertical position */
GLdouble verticalPos = 0.0;
GLfloat focalPoint = 0.0;
/*starting gaze rotation angle*/
GLdouble gazeRotation = 0.0;
GLfloat eyeRotation = 45.0;
GLfloat aspect = 1.0f;
GLfloat zoomFactor = 1.0f;

int currentTime = 0;
float lambda = .005;

static void reshape_CB(GLsizei width, GLsizei height)
{
	/* handle case for divide by 0 */
	if (height == 0)
		height = 1;

	/* compute aspect ratio of the new window */
	aspect = (GLfloat)width / (GLfloat)height;

	/* set the viewport */
	glViewport(0, 0, width, height);

}

float growthFactor;

// HeightIncrease is the amount the branch has moved up based on parent growth
void advanceTimeRecursion(struct branch* currBranch, glm::vec3 heightIncrease)
{
	if (currBranch->branchGrowing) {
		currBranch->steps++;
		glm::vec3 tempHeight;
		tempHeight.x = currBranch->endPoint.x;
		tempHeight.y = currBranch->endPoint.y;
		tempHeight.z = currBranch->endPoint.z;

		currBranch->endPoint.x += (sin(currBranch->angle))*(growthFactor / currBranch->steps) + heightIncrease.x;
		currBranch->endPoint.y += (cos(currBranch->angle))*(growthFactor / currBranch->steps) + heightIncrease.y;
		currBranch->endPoint.z += (sin(currBranch->z_angle))*(growthFactor / currBranch->steps) + heightIncrease.z;

		currBranch->startPoint.x += heightIncrease.x;
		currBranch->startPoint.y += heightIncrease.y;
		currBranch->startPoint.z += heightIncrease.z;

		// the "10" can be randomized or a set input value
		heightIncrease.x += ((currBranch->endPoint.x - heightIncrease.x) - tempHeight.x) / 10;
		heightIncrease.y += ((currBranch->endPoint.y - heightIncrease.y) - tempHeight.y) / 10;
		heightIncrease.z += ((currBranch->endPoint.z - heightIncrease.z) - tempHeight.z) / 10;

		vector<object>::iterator objIter;
		int j = 0;
		for (objIter = currBranch->branchGeometry.begin(); objIter != currBranch->branchGeometry.end(); objIter++) {

			for (int i = 0; i < ((*objIter).vertex_counts / 2); i++)
			{
				//punk[j]._shape_vertices[i].position = glm::translate(mat4(), vec3(0.0f, 0.0f, -10.0f)) * punk[j]._shape_vertices[i].position;

				(*objIter)._shape_vertices[i].position = s[j] * (*objIter)._shape_vertices[i].position;
				//punk[j]._shape_vertices[i].position = glm::translate(mat4(), vec3(0.0f, 0.0f, 10.0f)) * punk[j]._shape_vertices[i].position;

			}
			for (int i = (*objIter).vertex_counts / 2; i < ((*objIter).vertex_counts); i++)
			{
				//punk[j]._shape_vertices[i].position = glm::translate(mat4(), vec3(0.0f, 0.0f, -10.0f)) * punk[j]._shape_vertices[i].position;
				(*objIter)._shape_vertices[i].position = s[j + 1] * (*objIter)._shape_vertices[i].position;
				//punk[j]._shape_vertices[i].position = glm::translate(mat4(), vec3(0.0f, 0.0f, 10.0f)) * punk[j]._shape_vertices[i].position;
			}

			j++;
		}

	}

	for (int i = 0; i < currBranch->branches.size(); i++)
	{
		advanceTimeRecursion(currBranch->branches[i], glm::vec3(0.0f));
	}

	bool canHaveChild = growthFactor > .05;
	int childMod = glm::max(5, 11 - (currBranch->steps));

	if (currBranch->steps % childMod == 2 && canHaveChild)
	{
		// Can have up to 3 children. A "0" is treated as one child
		// 50% one child is generated ( 0, 1, 2 )
		// 33% two children ( 3, 4 )
		// 16.5% three children ( 5 )
		int numberOfChildren = rand() % 6;
		if (numberOfChildren < 3)
			numberOfChildren = 1;
		else if (numberOfChildren < 5)
			numberOfChildren = 2;
		else if (numberOfChildren < 6)
			numberOfChildren = 3;

		for (int j = 0; j < numberOfChildren; j++)
		{
			float branchFactor = ((double)rand() / (RAND_MAX));
			float branchZFactor = ((double)rand() / (RAND_MAX));

			int direction = rand() % 2;
			int zdirection = rand() % 2;
			//branchFactor = branchFactor / 1.5;
			if (direction) branchFactor = -branchFactor;
			if (zdirection) branchZFactor = -branchZFactor;
			branch* left = new branch;
			left->angle = currBranch->angle + branchFactor;

			float maxAngle = ((double)rand() / (RAND_MAX)* 2);
			if (left->angle > maxAngle)//0.950f)
				left->angle = maxAngle;//0.950f;
			if (left->angle < -maxAngle)//-0.950f)
				left->angle = -maxAngle;//-0.950f;
			left->z_angle = currBranch->z_angle + branchZFactor;
			left->startPoint = currBranch->endPoint;
			left->endPoint = currBranch->endPoint;
			left->steps = 0;
			left->branchGrowing = true;

			float height1 = 0.0;
			float height2 = 0.2;

			object_count = 3;

			for (int c = 0; c < object_count; c++)
			{
				object currentObject;
				currentObject.vertex_counts = 100;

				double rad = ((360.0 / (currentObject.vertex_counts / 2)) * 3.14) / 180.0;
				double radius = 0.5f;
				currentObject.init_vertex();
				currentObject.init_indices();
				for (int i = 0; i < (currentObject.vertex_counts / 2); i++)
				{
					currentObject._shape_vertices[i].position.x = (GLfloat)(radius * cosf(i * rad));
					currentObject._shape_vertices[i].position.z = (GLfloat)(radius * sinf(i * rad));
					currentObject._shape_vertices[i].position.y = height1;

					currentObject._shape_vertices[i + (currentObject.vertex_counts / 2)].position.x = (GLfloat)(radius * cosf(i * rad));
					currentObject._shape_vertices[i + (currentObject.vertex_counts / 2)].position.z = (GLfloat)(radius * sinf(i * rad));
					currentObject._shape_vertices[i + (currentObject.vertex_counts / 2)].position.y = height2;
				}

				currBranch->branchGeometry.push_back(currentObject);

				height1 = height2;
				height2 = height2 + 0.2;


			}
			currBranch->branches.push_back(left);
		}

	}
	if (currBranch->steps > 50 || currBranch->branches.size() > 20)
	{
		currBranch->branchGrowing = false;
	}

}

void advanceTime() {
	growthFactor = growthFactor*exp(-currentTime*lambda);
	advanceTimeRecursion(mainBranch, glm::vec3(0.0f));
	currentTime++;
}


void displayBranchRecursion(struct branch* currBranch)
{

	vector<object>::iterator objIter;
	glPushMatrix();
	glTranslatef(currBranch->startPoint.x, currBranch->startPoint.y, currBranch->startPoint.z);
	glRotatef(currBranch->angle, 1.0f, 0.0f, 0.0f);
	glRotatef(currBranch->z_angle, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);
	for (objIter = currBranch->branchGeometry.begin(); objIter != currBranch->branchGeometry.end(); objIter++) {
		int k = 0; ////tracking _shape_vertice indexes to draw triangle
		for (int i = 0; i < (((*objIter).vertex_counts / 2) * 3); i = i + 3)   ////incremeting i by 3.  Draw first 150 vertexes out of 300.
		{

			glVertex3f((*objIter)._shape_vertices[k].position.x, (*objIter)._shape_vertices[k].position.y, (*objIter)._shape_vertices[k].position.z);
			if ((k + 1) == ((*objIter).vertex_counts / 2)) {
				glVertex3f((*objIter)._shape_vertices[0].position.x, (*objIter)._shape_vertices[0].position.y, (*objIter)._shape_vertices[0].position.z);
			}
			else {
				glVertex3f((*objIter)._shape_vertices[k + 1].position.x, (*objIter)._shape_vertices[k + 1].position.y, (*objIter)._shape_vertices[k + 1].position.z);
			}

			glVertex3f((*objIter)._shape_vertices[k + ((*objIter).vertex_counts / 2)].position.x, (*objIter)._shape_vertices[k + ((*objIter).vertex_counts / 2)].position.y, (*objIter)._shape_vertices[k + ((*objIter).vertex_counts / 2)].position.z);
			k++;

		}

		for (int i = (((*objIter).vertex_counts / 2) * 3); i < (((*objIter).vertex_counts) * 3); i = i + 3) {  ////incremeting i by 3. drawing next set of 150 vertices. 
			//for (int i = 0; i < 15; i = i + 3) {  ////incremeting i by 3. drawing next set of 150 vertices. 
				{

					glVertex3f((*objIter)._shape_vertices[k].position.x, (*objIter)._shape_vertices[k].position.y, (*objIter)._shape_vertices[k].position.z);
					if ((k + 1) == ((*objIter).vertex_counts)) {
						glVertex3f((*objIter)._shape_vertices[(*objIter).vertex_counts / 2].position.x, (*objIter)._shape_vertices[(*objIter).vertex_counts / 2].position.y, (*objIter)._shape_vertices[(*objIter).vertex_counts / 2].position.z);

					}
					else {
						glVertex3f((*objIter)._shape_vertices[k + 1].position.x, (*objIter)._shape_vertices[k + 1].position.y, (*objIter)._shape_vertices[k + 1].position.z);

					}
					if ((k - ((*objIter).vertex_counts / 2) + 1) == ((*objIter).vertex_counts / 2))
					{
						glVertex3f((*objIter)._shape_vertices[0].position.x, (*objIter)._shape_vertices[0].position.y, (*objIter)._shape_vertices[0].position.z);

					}
					else
					{
						glVertex3f((*objIter)._shape_vertices[k + 1 - ((*objIter).vertex_counts / 2)].position.x, (*objIter)._shape_vertices[k + 1 - ((*objIter).vertex_counts / 2)].position.y, (*objIter)._shape_vertices[k + 1 - ((*objIter).vertex_counts / 2)].position.z);
					}

					k++;

				}
		}
	}
	glEnd();
	glPopMatrix();

	for (int i = 0; i < currBranch->branches.size(); i++)
	{
		displayBranchRecursion(currBranch->branches[i]);
	}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(15.0f*zoomFactor, aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(sin(gazeRotation)*verticalPos + 10.0, cos(gazeRotation)*verticalPos + 3, 10.0,
		0.0, 0.0, 0.0,
		sin(gazeRotation), cos(gazeRotation), 0.0);

	glTranslatef(0.0f, 0.0f, 0.0f);

	glRotatef(eyeRotation, 0.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	
	displayBranchRecursion(mainBranch);
	glEnd();
	glFlush();

	glutSwapBuffers();

}



void initialize()
{
	glMatrixMode(GL_PROJECTION);												// select projection matrix
	glViewport(0, 0, 640, 640);									// set the viewport											
	glLoadIdentity();															// reset projection matrix
	gluPerspective(45.0f, 1.0f, 0.1f, 10.0f);		// set up a perspective projection matrix
	glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);														// specify the clear value for the depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);						// specify implementation-specific hints
	glClearColor(0.0, 0.0, 0.0, 1.0);											// specify clear values for the color buffers			
	glLineWidth(2);
}


void key(unsigned char key, int x_cord, int y_cord) {

	if (key == 'a' || key == 'A') {
		advanceTime();
	}
	//move up view vector
	if (key == 'c' || key == 'C') {

		verticalPos -= .01;

	}
	if (key == 'G' || key == 'g') {



	}

	if (key == 'v' || key == 'V') {

		verticalPos += .01;

	}

	if (key == 'd' || key == 'D') {

	}


	if (key == 'Q' || key == 'q' || key == 27) {
		exit(0); //hard exit
	}

	if (key == 'R' || key == 'r') {
		//reset initial values
		verticalPos = 0;
		eyeRotation = 0;
		gazeRotation = 0;
		zoomFactor = 1.0;

		branch* trunk = new branch;
		growthFactor = .3;
		trunk->branchGrowing = true;
		trunk->startPoint.x = 0.0f;
		trunk->startPoint.y = 0.0f;
		trunk->startPoint.z = 0.0f;
		trunk->angle = 0;
		trunk->z_angle = 0;
		trunk->steps = 0;
		mainBranch = trunk;

		currentTime = 0;
		float height1 = 0.0;
		float height2 = 1.0;

		for (int c = 0; c < object_count; c++)
		{
			punk[c].vertex_counts = 100;

			double rad = ((360.0 / (punk[c].vertex_counts / 2)) * M_PI) / 180.0;
			double radius = 1.0f;
			punk[c].init_vertex();
			punk[c].init_indices();
			for (int i = 0; i < (punk[c].vertex_counts / 2); i++)
			{
				punk[c]._shape_vertices[i].position.x = (GLfloat)(radius * cosf(i * rad));
				punk[c]._shape_vertices[i].position.z = (GLfloat)(radius * sinf(i * rad));
				punk[c]._shape_vertices[i].position.y = height1;

				/*punk[c]._shape_vertices[i + (punk[c].vertex_counts / 2)].position.x = (GLfloat)(radius * cosf(i * rad));
				punk[c]._shape_vertices[i + (punk[c].vertex_counts / 2)].position.z = (GLfloat)(radius * sinf(i * rad));
				punk[c]._shape_vertices[i + (punk[c].vertex_counts / 2)].position.y = height2;*/
			}

			//for (int i = 0; i < punk[c].vertex_counts; i++)
			//	punk[c]._shape_vertices[i].position = glm::translate(mat4(), vec3(0.0f, 0.0f, 10.0f)) * punk[c]._shape_vertices[i].position;

			height1 = height2;
			height2 = height2 + 1.0;


		}
	}

	//solid fill
	if (key == 's' || key == 'S') {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//wireframe
	if (key == 'w' || key == 'W') {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}


	if (key == 'x') {
		//do counterclockwise rotation about gaze vector
		gazeRotation += 0.0174532925; //+ 1 degree in radians

	}
	if (key == 'z') {
		//do counterclockwise rotation about gaze vector
		gazeRotation -= 0.0174532925;

	}



	glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {

	if (key == GLUT_KEY_LEFT) {
		eyeRotation += 5;
	}

	if (key == GLUT_KEY_RIGHT) {
		eyeRotation -= 5;
	}

	if (key == GLUT_KEY_UP) {
		zoomFactor -= 0.1;
	}

	if (key == GLUT_KEY_DOWN) {
		zoomFactor += 0.1;
	}

	glutPostRedisplay();
}


int main(int argc, char **argv)
{

	s[0] = glm::scale(mat4(), vec3(1.1f, 0.0f, 1.1f));
	s[1] = glm::scale(mat4(), vec3(1.06f, 1.05f, 1.06f));
	s[2] = glm::scale(mat4(), vec3(1.02f, 1.1f, 1.02f));
	s[3] = glm::scale(mat4(), vec3(1.001f, 1.15f, 1.001f));

	srand(time(NULL));
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);  // Display Mode
	glutInitWindowSize(640, 640);					// set window size
	glutCreateWindow("Viewer");								// create Window
	/* handle window re-size event */
	glutReshapeFunc(reshape_CB);
	branch* trunk = new branch;
	growthFactor = .3;
	trunk->branchGrowing = true;
	trunk->startPoint.x = 0.0f;
	trunk->startPoint.y = 0.0f;
	trunk->startPoint.z = 0.0f;
	trunk->angle = 0;
	trunk->z_angle = 0;
	trunk->steps = 0;

	object_count = 3;

	float height1 = 0.0;
	float height2 = 0.2;

	for (int c = 0; c < object_count; c++)
	{
		object currentObject;
		currentObject.vertex_counts = 100;

		double rad = ((360.0 / (currentObject.vertex_counts / 2)) * 3.14) / 180.0;
		double radius = 0.5f;
		currentObject.init_vertex();
		currentObject.init_indices();
		for (int i = 0; i < (currentObject.vertex_counts / 2); i++)
		{
			currentObject._shape_vertices[i].position.x = (GLfloat)(radius * cosf(i * rad));
			currentObject._shape_vertices[i].position.z = (GLfloat)(radius * sinf(i * rad));
			currentObject._shape_vertices[i].position.y = height1;

			currentObject._shape_vertices[i + (currentObject.vertex_counts / 2)].position.x = (GLfloat)(radius * cosf(i * rad));
			currentObject._shape_vertices[i + (currentObject.vertex_counts / 2)].position.z = (GLfloat)(radius * sinf(i * rad));
			currentObject._shape_vertices[i + (currentObject.vertex_counts / 2)].position.y = height2;
		}

		trunk->branchGeometry.push_back(currentObject);

		height1 = height2;
		height2 = height2 + 0.2;


	}

	mainBranch = trunk;

	glutDisplayFunc(display);									// register Display Function
	//glutIdleFunc(display);									// register Idle Function
	glutKeyboardFunc(key);
	glutSpecialFunc(specialKeys);								// register Keyboard Handler
	initialize();
	glutMainLoop();												// run GLUT mainloop
	return 0;
}


