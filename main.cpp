#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <math.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

float red = 0.0;
float green = 0.0;
float blue = 1.0;

struct branch {
	glm::vec3 startPoint;
	glm::vec3 endPoint;

	bool branchGrowing;

	vector<struct branch*> branches;

	int steps;
	float angle;
	float z_angle;
};

struct trunk {
    struct branch * branches;
    struct trunk * next;
};

struct trunk* trees;

int amountOfTrees = 9;

//struct branch* mainBranch;

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
			//cout << branchFactor;
			//cout << "\n";
			int direction = rand() % 2;
			int zdirection = rand() % 2;
			//branchFactor = branchFactor / 1.5;
			if (direction) branchFactor = -branchFactor;
			if (zdirection) branchZFactor = -branchZFactor;
			branch* left = (struct branch*) calloc(1, sizeof(struct branch));
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

    // Go through each tree and advance the branches within them
    struct trunk* walker = trees;
    while(walker != NULL)
    {
	    advanceTimeRecursion(walker->branches, glm::vec3(0.0f));
        walker = walker->next;
    }
	currentTime++;
}

void displayBranchRecursion(struct branch* currBranch)
{

	glColor3f(red, blue, green);
	glVertex3f(currBranch->startPoint.x, currBranch->startPoint.y, currBranch->startPoint.z);
	glColor3f(red, blue, green);
	glVertex3f(currBranch->endPoint.x, currBranch->endPoint.y, currBranch->endPoint.z);


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

	glBegin(GL_LINES);

    // Go through each tree and draw the branches within them
    struct trunk* walker = trees;
    while(walker != NULL)
    {
	    displayBranchRecursion(walker->branches);
        walker = walker->next;
    }

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


// Create x trees where x is the input value
// Will create a y by y grid of trees where y = sqrt of input.
struct trunk* plantSeeds(int amount)
{
    // Have the trees spaced about 1 away from each other
    float maxDist = 1.0f * amount;

    // Start in negative, add to positive 
    // Reason: origin is in middle of screen, need negative half
    float xPos = maxDist / 2 * (-1);
    float zPos = maxDist / 2 * (-1);

    // The amount we add with each tree
    float xInc = maxDist / sqrt(amount);
    float zInc = maxDist / sqrt(amount);

    // Center if we are only generating one tree
    if(amount == 1)
    {
        xPos = 0.0f;
        zPos = 0.0f;
    }

    // Creates the amount of trees requested
    for(int i = 0; i < amount; i++)
    {
        trunk* trun = (struct trunk*) calloc(1, sizeof(struct trunk));
        branch* bran = (struct branch*) calloc(1, sizeof(struct branch));
	    growthFactor = .3;
	    bran->branchGrowing = true;

        // Since the xz plane is the "ground", we set them to move the tree position
	    bran->startPoint.x = bran->endPoint.x = xPos;
	    bran->startPoint.z = bran->endPoint.z = zPos;
    
        xPos += xInc;
        if(xPos >= maxDist / 2)
        {
            xPos = maxDist / 2 * (-1);
            zPos += zInc;
        }

        // y will always be one to start the tree growing on the "ground"
	    bran->startPoint.y = bran->endPoint.y = 0.0f;
	    bran->angle = 0;
	    bran->z_angle = 0;
	    bran->steps = 0;
        trun->branches = bran;
        trun->next = trees;
	    trees = trun;
    }
}


void key(unsigned char key, int x_cord, int y_cord) {

	if (key == 'a' || key == 'A') {
		advanceTime();
	}
	//move up view vector
	if (key == 'c' || key == 'C') {

		verticalPos -= .01;

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

        trees = NULL;
		plantSeeds(amountOfTrees);

		currentTime = 0;
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
	srand(time(NULL));
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);  // Display Mode
	glutInitWindowSize(640, 640);					// set window size
	glutCreateWindow("Viewer");								// create Window
	/* handle window re-size event */
	glutReshapeFunc(reshape_CB);
	
    // Plant 25 trees
    trees = NULL;
    plantSeeds(amountOfTrees);

	glutDisplayFunc(display);									// register Display Function
	//glutIdleFunc(display);									// register Idle Function
	glutKeyboardFunc(key);
	glutSpecialFunc(specialKeys);								// register Keyboard Handler
	initialize();
	glutMainLoop();												// run GLUT mainloop
	return 0;
}
