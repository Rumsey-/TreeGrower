
#include <iostream>
#define _USE_MATH_DEFINES
#include "vgl.h"
#include "LoadShaders.h"
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <ctype.h>
#include <cstring>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>



using namespace std;
using glm::vec4;
using glm::vec3;
using glm::mat4;
int screen_width = 512;
int screen_height = 512;
float x_max = -9999.0,y_max = -9999.0 ,z_max = -9999.0;
float x_min = 9999.0 ,y_min = 9999.0 ,z_min = 9999.0;
float x_range = 0.0, y_range = 0.0, z_range = 0.0;
float get_max;
float max_range;
vec3 eye_location;
vec3 focal_point;
vec3 up_vector;


mat4 projectionMatrix ;
mat4 WorldtoCamera ;
class VertexData {
public:
		//GLubyte color[4];
		//glm::vec4 color;
		glm::vec3 normal;
		glm::vec4 position;
		//glm::vec3 normal;
		glm::vec3 Kd;
		glm::vec3 Ka;
		VertexData ()
		{
			position = glm::vec4(0.0f,0.0f,0.0f,1.0f);
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
		int object_id;
		GLushort *indices;
		GLuint object_program_shader;
		GLint dominating_color_uniform_loc;
		object()
		{
			object_id = 0;
			vertex_counts = 0;
			_shape_vertices = NULL;
		}

		void init_vertex()
		{
			_shape_vertices = new VertexData[vertex_counts];

			indices = new GLushort[((vertex_counts/2) * 3) * 2];
		}
		
		void init_indices()
		{
			int j = 0;
			for (int i = 0; i < ((vertex_counts/2) * 3); i = i +3)
			{
				indices[i] = j;
				cout <<" \nindex "<<i <<"vertex " <<j;
				indices[i+1] = j+1;
				if ((j+1) == (vertex_counts/2) )
					indices[i+1] = 0;
				cout <<" \nindex "<<i+1 <<"vertex " <<j+1;
				indices [i + 2] = j + (vertex_counts/2);
				
				cout <<" \nindex "<<i+2 <<"vertex " <<j +(vertex_counts/2) ;
				j++;
				
			
			}
			for (int i = ((vertex_counts/2) * 3) ; i < (2*((vertex_counts/2) * 3)); i = i +3)
			{
				indices[i] = j;
				cout <<" \nindex "<<i <<"vertex " <<j;
				indices[i+1] = j+1;
				if ((j+1) == vertex_counts)
					indices[i+1] = vertex_counts/2;
				cout <<" \nindex "<<i+1 <<"vertex " <<j+1;
				indices [i + 2] = j - (vertex_counts/2) + 1;
				if (j - (vertex_counts/2) + 1 == (vertex_counts/2))
					indices [i + 2] = 0;
				cout <<" \nindex "<<i+2 <<"vertex " <<j - (vertex_counts/2) + 1;
				j++;
			
			}
		}
		

		void init_normal()
		{
			_vertex_normal = new vec3[vertex_counts];
		}
		void init_color()
		{
			kd = new vec3[vertex_counts];
			ka = new vec3[vertex_counts];
			for (int i = 0; i < vertex_counts;i++)
			{
				kd[i] =  vec3(0.9f,0.9f,0.9f);
				ka[i] =  vec3(0.1f,0.1f,0.1f);

			}


		}

		
		void obj_init(GLuint object_vertices_id[], GLuint object_buffer_id[], GLuint object_buffer_indices_id[] )
		{
			
		
				ShaderInfo  shaders[] = {
					{ GL_VERTEX_SHADER, "triangle.vert" },
					{ GL_FRAGMENT_SHADER, "triangle.frag" },
					{ GL_NONE, NULL }
				};
				object_program_shader = LoadShaders( shaders ); 
				if (object_id %2 == 0 )
				dominating_color = vec4(1.0f,0.0f,0.0f,1.0f);
				else
				dominating_color = vec4(0.0f,1.0f,0.0f,1.0f);

				dominating_color_uniform_loc = glGetUniformLocation(object_program_shader, "dominating_color");///dominating_color_uniform_loc location of register
			glGenVertexArrays( 1, &object_vertices_id[object_id -1] );
			glBindVertexArray(object_vertices_id[object_id -1]);
			glGenBuffers( 1, &object_buffer_id[object_id -1]);    ////buffer object maintain   //how many buffers correspnding to logic address _axis_bufferID
			glBindBuffer( GL_ARRAY_BUFFER, object_buffer_id[object_id -1] );  ///binds _axis_bufferID to the binding point "GL_ARRA_BUFFER"
			
			
			//enable data copied in buffers to go through the graphics processor pipeline  "0" indicates position attribute
			//glBufferData(GL_ARRAY_BUFFER,(vertex_counts * sizeof(VertexData)) , _shape_vertices, GL_STATIC_DRAW ); ///send data to binding point "GL_ARRAY_BUFFER"
			glBufferData( GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW );
			
			
			glGenBuffers(1,&object_buffer_indices_id[object_id-1]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,object_buffer_indices_id[object_id-1]);
			
			
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (vertex_counts * 3*sizeof(GLushort)), indices, GL_STATIC_DRAW);
			
			
	
		

		}

		
		~object()
		{
			delete [] _shape_vertices;
			//delete [] _vertex_normal;
			delete [] indices;
			//delete [] kd;
			//delete [] ka;
		}


};


