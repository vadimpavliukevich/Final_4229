/*
 *  Vadim Pavliukevich
 *  CSCI 4229/5229
 *  Final Project
 *
 *  Key bindings:
 *  l/L          Toggle lighting visulization
 *  s/S        Start/stop light movement
 *  -/+        Decrease/increase light elevation
 *  arrows     Change view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"

int ntex = 1;                     //  Number of textures
int move = 1;                     //  Move light
int axes = 0;                     //  Display axes
int mode = 2;                     //  Projection mode
int th = 0;                       //  Azimuth of view angle
int ph = 0;                       //  Elevation of view angle
int fov = 55;                     //  Field of view (for perspective)
double asp = 1;                   //  Aspect ratio
double dim = 2;                   //  Size of world
float angle = 0.0;                //  Rotation angle
float global_x = 0, global_z = 2; //  Global x and z camera coordinates
float lx = 0.0, lz = -1;          // actual vector representing the camera's direction
int emission = 0;                 // Emission intensity (%)
float shiny = 0.5;                // Shininess (value)
int zh = 90;                      //  Light azimuth
float Ylight = 0.8;               //  Light elevation
float Zlight = 0.4;               //  Light distance
int light = 1;                    //  Light animation
unsigned int object[6];           //  Object
unsigned int texture[5];          //  Texture

//  Light colors
const float Emission[] = {0.0, 0.0, 0.0, 1.0};
const float Ambient[] = {0.3, 0.3, 0.3, 1.0};
const float Diffuse[] = {1.0, 1.0, 1.0, 1.0};
const float Specular[] = {1.0, 1.0, 1.0, 1.0};
const float Shinyness[] = {16};
//  Set lighting parameters using uniforms
float Position[4];

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th, double ph)
{
   glNormal3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
   glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}
/*  From class example
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */

// Set up all objs
void objSetup(int index, double scale, double x, double y, double z, double r, double g, double b)
{
   glPushMatrix();
   glScaled(scale, scale, scale);
   glTranslated(x, y, z);
   if (r != -1)
   {
      glColor3f(r, g, b);
   }
   glCallList(object[index]);
   glPopMatrix();
}

static void texturedCube(double x, double y, double z,
                         double dx, double dy, double dz,
                         double th, float r, float g, float b, int index, float txA, float txB, float tyA, float tyB)
{
   //  Set specular color to white
   float white[] = {0.01, 0.01, 0.01, 0.01};
   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x, y, z);
   glRotated(th, 0, 1, th);
   glScaled(dx, dy, dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glColor3f(1, 1, 1);
   glBindTexture(GL_TEXTURE_2D, texture[index]);
   //  Front
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(txA, tyA);
   glVertex3f(-1, -1, 1);
   glTexCoord2f(txB, tyA);
   glVertex3f(+1, -1, 1);
   glTexCoord2f(txB, tyB);
   glVertex3f(+1, +1, 1);
   glTexCoord2f(txA, tyB);
   glVertex3f(-1, +1, 1);
   glEnd();
   //  Back
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glVertex3f(+1, -1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, +1, -1);
   glVertex3f(+1, +1, -1);
   glEnd();
   //  Right
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1, -1, +1);
   glVertex3f(+1, -1, -1);
   glVertex3f(+1, +1, -1);
   glVertex3f(+1, +1, +1);
   glEnd();
   //  Left
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1, +1);
   glVertex3f(-1, +1, +1);
   glVertex3f(-1, +1, -1);
   glEnd();
   //  Top
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, +1, 0);
   glVertex3f(-1, +1, +1);
   glVertex3f(+1, +1, +1);
   glVertex3f(+1, +1, -1);
   glVertex3f(-1, +1, -1);
   glEnd();
   //  Bottom
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glVertex3f(-1, -1, -1);
   glVertex3f(+1, -1, -1);
   glVertex3f(+1, -1, +1);
   glVertex3f(-1, -1, +1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

static void cube(double x, double y, double z,
                 double dx, double dy, double dz,
                 double th, float r, float g, float b)
{
   //  Set specular color to white
   float white[] = {0.01, 0.01, 0.01, 0.01};
   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x, y, z);
   glRotated(th, 0, 1, th);
   glScaled(dx, dy, dz);

   //  Front
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glVertex3f(-1, -1, 1);
   glVertex3f(+1, -1, 1);
   glVertex3f(+1, +1, 1);
   glVertex3f(-1, +1, 1);
   glEnd();
   //  Back
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glVertex3f(+1, -1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, +1, -1);
   glVertex3f(+1, +1, -1);
   glEnd();
   //  Right
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1, -1, +1);
   glVertex3f(+1, -1, -1);
   glVertex3f(+1, +1, -1);
   glVertex3f(+1, +1, +1);
   glEnd();
   //  Left
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1, +1);
   glVertex3f(-1, +1, +1);
   glVertex3f(-1, +1, -1);
   glEnd();
   //  Top
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, +1, 0);
   glVertex3f(-1, +1, +1);
   glVertex3f(+1, +1, +1);
   glVertex3f(+1, +1, -1);
   glVertex3f(-1, +1, -1);
   glEnd();
   //  Bottom
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glVertex3f(-1, -1, -1);
   glVertex3f(+1, -1, -1);
   glVertex3f(+1, -1, +1);
   glVertex3f(-1, -1, +1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
}

