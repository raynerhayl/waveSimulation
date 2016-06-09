//---------------------------------------------------------------------------
//
// Copyright (c) 2016 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "simple_image.hpp"
#include "simple_shader.hpp"
#include "simple_gui.hpp"

#include "shady_geometry.hpp"
#include "opengl.hpp"
#include "school.hpp"
#include "helpers.hpp"
#include "boid.hpp"
#include "octree.hpp"
#include "wave.hpp"
#include "geometry.hpp"

#include "perlin_noise.hpp"

using namespace std;
using namespace cgra;

// Window
//
GLFWwindow* g_window;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 10000.0;


// Mouse controlled Camera values
//
bool dragging = false;
bool g_leftMouseDown = false;
vec2 g_mousePosition;
float g_pitch = 0;
float g_yaw = 0;
float g_zoom = 1.0;
vec3 g_camPos = vec3(0,100.0,0);

BoundingBox scene_bounds = BoundingBox(vec3(-500,-500,-500),vec3(500,500,500));
bool drawOriginAxis = false;
//school related
School * g_school;
bool draw_school = true;

//wave related
Wave * wave;
float waveTime = 0.0;
int numWaves = 14;

Geometry * ship = nullptr;
Geometry * ground = nullptr;
GLfloat propsBuf[200]; // seto of properties to fade in
GLfloat props[200]; // main set of properties
GLfloat activeBuf[200]; // properties which actually get sent to shader

float medianWavelength = 80;
float amplitudeR = 1;
float windDir = 0; // wind direction from (x = 1, z = 0)
float dAngle = 45; // difference in angle from windDir
float medianS = 0.2;
float speedFactor = 1; // scales the speed

vec2 shipPos = vec2(0.0, -0.5);


//performance
float frameSpeed = 0;
int fps = 0;

// Values and fields to showcase the use of shaders
// Remove when modifying main.cpp for Assignment 3
//
bool g_useShader = true;
GLuint g_texture = 0;
GLuint g_plainShader = 0;
GLuint g_sobelShader = 0;
GLuint g_toonShader = 0;
GLuint g_shaderGerstner = 0;
GLuint g_shaderPhong = 0;
GLuint bumpTex = 0;
GLuint g_shipShader = 0;
GLuint g_gerstNormShader = 0;
GLuint gerstNormalTex;
GLuint g_causticShader = 0;


// Mouse Button callback
// Called for mouse movement event on since the last glfwPollEvents
//
void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	// cout << "Mouse Movement Callback :: xpos=" << xpos << "ypos=" << ypos << endl;
	if (g_leftMouseDown || dragging) {
		g_yaw -= (g_mousePosition.x - xpos)*.2;
		g_pitch -= (g_mousePosition.y - ypos)*.2;
	}
	g_mousePosition = vec2(xpos, ypos);
}


// Mouse Button callback
// Called for mouse button event on since the last glfwPollEvents
//
void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	//cout << "Mouse Button Callback :: button=" << button << "action=" << action << "mods=" << mods << endl;
	if (button == GLFW_MOUSE_BUTTON_LEFT){
		g_leftMouseDown = (action == GLFW_PRESS);
	}
	if(button == GLFW_MOUSE_BUTTON_MIDDLE){
		if(action ==1)dragging = ! dragging;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (g_useShader) {
			//g_useShader = false;
			cout << "Using the default OpenGL pipeline" << endl;
		}
		else {
			//g_useShader = true;
			cout << "Using a shader" << endl;
		}
	}
}
cgra::vec3 getCamDir(){
	float yaw = radians(g_yaw);
	float pitch = radians(g_pitch);
	float xzLen = cos(pitch);
	float z = xzLen * cos(yaw);
	float y = sin(pitch);
	float x = xzLen * sin(-yaw);
	return vec3(x,y,z);
}

// Scroll callback
// Called for scroll event on since the last glfwPollEvents
//
void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	// cout << "Scroll Callback :: xoffset=" << xoffset << "yoffset=" << yoffset << endl;
	g_zoom -= yoffset * g_zoom * 0.2;
	if(yoffset < 0) g_camPos -= getCamDir()*8;
	else  g_camPos += getCamDir()*8;
}