class Branch_class {
public:
	vec3 intial_position;
	int total_cyl;
	vec3 rotation;
	vector<vec3> resize;
	int vertex_count;
	glm::mat4 model2world; // construct identity matrix
	glm::mat4 world2model; // construct identity matrix
	GLuint *object_vertices_id;
	GLuint *object_buffer_id;
	GLuint *object_buffer_indices_id;
	int *track_object_vertices_count;
	object *trunk;
	//GLuint *object_program_shader ;
	
	Branch_class() {
		intial_position = vec3(0,0,0);
		total_cyl = 1;
		rotation = vec3(0,0,0);
		vertex_count = 100;
		model2world = glm::mat4(1.0f);
		world2model = glm::mat4(1.0f);
		object_vertices_id = NULL;
		object_buffer_id =  NULL;
		object_buffer_indices_id = NULL;
		track_object_vertices_count = NULL;
		trunk = NULL;
	}
	~Branch_class()
	{
		delete []object_vertices_id;
		delete []object_buffer_id;
		delete []object_buffer_indices_id;
		delete []track_object_vertices_count;
		delete []trunk;


	}

	void create_branch(vec3 initial_p_b, vec3 rotate_b, int object_count, vector<vec3> &resize_b )
	{
		intial_position = initial_p_b;
		rotation = rotate_b;
		total_cyl = object_count;
		resize = resize_b;
		model2world = glm::translate(mat4(),vec3(intial_position));
		world2model = glm::translate(mat4(),vec3(-1.0f * intial_position.x, -1.0f * intial_position.y, -1.0f * intial_position.z));
		trunk = new object[object_count];
		object_buffer_id = new GLuint[object_count];
		object_buffer_indices_id = new GLuint[object_count];
		object_vertices_id = new GLuint[object_count];
		track_object_vertices_count = new int[object_count];
		float height1 = 0.0;
		float height2 = 1.0;
		for (int c = 0; c < object_count; c++)
				{
					trunk[c].object_id = c + 1;
					trunk[c].vertex_counts = 100;
					track_object_vertices_count[c] = 100;
					double rad = ((360.0/(trunk[c].vertex_counts/2)) * M_PI)/ 180.0;
					double radius = 1.0f;
					trunk[c].init_vertex();
					trunk[c].init_indices();
					for (int i = 0; i < (trunk[c].vertex_counts/2); i++)
						{
							trunk[c]._shape_vertices[i].position.x = (GLfloat) (radius * cosf(i * rad));
							trunk[c]._shape_vertices[i].position.z = (GLfloat) (radius * sinf(i * rad));
							trunk[c]._shape_vertices[i].position.y = height1;
		
							trunk[c]._shape_vertices[i + (trunk[c].vertex_counts/2)].position.x = (GLfloat) (radius * cosf(i * rad));
							trunk[c]._shape_vertices[i + (trunk[c].vertex_counts/2)].position.z = (GLfloat) (radius * sinf(i * rad));
							trunk[c]._shape_vertices[i + (trunk[c].vertex_counts/2)].position.y = height2;
						}
					for (int i = 0; i < (trunk[c].vertex_counts); i++)
						{
							cout <<"\n x "<<trunk[c]._shape_vertices[i] .position.x<<
								" y "<<trunk[c]._shape_vertices[i] .position.y<<
								" z "<<trunk[c]._shape_vertices[i] .position.z;
		
						}
					for (int i = 0; i < trunk[c].vertex_counts;i++) 
							trunk[c]._shape_vertices[i].position =   model2world * trunk[c]._shape_vertices[i].position;

					height1 = height2;
					height2 = height2 + 1.0;


				}
		for (int j = 0; j < object_count; j++)
			{	
					cout<<"\n Initializing buffers of object "<<j;
					trunk[j].obj_init(object_vertices_id, object_buffer_id, object_buffer_indices_id );
	    	}


	}

