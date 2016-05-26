#pragma once

#include "cgra_math.hpp"
#include "opengl.hpp"

namespace cgra {

	inline void cgraLine(float length){
		glLineWidth(1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0, 0, -length);
		glEnd();
	}

	inline void cgraCube(vec3  centre, vec3 bounds) {
		glPushMatrix();{
			glTranslatef(centre.x,centre.y,centre.z);
			glTranslatef(-bounds.x/2,bounds.y/2,bounds.z/2);
			//z lines
			glPushMatrix();{
				cgraLine(bounds.z);
				glTranslatef(bounds.x,0,0);
				cgraLine(bounds.z);
				glTranslatef(0,-bounds.y,0);
				cgraLine(bounds.z);
				glTranslatef(-bounds.x,0,0);
				cgraLine(bounds.z);
			}glPopMatrix();
			//x
			glPushMatrix();{
				glTranslatef(bounds.x,0,0);
				glRotatef(90,0,1,0);
				cgraLine(bounds.x);
				glTranslatef(bounds.z,0,0);
				cgraLine(bounds.x);
				glTranslatef(0,-bounds.y,0);
				cgraLine(bounds.x);
				glTranslatef(-bounds.z,0,0);
				cgraLine(bounds.x);
			}glPopMatrix();
			//y
			glPushMatrix();{
				glRotatef(-90,1,0,0);
				cgraLine(bounds.y);
				glTranslatef(0,bounds.z,0);
				cgraLine(bounds.y);
				glTranslatef(bounds.x,0,0);
				cgraLine(bounds.y);
				glTranslatef(0,-bounds.z,0);
				cgraLine(bounds.y);
			}glPopMatrix();
		}glPopMatrix();
		// reset mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}