// Keyboard callback
// Called for every key event on since the last glfwPollEvents
//
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
	// cout << "Key Callback :: key=" << key << "scancode=" << scancode
	// 	<< "action=" << action << "mods=" << mods << endl;
	// YOUR CODE GOES HERE
	// ...
	switch(key){
		case 'F':
			if(action == 1)draw_school = !draw_school;
		break;
		case 'O':
			if(action == 1){
				drawOriginAxis = !drawOriginAxis;
			}
		break;
		case 'W':
		{
			g_camPos += normalize(getCamDir()) * 8;
		}
		break;
		case 'A':
		{
			vec3 camDir = getCamDir();
			vec3 left = cross(camDir,vec3(0,1,0));
			g_camPos -= normalize(left);
		}
		break;
		case 'S':
		{
			g_camPos -= getCamDir()* 4;
		}
		break;
		case 'D':
		{
			vec3 camDir = getCamDir();
			vec3 right = cross(camDir,vec3(0,1,0));
			g_camPos += normalize(right);
		}
		break;
	}
}


// Character callback
// Called for every character input event on since the last glfwPollEvents
//
void charCallback(GLFWwindow *win, unsigned int c) {
	// cout << "Char Callback :: c=" << char(c) << endl;
	// Not needed for this assignment, but useful to have later on
}

void initWaves();

void renderGUI() {
	// Start registering GUI components
	SimpleGUI::newFrame();

	if (ImGui::IsMouseClicked(1))
		ImGui::OpenPopup("Controls");

	if (ImGui::BeginPopup("Controls")) {
		if (ImGui::Selectable("Stormy")) {
 		medianWavelength = 100;
 		amplitudeR = 3;
 		windDir = 0; // wind direction from (x = 1, z = 0)
 		dAngle = 60; // difference in angle from windDir
 		medianS = 0.2;
 		speedFactor = 1; // scales the speed
 		initWaves();

		} if (ImGui::Selectable("calm")) {
 		medianWavelength = 80;
 		amplitudeR = 1;
 		windDir = 0; // wind direction from (x = 1, z = 0)
 		dAngle = 20; // difference in angle from windDir
 		medianS = 0.1;
 		speedFactor = 1; // scales the speed
		initWaves();
		}

		ImGui::EndPopup();
	}

	

	ImGui::SetNextWindowPos(ImVec2(10,10));
	ImGui::Begin("Fixed Overlay", nullptr, ImVec2(0,0), 0.3f,
	ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|
	ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings);
	ostringstream ss;
	// Replace this with your code
	ss << frameSpeed << " ms/frame  " << fps << " fps";
	ImGui::Text(ss.str().c_str());
	ImGui::End();

	// Flush components and render
	SimpleGUI::render();
}

void initSchool(){
	g_school = new School(500,3,scene_bounds);
}

float randF() {
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	return r;
}


/*
	Populates an array of properties at the given wave index
*/
void fillProps(GLfloat properties[], int waveIndex) {

	float wavelength = medianWavelength - medianWavelength*0.4 + 2* randF() *0.8 * medianWavelength;
	float speed = sqrt((9.81*wavelength) / (2 * 3.14));//speedFactor * (frequency*wavelength);
	float frequency = speed / wavelength;// sqrt((9.81 * 2 * 3.145) / wavelength);
	float amplitude = amplitudeR - amplitudeR*0.2 + randF() * 0.4 * amplitudeR;

	float angle = windDir - dAngle + randF() * 2 * dAngle;

	vec2 dir = vec2(std::sin((3.145 * angle) / 180), std::cos((3.145 * angle) / 180));

	float steepness =randF()*(medianS * 0.2) + medianS *0.7;

	properties[waveIndex * 6] = wavelength;	     // wavelength
	properties[waveIndex * 6 + 1] = amplitude;  // amplitude
	properties[waveIndex * 6 + 2] = speed;  // velocity
	properties[waveIndex * 6 + 3] = dir.x; // direction x
	properties[waveIndex * 6 + 4] = dir.y;  // direction y
	properties[waveIndex * 6 + 5] = steepness;  // steepness

}

