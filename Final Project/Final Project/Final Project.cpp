//Final Project - 3D Hierarchical Modelling
//Team members:
//1) Muhammad Luqman Bin Nazam (A20EC0095)
//2) Irfan Mustaqim Bin Abd Manap (A20EC0051)
//3) Afif Hafizin Bin Hamizan (A118CS0008)
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <fstream>
#include "imageloader.h"

using namespace std;

GLuint loadTexture(Image* image)
{
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width, image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,

		image->pixels);
	return textureId;
}

GLuint _textureId1;


//window size
int winSizeX = 650;
int winSizeY = 650;

//left button of mouse click status
int moving;
//left button of mouse click coordinate
int startx, starty;


//angle for rotate of whole body
static int angle_whole_body_scale = 1;
static int angle_whole_body_x = 0;
static int angle_whole_body_y = 0;
static int angle_whole_body_z = 0;

//torso rotation (angle)
static int torso_rotate_x = 0, torso_rotate_y = 0, torso_rotate_z = 0;

//neck parent, head child (angle)
/*Key: move neck: n, move head: h*/
static int neck = 0, head = 0;

//For shoulder, hand, foot ... ->>>/*Key: toggle left: l, toggle right: r*/
static int left_side_toggle = 0, right_side_toggle = 0;

//angle of body motion scale
static int part_motion_angle_scale = 5;
//shoulder grandparent, elbow parent, hand child (angle)
/*Key: move shoulder: q, move elbow: w, move hand: e*/
static int left_shoulder = 0, left_elbow = 0, left_hand = 0;
static int right_shoulder = 0, right_elbow = 0, right_hand = 0;

//upper leg grandparent, lower leg parent, foot child (angle)
/*Key: rotate upper_leg: a, rotate lower_leg: s, rotate foot: d*/
static int left_upper_leg = 0, left_lower_leg = 0, left_foot = 0;
static int right_upper_leg = 0, right_lower_leg = 0, right_foot = 0;

//surround object (angle)
static int surround_object_angle = 0;
static int surround_object_child_angle = 0;

//ear
static int ears_angle = 0;

//surround object translate
static GLfloat surround_object_child_translate = 0;

/*move far or near*/
GLfloat camera_distance = -10;

//camera center vector
GLfloat camera_center_x = 0;
GLfloat camera_center_y = 0;
GLfloat camera_center_scale = 0.1;

//lighting
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

//texture
GLuint texture; //the array for our texture

//state for animation
static int surround_object_child_translate_state = 0;
static int anim_head_state = 0;
static int anim_neck_state = 0;
static int anim_hand_state = 0;
static int anim_walk_state = 0;
static int anim_raise_sword_state = 0;
static int anim_bowing_state = 0;
static int anim_look_around_state = 0;
static int anim_bend_waist_state = 0;
static int anim_bend_waist_backward_state = 0;

//toggle for animation
static int anim_head_toggle = 0;
static int anim_neck_toggle = 0;
static int anim_hand_toggle = 0;
static int anim_walk_toggle = 0;
static int anim_raise_sword_toggle = 0;
static int anim_bowing_toggle = 0;
static int anim_look_around_toggle = 0;
static int anim_bend_waist_toggle = 0;
static int anim_bend_waist_backward_toggle = 0;


//toggle axis rotation variable
static int rotate_x_toggle = 0;
static int rotate_y_toggle = 0;
static int rotate_z_toggle = 0;

//toggle full screen
static int full_screen_toggle = 0;

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	Image* image1 = loadBMP("C:\\zmisc\\disney.bmp");
	_textureId1 = loadTexture(image1);
	delete image1;
}

void renderBitmap(float x, float y, void* font, char* string)
{
	char* c;

	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}


void FreeTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 1.0f, -8.0f);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	GLfloat directedLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat directedLightPos[] = { -10.0f, 15.0f, 20.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, directedLight);
	glLightfv(GL_LIGHT0, GL_POSITION, directedLightPos);

}

