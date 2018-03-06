//
//modified by: Derrick Alden
//date:
//
//program: lab7.cpp generated from openg.cpp
//author:  Gordon Griesel
//date:    Spring 2018
//
//An OpenGL 3D framework for students.
//
//Goal:
//     1. setup the keyboard input to move the camera location
//        and camera viewing direction.
//     2. move the camera down and to the left to line-up with the hole
//     3. move the camera smoothly through the hole in the wall
//
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "fonts.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif //USE_OPENAL_SOUND

typedef float Flt;
typedef Flt Vec[3];
typedef Flt	Matrix[4][4];
//some macros
#define MakeVector(x, y, z, v) (v)[0]=(x);(v)[1]=(y);(v)[2]=(z)
#define rnd() (Flt)rand() / (Flt)RAND_MAX
//some constants
const Flt PI = 3.141592653589793;

class Global {
public:
	int xres, yres;
	Flt aspectRatio;
	Flt boxRotate;
	Vec camera;
	Vec jet;
	GLfloat lightPosition[4];
        ALuint alSource;
        ALuint alBuffer;
	Global() {


        //Generate a source, and store it in a buffer.
        alGenSources(1, &alSource);
        //alSourcei(alSource, AL_BUFFER, alBuffer);
        //Set volume and pitch to normal, no looping of sound.
        alSourcef(alSource, AL_GAIN, 1.0f);
        alSourcef(alSource, AL_PITCH, 1.0f);
        alSourcei(alSource, AL_LOOPING, AL_TRUE);

		//constructor
		xres = 640;
		yres = 480;
		aspectRatio = (GLfloat)xres / (GLfloat)yres;
		boxRotate = 0.0;
		MakeVector(1.0, 0.1, -1.0, camera);
		MakeVector(12.0, 20.0, 240.0, jet);
		//light is up high, right a little, toward a little
		MakeVector(100.0f, 1240.0f, 40.0f, lightPosition);
		lightPosition[3] = 1.0f;
	}
} g;

//X Windows wrapper class
class X11_wrapper {
private:
	Display *dpy;
	Window win;
	GLXContext glc;
public:
	X11_wrapper() {
	    //Look here for information on XVisualInfo parameters.
	    //http://www.talisman.org/opengl-1.1/Reference/glXChooseVisual.html
	    //
	    GLint att[] = { GLX_RGBA,
			GLX_STENCIL_SIZE, 2,
			GLX_DEPTH_SIZE, 24,
			GLX_DOUBLEBUFFER, None };
	    XSetWindowAttributes swa;
	    setup_screen_res(g.xres, g.yres);
	    dpy = XOpenDisplay(NULL);
	    if (dpy == NULL) {
			printf("\ncannot connect to X server\n\n");
			exit(EXIT_FAILURE);
	    }
	    Window root = DefaultRootWindow(dpy);
	    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	    if (vi == NULL) {
			printf("\nno appropriate visual found\n\n");
			exit(EXIT_FAILURE);
	    }
	    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	    swa.colormap = cmap;
	    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
			StructureNotifyMask | SubstructureNotifyMask;
	    win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	    set_title("4490 OpenGL");
	    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	    glXMakeCurrent(dpy, win, glc);
	}
	~X11_wrapper() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void set_title(const char *str) {
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, str);
	}
	void setup_screen_res(const int w, const int h) {
		g.xres = w;
		g.yres = h;
		g.aspectRatio = (GLfloat)g.xres / (GLfloat)g.yres;
	}
	void reshape_window(int width, int height) {
		//window has been resized.
		setup_screen_res(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
	}
	void check_resize(XEvent *e) {
		//The ConfigureNotify is sent by the server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != g.xres || xce.height != g.yres) {
			//Window size did change.
			reshape_window(xce.width, xce.height);
		}
	}
	bool getXPending() {
		return XPending(dpy);
	}
	void swapBuffers() {
		glXSwapBuffers(dpy, win);
	}
	XEvent getXNextEvent() {
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
} x11;

void init_opengl();
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void render();