/*
	Initiate the wave class and populate property arrays
*/
void initWaves() {
	wave = new Wave();

	for (int i = 0; i < numWaves; i++) {

		fillProps(props, i);
		fillProps(propsBuf, i);
		fillProps(activeBuf, i);

	}

}

void fillAllProps(GLfloat properties[]) {
	for (int i = 0; i < numWaves; i++) {
		fillProps(properties, i);
	}
}

// Sets up where and what the light is
// Called once on start up
//
void initLight() {
	float direction[] = { 0.0, 1.0, 1.0, 0.0 };
	float diffintensity[] = { 0.8, 0.8, 0.8, 1.0 };
	float ambient[] = { 0.7, 0.7, 0.7, 1.0 };
	float specular[] = { 00.1,0.1,0.1, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);


	glEnable(GL_LIGHT0);
}


// An example of how to load a texure from a hardcoded location
//
void initTexture() {
	Image tex("./work/res/textures/normalMap2.jpg");
	glGenTextures(1, &bumpTex); // Generate texture ID

	glActiveTexture(GL_TEXTURE0); // Use slot 0, need to use GL_TEXTURE1 ... etc if using more than one texture PER OBJECT
	glBindTexture(GL_TEXTURE_2D, bumpTex); // Bind it as a 2D texture

	// Setup sampling strategies
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Finnaly, actually fill the data into our texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE, tex.dataPointer());

}


// An example of how to load a shader from a hardcoded location
//
void initShader() {
	// To create a shader program we use a helper function
	// We pass it an array of the types of shaders we want to compile
	// and the corrosponding locations for the files of each stage
	g_sobelShader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/sobel.vert", "./work/res/shaders/sobel.frag" });
	g_toonShader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/toon.vert", "./work/res/shaders/toon.frag" });
	g_plainShader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/plain.vert", "./work/res/shaders/plain.frag" });
	g_shaderGerstner = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/shaderGerstner.vert", "./work/res/shaders/toon.frag" });
	g_shaderPhong = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/shaderSimple.vert", "./work/res/shaders/shaderPhong.frag" });
	g_shipShader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/ship.vert", "./work/res/shaders/toon.frag" });
	g_gerstNormShader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/shaderGerstner.vert", "./work/res/shaders/norm.frag" });
	g_causticShader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/caustic.vert", "./work/res/shaders/toon.frag" });

}


// Sets up where the camera is in the scene
// 
void setupCamera(int width, int height) {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, width / float(height), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(g_pitch, 1, 0, 0);
	glRotatef(g_yaw, 0, 1, 0);
	glTranslatef(g_camPos.x, g_camPos.y, g_camPos.z);
}



void drawOrigin(){
	//x
	glColor3f(1,0,0);
	glPushMatrix();{
		glRotatef(90,0,1,0);
		glTranslatef(0,0,50);
		cgraLine(100);
	}glPopMatrix();
	//y
	glColor3f(0,1,0);
	glPushMatrix();{
		glRotatef(90,1,0,0);
		glTranslatef(0,0,50);
		cgraLine(100);
	}glPopMatrix();
	//z
	glColor3f(0,0,1);
	glPushMatrix();{
		glTranslatef(0,0,50);
		cgraLine(100);
	}glPopMatrix();
}


