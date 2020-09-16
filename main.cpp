// An OpenGL Keyboard and Mouse Interaction Program

#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <array>
#include <math.h>
#include "Vec2.h"

using namespace std;
// These are defined in a global scope

vector<Vec2> points;
bool polygonDrawn = false;
enum Mode {OUTLINE, TESSELATION, BAD_FILL, GOOD_FILL};
Mode currMode = OUTLINE;
vector< array<Vec2, 3> > triangles;

// Specity the values to place and size the window on the screen

const int WINDOW_POSITION_X = 100;
const int WINDOW_POSITION_Y = 100;
const int WINDOW_MAX_X = 800;
const int WINDOW_MAX_Y = 800;

// Specify the coordinate ranges for the world coordinates in the 2D Frame

const float WORLD_COORDINATE_MIN_X = 0.0;
const float WORLD_COORDINATE_MAX_X = WINDOW_MAX_X;
const float WORLD_COORDINATE_MIN_Y = 0.0;
const float WORLD_COORDINATE_MAX_Y = WINDOW_MAX_Y;


//determinate of 2x2 matrix
float det(float a, float b, float c, float d)
{
  /*
    | a b |
    | c d |
  */
  return a*d - b *c;
}

//returns true if two line segments intersect
bool intersect(Vec2 startPoint1, Vec2 endPoint1, Vec2 startPoint2, Vec2 endPoint2)
{
  float den = det(endPoint1.X - startPoint1.X, -(endPoint2.X - startPoint2.X), endPoint1.Y - startPoint1.Y,  -(endPoint2.Y - startPoint2.Y));
  if(den == 0) //parallel line segments
    return false;

  float u_a = det(startPoint2.X - startPoint1.X, -(endPoint2.X - startPoint2.X), startPoint2.Y - startPoint1.Y, -(endPoint2.Y - startPoint2.Y)) / den;
  float u_b = det(endPoint1.X - startPoint1.X, startPoint2.X - startPoint1.X, endPoint1.Y - startPoint1.Y, startPoint2.Y - startPoint1.Y) / den;

  return u_a > 0 && u_a < 1 && u_b > 0 && u_b < 1;
}

void myglutInit( int argc, char** argv )
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); /* default, not needed */
    glutInitWindowSize(WINDOW_MAX_X,WINDOW_MAX_Y); /* set pixel window */
    glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y); /* place window top left on display */
    glutCreateWindow("Polygon Tesselation"); /* window title */
}


void myInit(void)
{

/* standard OpenGL attributes */

      glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */

/* set up viewing window with origin lower left */

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(WORLD_COORDINATE_MIN_X, WORLD_COORDINATE_MAX_X,
                 WORLD_COORDINATE_MIN_Y, WORLD_COORDINATE_MAX_Y);
      glMatrixMode(GL_MODELVIEW);
}

void randomizeColor()
{
  //sin^2 returns a value between 0 and 1
  //we need to offset each color by a third of a revolution around a circle, so it's not just grays
  float r = 2 * M_PI * rand() / RAND_MAX; //random float between 0 and 2 pi
  float offset = 2 * M_PI / 3;
  float red = sin(r) * sin(r);
  float green = sin(r + offset) * sin(r + offset);
  float blue = sin(r + 2 * offset) * sin(r + 2 * offset);
  glColor3f(red, green, blue);
}

void drawOutline()
{
  glBegin(GL_LINES);
      int n = points.size();
      for(int i =0; i < n; i++)
      {
        randomizeColor();
        if( i == 0)
        {
          glVertex2f(points[i].X, points[i].Y);
          glVertex2f(points[n-1].X, points[n-1].Y);
        }
        else
        {
          glVertex2f(points[i].X, points[i].Y);
          glVertex2f(points[i-1].X, points[i-1].Y);
        }
      }

  glEnd();
}

//returns true if the points are defined in a clockwise manner
bool isClockwise(vector<Vec2> v)
{
  float sum = 0;
  int n = v.size();
  for(int i = 0; i < n; i++)
  {
    sum += (v[(i+1)%n].X - v[i].X) * (v[(i+1)%n].Y + v[i].Y);
  }
  return sum > 0;
}