int main()
{



	init_opengl();
	int done=0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
	}
	cleanup_fonts();

	/*
        //Cleanup.
        //First delete the source.
        alDeleteSources(1, &alSource);
        //Delete the buffer.
        alDeleteBuffers(1, &alBuffer);
        //Close out OpenAL itself.
        //Get active context.
        ALCcontext *Context = alcGetCurrentContext();
        //Get device for active context.
        ALCdevice *Device = alcGetContextsDevice(Context);
        //Disable context.
        alcMakeContextCurrent(NULL);
        //Release context(s).
        alcDestroyContext(Context);
        //Close device.
        alcCloseDevice(Device);
	*/
	return 0;
}

void init()
{

}

void init_opengl()
{


     //Get started right here.
     
        alutInit(0, NULL);
        if (alGetError() != AL_NO_ERROR) {
                printf("ERROR: alutInit()\n");
                return;
        }
        //Clear error state.
        alGetError();
        //
        //Setup the listener.
        //Forward and up vectors are used.
        float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alListenerfv(AL_ORIENTATION, vec);
        alListenerf(AL_GAIN, 1.0f);
        //
        //Buffer holds the sound information.
        //ALuint alBuffer;
        g.alBuffer = alutCreateBufferFromFile("./737engine.wav");
        //
        //Source refers to the sound.
        //ALuint alSource;
        //Generate a source, and store it in a buffer.
        alGenSources(1, &g.alSource);
        alSourcei(g.alSource, AL_BUFFER, g.alBuffer);
        //Set volume and pitch to normal, no looping of sound.
        alSourcef(g.alSource, AL_GAIN, 1.0f);
        alSourcef(g.alSource, AL_PITCH, 1.0f);
        alSourcei(g.alSource, AL_LOOPING, AL_TRUE);
        if (alGetError() != AL_NO_ERROR) {
                printf("ERROR: setting source\n");
                return;
        }
/*	
        for (int i=0; i<4; i++) {
                alSourcePlay(g.alSource);
                usleep(250000);
        }
*/	



	//OpenGL initialization
	glClearColor(0.5f, 0.6f, 1.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, g.aspectRatio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	//Enable this so material colors are the same as vert colors.
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	//Turn on a light
	glLightfv(GL_LIGHT0, GL_POSITION, g.lightPosition);
	glEnable(GL_LIGHT0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	//
	//Test the stencil buffer on this computer.
	//
	// https://www.opengl.org/discussion_boards/showthread.php/
	// 138452-stencil-buffer-works-on-one-machine-but-not-on-another
	//
	// Before you try using stencil buffer try this:
	// Code :
	// GLint stencilBits = 0;
	// glGetIntegerv(GL_STENCIL_BITS, &amp;stencilBits);
	// if (stencilBits < 1)
	//    MessageBox(NULL,"no stencil buffer.\n","Stencil test", MB_OK);
	GLint stencilBits = 0;
	glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
	if (stencilBits < 1) {
		printf("No stencil buffer on this computer.\n");
		printf("Exiting program.\n");
		exit(0);
	}
}

void check_mouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
	}
}

int check_keys(XEvent *e)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		switch(key) {
			case XK_r:
				MakeVector(12.0, 20.0, 240.0, g.jet);
				break;
			case XK_1:
				break;
			case XK_Escape:
				return 1;
		}
	}
	return 0;
}

