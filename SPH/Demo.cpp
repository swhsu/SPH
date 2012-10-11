/*  Demo.cpp
 *
 *  SPH Simulation Demo App
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#include <windows.h>
#include "Timerlist.h"
#include "Demo.h"

void drawAxes (void)
{
	glPushMatrix ();
	glTranslatef (0,0, 0);	
	glLineWidth (1.0);
	glBegin (GL_LINES);
	glColor3f (1,0,0); // X axis is red.
	glVertex3fv (ORG);
	glVertex3fv (XP );
	glColor3f (0,1,0); // Y axis is green.
	glVertex3fv (ORG);
	glVertex3fv (YP );
	glColor3f (0,0,1); // z axis is blue.
	glVertex3fv (ORG);
	glVertex3fv (ZP );
	glEnd();
	glPopMatrix ();
}


void motion(int x,int y)
{
	int diffx=x-lastx;
	int diffy=y-lasty;
	lastx=x;
	lasty=y;

	if( Buttons[2] )																	// zoom
		cam.moveDist(diffy*-0.02f);
	else if( Buttons[0] ) 														// ratate
		cam.rotate(diffx*0.01f, diffy*0.01f);
	else if( Buttons[1] )   													// drive (translate on screen space)
		cam.moveDrive(diffx*0.01f, -diffy*0.01f);      

	glutPostRedisplay();
}


void mouse(int b,int s,int x,int y)
{
	lastx=x;
	lasty=y;
	switch(b) {
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN==s)?1:0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN==s)?1:0);
		break;
	case GLUT_RIGHT_BUTTON:
		Buttons[2] = ((GLUT_DOWN==s)?1:0);
		break;
	default:
		break;		
	}

	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) 
{
	switch(key) {
		case ' ':
			if(!simStart)
				glutIdleFunc(simLoop);
			else
				glutIdleFunc(NULL);
			simStart = !simStart;
			break;
		case 'r':
			sph->numFrame = 0;
			sph->reset();
			break;
		case '[':
			sph->b->d-=0.1f;
			break;
		case ']':
			sph->b->d+=0.1f;
			break;
		case 'l':
			TIMER.PrintResults();
			break;
		case 'v':
			cerr << "\n[Start] compute volume..." << endl;
			sph->volume();
			cerr << "\n[End] compute volume.";
			break;
		case '+':
			isoValue+=0.0005f;
			break;
		case '-':
			isoValue-=0.0005f;
			break;
		case '1':
			++pick;
			break;
		case '2':
			if(pick>1)
				--pick;
			break;
		case 'd':
			sph->dumpVolume();
			break;
		case 'n':
			simLoop();
			glutPostRedisplay();
			break;
		case 'e':
			{
			isExtractData ^= 1;

			// create a folder for saving data
			SYSTEMTIME* p_st = new SYSTEMTIME;
			GetLocalTime(p_st);
			char filename[128];
			sprintf_s(filename, "numP_%d_(%d_%d)_(%d_%d)", NUM_PARTICLE, p_st->wHour, p_st->wMinute, p_st->wMonth, p_st->wDay);
			dataExtractor->createFolder(filename);

			cout << "Extract Data:" << isExtractData << endl;
			break;
			}
		case 'g':
			sph->sphLoop();
			break;
		case 't':
			TIMER.PrintResults();
			break;
		case 27:
			exit(0);
			break;
	}
	glutPostRedisplay();
}


void specialKeys(int key, int x, int y) 
{
	switch(key) {
		case GLUT_KEY_F1 : 
			sph->displayMode = 0;
			break;
		case GLUT_KEY_F2 : 
			sph->displayMode = 1;
			break;
		case GLUT_KEY_F3 : 
			sph->displayMode = 2;
			break;
		case GLUT_KEY_F4 : 
			sph->displayMode = 3;
			break;
	}
	glutPostRedisplay();
}


void reshape(int w, int h)
{
	winW = w;
	winH = h;
	glViewport(0,0,w,h);
	cam.setRatio((float)w/h);
	cam.setProj();
}


void render()
{
	glColor4f(0, 1, 1, 0.95f);
	sph->render();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.9f,0.9f,0.9f,0.05f);
	sph->b->render();
	glDisable(GL_BLEND);


	//glLoadIdentity();
	// draw axes
	glDisable(GL_LIGHTING);
	drawAxes();
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}


void display() 
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	cam.setView();
	render();
}


void lightInit()
{
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);	

	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);	
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);	

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


void simInit()
{
	// init the simulator
	sph = new SPHSystem(NUM_PARTICLE);
	sph->initScene();
	
	// init the data extractor
	dataExtractor = new DataExtractor(sph);
}


void simLoop()
{
	TIMER[ONESTEP].Start();
	sph->sphLoop();
	TIMER[ONESTEP].Stop();

	glutPostRedisplay();
	sph->numFrame++;

	
	if(isExtractData) {
		dataExtractor->saveSnapShot();
		dataExtractor->saveAvgGridVel(32,32);
		dataExtractor->numSnapShot++;
	}

	cerr << sph->numFrame << endl;
}


void glInit() 
{
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	lightInit();
	glPointSize(3);
}


int main(int argc, char** argv) 
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(winW,winH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("SPH");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);	
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glInit();
	simInit();	
	cam.setPos(0.01f, -PI, 15.0f, Point3f(0.2f,0.2f,0));
	cam.moveDrive(-3,-3);
	glutMainLoop();
	return 0;
}