void renderWave() {
	glUseProgram(0);
	glUseProgramObjectARB(0);

	glUseProgram(0);
	glUseProgramObjectARB(0);


				// Texture setup
		//
		// Enable Drawing texures
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// Set the location for binding the texture
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, gerstNormalTex);

		glUseProgram(g_shaderGerstner);


		glUniform1i(glGetUniformLocation(g_shaderPhong, "texture0"), 0);
		// Use the shader we made

		// Set our sampler (texture0) to use GL_TEXTURE0 as the source
		glUniform1i(glGetUniformLocation(g_shaderGerstner, "texture0"), 0);
		//glUniform1i(glGetUniformLocation(g_shaderGerstner, "texture1"), 0);

		// Set the current time for the shader 
		glUniform1f(glGetUniformLocation(g_shaderGerstner, "time"), waveTime);
		// Send the shader the current main buffer of wave properties
		glUniform1fv(glGetUniformLocation(g_shaderGerstner, "waveProperties"), 100, activeBuf);
		// Specify the number of waves to use from the buffer
		glUniform1i(glGetUniformLocation(g_shaderGerstner, "numWaves"), numWaves);


		glPushMatrix;{
		glScalef(10,10,10);


		wave->render();

	}glPopMatrix;

		// Unbind our shader
		glUseProgram(0);
		glUseProgramObjectARB(0);
		glDisable(GL_TEXTURE_2D);

}


// Draw function
//
void render(int width, int height) {



	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	//glViewport(0,0,width,height);
	glViewport(0,0,width,height);

	

	glGenFramebuffersEXT(1, &FramebufferName);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FramebufferName);
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);


	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint normalTexture;
	glGenTextures(1, &normalTexture);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint depth_tex;
	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

/*
	glGenTextures(1, &gerstNormalTex);
	glBindTexture(GL_TEXTURE_2D, gerstNormalTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	*/


	//-------------------------
	//Attach depth buffer to FBO

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gerstNormalTex, 0);



	// Set the list of draw buffers.
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers); // "1" is the size of DrawBuffers

								   //glDrawBuffer(GL_NONE); // No color buffer is drawn to.

								   // Always check that our framebuffer is ok

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glViewport(0, 0, width, height);
	// Grey/Blueish background

	glClearColor(135/255.0,206/255.0,250/255.0,1.0f);          // We'll Clear To The Color Of The Fog ( Modified )
	GLfloat fogColor[4] = { 52 / 255.0,104 / 255.0,125 / 255.0,1.0f };
	glFogi(GL_FOG_MODE, GL_LINEAR);        // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.35f);              // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
	glFogf(GL_FOG_START, 2800.0f);             // Fog Start Depth
	glFogf(GL_FOG_END, 4000.0f);               // Fog End Depth
	glEnable(GL_FOG);                   // Enables GL_FOG
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	setupCamera(width, height);


	float direction[] = { 0.7f, 0.7f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, direction);

	glDisable(GL_LIGHTING);

	//draw unlit stuff here
	if(drawOriginAxis){
		drawOrigin();
		glColor3f(1,1,1);
		cgraCube(
			vec3((scene_bounds.max.x+scene_bounds.min.x)/2,
			(scene_bounds.max.y+scene_bounds.min.y)/2,
			(scene_bounds.max.z+scene_bounds.min.z)/2),vec3(
			abs(scene_bounds.max.x-scene_bounds.min.x),
			abs(scene_bounds.max.y-scene_bounds.min.y),
			abs(scene_bounds.max.z-scene_bounds.min.z)
		));
	}
	// Use the shader we made
	if (g_useShader) {
		glUseProgram(g_toonShader);
	}
	else {
		glUseProgram(0);
	}
	if(draw_school) g_school->renderSchool();



		glUseProgram(g_causticShader);


		glUniform1i(glGetUniformLocation(g_causticShader, "texture0"), 0);
		// Use the shader we made

		// Set our sampler (texture0) to use GL_TEXTURE0 as the source
		glUniform1i(glGetUniformLocation(g_causticShader, "texture0"), 0);
		//glUniform1i(glGetUniformLocation(g_shaderGerstner, "texture1"), 0);

		// Set the current time for the shader 
		glUniform1f(glGetUniformLocation(g_causticShader, "time"), waveTime);
		// Send the shader the current main buffer of wave properties
		glUniform1fv(glGetUniformLocation(g_causticShader, "waveProperties"), 100, activeBuf);
		// Specify the number of waves to use from the buffer
		
		glUniform1i(glGetUniformLocation(g_causticShader, "numWaves"), numWaves);

	glPushMatrix();
	glTranslatef(0, -500, 0);
	glColor3f(0.3f,0.3f,0.3f);
	glScalef(5, 5, 5);
	ground->renderGeometry();
	glPopMatrix();


		glUseProgram(g_shipShader);


		glUniform1i(glGetUniformLocation(g_shaderPhong, "texture0"), 0);
		// Use the shader we made

		// Set our sampler (texture0) to use GL_TEXTURE0 as the source
		glUniform1i(glGetUniformLocation(g_shaderGerstner, "texture0"), 0);
		//glUniform1i(glGetUniformLocation(g_shaderGerstner, "texture1"), 0);

		// Set the current time for the shader 
		glUniform1f(glGetUniformLocation(g_shaderGerstner, "time"), waveTime);
		// Send the shader the current main buffer of wave properties
		glUniform1fv(glGetUniformLocation(g_shaderGerstner, "waveProperties"), 100, activeBuf);
		// Specify the number of waves to use from the buffer
		glUniform1i(glGetUniformLocation(g_shaderGerstner, "numWaves"), numWaves);





	glPushMatrix();{
		glTranslatef(shipPos.x,-10.0,shipPos.y);

		glScalef(10,10,10);
		ship->renderGeometry();

	} glPopMatrix();



		glColor3f(52 / 255.0,104 / 255.0,125 / 255.0);
		renderWave();

	//renderWave();
	glEnable(GL_LIGHTING);


		// Unbind our shader
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		if (g_useShader){
		glUseProgram(g_sobelShader);
	} else {
		glUseProgram(g_plainShader);
	}
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		glDisable(GL_LIGHTING);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Enable Drawing texures
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// Set the location for binding the texture
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glActiveTexture(GL_TEXTURE1);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, renderedTexture);

		glActiveTexture(GL_TEXTURE2);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, depth_tex);
		// Set our sampler (texture0) to use GL_TEXTURE0 as the source
			if (g_useShader){
		glUniform1i(glGetUniformLocation(g_sobelShader, "edge"), 0);
		glUniform1i(glGetUniformLocation(g_sobelShader, "colorMap"), 1);
		glUniform1i(glGetUniformLocation(g_sobelShader, "depthMap"), 2);
		glUniform1f(glGetUniformLocation(g_sobelShader, "width"), width);
	} else {
		glUniform1f(glGetUniformLocation(g_plainShader, "rendered"), 1);
	}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-1, -1,-1);
		glTexCoord2f(1, 0);
		glVertex3f(1, -1,-1);
		glTexCoord2f(1, 1);
		glVertex3f(1, 1,-1);
		glTexCoord2f(0, 1);
		glVertex3f(-1, 1,-1);
		glEnd();


		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);


	// Disable flags for cleanup (optional)
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);


		glUseProgram(0);
	
	
	glDeleteTextures(1, &depth_tex);
	glDeleteTextures(1, &renderedTexture);
	glDeleteTextures(1, &normalTexture);
	glDeleteFramebuffers(1, &FramebufferName);

	
}