	void growing_rate(vector<vec3> &resize_b)
	{
		for (int j = 0; j < total_cyl; j++)
				{
					for (int i = 0; i < (trunk[j].vertex_counts/2);i++) 
					{
						trunk[j]._shape_vertices[i].position =   world2model * trunk[j]._shape_vertices[i].position;
					
						trunk[j]._shape_vertices[i].position =   glm::scale(mat4(),resize_b[j]) * trunk[j]._shape_vertices[i].position;
						trunk[j]._shape_vertices[i].position =   model2world * trunk[j]._shape_vertices[i].position;
						
					}
					for (int i = trunk[j].vertex_counts/2; i < (trunk[j].vertex_counts);i++) 
					{
						trunk[j]._shape_vertices[i].position =   world2model * trunk[j]._shape_vertices[i].position;
						trunk[j]._shape_vertices[i].position =  glm::scale(mat4(),resize_b[j+1]) * trunk[j]._shape_vertices[i].position;
						trunk[j]._shape_vertices[i].position =   model2world * trunk[j]._shape_vertices[i].position;
					}

						
				}
	}




	void display_branch() 
	{
		for (int j = 0; j < total_cyl; j++)
		{
			glUseProgram(trunk[j].object_program_shader);
			GLint WorldtoCamera_loc = glGetUniformLocation(trunk[j].object_program_shader,"WorldtoCameraMatrix");
			GLint modelProjectUniform_loc = glGetUniformLocation(trunk[j].object_program_shader,"projection_matrix");
			glUniform4fv(trunk[j].dominating_color_uniform_loc,1,&trunk[j].dominating_color[0]); //1 homany groups sending down
			glUniformMatrix4fv(modelProjectUniform_loc,1,GL_FALSE, &projectionMatrix[0][0]);
			glUniformMatrix4fv(WorldtoCamera_loc,1,GL_FALSE, &WorldtoCamera[0][0]);
	
			glBindVertexArray(object_vertices_id[j]);
		

			glBindBuffer( GL_ARRAY_BUFFER, object_buffer_id[j] );
			glBufferData(GL_ARRAY_BUFFER,(trunk[j].vertex_counts * sizeof(VertexData)) , trunk[j]._shape_vertices, GL_DYNAMIC_DRAW );
			glVertexAttribPointer( 0, 4,GL_FLOAT,GL_FALSE,sizeof(VertexData),BUFFER_OFFSET(sizeof(trunk[j]._shape_vertices[0].normal))); //0 position
			glEnableVertexAttribArray(0); 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,object_buffer_indices_id[j]);
			//draw_object(j);
			glDrawElements(GL_TRIANGLES,(3 * track_object_vertices_count[j]) , GL_UNSIGNED_SHORT, NULL);
			//glutSwapBuffers();  ///without this was getting white window
			glutPostRedisplay();

		}


	}




};