void drawBox(float w1, float h1, float d1)
{
	//
	//   1================2
	//   |\              /|
	//   | \            / |
	//   |  \          /  |
	//   |   \        /   |
	//   |    5------6    |
	//   |    |      |    |
	//   |    |      |    |
	//   |    |      |    |
	//   |    4------7    |
	//   |   /        \   |
	//   |  /          \  |
	//   | /            \ |
	//   |/              \|
	//   0================3
	//
	const Flt vert[][3] = {
	-1.0, -1.0,  1.0,
	-1.0,  1.0,  1.0,
	 1.0,  1.0,  1.0,
	 1.0, -1.0,  1.0,
	-1.0, -1.0, -1.0,
	-1.0,  1.0, -1.0,
	 1.0,  1.0, -1.0,
	 1.0, -1.0, -1.0 };
	//left,top,right,bottom,front,back.
	const int face[][4] = {
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	0,4,7,3,
	2,1,0,3,
	4,5,6,7 };
	const Flt norm[][3] = {
	-1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	1.0, 0.0, 0.0,
	0.0,-1.0, 0.0,
	0.0, 0.0, 1.0,
	0.0, 0.0,-1.0 };
	//half the width from center.
	Flt w = w1 * 0.5;
	Flt d = d1 * 0.5;
	Flt h = h1 * 0.5;
	//Normals are required for any lighting.
	glBegin(GL_QUADS);
		for (int i=0; i<6; i++) {
			glNormal3fv(norm[i]);
			for (int j=0; j<4; j++) {
				int k = face[i][j];
				glVertex3f(vert[k][0]*w, vert[k][1]*h, vert[k][2]*d);
			}
		}
	glEnd();
}

void drawGround()
{
	glColor3f(0.5f, 0.4f, 0.1f);
	Flt w = 12500.0 * 0.5;
	Flt d = 12500.0 * 0.5;
	Flt h = 0.0;
	glBegin(GL_QUADS);
		//top
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-w, h,-d);
		glVertex3f( w, h,-d);
		glVertex3f( w, h, d);
		glVertex3f(-w, h, d);
	glEnd();
}

void drawBuildings()
{
	glColor3f(0.2f, 0.7f, 1.0f);
	glPushMatrix();
	glTranslated(-0.5, 1.0, -6.0);
	drawBox(0.5, 2.0, 0.5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(2.5, 1.0, 4.0);
	glColor3ub(0,255,0);
	drawBox(0.5, 1.5, 1.5);
	glPopMatrix();
}

void drawWall()
{
	//
	//   dimensions are 1.0 x 1.0
	//
	//   0--------------1--------2--------------3
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   4--------------5--------6--------------7
	//   |              |        |              |
	//   |              |  hole  |              |
	//   |              |        |              |
	//   |              |        |              |
	//   8--------------9--------10-------------11
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   |              |        |              |
	//   12-------------13-------14-------------15
	//
	//
	//draw a wall with a hole in it.
	float vert[16][3] = {
	-.5, .5, 0,
	-.1, .5, 0,
	 .1, .5, 0,
	 .5, .5, 0,
	-.5, .1, 0,
	-.1, .1, 0,
	 .1, .1, 0,
	 .5, .1, 0,
	-.5,-.1, 0,
	-.1,-.1, 0,
	 .1,-.1, 0,
	 .5,-.1, 0,
	-.5,-.5, 0,
	-.1,-.5, 0,
	 .1,-.5, 0,
	 .5,-.5, 0 };
	const int n = 21;
	int idx[n] = {0,4,1,5,2,6,3,7,6,11,10,15,14,10,13,9,12,8,9,4,5};
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslated(0.0, 0.0, 0.0);
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		for (int i=0; i<n; i++)
	 		glVertex3fv(vert[idx[i]]);
	glEnd();
	glPopMatrix();
}



void playJetSound() {

        //Buffer holds the sound information.
       // ALuint alBuffer;
        g.alBuffer = alutCreateBufferFromFile("./737engine.wav");
        //
        //Source refers to the sound.
       // ALuint alSource;
        //Generate a source, and store it in a buffer.
        alGenSources(1, &g.alSource);
        alSourcei(g.alSource, AL_BUFFER, g.alBuffer);
        //Set volume and pitch to normal, no looping of sound.
        alSourcef(g.alSource, AL_GAIN, 1.0f);
        alSourcef(g.alSource, AL_PITCH, 1.0f);
        alSourcei(g.alSource, AL_LOOPING, AL_FALSE);


	float p[3];
	p[0] = (float)g.jet[0] * 0.1f;
	p[1] = (float)g.jet[1] * 0.1f;
	p[2] = (float)g.jet[2] * 0.1f;

	alSourcefv(g.alSource, AL_POSITION, p);
//	alSourcefv(alSource, AL_POSITION, p);
	//alSourcePlay(alSource);

        if (alGetError() != AL_NO_ERROR) {
              //  printf("ERROR: setting source\n");
                return;
        }
        for (int i=0; i<4; i++) {

                alSourcePlay(g.alSource);
                usleep(250000);
        }



		
        //Cleanup.
        //First delete the source.
        //alDeleteSources(1, &g.alSource);
        //Delete the buffer.
        //alDeleteBuffers(1, &alBuffer);
        //Close out OpenAL itself.
        //Get active context.
        //ALCcontext *Context = alcGetCurrentContext();
        //Get device for active context.
        //ALCdevice *Device = alcGetContextsDevice(Context);
        //Disable context.
        //alcMakeContextCurrent(NULL);
        //Release context(s).
        //alcDestroyContext(Context);
        //Close device.
        //alcCloseDevice(Device);
	
	

}

void drawJet()
{
	glColor3ub(255,0,0);
	glPushMatrix();
	glTranslatef(g.jet[0], g.jet[1], g.jet[2]);
	glScalef(0.5, 0.0, 1.5);
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, -1.0f, 0.0f);
		//nose
 		glVertex3f(-1.0, 0.0, -0.4);
 		glVertex3f( 0.0, 0.0, -2.0);
 		glVertex3f( 1.0, 0.0, -0.4);
		//wing 
		glVertex3f(-4.0, 0.0,  0.0);
 		glVertex3f( 0.0, 0.0, -1.2);
 		glVertex3f( 4.0, 0.0,  0.0);
		//fuselage
		glVertex3f(-1.0, 0.0, -0.6);
 		glVertex3f( 0.0, 0.0,  2.0);
 		glVertex3f( 1.0, 0.0, -0.6);
		//tail
		glVertex3f(-1.5, 0.0, 2.0);
 		glVertex3f( 0.0, 0.0, 1.5);
 		glVertex3f( 1.5, 0.0, 2.0);
	glEnd();
	glPopMatrix();


	/*
	//clear error state
	alGetError();
	//exaggerate pitch
	alDopplerVelocity(16);
	if (alGetError() != AL_NO_ERROR) {
		errorflag++;
	}
	//
	vec[0] = 0.0;
	*/

}