// Forward decleration for cleanliness (Ignore)
void APIENTRY debugCallbackARB(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);


//Main program
// 
int main(int argc, char **argv) {


	PerlinNoise p_noise;

	cout << "Perlinine" <<  endl;

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			double dx = x/double(9);
			double dy = y/double(9);

			cout << p_noise.noise(dx, dy, 0);
		
		}
		cout << endl;
	}
	cout << endl;



	// Initialize the GLFW library
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW" << endl;
		abort(); // Unrecoverable error
	}

	// Get the version for GLFW for later
	int glfwMajor, glfwMinor, glfwRevision;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

	// Create a windowed mode window and its OpenGL context
	g_window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!g_window) {
		cerr << "Error: Could not create GLFW window" << endl;
		abort(); // Unrecoverable error
	}

	// Make the g_window's context is current.
	// If we have multiple windows we will need to switch contexts
	glfwMakeContextCurrent(g_window);



	// Initialize GLEW
	// must be done after making a GL context current (glfwMakeContextCurrent in this case)
	glewExperimental = GL_TRUE; // required for full GLEW functionality for OpenGL 3.0+
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}



	// Print out our OpenGL verisions
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using GLFW " << glfwMajor << "." << glfwMinor << "." << glfwRevision << endl;



	// Attach input callbacks to g_window
	glfwSetCursorPosCallback(g_window, cursorPosCallback);
	glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
	glfwSetScrollCallback(g_window, scrollCallback);
	glfwSetKeyCallback(g_window, keyCallback);
	glfwSetCharCallback(g_window, charCallback);



	// Enable GL_ARB_debug_output if available. Not nessesary, just helpful
	if (glfwExtensionSupported("GL_ARB_debug_output")) {
		// This allows the error location to be determined from a stacktrace
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// Set the up callback
		glDebugMessageCallbackARB(debugCallbackARB, nullptr);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
		cout << "GL_ARB_debug_output callback installed" << endl;
	}
	else {
		cout << "GL_ARB_debug_output not available. No worries." << endl;
	}

	// Initialize IMGUI
	// Second argument is true if we dont need to use GLFW bindings for input
	// if set to false we must manually call the SimpleGUI callbacks when we
	// process the input.
	if (!SimpleGUI::init(g_window, false)) {
		cerr << "Error: Could not initialize IMGUI" << endl;
		abort();
	}


	// Initialize Geometry/Material/Lights
	// YOUR CODE GOES HERE
	// ...
	initLight();
	initTexture();
	initShader();
	initSchool();
	initWaves();
	//for fps calculation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	
	float r = 1.0; // mix ratio, for fading waves
	float dr = 0.01;


	int wave = 0;

	bool fade = true; // fade out

	ship = new Geometry("./work/res/assets/ship.obj");
	ground = new Geometry("./work/res/assets/ground2.obj");


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(g_window)) {

		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {

			frameSpeed = 1000.0 / double(nbFrames);
			fps = nbFrames;
			nbFrames = 0;
			lastTime += 1.0;
		}

			waveTime = waveTime + 0.1;

			activeBuf[wave * 6 + 1] = propsBuf[wave * 6 + 1] * r;

			if (fade) {
				r = r - dr;
				if (r <= 0) {
					r = 0 + dr;
					fade = false;
					fillProps(propsBuf, wave); // add new wave
				}
			}
			else { // fade in new wave
				r = r + dr;
				if (r > 1) {
					r = 1;
					fade = true;
					wave = wave + 1;
					if (wave > numWaves) {
						wave = 0;
					}
				}
			}


		// Make sure we draw to the WHOLE window
		int width, height;
		glfwGetFramebufferSize(g_window, &width, &height);

		// Main Render
		render(width, height);

		//Render GUI on top
		renderGUI();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}


	glfwTerminate();
}