//returns true if the diagonal line segment intersects any other line segments
bool diagonalIntersect(vector<Vec2> local_points, int index)
{
  int n = local_points.size();
  for(int i = 1; i < n; i++)
  {
    if(intersect(local_points[index], local_points[(index+2)%n], local_points[i], local_points[i-1]))
    {
      return true;
    }
  }
  return false;
}

bool validTriangle(vector<Vec2> local_points, int index, float & winding)
{
  int n = local_points.size();

  //check that it has a ccw winding
  Vec2 line1 = local_points[index] - local_points[(index+1)%n];
  Vec2 line2 = local_points[(index+2)%n] - local_points[(index+1)%n];
  winding = line1.winding(line2);
  if (winding >= 0)
  {
    return false;
  }

  //check that the diagonal does not intersect anything
  if(diagonalIntersect(local_points, index))
  {
    return false;
  }

  //check the special case where it trys to draws a line that is outside the polygon
  Vec2 nextLine = local_points[(index+3)%n] - local_points[(index+2)%n];
  Vec2 imminentLine = local_points[index] - local_points[(index + 2)%n];
  if(imminentLine.angleBetween(line2) < nextLine.angleBetween(line2))
  {
    return false;
  }

  return true;
}

//returns the triangle list
vector< array<Vec2, 3> > tesselate()
{
  vector< array<Vec2, 3> > triangles;

  vector<Vec2> local_points = points; //we need a local copy because we don't want to destroy our points list

  if(isClockwise(local_points)) //if the points are not defined in a CCW manner, then reverse them
    reverse(local_points.begin(), local_points.end());

  int n = local_points.size();
  while(n > 3)
  {

    for(int i = 0; i < n; i++)
    {
      float winding;
      if(validTriangle(local_points, i, winding)) //ccw winding and the diagonal does not intersect any line segments
      {
          array<Vec2, 3> triangle = {local_points[i], local_points[(i+1)%n], local_points[(i+2)%n]};
          triangles.push_back(triangle);

          //remove middle point
          local_points.erase(local_points.begin() + (i + 1)%n);
          n--;
          break;
      }
      else if(winding == 0)
      {
          printf("winding = 0\n");
          local_points.erase(local_points.begin() + (i + 1)%n);
          n--;
          break;
      }
      if(i == n-1)
      {
        printf("stuck :(\n");
      }
    }
  }
  array<Vec2, 3> finalTriangle = {local_points[0], local_points[1], local_points[2]};
  triangles.push_back(finalTriangle);

  return triangles;

}

void drawTesselation()
{
  if(triangles.empty())
    triangles = tesselate();

  for(int i = 0; i < triangles.size(); i++)
  {
    randomizeColor();
    glBegin(GL_LINES);
        glVertex2f(triangles[i][0].X, triangles[i][0].Y);
        glVertex2f(triangles[i][1].X, triangles[i][1].Y);

        glVertex2f(triangles[i][1].X, triangles[i][1].Y);
        glVertex2f(triangles[i][2].X, triangles[i][2].Y);

        glVertex2f(triangles[i][2].X, triangles[i][2].Y);
        glVertex2f(triangles[i][0].X, triangles[i][0].Y);
    glEnd();
  }
}

void drawGoodFill()
{
  if(triangles.empty())
    triangles = tesselate();

  for (int i = 0; i < triangles.size(); i++)
  {
    randomizeColor();
    glBegin(GL_POLYGON);
        glVertex2f(triangles[i][0].X, triangles[i][0].Y);
        glVertex2f(triangles[i][1].X, triangles[i][1].Y);
        glVertex2f(triangles[i][2].X, triangles[i][2].Y);
    glEnd();
  }
}

void drawBadFill()
{
  randomizeColor();
  glBegin(GL_POLYGON);
      int n = points.size();
      for(int i =0; i < n; i++)
      {
        if( i == 0)
        {
          glVertex2f(points[i].X, points[i].Y);
          glVertex2f(points[n-1].X, points[n-1].Y);
        }
        else
        {
          glVertex2f(points[i].X, points[i].Y);
          glVertex2f(points[i-1].X, points[i-1].Y);
        }
      }

  glEnd();
}