Branch_class *branch;
int branch_count;





static void idle_CB (void)
{
	/* tell GLUT to redraw */
	glutPostRedisplay();
}


void display ( )
{
	glClear( GL_COLOR_BUFFER_BIT  );
	glClear(GL_DEPTH_BUFFER_BIT);  ///every time paint clear it to furthest most value i.e. to 1
	glDepthMask(GL_TRUE);
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
	

	//glViewPort(0,0, screen_width, screen_height);
	projectionMatrix = glm::perspective (2.094f /*90 in deg*/ ,((float)screen_width/(float)screen_height) , 0.1f ,100.0f);
	WorldtoCamera = glm::lookAt(eye_location,focal_point, up_vector );

	
	
	for (int k = 0; k < branch_count; k++)
	{
		branch[k].display_branch();
	}

		

	
	glutSwapBuffers();  ///without this was getting white window
	//glLoadIdentity();
	//glFlush();


}	
void reshape (GLsizei width, GLsizei height)
{
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport (0,0,width,height);
	projectionMatrix = glm::perspective (2.094f /*90 in deg*/ ,((float)width/(float)height) , 0.1f ,(9*get_max));
	screen_width = width;
	screen_height = height;
	


	glMatrixMode(GL_MODELVIEW);
	


}


void key_Board (unsigned char key, int x_cord, int y_cord)
{
	switch (key)
	{
		
		case 'w':
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			break;
		case 's':
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			break;
		case 'r':
			
				eye_location = vec3(0.0f,10.0f,0.0f);
				focal_point = vec3(0.0f,0.0f,10.0f);
				up_vector = vec3 (0.0f,1.0f,0.0f);
				
			break;
		case 'c':
			eye_location = eye_location - (0.1f * max_range * glm::normalize(up_vector));
			break;

		case 'v':
			eye_location = eye_location + (0.1f * max_range * (glm::normalize(up_vector)));
			break;
		

		case 'z':
			{
				glm::mat4 rotationmat(1);
				
				rotationmat = glm::rotate(rotationmat, -0.0174533f , glm::normalize(focal_point - eye_location));
				up_vector = glm::vec3(rotationmat * glm::vec4(up_vector , 1.0f));
				
			}
			break;
		case 'x':
			{
				glm::mat4 rotationmat(1);
				rotationmat = glm::rotate(rotationmat, 0.0174533f , glm::normalize(focal_point - eye_location));
				up_vector = glm::vec3(rotationmat * glm::vec4(up_vector , 1.0f));
				
			}
			break;
		case 'g':
			{
				
				
				
				for (int k = 0; k < branch_count; k++)
				{
					vector<vec3> resize_b;
					resize_b.push_back(vec3(1.1f,0.0f,1.1f)) ;
					resize_b.push_back(vec3(1.06f,1.05f,1.06f)) ;
					resize_b.push_back(vec3(1.02f,1.1f,1.02f)) ;
					resize_b.push_back(vec3(1.001f,1.15f,1.001f)) ;
					branch[k].growing_rate(resize_b);  ////////////////////////function to 
					std::vector<vec3>().swap(resize_b);
					
				}


				break;
			}

		
		case 27:
			delete []branch;
			exit(0);
			break;
		case 'q':
			 
			delete []branch;
			exit(0);
			break;
		default:
			break;
	}
}
void specialkeys (int key, int x_cord, int y_cord)
{
	switch (key)
	{
		
		

	
	case GLUT_KEY_UP:
			{
				vec3 gaze = focal_point - eye_location;
				float magnitude;
				magnitude = glm::length(gaze);


				eye_location = eye_location + (0.1f * magnitude * glm::normalize(focal_point - eye_location));
				
				break;
			}

	case GLUT_KEY_DOWN:
			{
				vec3 gaze = focal_point - eye_location;
				float magnitude;
				magnitude = glm::length(gaze);
				eye_location = eye_location - (0.1f * magnitude * glm::normalize(focal_point - eye_location));
				break;
			}

		
			break;
	default:
			break;
	}
}




