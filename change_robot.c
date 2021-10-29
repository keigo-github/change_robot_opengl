
/* List: p3-robot.c
 * Robot arm with two links and two joints.
 * Examination of world-local coordinates, modeling transfomation and
 * operation of matrix stack.
 */
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
//#include <cstdio>

static int shoulder = 0, elbow = 0, elbow2 = 0, viewx = 0,viewy = 0, viewz = 0;
unsigned char	mouseFlag = GL_FALSE;		// flag for moving or not
int				xStart, yStart;				// start position when drug begins
double			xAngle = 0.0, yAngle = 0.0;	// angles of the teapot

#define	imageWidth 256
#define	imageHeight 256

double	 theta = 0.0;						// angular of tea pot

//white color
float mtrl_ambientw[] = { 0, 0, 0, 1.0 };
float mtrl_diffusew[] = { 0.55,0.55,0.55,1.0};
float mtrl_specularw[] = { 0.7, 0.7, 0.7, 1.0 };
float mtrl_shininessw[] = { 0.25 };					// range [0,128]
//red color
float mtrl_ambientr[] = { 0.0, 0.0, 0.0, 1.0 };
float mtrl_diffuser[] = { 0.5,0.0,0.0,1.0 };
float mtrl_specularr[] = { 0.7, 0.6, 0.6, 1.0 };
float mtrl_shininessr[] = { 0.25};					// range [0,128]
//blue color
float mtrl_ambientb[] = { 0.2, 0.2, 0.7, 1.0 };
float mtrl_diffuseb[] = { 0.0,0.5,0.5,1.0 };
float mtrl_specularb[] = { 0.5, 0.5, 0.5, 1.0 };
float mtrl_shininessb[] = { 0.1};					// range [0,128]
//dark color
float mtrl_ambientd[] = { 0.0, 0.0, 0.0, 1.0 };
float mtrl_diffused[] = { 0.01,0.01,0.01,1.0 };
float mtrl_speculard[] = { 0.5, 0.5, 0.5, 1.0 };
float mtrl_shininessd[] = { 0.25 };


unsigned char texImage[imageHeight][imageWidth][3];

void mySetLight()
{
	float light0_position[] = { -30.0,30.0, 30,0, 1.0 };

	/* LIGHT0 uses the default parameters except position */
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);
	/* Shading the both sides of box */
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


void readPPMImage(char* filename)
{
	FILE* fp;
	int  ch, i;

	if (fopen_s(&fp, filename, "r") != 0) {
		fprintf(stderr, "Cannot open ppm file %s\n", filename);
		exit(1);
	}
	/*
	for (i = 0; i < 4; i++){ 						// skip four in header
		while (1){
			if ((ch = fgetc(fp)) != '#') break;		// skip comment
			fgets((char*)texImage, 1024, fp);   	// dummy read
			while(isspace(ch)) ch = fgetc(fp);  	// skip space
		}
		while (!isspace(ch)) ch = fgetc(fp);		// read header
		if (i < 3){
			while (isspace(ch)) ch = fgetc(fp);		// skip terminator
		}
	}*/
	fread(texImage, 1, imageWidth * imageHeight * 3, fp);	// read RGB data
	fclose(fp);
}

void setUpTexture(void)
{
	readPPMImage("kusa.ppm");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
}



void myKeyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case 'e':
		elbow = (elbow + 5) % 360;
		glutPostRedisplay();
		break;

	case 'E':
		elbow = (elbow - 5) % 360;
		glutPostRedisplay();
		break;

	case 'x':
		viewx = (viewx + 1);
		glutPostRedisplay();
		break;

	case 'X':
		viewx = (viewx - 1);
		glutPostRedisplay();
		break;

	case 'y':
		viewy = (viewy + 1);
		glutPostRedisplay();
		break;

	case 'Y':
		viewy = (viewy -1);
		glutPostRedisplay();
		break;

	case 'z':
		viewz = (viewz + 1);
		glutPostRedisplay();
		break;

	case 'Z':
		viewz = (viewz - 1);
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}


void myInit(char* progname)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(progname);
	glClearColor(0.1, 0.5, 0.8, 0.7);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

}


void myReshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)width / (double)height, 0.1, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0, 0.0, -10.0);				// move to enable viewing

}