void display( void )
{
    glClear(GL_COLOR_BUFFER_BIT);  /*clear the window */

    switch(currMode)
    {
      case OUTLINE:
        drawOutline();
        break;
      case TESSELATION:
        drawTesselation();
        break;
      case GOOD_FILL: //fill in all triangles created in tesselations
        drawGoodFill();
        break;
      case BAD_FILL: //fill in one big polygon
        drawBadFill();
        break;
    }

    glFlush();
 }

//returns true if it is successful
bool addPoint( int x, int y, bool isLast = false)
{
    Vec2 p(x,y);

    if(points.size() == 0)
    {
      points.push_back(p);
      return true;
    }

    Vec2 b = points.back();

    //check that this new line segment p->b does not intersect any other line segments
    for(int i = 1; i < points.size(); i++)
    {
      if(intersect(p, b, points[i], points[i-1]))
      {
        printf ("(%d,%d) is an invalid point because it causes an intersection \n", x, y);
        return false;
      }
      //if it's the last point we must also make sure the line segment from the first to last point does not intersect anything
      if(isLast && intersect(p, points[0], points[i], points[i-1]))
      {
        printf ("(%d,%d) is an invalid point because it causes an intersection \n", x, y);
        return false;
      }

    }
    printf ("(%d,%d)\n", x, y);

    randomizeColor();
    glBegin(GL_LINES);
        glVertex2f(p.X, p.Y);
        glVertex2f(b.X, b.Y);
    glEnd();
    glFlush();


    points.push_back(p);
    return true;
}


void mouse( int button, int state, int x, int y )
{
  if(state == GLUT_DOWN && currMode != OUTLINE)
  {
      printf("To draw you must be in outline mode.\n");
      printf("Press l to enter outline mode.\n");
      return;
  }

  if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
     {
       if(polygonDrawn)
       {
         points.clear();
         triangles.clear();
         polygonDrawn = false;
         glutPostRedisplay();
       }
       addPoint( x, WINDOW_MAX_Y - y);
     }

  if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
     {
        if(addPoint(x, WINDOW_MAX_Y - y, true))
        {
          polygonDrawn = true;
          glutPostRedisplay();
        }
     }
}


void keyboard( unsigned char key, int x, int y )
{
  if ( key == 'q' || key == 'Q')
    exit(0);
  else if ( key == 'f' || key == 'F')
  {
    if(polygonDrawn)
    {
      currMode = BAD_FILL;
      glutPostRedisplay();
    }
    else
    {
      printf("Unable to fill polygon because it has not been completely drawn.\n");
      printf("Please left click to draw the final point.\n");
    }
  }
  else if ( key == 't' || key == 'T')
  {
    if(polygonDrawn)
    {
      currMode = TESSELATION;
      glutPostRedisplay();
    }
    else
    {
      printf("Unable to tesselate polygon because it has not been completely drawn.\n");
      printf("Please left click to draw the final point.\n");
    }
  }
  else if ( key == 'p' || key == 'P')
  {
    if(polygonDrawn)
    {
      currMode = GOOD_FILL;
      glutPostRedisplay();
    }
    else
    {
      printf("Unable to fill polygon because it has not been completely drawn.\n");
      printf("Please left click to draw the final point.\n");
    }
  }
  else if ( key == 'l' || key == 'L')
  {
    if(currMode != OUTLINE)
    {
      currMode = OUTLINE;
      glutPostRedisplay();
    }
  }
}


int main(int argc, char** argv)
{
    myglutInit(argc,argv); /* Set up Window */
    myInit(); /* set attributes */

    // Now start the standard OpenGL glut callbacks //

    glutMouseFunc(mouse);  /* Define Mouse Handler */
    glutKeyboardFunc(keyboard); /* Define Keyboard Handler */
    glutDisplayFunc(display); /* Display callback invoked when window opened */
    glutMainLoop(); /* enter event loop */
}