void physics()
{
	g.jet[2] -= 1.0;


	float p[3];
	p[0] = (float)g.jet[0] * 0.1f;
	p[1] = (float)g.jet[1] * 0.1f;
	p[2] = (float)g.jet[2] * 0.1f;

	alSourcefv(g.alSource, AL_POSITION, p);
	alSourcefv(g.alSource, AL_POSITION, p);
	alSourcePlay(g.alSource);

	/*
        if (alGetError() != AL_NO_ERROR) {
              //  printf("ERROR: setting source\n");
                return;
        }
        for (int i=0; i<4; i++) {

                alSourcePlay(g.alSource);
                usleep(250000);
        }
	*/




//	playJetSound();


}

void render()
{
	//Clear the depth buffer and screen.
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//
	//Render a 3D scene
	//
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	gluPerspective(45.0f, g.aspectRatio, 0.1f, 10000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		g.camera[0], g.camera[1], g.camera[2],
		g.jet[0], g.jet[1], g.jet[2],
		0,1,0);
	glLightfv(GL_LIGHT0, GL_POSITION, g.lightPosition);
	//
	drawGround();
	drawBuildings();
	drawWall();
	//sound called in draw Jet
	drawJet();

	//
	//playJetSound();
	//
	//
	//switch to 2D mode
	//
	Rect r;
	glViewport(0, 0, g.xres, g.yres);
	glMatrixMode(GL_MODELVIEW);   glLoadIdentity();
	glMatrixMode (GL_PROJECTION); glLoadIdentity();
	gluOrtho2D(0, g.xres, 0, g.yres);
	glDisable(GL_LIGHTING);
	r.bot = g.yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00000000, "Jet flyby with doppler effect");
}