void myDisplay(void)
{

	int i = 0;
	double	tc = 15.0;
	double	p0[] = { -15.0, -4.0, -15.0 }, p1[] = { -15.0, -4.0, 15.0 },
		p2[] = { 15.0, -4.0, 15.0 }, p3[] = { 15.0, -4.0, -15.0 };


	//glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	/* View link */
	glTranslated((double)viewx, (double)viewy, (double)viewz);
	glRotated(xAngle, 1.0, 0.0, 0.0);
	glRotated(yAngle, 0.0, 1.0, 0.0);

	/*texture mapping*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex3dv(p0);
	glTexCoord2d(0.0, tc); glVertex3dv(p1);
	glTexCoord2d(tc, tc); glVertex3dv(p2);
	glTexCoord2d(tc, 0.0); glVertex3dv(p3);

	glEnd();

	glFlush();
	glDisable(GL_TEXTURE_2D);
	/*texturemappingend*/

	glColor3d(1.0, 1.0, 1.0);		// draw floor
	glBegin(GL_LINES);
	for (i = -35; i < 36; i += 2) {
		glVertex3i(i, -4, -35);
		glVertex3i(i, -4, 35);
		glVertex3i(-50, -4, i);
		glVertex3i(50, -4, i);
	}
	glEnd();
	//if (keysIsPressed['a'])
	//glTranslated(1.0, 0.0, 0.0);


	/* 1st link */


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glRotated((double)elbow, 0.0, 0.0, 1.0);
	glTranslated(1.0, 0.0, 0.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffused);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_speculard);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessd);
	glPushMatrix();
	glScaled(2.0, 0.4, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

//glRotated((double)shoulder, 0.0, 0.0, 1.0);
	glTranslated(0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPushMatrix();
	glScaled(2.0, 0.4, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glTranslated(-1.0, 0.4, -0.5);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffuseb);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularb);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessb);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(1.0, 1.0, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glTranslated(.0, -0.6, -0.);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffuseb);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularb);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessb);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.8, 0.8, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glTranslated(0, 0.6, -0.);


	glRotated((double)elbow, 0.0, 0.0, 1.0);


	//body front

	glTranslated(-1.0, -0.4, 0.0);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffuser);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientr);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularr);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessr);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(1.5, 0.7, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glTranslated(-0.6, 0., 0);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffusew);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientw);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularw);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessw);
	glScaled(0.9, 0.9, 1.7);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.6, 0., 0);


	glTranslated(1.0, 0.4, 0);

	//body bone 
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffusew);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientw);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularw);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessw);
	glScaled(0.3, 0.3, 2.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glutSolidCube(1.0);
	glPopMatrix();

	//face
	glTranslated(0.7, 0.0, 0);
	glRotated((double)elbow, 0, 0, 1);
	glRotated((double)elbow, 0, 0, 1);
	glTranslated(-0.3, 0.0, 0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.6, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glTranslated(-0.3, 0.0, 0.3);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.3, 0.0, -0.3);

	glTranslated(-0.35, 0.05, 0.35);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.35, -0.05, -0.35);

	glTranslated(-0.4, 0.1, 0.4);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.4, -0.1, -0.4);

	glTranslated(-0.45, 0.15, 0.45);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.45, -0.15, -0.45);

	glTranslated(-0.3, 0.0, -0.3);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.3, 0.0, 0.3);

	glTranslated(-0.35, 0.05, -0.35);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.35, -0.05, 0.35);

	glTranslated(-0.4, 0.1, -0.4);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.4, -0.1, 0.4);

	glTranslated(-0.45, 0.15, -0.45);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.1, 0.1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.45, -0.15, 0.45);

	glTranslated(0.3, 0.3,-1.3);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.7, 0.7, 0.7);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(-0.3, -0.3, 1.3);

	glTranslated(0.3, 0.3, 1.3);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.7, 0.7, 0.7);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(-0.3, -0.3, -1.3);


	glTranslated(-0.3, 0.0, 0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.15, 0.5, 0.15);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.3, 0.0, 0);

	glTranslated(-0.2, 0.0, 0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffuser);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientr);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularr);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessr);
	glScaled(0.15, 0.6, 0.15);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.2, 0.0, 0);

	glTranslated(-0.1,0, -0.18);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.15, 0.6, 0.15);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.1,0, 0.18);

	glTranslated(-0.1,0, 0.18);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.15, 0.6, 0.15);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(0.1,0, -0.18);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);




	glTranslated(0.3, 0.0, 0);
	glRotated((double)elbow, 0, 0, -1);
	glRotated((double)elbow, 0, 0, -1);
	glTranslated(0.3, 0.0, 0);

	//lefthand
	glTranslated(0.0, 0.0, 1.5);
	glRotated((double)elbow, 0, 1.0, 0);
	glTranslated(.0, 0.0, -1.5);
	glPushMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtrl_diffusew);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtrl_ambientw);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtrl_specularw);
	glMaterialfv(GL_FRONT, GL_SHININESS, mtrl_shininessw);
	glScaled(0.3, 0.3, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(.0, 0.0, 1.5);
	glRotated((double)elbow, 0, -1.0, 0);
	glTranslated(0, 0.0, -1.5);

	//right hand
	glTranslated(0.0, 0.0, -1.5);
	glRotated((double)elbow, 0, -1.0, 0);
	glTranslated(.0, 0.0, 1.5);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.3, 0.3, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(.0, 0.0, -1.5);
	glRotated((double)elbow, 0, 1.0, 0);
	glTranslated(0, 0.0, 1.5);


	glTranslated(0.0, -0.4, -0.5);
	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glTranslated(1.0, 0.0, 0.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(2.0, 0.4, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();


	//glRotated((double)shoulder, 0.0, 0.0, 1.0);
	glTranslated(0.0, 0.0, 1.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(2.0, 0.4, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();



	/* 2nd link */
	glTranslated(1.0, 0.0, -1.0);				//move to the end of 1st link

	glRotated((double)elbow, 0.0, 1.0, 0.0);
	glTranslated(0.5, 0.0, 0.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.4, 0.4, 0.4);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(-0.5, 0.0, 0.0);
	glRotated((double)elbow, 0.0, -1.0, 0.0);


	glTranslated(0.5, 0.0, -0.5);
	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.4, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	//	glTranslated(0.5, 0.0, 0.5);
	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glTranslated(-2.0, 0.0, 0.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(-3.0, 0.5, -0.5);
	glutSolidCube(1.0);
	glPopMatrix();


	// left legg
	glTranslated(2.0, 0.0, 0.0);
	glRotated((double)elbow, 0.0, 0.0, 1.0);
	glRotated((double)elbow, 0.0, 0.0, 1.0);
	/* 2nd link */
	glTranslated(-0.5, 0.0, 1.5);
	glRotated((double)elbow, 0.0, -1.0, 0.0);
	//glTranslated(0.0, 0.0, 1.0);			//move to the end of 1st link
	//glRotated((double)elbow, 0.0, -1.0, 0.0);
	glTranslated(0.5, 0.0, 0.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.4, 0.4, 0.4);
	glutSolidCube(1.0);
	glPopMatrix();
	glTranslated(-0.5, 0.0, 0.0);
	glRotated((double)elbow, 0.0, 1.0, 0.0);

	glTranslated(0.5, 0.0, 0.5);
	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(0.4, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	
	glRotated((double)elbow, 0.0, 0.0, -1.0);
	glTranslated(-2.0, 0.0, 0.);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glScaled(3, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);



	glPopMatrix();
	glutSwapBuffers();
}




void myMouseMotion(int x, int y)
{
	int		xdis, ydis;
	double	a = 0.5;

	if (mouseFlag == GL_FALSE) return;
	xdis = x - xStart;
	ydis = y - yStart;
	/* To match mouse's movement and its rotation axis */
	xAngle += (double)ydis * a;
	yAngle += (double)xdis * a;

	xStart = x;
	yStart = y;
	glutPostRedisplay();
}

void myMouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		xStart = x;
		yStart = y;
		mouseFlag = GL_TRUE;
	}
	else {
		mouseFlag = GL_FALSE;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	myInit(argv[0]);
	mySetLight();
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMouseMotion);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	setUpTexture();
	glutMainLoop();
	return 0;
}