void display(void)
{
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//color material
	GLfloat ColorMaterialEmission[] = { 0.0, 0.0, 0.1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ColorMaterialEmission);

	glPushMatrix(); //start hierarchy
	{
		gluLookAt(0, 0, camera_distance, camera_center_x, camera_center_y, 0, 0, 1, 0);
		//set model to face front
		glRotatef(180, 0.0, 1.0, 0.0);

		glRotatef(angle_whole_body_x, 1.0, 0.0, 0.0);
		glRotatef(angle_whole_body_y, 0.0, 1.0, 0.0);
		glRotatef(angle_whole_body_z, 0.0, 0.0, 1.0);

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, -1.0f, 1.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(5.0f, -2.0f, 1.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(4.0f, -1.0f, 2.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(4.0f, -2.0f, 2.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(3.0f, -1.0f, 3.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(3.0f, -2.0f, 3.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(2.0f, -1.0f, 4.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(2.0f, -2.0f, 4.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, -1.0f, 1.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(-5.0f, -2.0f, 1.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(-4.0f, -1.0f, 2.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(-4.0f, -2.0f, 2.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(-3.0f, -1.0f, 3.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(-3.0f, -2.0f, 3.0f);
		glutSolidCube(0.5);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(-2.0f, -1.0f, 4.0f);
		glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.8, 1.0, 0.4);
		glTranslatef(-2.0f, -2.0f, 4.0f);
		glutSolidCube(0.5);
		glPopMatrix();
		

		//Background
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _textureId1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glColor3f(0.9f, 0.9f, 0.9f);
		glScaled(1.5, 1.5, 1.5);
		glTranslated(0, 1.5, 0);
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, -2.5f, -2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f, 2.5f, -2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(2.5f, 2.5f, -2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(2.5f, -2.5f, -2.5f);
		glEnd();
		glPopMatrix();

		

		//start torso, torso is the center of the body
		glPushMatrix();
		{
			glRotatef((GLfloat)torso_rotate_x, 1.0, 0.0, 0.0);
			glRotatef((GLfloat)torso_rotate_y, 0.0, 1.0, 0.0);
			glRotatef((GLfloat)torso_rotate_z, 0.0, 0.0, 1.0);

			//start chest
			glPushMatrix();
			{
				glTranslatef(0, 0.5, 0);
				glScalef(1.7, 1.7, 1.7);

				//draw chest
				glColor3f(0.6, 0.8, 0.4);
				glutSolidSphere(1, 10, 10);
			}
			glPopMatrix();  //end chest

		//start neck

			glPushMatrix();
			{
				glTranslatef(0, 1.15, 0);
				glRotatef((GLfloat)neck, 0, 0, 1);
				glScalef(0.6, 0.3, 0.6);
				//start head
				glPushMatrix();
				{
					//start horn
					glPushMatrix();
					{
						glTranslatef(0.0, 0.8, 0.2);
						glScalef(4, 4, 0.5);

						glColor3f(0.6, 0.9, 0.3);
						glutSolidCube(1);

					}
					glPopMatrix(); //end horn

					glTranslatef(0, -1.5, 2);
					glScalef(1.5, 3, 1.5);
					glRotatef((GLfloat)head, 0, 1, 0);

					glEnable(GL_LIGHT3);
					GLfloat eye_emission_light[] = { 1.0,1.0,1.0 };
					glLightfv(GL_LIGHT3, GL_EMISSION, eye_emission_light);
					GLfloat ColorMaterialEmission2[] = { 0.0, 0.0, 0.0 };
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ColorMaterialEmission2);

					//start eye
					glPushMatrix();
					{
						glTranslatef(0.0, 0.1, 0.9);
						glScalef(0.25, 0.2, 0.2);

						glColor3f(0, 0, 0);
						glutSolidSphere(1, 30, 30);
					}
					glPopMatrix(); //end right eye

					glDisable(GL_LIGHT3);
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ColorMaterialEmission);

					//start mouth
					glPushMatrix();
					{
						glTranslatef(0, -1.27, 0.15);
						glScalef(1.3, 0.3, 0.5);
						glColor3f(1, 1, 1);
						glutSolidCube(1);
					}
					glPopMatrix();
					//draw head
					glColor3f(1.0, 1.0, 1.0);
					glutSolidSphere(1, 30, 30);
				}
				glPopMatrix(); //end head

				//draw neck
				glColor3f(0.6, 0.9, 0.3);
				glutSolidCube(1);


			}
			glPopMatrix(); //end neck




			//start left shoulder
			glPushMatrix();
			{
				glRotatef((GLfloat)left_shoulder, 0.3, 0, 0);
				glTranslatef(-1.8, 0.4, 0);
				glScalef(0.4, 1.2, 0.4);
				//start left elbow, inherit translation 
				glPushMatrix();
				{
					glRotatef((GLfloat)left_elbow, 0, 1, 0);
					glTranslatef(0, -0.9, 0);
					glScalef(1, 0.8, 1);

					//start left hand, inherit parent (left elbow)'s transformation
					glPushMatrix();
					{
						glRotatef((GLfloat)left_hand, 0, -0.1, 0);
						glTranslatef(0, -0.75, 0);
						glScalef(1, 0.5, 1);
						//start sword 
						glPushMatrix();
						{
							//start sword's grip tail
							glPushMatrix();
							{
								glTranslatef(0, 0, -0.8);
								glScalef(0.4, 0.4, 1);
								glColor3f(0.6, 0.5, 0.2);
								glutSolidCube(1);

								//start sword's grip tail's diamond
								glPushMatrix();
								{
									glTranslatef(0, 0, -0.8);
									glScalef(1.2, 1.2, 0.4);
									glColor3f(0.6, 0.6, 0.1);
									glutSolidSphere(1, 30, 30);
								}
								glPopMatrix(); //end sword's grip tail's diamond
							}
							glPopMatrix(); //end sword's grip tail

							//start sword's body
							glPushMatrix();
							{
								glTranslatef(0, 0, 0.5);
								glScalef(1, 0.3, 10);
								glDisable(GL_LIGHTING);
								glLightfv(GL_LIGHT0, GL_POSITION, light_position);
								//draw weapon
								glColor3f(0.9, 1.0, 1.0);
								glutSolidCone(1, 1, 3, 3);
								//we want the light source to be at the weapon
								//we disable lighting on the weapon as it will be dull because its surface is weaken by light
								//enable lighting for the rest
								glEnable(GL_LIGHTING);
								glEnable(GL_LIGHT0);
							}
							glPopMatrix(); //end sword's body

						}
						glPopMatrix(); //end sword

						//draw left hand
						glColor3f(0.6, 0.8, 0.2);
						glutSolidCube(1);
					}
					glPopMatrix();  //end left hand

					//draw left elbow
					glColor3f(0.6, 0.8, 0.4);
					glutSolidCube(1);
				}
				glPopMatrix();  //end left elbow
				//draw left shoulder
				glColor3f(0.6, 0.9, 0.3);
				glutSolidCube(1);
			}
			glPopMatrix();// end left shoulder

			//start right shoulder
			glPushMatrix();
			{
				glRotatef((GLfloat)right_shoulder, 1, 0, 0);
				glTranslatef(1.8, 0.4, 0);
				glScalef(0.4, 1.2, 0.4);

				//start right elbow, inherit translation 
				glPushMatrix();
				{
					glRotatef((GLfloat)right_elbow, 0, 1, 0);
					glTranslatef(0, -0.9, 0);
					glScalef(1, 0.8, 1);
					//start right hand, inherit parent (right elbow)'s transformation
					glPushMatrix();
					{
						glRotatef((GLfloat)right_hand, 0, 1, 0);
						glTranslatef(0, -0.75, 0);
						glScalef(1, 0.5, 1);

						//start shield
						glPushMatrix();
						{
							//start shield grip
							glPushMatrix();
							{
								glTranslatef(0.5, 0, 0);
								glScalef(0.5, 0.5, 0.5);
								glColor3f(1, 1, 1);
								glutSolidCube(1);
							}
							glPopMatrix(); //end shield grip

							//start shield inner body
							glPushMatrix();
							{
								glTranslatef(0.8, 0, 0);
								glRotatef(45, 1.0, 0.0, 0.0);
								glScalef(0.2, 4, 4);
								glColor3f(0.8, 0.6, 0.3);
								glutSolidCube(1);
							}
							glPopMatrix(); //end shield inner body

							//start shield outer body
							glPushMatrix();
							{
								glTranslatef(1.2, 0, 0);
								glRotatef(45, 1.0, 0.0, 0.0);
								glScalef(0.5, 3.0, 3.0);
								glColor3f(0.8, 0.7, 0.2);
								glutSolidCube(1);
							}
							glPopMatrix(); //end shield outer body

							//start shield outer body top diamond
							glPushMatrix();
							{
								//light_shader
								GLfloat light_diffuse[] = { 0.1,0.1,0.4 };
								glEnable(GL_LIGHT1);
								glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

								glTranslatef(1.5, 1.4, 0);
								glScalef(0.2, 0.2, 0.2);
								glColor3f(0.8, 1.0, 0.4);
								glutSolidSphere(1, 30, 30);
							}
							glPopMatrix(); //end shield outer body top diamond

							//start shield outer body middle diamond
							glPushMatrix();
							{
								glTranslatef(1.5, 0, 0);
								glScalef(0.5, 0.5, 0.5);
								glColor3f(0.8, 1.0, 1.0);
								glutSolidSphere(1, 30, 30);

							}
							glPopMatrix(); //end shield outer body middle diamond

						   //start shield outer body bottom diamond
							glPushMatrix();
							{
								glTranslatef(1.5, -1.4, 0);
								glScalef(0.2, 0.2, 0.2);
								glColor3f(0.8, 1.0, 0.4);
								glutSolidSphere(1, 30, 30);
							}
							glPopMatrix(); //end shield outer body top diamond

							//start shield outer body front diamond
							glPushMatrix();
							{
								glTranslatef(1.5, 0, 1.4);
								glScalef(0.2, 0.2, 0.2);
								glColor3f(0.8, 1.0, 0.4);
								glutSolidSphere(1, 30, 30);
							}
							glPopMatrix(); //end shield outer body front diamond

							//start shield outer body back diamond
							glPushMatrix();
							{
								glTranslatef(1.5, 0, -1.4);
								glScalef(0.2, 0.2, 0.2);
								glColor3f(0.8, 1.0, 0.4);
								glutSolidSphere(1, 30, 30);

								//disable the second light source
								glDisable(GL_LIGHT1);
							}
							glPopMatrix(); //end shield outer body back diamond

						}
						glPopMatrix(); //end shield

						//draw right hand
						glColor3f(0.6, 0.8, 0.2);
						glutSolidCube(1);
					}
					glPopMatrix(); //end right hand

					//draw right elbow
					glColor3f(0.6, 0.8, 0.4);
					glutSolidCube(1);
				}
				glPopMatrix();  //end right elbow

				//draw right shoulder
				glColor3f(0.6, 0.9, 0.3);
				glutSolidCube(1);
			}
			glPopMatrix(); // end right shoulder

			glTranslatef(0, -0.35, 0);
			glScalef(1.3, 0.7, 1.3);
			//draw torso
			glColor3f(0.6, 0.9, 0.3);
			glutSolidCube(1);
		}
		glPopMatrix(); //end torso

		//start upper left leg
		glPushMatrix();
		{
			glRotatef((GLfloat)left_upper_leg, 1, 0, 0);
			glTranslatef(-0.5, -1.1, 0);
			glScalef(0.5, 0.8, 0.5);

			//start lower left leg
			glPushMatrix();
			{
				glRotatef((GLfloat)left_lower_leg, 1, 0, 0);
				glTranslatef(0, -1, 0);

				//start left foot
				glPushMatrix();
				{
					glRotatef((GLfloat)left_foot, 1, 0, 0);
					glTranslatef(0, -0.75, 0.25);
					glScalef(1, 0.5, 1.5);
					//draw left foot
					glColor3f(0.6, 0.8, 0.4);
					glutSolidCube(1);
				}
				glPopMatrix(); //end left foot

				//draw lower left leg
				glColor3f(0.6, 0.8, 0.4);
				glutSolidCube(1);
			}
			glPopMatrix(); //end lower leg

			//draw upper left leg
			glColor3f(0.6, 0.9, 0.3);
			glutSolidCube(1);
		}
		glPopMatrix(); //end left upper leg

		//start upper right leg
		glPushMatrix();
		{
			glRotatef((GLfloat)right_upper_leg, 1, 0, 0);
			glTranslatef(0.5, -1.1, 0);
			glScalef(0.5, 0.8, 0.5);
			//start lower right leg
			glPushMatrix();
			{
				glRotatef((GLfloat)right_lower_leg, 1, 0, 0);
				glTranslatef(0, -1, 0);
				//start right foot
				glPushMatrix();
				{
					glRotatef((GLfloat)right_foot, 1, 0, 0);
					glTranslatef(0, -0.75, 0.25);
					glScalef(1, 0.5, 1.5);
					//draw right foot
					glColor3f(0.6, 0.8, 0.4);
					glutSolidCube(1);
				}
				glPopMatrix(); //end right foot

				//draw lower right leg
				glColor3f(0.6, 0.8, 0.4);
				glutSolidCube(1);
			}
			glPopMatrix(); //end lower leg

			//draw upper right leg
			glColor3f(0.6, 0.9, 0.3);
			glutSolidCube(1);
		}
		glPopMatrix(); //end upper right leg

	}
	glPopMatrix(); //end hierarchy

	

	//free the texture
	FreeTexture(texture);
	glutPostRedisplay();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//distance from camera far 10px
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
	case 'A':
		PlaySound(TEXT("C:\\zmisc\\monster-inc.wav"), NULL, SND_ASYNC);
		break;

	case 's':
	case 'S':
		PlaySound(NULL, 0, 0);
		break;
	}
	glFlush();
	glutPostRedisplay();

}

void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		camera_center_y -= camera_center_scale;
		break;
	case GLUT_KEY_DOWN:
		camera_center_y += camera_center_scale;
		break;
	case GLUT_KEY_LEFT:
		camera_center_x -= camera_center_scale;
		break;
	case GLUT_KEY_RIGHT:
		camera_center_x += camera_center_scale;
		break;
	}

	glutPostRedisplay();
}

//camera move
static int mouseX;  //record button down coordination
static int moveCamera;

/* mouse press detection */
static void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_MIDDLE_BUTTON) {
		if (state == GLUT_DOWN) {
			moving = 1;
			startx = x;
			starty = y;
		}
		if (state == GLUT_UP) {
			moving = 0;
		}
	}
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {

			mouseX = x;
			moveCamera = 1;
		}
		if (state == GLUT_UP) {
			moveCamera = 0;
		}
	}
}
/* motion of mouse */
static void motion(int x, int y)
{
	if (moving) {
		angle_whole_body_y += (x - startx);
		angle_whole_body_x += (y - starty);
		startx = x;
		starty = y;
		glutPostRedisplay();
	}

	if (moveCamera) {
		GLfloat camera_distance_scale = 0.05;
		//motion to the left, diminish
		if (x < mouseX)
			camera_distance -= camera_distance_scale;
		//motion to the right, enlarge
		if (x >= mouseX)
			camera_distance += camera_distance_scale;

	}

}

/*Main Menu*/
void MainMenu(int id)
{
	//reset model
	if (id == 15) {
		//reset also axis rotation origin
		angle_whole_body_x = 0;
		angle_whole_body_y = 0;
		angle_whole_body_z = 0;

		torso_rotate_x = 0;
		torso_rotate_y = 0;
		torso_rotate_z = 0;

		neck = 0;
		head = 0;

		left_shoulder = 0;
		left_elbow = 0;
		left_hand = 0;

		right_shoulder = 0;
		right_elbow = 0;
		right_hand = 0;

		left_upper_leg = 0;
		left_lower_leg = 0;
		left_foot = 0;

		right_upper_leg = 0;
		right_lower_leg = 0;
		right_foot = 0;
	}
	if (id == 16)
	{
		if (full_screen_toggle == 0)
		{
			glutFullScreen();
			full_screen_toggle = 1;
		}
		else
		{
			glutPositionWindow(10, 30);
			glutReshapeWindow(winSizeX, winSizeY);
			full_screen_toggle = 0;
		}
	}

	if (id == 17)
		exit(0);
}
/* Animation Menu*/
void AnimationMenu(int id)
{
	if (id == 0)
	{
		anim_head_toggle = 0;
		anim_neck_toggle = 0;
		anim_hand_toggle = 0;
		anim_walk_toggle = 0;
		anim_raise_sword_toggle = 0;
		anim_bowing_toggle = 0;
		anim_look_around_toggle = 0;
		anim_bend_waist_toggle = 0;
		anim_bend_waist_backward_toggle = 0;

	}
	if (id == 1)
	{
		if (anim_head_toggle == 0)
			anim_head_toggle = 1;
		else
			anim_head_toggle = 0;
	}

	if (id == 2)
	{
		if (anim_neck_toggle == 0)
			anim_neck_toggle = 1;
		else
			anim_neck_toggle = 0;
	}

	if (id == 3)
	{
		if (anim_hand_toggle == 0)
			anim_hand_toggle = 1;
		else
			anim_hand_toggle = 0;
	}

	if (id == 4)
	{
		if (anim_walk_toggle == 0)
			anim_walk_toggle = 1;
		else
			anim_walk_toggle = 0;
	}

	if (id == 5)
	{
		if (anim_raise_sword_toggle == 0)
			anim_raise_sword_toggle = 1;
		else
			anim_raise_sword_toggle = 0;
	}

	if (id == 6)
	{
		if (anim_bowing_toggle == 0)
			anim_bowing_toggle = 1;
		else
			anim_bowing_toggle = 0;
	}
	if (id == 7)
	{
		if (anim_look_around_toggle == 0)
			anim_look_around_toggle = 1;
		else
			anim_look_around_toggle = 0;
	}
	if (id == 8)
	{
		if (anim_bend_waist_toggle == 0)
			anim_bend_waist_toggle = 1;
		else
			anim_bend_waist_toggle = 0;
	}
	if (id == 9)
	{
		if (anim_bend_waist_backward_toggle == 0)
			anim_bend_waist_backward_toggle = 1;
		else
			anim_bend_waist_backward_toggle = 0;
	}
	if (id == 10)
	{

		anim_head_toggle = 0;
		anim_neck_toggle = 0;
		anim_hand_toggle = 0;
		anim_walk_toggle = 0;
		anim_bowing_toggle = 0;
		anim_look_around_toggle = 0;
		anim_bend_waist_toggle = 0;

	}
}
/*Axis Rotation Menu*/
void AxisRotationMenu(int id)
{
	if (id == 11)
	{
		rotate_x_toggle = 0;
		rotate_y_toggle = 0;
		rotate_z_toggle = 0;
	}
	if (id == 12)
	{
		if (rotate_x_toggle == 0)
			rotate_x_toggle = 1;
		else
			rotate_x_toggle = 0;
	}

	if (id == 13)
	{
		if (rotate_y_toggle == 0)
			rotate_y_toggle = 1;
		else
			rotate_y_toggle = 0;
	}

	if (id == 14)
	{
		if (rotate_z_toggle == 0)
			rotate_z_toggle = 1;
		else
			rotate_z_toggle = 0;
	}

}
/*Item Insertion Menu*/
void Menu()
{
	int anim_submenu = glutCreateMenu(AnimationMenu);
	glutAddMenuEntry("Cancel All Animation", 0);
	glutAddMenuEntry("Toggle Eye", 1);
	glutAddMenuEntry("Toggle Neck", 2);
	glutAddMenuEntry("Toggle Hand", 3);
	glutAddMenuEntry("Toggle Walk", 4);
	glutAddMenuEntry("Toggle Raise Sword", 5);
	glutAddMenuEntry("Toggle Bowing", 6);
	glutAddMenuEntry("Toggle Look Around", 7);
	glutAddMenuEntry("Toggle Bend Waist", 8);
	glutAddMenuEntry("Toggle Bend Waist Backward", 9);

	int axis_rotation_submenu = glutCreateMenu(AxisRotationMenu);
	glutAddMenuEntry("Cancel All Axis Rotation", 11);
	glutAddMenuEntry("Toggle x-axis Rotation", 12);
	glutAddMenuEntry("Toggle y-axis Rotation", 13);
	glutAddMenuEntry("Toggle z-axis Rotation", 14);

	glutCreateMenu(MainMenu);
	glutAddSubMenu("Animation", anim_submenu);
	glutAddSubMenu("Axis Rotation", axis_rotation_submenu);

	glutAddMenuEntry("Reset", 15);

	glutAddMenuEntry("Toggle Full Screen", 16);
	glutAddMenuEntry("Exit", 17);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



//for animation
static void timer(int)
{
	//if axis rotation activated
	if (rotate_x_toggle) angle_whole_body_x = (angle_whole_body_x + 1) % 360;
	if (rotate_y_toggle) angle_whole_body_y = (angle_whole_body_y + 1) % 360;
	if (rotate_z_toggle) angle_whole_body_z = (angle_whole_body_z + 1) % 360;

	//rotate surrounding object
	surround_object_angle = (surround_object_angle + 2) % 360;
	//rotate surrounding object child
	surround_object_child_angle = (surround_object_child_angle + 3) % 360;

	//rotate ears
	ears_angle = (ears_angle + 5) % 360;

	//translate surround object
	switch (surround_object_child_translate_state)
	{
	case 0:
		if (surround_object_child_translate < 5.0)
			surround_object_child_translate += 0.05;
		else surround_object_child_translate_state = 1;

		break;
	case 1:
		if (surround_object_child_translate > 0.0)
			surround_object_child_translate -= 0.05;
		else surround_object_child_translate_state = 0;
		break;
	}


	if (anim_head_toggle)
	{
		switch (anim_head_state)
		{
		case 0:
			if (head < 20)
				head++;
			else
				anim_head_state = 1;
			break;
		case 1:
			if (head > -20)
				head--;
			else
				anim_head_state = 0;
			break;
		}
	}

	if (anim_neck_toggle)
	{
		switch (anim_neck_state)
		{
		case 0:
			if (neck < 20)
				neck++;
			else
				anim_neck_state = 1;
			break;
		case 1:
			if (neck > -20)
				neck--;
			else
				anim_neck_state = 0;
			break;
		}
	}

	if (anim_hand_toggle)
	{
		switch (anim_hand_state)
		{
		case 0:
			if (left_shoulder < 45)
			{
				left_shoulder++;
				right_shoulder--;
				left_hand--;
				right_hand--;
			}
			else
				anim_hand_state = 1;
			break;

		case 1:
			if (left_shoulder > -45)
			{
				left_shoulder--;
				right_shoulder++;
				left_hand++;
				right_hand++;

			}
			else
				anim_hand_state = 0;
			break;
		}
	}

	if (anim_walk_toggle)
	{
		switch (anim_walk_state)
		{
		case 0:
			if (left_upper_leg > -45)
			{
				left_upper_leg--;
				right_upper_leg++;
				left_lower_leg++;
				right_lower_leg--;
				left_foot--;
				right_foot++;
			}
			else
				anim_walk_state = 1;
			break;
		case 1:
			if (left_upper_leg < 45)
			{
				left_upper_leg++;
				right_upper_leg--;
				left_lower_leg--;
				right_lower_leg++;
				left_foot++;
				right_foot--;
			}
			else
				anim_walk_state = 0;
			break;
		}
	}


	if (anim_raise_sword_toggle)
	{
		switch (anim_raise_sword_state)
		{
		case 0:
			if (left_shoulder > -80)
			{
				left_shoulder--;
				left_hand++;
			}
			else
				anim_raise_sword_state = 1;
			break;
		case 1:
			if (left_shoulder < 0)
			{
				left_shoulder++;
				left_hand--;
			}
			else
				anim_raise_sword_state = 0;
			break;
		}
	}

	if (anim_bowing_toggle)
	{

		switch (anim_bowing_state)
		{
		case 0:
			if (torso_rotate_x < 90)
				torso_rotate_x++;
			else anim_bowing_state = 1;
			break;
		case 1:
			if (torso_rotate_x > 0)
				torso_rotate_x--;
			else anim_bowing_state = 0;
			break;
		}
	}

	if (anim_look_around_toggle)
	{
		switch (anim_look_around_state)
		{
		case 0:
			if (torso_rotate_y < 45)
				torso_rotate_y++;
			else
				anim_look_around_state = 1;
			break;
		case 1:
			if (torso_rotate_y > -45)
				torso_rotate_y--;
			else
				anim_look_around_state = 0;
			break;
		}
	}

	if (anim_bend_waist_toggle)
	{
		switch (anim_bend_waist_state)
		{
		case 0:
			if (torso_rotate_z < 45)
				torso_rotate_z++;
			else
				anim_bend_waist_state = 1;
			break;
		case 1:
			if (torso_rotate_z > -45)
				torso_rotate_z--;
			else
				anim_bend_waist_state = 0;
			break;
		}
	}

	if (anim_bend_waist_backward_toggle)
	{
		switch (anim_bend_waist_backward_state)
		{
		case 0:
			if (torso_rotate_x > -45)
			{
				torso_rotate_x--;
				left_upper_leg++;
				right_upper_leg++;
				left_lower_leg++;
				right_lower_leg++;
				left_foot++;
				right_foot++;
				left_shoulder--;
			}
			else
				anim_bend_waist_backward_state = 1;
			break;
		case 1:
			if (torso_rotate_x < 0)
			{
				torso_rotate_x++;
				left_upper_leg--;
				right_upper_leg--;
				left_lower_leg--;
				right_lower_leg--;
				left_foot--;
				right_foot--;
				left_shoulder++;
			}
			else
				anim_bend_waist_backward_state = 0;
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);

}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(winSizeX, winSizeY);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Final project");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialInput);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(0, timer, 0);

	//create menu for animation
	Menu();
	glutMainLoop();
	return 0;
}