// Cube with no bottom
static void badCube(double x, double y, double z,
                    double dx, double dy, double dz,
                    double th, float r, float g, float b)
{
   //  Set specular color to white
   float white[] = {0.01, 0.01, 0.01, 0.01};
   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x, y, z);
   glRotated(th, 0, 1, th);
   glScaled(dx, dy, dz);

   //  Front
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glVertex3f(-1, -1, 1);
   glVertex3f(+1, -1, 1);
   glVertex3f(+1, +1, 1);
   glVertex3f(-1, +1, 1);
   glEnd();
   //  Back
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glVertex3f(+1, -1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, +1, -1);
   glVertex3f(+1, +1, -1);
   glEnd();
   //  Right
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1, -1, +1);
   glVertex3f(+1, -1, -1);
   glVertex3f(+1, +1, -1);
   glVertex3f(+1, +1, +1);
   glEnd();
   //  Left
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1, +1);
   glVertex3f(-1, +1, +1);
   glVertex3f(-1, +1, -1);
   glEnd();
   //  Top
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
   glNormal3f(0, +1, 0);
   glVertex3f(-1, +1, +1);
   glVertex3f(+1, +1, +1);
   glVertex3f(+1, +1, -1);
   glVertex3f(-1, +1, -1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

/*  From class example
 *  Draw a sphere (version 1)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x, double y, double z, double r, float r_, float g_, float b_)
{
   const int d = 15;
   glColor3f(r_, g_, b_);
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x, y, z);
   glScaled(r, r, r);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0, -90);
   for (int th = 0; th <= 360; th += d)
   {
      Vertex(th, d - 90);
   }
   glEnd();

   //  Latitude bands
   for (int ph = d - 90; ph <= 90 - 2 * d; ph += d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += d)
      {
         Vertex(th, ph);
         Vertex(th, ph + d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0, 90);
   for (int th = 0; th <= 360; th += d)
   {
      Vertex(th, 90 - d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

// https://community.khronos.org/t/half-sphere/49408/3 took halfSphere vertex implementation from here for the legs
void halfSphere(int dy, int dx, double x, double y, double z, GLfloat r, double r_, double g_, double b_)
{
   glPushMatrix();
   glColor3f(r_, g_, b_);
   glRotated(180, 1, 0, 0);
   glTranslated(x, y, z);
   int i, j;
   GLfloat v[dx * dy][3];
   for (i = 0; i < dx; ++i)
   {
      for (j = 0; j < dy; ++j)
      {
         v[i * dy + j][0] = r * cos(j * 2 * M_PI / dy) * cos(i * M_PI / (2 * dx));
         v[i * dy + j][1] = r * sin(i * M_PI / (2 * dx));
         v[i * dy + j][2] = r * sin(j * 2 * M_PI / dy) * cos(i * M_PI / (2 * dx));
      }
   }
   glBegin(GL_QUADS);
   for (i = 0; i < dx - 1; ++i)
   {
      for (j = 0; j < dy; ++j)
      {
         glVertex3fv(v[i * dy + j]);
         glVertex3fv(v[i * dy + (j + 1) % dy]);
         glVertex3fv(v[(i + 1) * dy + (j + 1) % dy]);
         glVertex3fv(v[(i + 1) * dy + j]);
      }
   }
   glEnd();
   glPopMatrix();
}

// Calculate normals
/*normal[0] = (x1-x2)*(x2-x3);
   normal[1] = (y1-y2)*(y2-y3);
   normal[2] = (z1-z2)*(z2-z3);*/
void tringleNormal(double x1, double x2, double x3, double y1, double y2, double y3, double z1, double z2, double z3, double *normal)
{
   double edge1[3], edge2[3];
   // Calculate the two edges
   edge1[0] = x2 - x1;
   edge1[1] = y2 - y1;
   edge1[2] = z2 - z1;
   edge2[0] = x3 - x1;
   edge2[1] = y3 - y1;
   edge2[2] = z3 - z1;
   // Calculate the cross product
   normal[0] = edge1[1] * edge2[2] - edge1[2] * edge2[1];
   normal[1] = edge1[2] * edge2[0] - edge1[0] * edge2[2];
   normal[2] = edge1[0] * edge2[1] - edge1[1] * edge2[0];
   // Normalize the result
   float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
   normal[0] = normal[0] / length;
   normal[1] = normal[1] / length;
   normal[2] = normal[2] / length;
}

// Table with goods
void table()
{
   // Save transformation
   glPushMatrix();
   // table top
   cube(-1.05, 0.35, 0.2, 0.35, 0.015, 0.15, 0, 0.7, 0.7, 0.7);
   cube(-1.24, 0.35, -0.3, 0.16, 0.015, 0.4, 0, 0.7, 0.7, 0.7);
   cube(-0.8, 0.35, -0.85, 0.6, 0.015, 0.15, 0, 0.7, 0.7, 0.7);
   // table legs
   cube(-0.72, 0.17, 0.32, 0.018, 0.168, 0.018, 0, 0.6, 0.48, 0.31);
   cube(-0.72, 0.17, 0.08, 0.018, 0.168, 0.018, 0, 0.6, 0.48, 0.31);
   // table base
   cube(-1.06, 0.32, 0.32, 0.35, 0.018, 0.018, 0, 0.6, 0.48, 0.31);
   cube(-1.06, 0.32, 0.08, 0.35, 0.018, 0.018, 0, 0.6, 0.48, 0.31);
   // drawers
   cube(-1.25, 0.17, -0.3, 0.158, 0.168, 0.4, 0, 0.04, 0.21, 0.24);
   cube(-0.805, 0.167, -0.84, 0.6, 0.168, 0.148, 0, 0.04, 0.21, 0.24);
   // drawer handles back
   // left
   cube(-0.96, 0.17, -0.7, 0.08, 0.13, 0.01, 0, 0.29, 0.50, 0.56);
   cube(-0.78, 0.17, -0.7, 0.08, 0.13, 0.01, 0, 0.29, 0.50, 0.56);
   // right
   cube(-0.52, 0.17, -0.7, 0.08, 0.13, 0.01, 0, 0.29, 0.50, 0.56);
   cube(-0.34, 0.17, -0.7, 0.08, 0.13, 0.01, 0, 0.29, 0.50, 0.56);
   // drawer handles front
   // left
   cube(-1.092, 0.17, -0.02, 0.01, 0.13, 0.08, 0, 0.29, 0.50, 0.56);
   cube(-1.092, 0.17, -0.2, 0.01, 0.13, 0.08, 0, 0.29, 0.50, 0.56);
   // right
   cube(-1.092, 0.17, -0.42, 0.01, 0.13, 0.08, 0, 0.29, 0.50, 0.56);
   cube(-1.092, 0.17, -0.6, 0.01, 0.13, 0.08, 0, 0.29, 0.50, 0.56);
   // box
   cube(-1.26, 0.47, -0.76, 0.1, 0.12, 0.14, 0, 0.65, 0.79, 0.57);
   cube(-1.26, 0.59, -0.76, 0.11, 0.01, 0.15, 0, 0.67, 0.80, 0.57);
   // safe
   texturedCube(-0.86, 0.43, -0.822, 0.14, 0.08, 0.08, 0, 0.44, 0.65, 0.49, 2, 0.3, 0.7, 0.3, 0.7);
   // more boxes
   cube(-1.26, 0.45, -0.4, 0.1, 0.1, 0.13, 0, 0.57, 0.6, 0.79);
   texturedCube(-1.22, 0.48, -0.06, 0.12, 0.115, 0.13, 0, 0.6, 0.48, 0.31, 4, 0.25, 0.75, 0, 1);
   // helmet thing
   objSetup(1, 0.08, -10, 4.7, 3, 0.92, 0.73, 0.73);
   // far colbs
   objSetup(2, 0.015, -30, 31, -55, 0.5, 0.7, 0.6);
   objSetup(2, 0.015, -40, 31, -52, 0.7, 0.4, 0.6);
   objSetup(2, 0.015, -35, 31, -57, 0.97, 0.98, 0.53);
   // near colbs
   objSetup(2, 0.015, -86, 31, 16, 0.87, 0.71, 0.41);
   objSetup(2, 0.015, -66, 31, 18, 0.58, 0.41, 0.87);
   // big colb
   objSetup(2, 0.022, -57, 34, -35, 0.94, 0.86, 0.86);
   glPopMatrix();
}

// Lamps with light source
void bigLamp(double x, double y, double z, float r, float g, float b)
{
   double normals[3] = {0, 0, 0};
   //  Save transformation
   glPushMatrix();
   glScaled(0.9, 0.9, 0.9);
   //  Cube
   badCube(x, y, z, 0.6, 0.05, 0.1, 0, r, g, b);
   glColor3f(r, g, b);

   tringleNormal(x + 0.6, x - 0.6, x - 0.5, y + 0.05, y + 0.05, y + 0.15, z + 0.1, z + 0.1, z + 0.05, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x + 0.6, y + 0.05, z + 0.1);
   glVertex3f(x - 0.6, y + 0.05, z + 0.1);
   glVertex3f(x - 0.5, y + 0.15, z + 0.05);
   glVertex3f(x + 0.5, y + 0.15, z + 0.05);
   glEnd();

   tringleNormal(x + 0.6, x - 0.6, x - 0.5, y + 0.05, y + 0.05, y + 0.15, z - 0.1, z - 0.1, z - 0.05, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x + 0.6, y + 0.05, z - 0.1);
   glVertex3f(x - 0.6, y + 0.05, z - 0.1);
   glVertex3f(x - 0.5, y + 0.15, z - 0.05);
   glVertex3f(x + 0.5, y + 0.15, z - 0.05);
   glEnd();

   tringleNormal(x + 0.6, x + 0.5, x + 0.5, y + 0.05, y + 0.15, y + 0.15, z + 0.1, z + 0.05, z - 0.05, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x + 0.6, y + 0.05, z + 0.1);
   glVertex3f(x + 0.5, y + 0.15, z + 0.05);
   glVertex3f(x + 0.5, y + 0.15, z - 0.05);
   glVertex3f(x + 0.6, y + 0.05, z - 0.1);
   glEnd();

   tringleNormal(x - 0.6, x - 0.5, x - 0.5, y + 0.05, y + 0.15, y + 0.15, z + 0.1, z + 0.05, z - 0.05, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.6, y + 0.05, z + 0.1);
   glVertex3f(x - 0.5, y + 0.15, z + 0.05);
   glVertex3f(x - 0.5, y + 0.15, z - 0.05);
   glVertex3f(x - 0.6, y + 0.05, z - 0.1);
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

// Storage with shells
void storage()
{
   // Save transformation
   glTranslated(0, 0, 0.5);

   glPushMatrix();
   // storage shells
   cube(0.9, 0.93, 0.1, 0.1, 0.01, 0.3, 0, 0.4, 0.4, 0.4);
   cube(0.9, 0.63, 0.1, 0.1, 0.01, 0.3, 0, 0.4, 0.4, 0.4);
   cube(0.9, 0.33, 0.1, 0.1, 0.01, 0.3, 0, 0.4, 0.4, 0.4);
   cube(0.9, 0.03, 0.1, 0.1, 0.01, 0.3, 0, 0.4, 0.4, 0.4);
   // storage legs
   cube(0.98, 0.46, 0.38, 0.014, 0.466, 0.014, 0, 0.5, 0.5, 0.5);
   cube(0.82, 0.46, 0.38, 0.014, 0.466, 0.014, 0, 0.5, 0.5, 0.5);
   cube(0.98, 0.46, -0.18, 0.014, 0.466, 0.014, 0, 0.5, 0.5, 0.5);
   cube(0.82, 0.46, -0.18, 0.014, 0.466, 0.014, 0, 0.5, 0.5, 0.5);
   // time travel stuff
   badCube(0.9, 0.74, -0.01, 0.08, 0.1, 0.12, 180, 0.6, 0.48, 0.31);
   // boombox looking machine
   cube(0.9, 0.76, 0.24, 0.07, 0.1, 0.07, 180, 0.65, 0.65, 0.65);
   cube(0.9, 0.66, 0.24, 0.09, 0.02, 0.09, 180, 0.6, 0.6, 0.6);
   cube(0.9, 0.86, 0.24, 0.09, 0.02, 0.09, 180, 0.6, 0.6, 0.6);
   // storage box upper
   cube(0.9, 0.42, -0.05, 0.07, 0.09, 0.08, 0, 0.6, 0.5, 0.3);
   cube(0.9, 0.51, -0.05, 0.075, 0.01, 0.085, 0, 0.6, 0.49, 0.31);
   // storage box lower
   cube(0.9, 0.16, 0.24, 0.07, 0.13, 0.08, 0, 0.57, 0.49, 0.29);
   cube(0.9, 0.29, 0.24, 0.075, 0.01, 0.085, 0, 0.58, 0.5, 0.3);
   // most upper box
   cube(0.9, 1.04, 0.3, 0.07, 0.1, 0.08, 0, 0.64, 0.5, 0.32);
   cube(0.9, 1.13, 0.3, 0.075, 0.01, 0.085, 0, 0.66, 0.52, 0.33);
   // Weapon
   objSetup(0, 0.015, 57, 22.8, 15, 0.5, 0.5, 0.5);
   // Orc
   objSetup(4, 0.06, 14, 17.4, 0, 0.64, 0.75, 0.48);
   // Object
   objSetup(5, 0.05, 18, 0.7, 0, 0.45, 0.43, 0.64);
   glPopMatrix();
}

// Machines code
void machine(double x, double y, double z, float r, float g, float b)
{
   // Normal array
   double normals[3] = {0, 0, 0};
   // Save transformation
   glPushMatrix();
   // machine 1
   glScaled(0.9, 0.9, 0.9);
   cube(x, y, z, 0.2, 0.2, 0.16, 0, r, g, b);
   sphere(x - 0.1, y + 0.22, z - 0.192, 0.028, r - 0.01, g - 0.01, b - 0.01);
   glColor3f(r, g, b);

   tringleNormal(x - 0.2, x + 0.2, x + 0.2, y + 0.2, y + 0.2, y + 0.26, z - 0.16, z - 0.16, z - 0.2, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.2, y + 0.2, z - 0.16);
   glVertex3f(x + 0.2, y + 0.2, z - 0.16);
   glVertex3f(x + 0.2, y + 0.26, z - 0.2);
   glVertex3f(x - 0.2, y + 0.26, z - 0.2);
   glEnd();

   tringleNormal(x - 0.2, x + 0.2, x + 0.2, y + 0.26, y + 0.26, y + 0.26, z - 0.2, z - 0.2, z - 0.24, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.2, y + 0.26, z - 0.2);
   glVertex3f(x + 0.2, y + 0.26, z - 0.2);
   glVertex3f(x + 0.2, y + 0.26, z - 0.24);
   glVertex3f(x - 0.2, y + 0.26, z - 0.24);
   glEnd();

   tringleNormal(x - 0.2, x + 0.2, x + 0.2, y + 0.26, y + 0.26, y - 0.2, z - 0.24, z - 0.24, z - 0.24, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.2, y + 0.26, z - 0.24);
   glVertex3f(x + 0.2, y + 0.26, z - 0.24);
   glVertex3f(x + 0.2, y - 0.2, z - 0.24);
   glVertex3f(x - 0.2, y - 0.2, z - 0.24);
   glEnd();

   tringleNormal(x - 0.2, x + 0.2, x + 0.2, y - 0.2, y - 0.2, y - 0.2, z - 0.24, z - 0.24, z - 0.16, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.2, y - 0.2, z - 0.24);
   glVertex3f(x + 0.2, y - 0.2, z - 0.24);
   glVertex3f(x + 0.2, y - 0.2, z - 0.16);
   glVertex3f(x - 0.2, y - 0.2, z - 0.16);
   glEnd();

   tringleNormal(x - 0.2, x - 0.2, x - 0.2, y - 0.2, y - 0.2, y + 0.2, z - 0.24, z - 0.16, z - 0.16, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.2, y - 0.2, z - 0.24);
   glVertex3f(x - 0.2, y - 0.2, z - 0.16);
   glVertex3f(x - 0.2, y + 0.2, z - 0.16);
   glVertex3f(x - 0.2, y + 0.2, z - 0.24);
   glEnd();

   tringleNormal(x - 0.2, x - 0.2, x - 0.2, y + 0.2, y + 0.2, y + 0.26, z - 0.24, z - 0.16, z - 0.2, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x - 0.2, y + 0.2, z - 0.24);
   glVertex3f(x - 0.2, y + 0.2, z - 0.16);
   glVertex3f(x - 0.2, y + 0.26, z - 0.2);
   glVertex3f(x - 0.2, y + 0.26, z - 0.24);
   glEnd();

   tringleNormal(x + 0.2, x + 0.2, x + 0.2, y - 0.2, y - 0.2, y + 0.2, z - 0.24, z - 0.16, z - 0.16, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x + 0.2, y - 0.2, z - 0.24);
   glVertex3f(x + 0.2, y - 0.2, z - 0.16);
   glVertex3f(x + 0.2, y + 0.2, z - 0.16);
   glVertex3f(x + 0.2, y + 0.2, z - 0.24);
   glEnd();

   tringleNormal(x + 0.2, x + 0.2, x + 0.2, y + 0.2, y + 0.2, y + 0.26, z - 0.24, z - 0.16, z - 0.2, normals);
   glNormal3f(normals[0], normals[1], normals[2]);
   glBegin(GL_QUADS);
   glVertex3f(x + 0.2, y + 0.2, z - 0.24);
   glVertex3f(x + 0.2, y + 0.2, z - 0.16);
   glVertex3f(x + 0.2, y + 0.26, z - 0.2);
   glVertex3f(x + 0.2, y + 0.26, z - 0.24);
   glEnd();

   glPopMatrix();
}

// Sink code
void sink()
{
   // Pink sink
   cube(0.007, 0.39, -0.98, 0.2, 0.1, 0.02, 0, 1, 0.86, 0.91);
   badCube(0.007, 0.34, -0.83, 0.2, 0.05, 0.16, 180, 1, 0.86, 0.91);
   // Bottom part
   halfSphere(30, 30, 0.01, -0.3, 0.84, 0.168, 0.56, 0.36, 0.43);
   cube(-0.16, 0.17, -0.7, 0.018, 0.168, 0.018, 0, 0.56, 0.36, 0.43);
   cube(0.17, 0.17, -0.7, 0.018, 0.168, 0.018, 0, 0.56, 0.36, 0.43);
   // Right crane
   cube(-0.08, 0.42, -0.94, 0.04, 0.01, 0.01, 0, 0.7, 0.7, 0.7);
   sphere(0.07, 0.42, -0.96, 0.0178, 0.7, 0.7, 0.7);
   // Left crane
   cube(0.1, 0.42, -0.94, 0.04, 0.01, 0.01, 0, 0.7, 0.7, 0.7);
   sphere(-0.05, 0.42, -0.96, 0.0178, 0.7, 0.7, 0.7);
   // Main crane
   cube(0.01, 0.44, -0.94, 0.014, 0.014, 0.06, 0, 0.7, 0.7, 0.7);
   cube(0.01, 0.43, -0.88, 0.014, 0.026, 0.014, 0, 0.7, 0.7, 0.7);
}

// Board code
void board()
{
   // Calendar
   texturedCube(-0.34, 0.68, -0.94, 0.08, 0.16, 0.01, 0, 0.8, 0.8, 0.8, 1, 0.3, 0.7, 0.15, 0.9);

   // Note board
   cube(-0.9, 0.7, -0.94, 0.35, 0.25, 0.01, 0, 0.8, 0.71, 0.58);
   texturedCube(-0.9, 0.7, -0.93, 0.32, 0.22, 0.01, 0, 0.89, 0.83, 0.63, 0, 0, 1, 0, 1);

   // Instrument board
   cube(-1.38, 0.82, -0.25, 0.01, 0.2, 0.3, 0, 0.8, 0.71, 0.58);
   cube(-1.36, 1.03, -0.25, 0.03, 0.01, 0.3, 0, 0.25, 0.2, 0.1);

   // Disks
   objSetup(3, 0.04, -34, 23.5, -0.5, 0.48, 0.48, 0.48);
   objSetup(3, 0.04, -34, 20.5, -0.5, 0.5, 0.5, 0.5);
   objSetup(3, 0.04, -34, 17.5, -0.5, 0.48, 0.48, 0.48);
   objSetup(3, 0.03, -45.5, 22.5, -5, 0.6, 0.5, 0.5);
   objSetup(3, 0.03, -45.5, 22.5, -8, 0.6, 0.5, 0.5);
   objSetup(3, 0.03, -45.5, 22.5, -11, 0.6, 0.5, 0.5);
   objSetup(3, 0.03, -45.5, 22.5, -14, 0.6, 0.5, 0.5);
   objSetup(3, 0.12, -11.2, 7.2, -2.5, 0.4, 0.39, 0.49);
}

// Create a base for the house
void base()
{
   // Save transformation
   glPushMatrix();
   // Floor
   cube(-0.2, 0, 1.2, 1.2, 0.01, 2.2, 0, 0.63, 0.86, 0.67);
   // Left Wall
   cube(-1.4, 0.7, 1.2, 0.01, 0.7, 2.2, 0, 0.23, 0.19, 0.24);
   // Right Wall
   cube(1, 0.7, 1.2, 0.01, 0.7, 2.2, 0, 0.23, 0.19, 0.24);
   // Back Wall
   texturedCube(-0.2, 0.7, -1, 1.2, 0.7, 0.01, 0, 0.6, 0.5, 0.5, 3, 0, 4, 0, 4);
   // Roof
   cube(-0.2, 1.2, 1.2, 1.2, 0.01, 2.2, 0, 0.25, 0.2, 0.26);
   // Door
   cube(0.98, 0.4, -0.36, 0.01, 0.4, 0.2, 0, 0.2, 0.2, 0.2);
   sphere(0.962, 0.4, -0.24, 0.014, 1, 0.92, 0.42);
   // Garage Door
   cube(-0.2, 0.9, 3.4, 1.2, 0.7, 0.01, 0, 0.7, 0.5, 0.5);
   // Right block
   cube(-1.5, 0.7, 3.4, 0.4, 0.7, 0.1, 0, 0.6, 0.5, 0.5);
   // Left block
   cube(1.1, 0.7, 3.4, 0.4, 0.7, 0.1, 0, 0.6, 0.5, 0.5);
   // Asphalt
   cube(-0.2, 0, 5.60, 1, 0.01, 2.2, 0, 0.72, 0.69, 0.68);
   // Grass
   cube(-2.2, 0, 5.60, 1, 0.01, 2.2, 0, 0.12, 0.84, 0.33);
   cube(1.8, 0, 5.60, 1, 0.01, 2.2, 0, 0.12, 0.84, 0.33);
   // Right block lifter
   cube(-1.1, 0.75, 3.4, 0.01, 0.68, 0.025, 0, 0.55, 0.55, 0.55);
   // Left block lifter
   cube(0.7, 0.75, 3.4, 0.01, 0.68, 0.025, 0, 0.55, 0.55, 0.55);
   glPopMatrix();
}

// Put everything together
void garage()
{
   base();
   table();
   machine(0.44, 0.2, -0.86, 0.95, 0.98, 0.68);
   cube(0.44 - 0.16, 0.1, -0.6, 0.01, 0.02, 0.01, 0, 0.77, 0.8, 0.54);
   machine(0.86, 0.2, -0.86, 0.77, 0.8, 0.54);
   cube(0.764, 0.2, -0.62, 0.12, 0.13, 0.01, 0, 0.75, 0.78, 0.52);
   bigLamp(-0.2, 1.17, -0.5, 0, 0.13, 0);
   bigLamp(-0.2, 1.17, 0.5, 0, 0.13, 0);
   bigLamp(-0.2, 1.17, 1.5, 0, 0.13, 0);
   bigLamp(-0.2, 1.17, 2.0, 0, 0.13, 0);
   board();
   sink();
   storage();
}

// Draw the scene
void Scene()
{
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode == 1)
   {
      double Ex = -2 * dim * Sin(th) * Cos(ph);
      double Ey = +2 * dim * Sin(ph);
      double Ez = +2 * dim * Cos(th) * Cos(ph);
      gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
   }
   else if (mode == 2)
   {
      gluLookAt(global_x, 0.6, global_z, lx + global_x, 0.6, lz + global_z, 0, 1, 0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph, 1, 0, 0);
      glRotatef(th, 0, 1, 0);
   }

   float Color[3] = {0.9, 1, 1};
   float Position[] = {-0.2, Ylight, Zlight, 0.0};
   if (light == 1)
   {
      Color[0] = 0.9;
      Color[1] = 1;
      Color[2] = 1;
   }
   else if (light == 0)
   {
      Color[0] = 0.0;
      Color[1] = 0.05;
      Color[2] = 0.0;
   }
   glPushMatrix();
   glScaled(0.9, 0.1, 0.9);
   cube(-0.2, 10, 0.5, 0.6, 0.1, 0.1, 0, Color[0], Color[1], Color[2]);
   cube(-0.2, 10, -0.5, 0.6, 0.1, 0.1, 0, Color[0], Color[1], Color[2]);
   cube(-0.2, 10, 1.5, 0.6, 0.1, 0.1, 0, Color[0], Color[1], Color[2]);
   cube(-0.2, 10, 2.0, 0.6, 0.1, 0.1, 0, Color[0], Color[1], Color[2]);
   glPopMatrix();
   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
   glLightfv(GL_LIGHT0, GL_POSITION, Position);
   //  Set materials
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, Shinyness);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);

   glEnable(GL_COLOR_MATERIAL);

   //  Draw the objects
   garage();

   glDisable(GL_TEXTURE_2D);

   //  Revert to fixed pipeline
   glUseProgram(0);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1, 1, 1);
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   Scene();

   //  Display parameters
   glWindowPos2i(5, 5);
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
   if (move)
      zh = fmod(90 * t, 360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim > 1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(55, asp, dim, mode);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Switch display mode for debugging purposes

   else if (ch == 'm' || ch == 'M')
      mode = (mode + 1) % 3;

   //  Turn left
   else if (ch == 'a' || ch == 'A')
   {
      angle -= 0.08f;
      lx = sin(angle);
      lz = -cos(angle);
   }
   // Turm right
   else if (ch == 'd' || ch == 'D')
   {
      angle += 0.08f;
      lx = sin(angle);
      lz = -cos(angle);
   }
   // Move forward
   else if (ch == 'w' || ch == 'W')
   {
      global_z += lz * 0.05;
   }
   // Move backward
   else if (ch == 's' || ch == 'S')
   {
      global_z -= lz * 0.05;
   }
   // Switch lights on and off
   else if (ch == '+')
   {
      light = 1;
      Ylight = 0.8;
      Zlight = 0.4;
   }
   else if (ch == '-')
   {
      light = 0;
      Ylight = -10000000;
      Zlight = -10000000;
   }
   //  Reproject
   Project(55, asp, dim, mode);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height > 0) ? (double)width / height : 1;
   //  Set the viewport to the entire window
   glViewport(0, 0, RES * width, RES * height);
   //  Set projection
   Project(55, asp, dim, mode);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[])
{
   //  Initialize GLUT
   glutInit(&argc, argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutCreateWindow("Vadim Pavliukevich Project");
   glutFullScreen();
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   // Load textures
   texture[0] = LoadTexBMP("./textures/board.bmp");
   texture[1] = LoadTexBMP("./textures/calendar.bmp");
   texture[2] = LoadTexBMP("./textures/safe.bmp");
   texture[3] = LoadTexBMP("./textures/wall.bmp");
   texture[4] = LoadTexBMP("./textures/box.bmp");
   object[1] = LoadOBJ("./objects/helm.obj");
   object[0] = LoadOBJ("./objects/palitsa.obj");
   object[2] = LoadOBJ("./objects/bottle.obj");
   object[3] = LoadOBJ("./objects/disk.obj");
   object[4] = LoadOBJ("./objects/orc.obj");
   object[5] = LoadOBJ("./objects/piece.obj");
   //  Switch font to nearest
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
