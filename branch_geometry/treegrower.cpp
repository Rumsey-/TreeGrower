
#include "glm/glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#define FREEGLUT_LIB_PRAGMAS 0
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

//int object_count = 0;

struct branch {
	glm::vec3 startPoint;
	glm::vec3 endPoint;

	bool branchGrowing;

	vector<struct branch*> branches;

	int steps;
	float angle;
	float z_angle;
};


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

		indices = new GLushort[((vertex_counts / 2) * 3) * 2];
	}

	void init_indices()
	{
		int j = 0;
		for (int i = 0; i < ((vertex_counts / 2) * 3); i = i + 3)
		{
			indices[i] = j;
			//cout <<" \nindex "<<i <<"vertex " <<j;
			indices[i + 1] = j + 1;
			if ((j + 1) == (vertex_counts / 2))
				indices[i + 1] = 0;
			//cout <<" \nindex "<<i+1 <<"vertex " <<j+1;
			indices[i + 2] = j + (vertex_counts / 2);

			//cout <<" \nindex "<<i+2 <<"vertex " <<j +(vertex_counts/2) ;
			j++;


		}
		for (int i = ((vertex_counts / 2) * 3); i < (2 * ((vertex_counts / 2) * 3)); i = i + 3)
		{
			indices[i] = j;
			//cout <<" \nindex "<<i <<"vertex " <<j;
			indices[i + 1] = j + 1;
			if ((j + 1) == vertex_counts)
				indices[i + 1] = vertex_counts / 2;
			//cout <<" \nindex "<<i+1 <<"vertex " <<j+1;
			indices[i + 2] = j - (vertex_counts / 2) + 1;
			if (j - (vertex_counts / 2) + 1 == (vertex_counts / 2))
				indices[i + 2] = 0;
			//cout <<" \nindex "<<i+2 <<"vertex " <<j - (vertex_counts/2) + 1;
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


	~object()
	{
		delete[] _shape_vertices;
		//delete [] _vertex_normal;
		delete[] indices;
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
	object *punk;
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
		punk = NULL;
	}
	~Branch_class()
	{
		delete []object_vertices_id;
		delete []object_buffer_id;
		delete []object_buffer_indices_id;
		delete []track_object_vertices_count;
		delete []punk;


	}

	//void create_branch(vec3 initial_p_b, vec3 rotate_b, int object_count, vector<vec3> &resize_b )
	void create_branch( )
	{
		//intial_position = initial_p_b;
		//rotation = rotate_b;
		//total_cyl = object_count;
		total_cyl = 3;
		//resize = resize_b;
		//model2world = glm::translate(mat4(),vec3(intial_position));
		//world2model = glm::translate(mat4(),vec3(-1.0f * intial_position.x, -1.0f * intial_position.y, -1.0f * intial_position.z));
		punk = new object[total_cyl];
		object_buffer_id = new GLuint[total_cyl];
		object_buffer_indices_id = new GLuint[total_cyl];
		object_vertices_id = new GLuint[total_cyl];
		track_object_vertices_count = new int[total_cyl];
		//float height1 = 0.0;
		//float height2 = 0.2;
		for (int c = 0; c < total_cyl; c++) 
				{
					punk[c].object_id = c + 1;
					punk[c].vertex_counts = 100;
					track_object_vertices_count[c] = 100;
					//double rad = ((360.0/(punk[c].vertex_counts/2)) * M_PI)/ 180.0;
					//double radius = 1.0f;
					punk[c].init_vertex();
					punk[c].init_indices();
					/*
					for (int i = 0; i < (punk[c].vertex_counts/2); i++)
						{
							punk[c]._shape_vertices[i].position.x = (GLfloat) (radius * cosf(i * rad));
							punk[c]._shape_vertices[i].position.z = (GLfloat) (radius * sinf(i * rad));
							punk[c]._shape_vertices[i].position.y = height1;
		
							punk[c]._shape_vertices[i + (punk[c].vertex_counts/2)].position.x = (GLfloat) (radius * cosf(i * rad));
							punk[c]._shape_vertices[i + (punk[c].vertex_counts/2)].position.z = (GLfloat) (radius * sinf(i * rad));
							punk[c]._shape_vertices[i + (punk[c].vertex_counts/2)].position.y = height2;
						}

						*/
					/*for (int i = 0; i < (punk[c].vertex_counts); i++)
						{
							cout <<"\n x "<<punk[c]._shape_vertices[i] .position.x<<
								" y "<<punk[c]._shape_vertices[i] .position.y<<
								" z "<<punk[c]._shape_vertices[i] .position.z;
		
						}*/
				/*	for (int i = 0; i < punk[c].vertex_counts;i++) 
							punk[c]._shape_vertices[i].position =   model2world * punk[c]._shape_vertices[i].position;

					height1 = height2;
					height2 = height2 + 0.2;
					*/

				}
		

	}

	void redraw( vec3 top, vec3 base)
	{
		mat4 translate_to_origin_base = glm::translate(mat4(),vec3(-1.0 * base.x , -1.0 * base.y, -1.0 * base.z));
		vec3 new_base = vec3(translate_to_origin_base * vec4(base,1.0));
		vec3 new_top = vec3(translate_to_origin_base * vec4(top,1.0));
		cout <<new_base.x<<" "<<new_base.y<<" "<<new_base.z<<endl;
		cout <<new_top.x<<" "<<new_top.y<<" "<<new_top.z<<endl;
		cout<<"vector length" <<sqrt(pow((new_top.x - new_base.x),2.0) + pow((new_top.y - new_base.y),2.0) +pow((new_top.z - new_base.z),2.0) ) << endl;



		float angle_rot_y;
		float angle_rot_z;
		float vector_length = sqrt(pow((new_top.x - new_base.x),2.0) + pow((new_top.y - new_base.y),2.0) +pow((new_top.z - new_base.z),2.0) );


		
		
		if (((new_top.z -new_base.z) > 0) &&  ((new_top.x -new_base.x) > 0)) //quad 1
		{
			angle_rot_y =  atan((new_top.z - new_base.z)/(new_top.x -new_base.x));//
		}
		else if (((new_top.z -new_base.z) < 0) &&  ((new_top.x -new_base.x) < 0))//quad 3
		{
			angle_rot_y =  atan((new_top.z - new_base.z)/(new_top.x -new_base.x)) + M_PI;//
		}
		else if (((new_top.z -new_base.z) < 0) &&  ((new_top.x -new_base.x) > 0))//quad 4
		{
			angle_rot_y =  atan((new_top.z - new_base.z)/(new_top.x -new_base.x));//
		}
		else if (((new_top.z -new_base.z) > 0) &&  ((new_top.x -new_base.x) < 0))//quad 2
		{
			angle_rot_y =  atan((new_top.z - new_base.z)/(new_top.x -new_base.x)) + M_PI  ;//////// 2nd quad
		}
		else if ( ((new_top.z -new_base.z) > 0) && ((new_top.x -new_base.x) == 0) )   //
		{
			angle_rot_y =  M_PI/2.0;//
		}
		else if ( ((new_top.z -new_base.z) < 0) && ((new_top.x -new_base.x) == 0))   ///x/y
		{
			angle_rot_y = -1.0 * ( M_PI/2.0);
		}
		else if (((new_top.z -new_base.z) == 0) && ((new_top.x -new_base.x) < 0)) ///180
		{
			angle_rot_y = M_PI;
		}
		else
			angle_rot_y = 0;
		cout<<"rot" << angle_rot_y << " deg "<< angle_rot_y * 180 / M_PI <<endl;
		new_base = vec3(glm::rotate(mat4(), ( angle_rot_y) , glm::vec3(0.0f,1.0f,0.0f)) * vec4(new_base,1.0));
		new_top = vec3(glm::rotate(mat4(), ( angle_rot_y) , glm::vec3(0.0f,1.0f,0.0f)) * vec4(new_top,1.0));



		cout <<new_base.x<<" "<<new_base.y<<" "<<new_base.z<<endl;
		cout <<new_top.x<<" "<<new_top.y<<" "<<new_top.z<<endl;
		cout<<"vector length" <<sqrt(pow((new_top.x - new_base.x),2.0) + pow((new_top.y - new_base.y),2.0) +pow((new_top.z - new_base.z),2.0) ) << endl;



		if (((new_top.x -new_base.x) > 0) &&  ((new_top.y -new_base.y) > 0)) //quad 1
		{
			angle_rot_z =  atan((new_top.x - new_base.x)/(new_top.y -new_base.y));
		}
		/*else if (((new_top.x -new_base.x) < 0) &&  ((new_top.y -new_base.y) < 0)) //quad 3
		{
			angle_rot_z = -1.0 * atan((new_top.x - new_base.x)/(new_top.y -new_base.y))  + M_PI;
		}
		
		else if (((new_top.x -new_base.x) < 0) &&  ((new_top.y -new_base.y) > 0)) //
		{
			angle_rot_z = -1.0 * atan((new_top.x - new_base.x)/(new_top.y -new_base.y));
		}*/
		else if (((new_top.x -new_base.x) > 0) &&  ((new_top.y -new_base.y) < 0))
		{
			angle_rot_z =  atan((new_top.x - new_base.x)/(new_top.y -new_base.y))  + M_PI;
		}
		else if (((new_top.y -new_base.y) == 0) && ((new_top.x -new_base.x) > 0))   /////x/y
		{
			angle_rot_z =  M_PI/2.0;
		}
		/*else if (((new_top.y -new_base.y) == 0) && ((new_top.x -new_base.x) < 0))   ///x/y
		{
			angle_rot_z = (-1.0 * M_PI/2.0) + M_PI;
		}*/
		else if (((new_top.x -new_base.x) == 0) && ((new_top.y -new_base.y) < 0)) ///180
		{
			angle_rot_z = M_PI;
		}
		else
			angle_rot_z = 0;


		cout<<"rot" << angle_rot_z << " deg "<< angle_rot_z * 180 / M_PI <<endl;
		new_base = vec3(glm::rotate(mat4(), ( angle_rot_z) , glm::vec3(0.0f,0.0f,1.0f)) * vec4(new_base,1.0));
		new_top = vec3(glm::rotate(mat4(), ( angle_rot_z) , glm::vec3(0.0f,0.0f,1.0f)) * vec4(new_top,1.0));



		cout <<new_base.x<<" "<<new_base.y<<" "<<new_base.z<<endl;
		cout <<new_top.x<<" "<<new_top.y<<" "<<new_top.z<<endl;
		cout<<"vector length" <<sqrt(pow((new_top.x - new_base.x),2.0) + pow((new_top.y - new_base.y),2.0) +pow((new_top.z - new_base.z),2.0) ) << endl;




		/*






		
		
		float ang_xy; ///to rotate arround z axis
		float ang_zx;
		if (((top.x -base.x) > 0) &&  ((top.y -base.y) > 0))
		{
			ang_xy = -1.0 * atan((top.x - base.x)/(top.y -base.y));
		}
		else if (((top.x -base.x) < 0) &&  ((top.y -base.y) < 0))
		{
			ang_xy = -1.0 * atan((top.x - base.x)/(top.y -base.y))  + M_PI;
		}
		else if (((top.x -base.x) < 0) &&  ((top.y -base.y) > 0))
		{
			ang_xy = -1.0 * atan((top.x - base.x)/(top.y -base.y));
		}
		else if (((top.x -base.x) > 0) &&  ((top.y -base.y) < 0))
		{
			ang_xy = -1.0 * atan((top.x - base.x)/(top.y -base.y))  + M_PI;
		}
		else if (((top.y -base.y) == 0) && ((top.x -base.x) > 0))   /////x/y
		{
			ang_xy = -1.0 * M_PI/2.0;
		}
		else if (((top.y -base.y) == 0) && ((top.x -base.x) < 0))   ///x/y
		{
			ang_xy = (-1.0 * M_PI/2.0) + M_PI;
		}
		else if (((top.x -base.x) == 0) && ((top.y -base.y) < 0)) ///180
		{
			ang_xy = M_PI;
		}
		else
			ang_xy = 0;


		
		if (((top.z -base.z) > 0) &&  ((top.x -base.x) > 0)) //
		{
			ang_zx =  atan((top.z - base.z)/(top.x -base.x));//
		}
		else if (((top.z -base.z) < 0) &&  ((top.x -base.x) < 0))//
		{
			ang_zx =  atan((top.z - base.z)/(top.x -base.x)) + M_PI;//
		}
		else if (((top.z -base.z) < 0) &&  ((top.x -base.x) > 0))//
		{
			ang_zx =  atan((top.z - base.z)/(top.x -base.x));//
		}
		else if (((top.z -base.z) > 0) &&  ((top.x -base.x) < 0))//
		{
			ang_zx =  atan((top.z - base.z)/(top.x -base.x));  + M_PI;//
		}
		else if ( ((top.z -base.z) > 0) && ((top.x -base.x) == 0) )   /////x/y//
		{
			ang_zx =  M_PI/2.0;//
		}
		else if ( ((top.z -base.z) < 0) && ((top.x -base.x) == 0))   ///x/y
		{
			ang_zx = ( M_PI/2.0) + M_PI;
		}
		else if (((top.z -base.z) == 0) && ((top.x -base.x) < 0)) ///180
		{
			ang_zx = M_PI;
		}
		else
			ang_zx = 0;

			*/

		//if((top.x -base.x) != 0.0)
		//{
		//	ang_xy = -1.0 * atan((top.x - base.x)/(top.y -base.y));
		//}

		//ang_zx = atan((top.z - base.z)/(top.x -base.x));
		
		float height1 = 0.0;
		float height2 = vector_length /8.0;
		float radius1 = vector_length  /10.0;
		float radius2 = vector_length  / 14.0;
		
		for (int c = 0; c < total_cyl; c++)
		{
			double rad = ((360.0/(punk[c].vertex_counts/2)) * M_PI)/ 180.0;

			for (int i = 0; i < (punk[c].vertex_counts/2); i++)
				{
			
					punk[c]._shape_vertices[i].position.x = (GLfloat) (radius1 * cosf(i * rad));
					punk[c]._shape_vertices[i].position.z = (GLfloat) (radius1 * sinf(i * rad));
					punk[c]._shape_vertices[i].position.y = height1;
		
					punk[c]._shape_vertices[i + (punk[c].vertex_counts/2)].position.x = (GLfloat) (radius2 * cosf(i * rad));
					punk[c]._shape_vertices[i + (punk[c].vertex_counts/2)].position.z = (GLfloat) (radius2 * sinf(i * rad));
					punk[c]._shape_vertices[i + (punk[c].vertex_counts/2)].position.y = height2;
					



				}
				if (c == 0)
					{
						height1 = height2;
						radius1 = radius2;
						height2 = (3.0 * vector_length )/8.0 + height2;
						radius2 = vector_length  /20.0;

					} else if (c == 1)
					{
						height1 = height2;
						radius1 = radius2;
						height2 = (vector_length )/2.0 + height2;
						radius2 = 0.0001;
					}
			

			

			for (int i = 0; i < punk[c].vertex_counts;i++) 
			{
				//punk[c]._shape_vertices[i].position =   glm::rotate(mat4(), x , glm::vec3(1.0f,0.0f,0.0f)) * obj[count]._shape_vertices[i].position; 

				
				punk[c]._shape_vertices[i].position = glm::rotate(mat4(), ( (float )(-1.0)  * angle_rot_z) , glm::vec3(0.0f,0.0f,1.0f)) * punk[c]._shape_vertices[i].position;
				
				punk[c]._shape_vertices[i].position = glm::rotate(mat4(), ((float )(-1.0)  * angle_rot_y) , glm::vec3(0.0f,1.0f,0.0f)) * punk[c]._shape_vertices[i].position;
				
			}


		}
		new_base = vec3(glm::rotate(mat4(), ((float )(-1.0) *  angle_rot_z) , glm::vec3(0.0f,0.0f,1.0f)) * vec4(new_base,1.0));
			new_top = vec3(glm::rotate(mat4(), ( (float )(-1.0) * angle_rot_z) , glm::vec3(0.0f,0.0f,1.0f)) * vec4(new_top,1.0));
			new_base = vec3(glm::rotate(mat4(), ((float )(-1.0) * angle_rot_y) , glm::vec3(0.0f,1.0f,0.0f)) * vec4(new_base,1.0));
			new_top = vec3(glm::rotate(mat4(), ((float )(-1.0) * angle_rot_y) , glm::vec3(0.0f,1.0f,0.0f)) * vec4(new_top,1.0));
			cout <<new_base.x<<" "<<new_base.y<<" "<<new_base.z<<endl;
			cout <<new_top.x<<" "<<new_top.y<<" "<<new_top.z<<endl;
			cout<<"vector length" <<sqrt(pow((new_top.x - new_base.x),2.0) + pow((new_top.y - new_base.y),2.0) +pow((new_top.z - new_base.z),2.0) ) << endl;
		

	}

	void growing_rate(vector<vec3> &resize_b)
	{
		for (int j = 0; j < total_cyl; j++)
				{
					for (int i = 0; i < (punk[j].vertex_counts/2);i++) 
					{
						punk[j]._shape_vertices[i].position =   world2model * punk[j]._shape_vertices[i].position;
					
						punk[j]._shape_vertices[i].position =   glm::scale(mat4(),resize_b[j]) * punk[j]._shape_vertices[i].position;
						punk[j]._shape_vertices[i].position =   model2world * punk[j]._shape_vertices[i].position;
						
					}
					for (int i = punk[j].vertex_counts/2; i < (punk[j].vertex_counts);i++) 
					{
						punk[j]._shape_vertices[i].position =   world2model * punk[j]._shape_vertices[i].position;
						punk[j]._shape_vertices[i].position =  glm::scale(mat4(),resize_b[j+1]) * punk[j]._shape_vertices[i].position;
						punk[j]._shape_vertices[i].position =   model2world * punk[j]._shape_vertices[i].position;
					}

						
				}
	}




	void display_branch() 
	{
		/*for (int j = 0; j < total_cyl; j++)
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

		}*/
		glBegin(GL_TRIANGLES);
	for (int j = 0; j < total_cyl; j++) {
		
		
		int k = 0; ////tracking _shape_vertice indexes to draw triangle
		for (int i = 0; i < ((punk[j].vertex_counts / 2) * 3); i = i + 3)   ////incremeting i by 3.  Draw first 150 vertexes out of 300.
				{

					glVertex3f(punk[j]._shape_vertices[k].position.x, punk[j]._shape_vertices[k].position.y, punk[j]._shape_vertices[k].position.z);
					if ((k + 1) == (punk[j].vertex_counts / 2)) {
						glVertex3f(punk[j]._shape_vertices[0].position.x, punk[j]._shape_vertices[0].position.y, punk[j]._shape_vertices[0].position.z);
					}
					else {
						glVertex3f(punk[j]._shape_vertices[k + 1].position.x, punk[j]._shape_vertices[k + 1].position.y, punk[j]._shape_vertices[k + 1].position.z);
					}

					glVertex3f(punk[j]._shape_vertices[k + (punk[j].vertex_counts / 2)].position.x, punk[j]._shape_vertices[k + (punk[j].vertex_counts / 2)].position.y, punk[j]._shape_vertices[k + (punk[j].vertex_counts / 2)].position.z);
					k++;

				}
		
			for (int i = ((punk[j].vertex_counts / 2) * 3); i < ((punk[j].vertex_counts) * 3); i = i + 3) {  ////incremeting i by 3. drawing next set of 150 vertices. 
		    //for (int i = 0; i < 15; i = i + 3) {  ////incremeting i by 3. drawing next set of 150 vertices. 
					{
						
						//cout <<endl<<"Triangle "<<k;
						glVertex3f(punk[j]._shape_vertices[k].position.x, punk[j]._shape_vertices[k].position.y, punk[j]._shape_vertices[k].position.z);
						//cout <<std::endl<<k;
						if ((k + 1) == (punk[j].vertex_counts)) {
							glVertex3f(punk[j]._shape_vertices[punk[j].vertex_counts / 2].position.x, punk[j]._shape_vertices[punk[j].vertex_counts / 2].position.y, punk[j]._shape_vertices[punk[j].vertex_counts / 2].position.z);
							//cout<<endl<<punk[j].vertex_counts / 2;
						}
						else {
							glVertex3f(punk[j]._shape_vertices[k + 1].position.x, punk[j]._shape_vertices[k + 1].position.y, punk[j]._shape_vertices[k + 1].position.z);
							//cout<<endl<<k+1;
						}
						if ((k - (punk[j].vertex_counts / 2) + 1) == (punk[j].vertex_counts / 2))
						{
							glVertex3f(punk[j]._shape_vertices[0].position.x, punk[j]._shape_vertices[0].position.y, punk[j]._shape_vertices[0].position.z);
							//cout<<endl<<0;
						}
						else
						{
							glVertex3f(punk[j]._shape_vertices[k+1 - (punk[j].vertex_counts / 2) ].position.x, punk[j]._shape_vertices[k+1 - (punk[j].vertex_counts / 2) ].position.y, punk[j]._shape_vertices[k+1 - (punk[j].vertex_counts / 2) ].position.z);
							/*
							cout<<endl<<(k+1 - (punk[j].vertex_counts / 2))<<endl;
							cout<<punk[j]._shape_vertices[k+1 - (punk[j].vertex_counts / 2)].position.x<<endl;
							cout<<punk[j]._shape_vertices[k+1 - (punk[j].vertex_counts / 2)].position.y<<endl;
							cout<<punk[j]._shape_vertices[k+1 - (punk[j].vertex_counts / 2)].position.z<<endl;
							
							cout<<punk[j]._shape_vertices[5].position.x<<endl;
							cout<<punk[j]._shape_vertices[5].position.y<<endl;
							cout<<punk[j]._shape_vertices[5].position.z<<endl;
							cout<<punk[j]._shape_vertices[6].position.x<<endl;
							cout<<punk[j]._shape_vertices[6].position.y<<endl;
							cout<<punk[j]._shape_vertices[6].position.z<<endl;
							*/
						}

						k++;
						
					}
			}
		
	}
			
	glEnd();


	}




};



Branch_class *stem;
int stem_count =0;
//object *punk;

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
	advanceTimeRecursion(mainBranch, glm::vec3(0.0f));
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
	displayBranchRecursion(mainBranch);
	glEnd();
	glFlush();

	for (int k = 0; k < stem_count;k++)
	{
		stem[k].display_branch();
	}

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
	/*if (key == 'G' || key == 'g') {
			for (int k = 0; k < stem_count; k++)
				{
					vector<vec3> resize_b;
					resize_b.push_back(vec3(1.1f,0.0f,1.1f)) ;
					resize_b.push_back(vec3(1.06f,1.05f,1.06f)) ;
					resize_b.push_back(vec3(1.02f,1.1f,1.02f)) ;
					resize_b.push_back(vec3(1.001f,1.15f,1.001f)) ;
					stem[k].growing_rate(resize_b);
					std::vector<vec3>().swap(resize_b);
					
				}
		

	}*/

	if (key == 'v' || key == 'V') {

		verticalPos += .01;

	}

	if (key == 'd' || key == 'D') {

	}


	if (key == 'Q' || key == 'q' || key == 27) {
		delete []stem;
		exit(0); //hard exit
	}

	if (key == 'R' || key == 'r') {
		//reset initial values
		verticalPos = 0;
		eyeRotation = 45.0;
		gazeRotation = 0;
		zoomFactor = 1.0;

		branch* trunk = (struct branch*) calloc(1, sizeof(struct branch));
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
		delete []stem;
		stem_count = 1;   ////how many trunk parts

	//punk = new object[object_count];
		stem = new Branch_class[stem_count];
		vector<vec3> resize;
	


		for (int k = 0; k < stem_count; k++)
		{
			//stem[k].create_branch(vec3(0.0f,0.0f,0.0f), vec3(0.0f,0.0f,0.0f), 3,resize);
			stem[k].create_branch();
			//stem[k].redraw( vec3(1.0f,1.0f,1.0f) , vec3(-1.0f,-1.0f,-1.0f));
			vec3 base = vec3(1.0f,1.0f,1.0f);////from here (base of the line)
			vec3 top = vec3(-1.0f,-1.0f,-1.0f);   //////to here (top of the line)
    
			stem[k].redraw( top , base);
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

	stem_count = 1;   ////how many stems in the design

	//punk = new object[object_count];
	stem = new Branch_class[stem_count]; //allocate memory to all the stems
	vector<vec3> resize;


	vec3 base = vec3(1.0f,1.0f,1.0f);////from here (base of the line)
	vec3 top = vec3(-1.0f,-1.0f,-1.0f);   //////to here (top of the line)
    


	for (int k = 0; k < stem_count; k++)
	{
		stem[k].create_branch();  /////create branch geometry for all the stems..with each stem made up of 3 cylinders
		//stem[k].create_branch(vec3(0.0f,0.0f,0.0f), vec3(0.0f,0.0f,0.0f), 3,resize);  /////create branch geometry for all the stems..with each stem made up of 3 cylinders
		//stem[k].redraw( vec3(1.0f,1.0f,1.0f) , vec3(-1.0f,-1.0f,-1.0f));
		//stem[k].redraw( vec3(0.1f,-1.0f,0.0f) , vec3(-0.1f,1.0f,0.0f));
		//stem[k].redraw( vec3(-0.1f, -1.0f,0.0f) , vec3(0.1f,1.0f,0.0f));
		//stem[k].redraw( vec3(0.1f, -1.0f,0.0f) , vec3(-0.1f,-1.0f,0.0f));///90
		//stem[k].redraw( vec3(0.0f, -2.0f,0.0f) , vec3(0.0f,-1.0f,0.0f));
		//stem[k].redraw( vec3(1.0f,1.0f,-1.0f) , vec3(-1.0f,-1.0f,1.0f));
		//stem[k].redraw( vec3(1.0f,1.0f,1.0f) , vec3(-1.0f,-1.0f,-1.0f));
		//stem[k].redraw( vec3(0,0.0f,0.5f) , vec3(0.0f,0.0f,-0.5f));
		//stem[k].redraw( vec3(0.1f,1.0f,1.0f) , vec3(0.0f,0.0f,0.0f));///resolve this
		//stem[k].redraw( vec3(0,3.0f,0) , vec3(0,0,0));
		//stem[k].redraw( vec3(0.1f,1.0f,1.0f) , vec3(0.5f,0.2f,-9.0f));
		stem[k].redraw( top,base);  ////////////////Sample
	}
	

	
	srand(time(NULL));
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);  // Display Mode
	glutInitWindowSize(640, 640);					// set window size
	glutCreateWindow("Viewer");								// create Window
	/* handle window re-size event */
	glutReshapeFunc(reshape_CB);
	branch* trunk = (struct branch*) calloc(1, sizeof(struct branch));
	growthFactor = .3;
	trunk->branchGrowing = true;
	trunk->startPoint.x = 0.0f;
	trunk->startPoint.y = 0.0f;
	trunk->startPoint.z = 0.0f;
	trunk->angle = 0;
	trunk->z_angle = 0;
	trunk->steps = 0;
	mainBranch = trunk;

	glutDisplayFunc(display);									// register Display Function
	//glutIdleFunc(display);									// register Idle Function
	glutKeyboardFunc(key);
	glutSpecialFunc(specialKeys);								// register Keyboard Handler
	initialize();
	glutMainLoop();												// run GLUT mainloop
	return 0;
}