int main(int argc, char* argv[])
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	//glutInitDisplayMode( GLUT_RGBA );
	glutInitWindowSize( 512, 512 );
	glutInitContextVersion( 4, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );// GLUT_COMPATIBILITY_PROFILE );
	glutCreateWindow( argv[0] );
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	
	glewExperimental = GL_TRUE;	// added for glew to work!
	if ( glewInit() )   //KING here some points keep track of them and draw 
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit (EXIT_FAILURE );
	}
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	

	branch_count = 1;   ////how many trunk parts
	
	
	
	branch = new Branch_class[branch_count];
	vector<vec3> resize;
	


	for (int k = 0; k < branch_count; k++)
	{
		branch[k].create_branch(vec3(0.0f,0.0f,10.0f), vec3(0.0f,0.0f,0.0f), 3,resize);
	}
	
	for (int k = 0; k < branch_count; k++)
	{

		for (int j = 0; j< branch[k].total_cyl;j++)
				{
					for (int i = 0; i<branch[k].trunk[j].vertex_counts; i++)
					{
						if (branch[k].trunk[j]._shape_vertices[i].position.x > x_max)
							x_max = branch[k].trunk[j]._shape_vertices[i].position.x;
						if (branch[k].trunk[j]._shape_vertices[i].position.y > y_max)
							y_max = branch[k].trunk[j]._shape_vertices[i].position.y;
						if (branch[k].trunk[j]._shape_vertices[i].position.z > z_max)
							z_max = branch[k].trunk[j]._shape_vertices[i].position.z;
						if (branch[k].trunk[j]._shape_vertices[i].position.x < x_min)
							x_min = branch[k].trunk[j]._shape_vertices[i].position.x;
						if (branch[k].trunk[j]._shape_vertices[i].position.y < y_min)
							y_min = branch[k].trunk[j]._shape_vertices[i].position.y;
						if (branch[k].trunk[j]._shape_vertices[i].position.z < z_min)
							z_min = branch[k].trunk[j]._shape_vertices[i].position.z;

					}
		}
	}
					
	
	x_range = x_max - x_min;
	y_range = y_max - y_min;
	z_range = z_max -z_min;
	//eye_location = vec3(3*x_max,3*y_max,z_max);
	eye_location = vec3(0.0f,10.0f,0.0f);
	//focal_point = (vec3((x_min + (x_range/2)),(y_min + (y_range/2)),(z_min + (z_range/2))));
	focal_point = vec3(0.0f,0.0f,10.0f);
	up_vector = vec3 (0.0f,1.0f,0.0f);
	
	if (x_max > y_max)
		get_max = x_max;
	else
		get_max = y_max;
	if (get_max < z_max)
		get_max = z_max;
	
	if (x_range > y_range)
		max_range = x_range;
	else
		max_range = y_range;
	if (max_range < z_range)
		max_range = z_range;
	

	
	


	//trunk.obj_init();
	
	//delete [] trunk;

	
	glutDisplayFunc( display );
	//glutReshapeFunc(reshape);
	glutIdleFunc(idle_CB);
	glutKeyboardFunc(key_Board);
	glutSpecialFunc(specialkeys);
	glutMainLoop();
	
	return 0;
	
	
}

