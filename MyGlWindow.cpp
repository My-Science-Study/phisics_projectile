

#include "MyGlWindow.h"



#include <iostream>
#include "drawUtils.h"


#include "timing.h"


static double DEFAULT_VIEW_POINT[3] = {30, 30, 30};
static double DEFAULT_VIEW_CENTER[3] = {0, 0, 0};
static double DEFAULT_UP_VECTOR[3] = {0, 1, 0};

MyGlWindow::MyGlWindow(int x, int y, int w, int h) :
  Fl_Gl_Window(x,y,w,h)
//==========================================================================
{
    
    mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );
 	
	fieldOfView = 45;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);

	m_mover = new Mover();

	TimingData::init();
	run = 0;
	test0 = 0;
}


  void MyGlWindow::setupLight(float x, float y, float z)
  {
	  
	  // set up the lighting
	  GLfloat lightPosition[] = {500, 900.0, 500, 1.0};
	  GLfloat lightPosition2[] = {1, 0, 0, 0};
	  GLfloat lightPosition3[] = {0, -1, 0, 0};

	  GLfloat violetLight[] = {0.5f, 0.1f, .5f, 1.0};
	  GLfloat whiteLight[] = {1, 1, 1, 1.0};
	  GLfloat whiteLight2[] = { .3, .3, .3, 1.0 };
	  GLfloat blueLight[] = {.1f,.1f,.3f,1.0};


	  glEnable(GL_COLOR_MATERIAL);
	  glEnable(GL_DEPTH_TEST);
	  glEnable(GL_LIGHTING);
	  glEnable(GL_LIGHT0);
	  glEnable(GL_LIGHT1);
	  glEnable(GL_LIGHT2);
	  glEnable(GL_DEPTH);

	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	  lightPosition[0] = x;
	  lightPosition[1] = y;
      lightPosition[2] = z;

	  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	  glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

 	  glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
 	  glLightfv(GL_LIGHT1, GL_AMBIENT, whiteLight2);
// 
	  glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	  glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLight);  

	  	
	  glEnable (GL_BLEND); 
	  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  }





void MyGlWindow::drawStuff()
{
   polygonf( 4, 20., 0.,-25.,  20., 0., 25.,  20., 30., 25.,  20., 30., -25.);
}

//==========================================================================
void MyGlWindow::draw()
//==========================================================================
{

  glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
  glClearColor(0.2f,0.2f,.2f,0);		// background should be blue


  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  
  
  // now draw the ground plane
  setProjection();
  setupFloor();

  glPushMatrix();
	drawFloor(200,20);
  glPopMatrix();


  setupLight(m_viewer->getViewPoint().x,m_viewer->getViewPoint().y,m_viewer->getViewPoint().z);

    
 // Add a sphere to the scene.
//Draw axises
  glLineWidth(3.0f);
  glBegin(GL_LINES);
  glColor3f(1,0,0);

  glVertex3f(0,0.1,0);
  glVertex3f(0,100,0);

  glColor3f(0,1,0);

  glVertex3f(0,0.1,0);
  glVertex3f(100,0.1,0);

  glColor3f(0,0,1);

  glVertex3f(0,0.1,0);
  glVertex3f(0,0.1,100);
  glEnd();
  glLineWidth(1.0f);




  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  //draw shadow
  setupShadows();
  if (m_mover) m_mover->draw(1);
   unsetupShadows();


  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);

  //draw objects
  glPushMatrix();
  if (m_mover) m_mover->draw(0);
  glPopMatrix();

  

 glEnable(GL_COLOR_MATERIAL);

}

void MyGlWindow::test()
{
	m_mover = new Mover();
	float duration = (float)TimingData::get().lastFrameDuration * 0.003;
	m_mover->update(duration);
}

void MyGlWindow::update()
{

	TimingData::get().update();

	if (!run) return;
	
	float duration = (float)TimingData::get().lastFrameDuration * 0.003;

	if (test0 = 0) {
		test();
	}

	if (m_mover)
		m_mover->update(duration);
}


void MyGlWindow::doPick()
{
	make_current();		// since we'll need to do some GL stuff

	int mx = Fl::event_x(); // where is the mouse?
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my), 5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	//for (size_t i = 0; i < world->points.size(); ++i) {
	//	glLoadName((GLuint)(i + 1));
	//	draw();
	//}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		//selectedCube = buf[3] - 1;
	}
	else {// nothing hit, nothing selected
		//selectedCube = -1;
	}
	//printf("Selected Cube %d\n", selectedCube);
}



void MyGlWindow::setProjection(int clearProjection)
//==========================================================================
{
  glMatrixMode(GL_PROJECTION);
  glViewport(0,0,w(),h());
  if (clearProjection)
	glLoadIdentity();
  // compute the aspect ratio so we don't distort things
  double aspect = ((double) w()) / ((double) h());
  gluPerspective(fieldOfView, aspect, 1, 1000);

  // put the camera where we want it to be
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // use the transformation in the ArcBall

  gluLookAt( 
	  m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z, 
	  m_viewer->getViewCenter().x, m_viewer->getViewCenter().y, m_viewer->getViewCenter().z, 
	  m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z
	  );
  
  glDisable (GL_BLEND); 
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e)
//==========================================================================
{

  switch(e) {
  case FL_SHOW:		// you must handle this, or not be seen!
	  show();
	  return 1;
  case FL_PUSH:
	  {
		 
		  m_pressedMouseButton = Fl::event_button();
		  m_lastMouseX = Fl::event_x();
		  m_lastMouseY = Fl::event_y();

		 
	  }
	  damage(1);
	  return 1;
  case FL_RELEASE:
	  m_pressedMouseButton = -1;
	  damage(1);
	  return 1;
  case FL_DRAG: // if the user drags the mouse
	  {

		  double fractionChangeX = static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
		  double fractionChangeY = static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

		  if( m_pressedMouseButton == 1 ) {
			  m_viewer->rotate( fractionChangeX, fractionChangeY );
		  } else if( m_pressedMouseButton == 2 ) {
			  m_viewer->zoom( fractionChangeY );
		  } else if( m_pressedMouseButton == 3 ) {
			  m_viewer->translate( -fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
		  } else {
			  std::cout << "Warning: dragging with unknown mouse button!  Nothing will be done" << std::endl;
		  }

		  m_lastMouseX = Fl::event_x();
		  m_lastMouseY = Fl::event_y();
		  redraw();
	  }
	 
	  return 1;
  
  case FL_KEYBOARD:
	  return 0;

  default:
	return 0;
  }
}


//
// get the mouse in NDC
//==========================================================================
void MyGlWindow::getMouseNDC(float& x, float& y)
//==========================================================================
{
	// notice, we put everything into doubles so we can do the math
	  float mx = (float) Fl::event_x();	// remeber where the mouse went down
	  float my = (float) Fl::event_y();

	  // we will assume that the viewport is the same as the window size
	  float wd = (float) w();
	  float hd = (float) h();

	  // remember that FlTk has Y going the wrong way!
	  my = hd-my;
	
	  x = (mx / wd) * 2.0f - 1.f;
	  y = (my / hd) * 2.0f - 1.f;
}