//-------------------------------------------------------------
// Fancy debug stuff
//-------------------------------------------------------------

// function to translate source to string
string getStringForSource(GLenum source) {

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		return("API");
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return("Window System");
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return("Shader Compiler");
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return("Third Party");
	case GL_DEBUG_SOURCE_APPLICATION:
		return("Application");
	case GL_DEBUG_SOURCE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// function to translate severity to string
string getStringForSeverity(GLenum severity) {

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		return("HIGH!");
	case GL_DEBUG_SEVERITY_MEDIUM:
		return("Medium");
	case GL_DEBUG_SEVERITY_LOW:
		return("Low");
	default:
		return("n/a");
	}
}

// function to translate type to string
string getStringForType(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		return("Error");
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return("Deprecated Behaviour");
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return("Undefined Behaviour");
	case GL_DEBUG_TYPE_PORTABILITY:
		return("Portability Issue");
	case GL_DEBUG_TYPE_PERFORMANCE:
		return("Performance Issue");
	case GL_DEBUG_TYPE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// actually define the function
void APIENTRY debugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, GLvoid*) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) return;

	cerr << endl; // extra space

	cerr << "Type: " <<
		getStringForType(type) << "; Source: " <<
		getStringForSource(source) << "; ID: " << id << "; Severity: " <<
		getStringForSeverity(severity) << endl;

	cerr << message << endl;

	if (type == GL_DEBUG_TYPE_ERROR_ARB) throw runtime_error("");
}