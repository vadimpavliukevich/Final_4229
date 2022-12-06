//  CSCIx229 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx229.h"

//
//  Set projection
//
void Project(double fov,double asp,double dim, int mode)
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   if (mode == 0)
      //  Orthogonal projection
      glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
   else
      //  Perspective transformation
      gluPerspective(fov, asp, dim / 4, 4 * dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

