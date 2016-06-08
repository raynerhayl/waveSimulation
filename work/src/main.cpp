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
bool g_leftMouseDown = false;
vec2 g_mousePosition;
float g_pitch = 0;
float g_yaw = 0;
float g_zoom = 1.0;

BoundingBox scene_bounds = BoundingBox(vec3(-500,-500,-500),vec3(500,500,500));

//school related
School * g_school;
bool draw_school = true;

//performance
float frameSpeed = 0;
int fps = 0;

// Values and fields to showcase the use of shaders
// Remove when modifying main.cpp for Assignment 3
//
bool g_useShader = false;
GLuint g_texture = 0;
GLuint g_plainShader = 0;
GLuint g_sobelShader = 0;
GLuint g_toonShader = 0;

// Mouse Button callback
// Called for mouse movement event on since the last glfwPollEvents
//
void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	// cout << "Mouse Movement Callback :: xpos=" << xpos << "ypos=" << ypos << endl;
	if (g_leftMouseDown) {
		g_yaw -= g_mousePosition.x - xpos;
		g_pitch -= g_mousePosition.y - ypos;
	}
	g_mousePosition = vec2(xpos, ypos);
}


// Mouse Button callback
// Called for mouse button event on since the last glfwPollEvents
//
void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	// cout << "Mouse Button Callback :: button=" << button << "action=" << action << "mods=" << mods << endl;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		g_leftMouseDown = (action == GLFW_PRESS);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (g_useShader) {
			g_useShader = false;
			cout << "Using the default OpenGL pipeline" << endl;
		}
		else {
			g_useShader = true;
			cout << "Using a shader" << endl;
		}
	}
}


// Scroll callback
// Called for scroll event on since the last glfwPollEvents
//
void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	// cout << "Scroll Callback :: xoffset=" << xoffset << "yoffset=" << yoffset << endl;
	g_zoom -= yoffset * g_zoom * 0.2;
}
OctreeNode * m_octree;
Octree * m_newtree;
std::vector<Boid*> temp_boids;
int boid = 0;
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
				for(int i = 0; i != 2000; i++){
					vec3 position = vec3(math::random(scene_bounds.min.x,scene_bounds.max.x),math::random(scene_bounds.min.y,scene_bounds.max.y),math::random(scene_bounds.min.z,scene_bounds.max.z));
					Prey* b = new Prey(position);
					temp_boids.push_back(b);
					cout << b << endl;
					m_newtree->insert(b);
				}
				//cout << temp_boids.size() << endl;
			}
		break;
		case 'P':
		{
			temp_boids[boid%temp_boids.size()]->mPosition += vec3(0,0.4,0);
		}
		break;
		case 'L':
		{
			temp_boids[boid%temp_boids.size()]->mPosition += vec3(0.4,0,0);
		}
		break;
		case 'M':
		{
			temp_boids[boid%temp_boids.size()]->mPosition += vec3(0,0,0.4);
		}
		break;
		case 'I':
		{
			if(action ==1)boid++;
		}
		break;
		case 'D':
		{
			m_newtree->clear();
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

void renderGUI() {
	// Start registering GUI components
	SimpleGUI::newFrame();

	if (ImGui::IsMouseClicked(1))
		ImGui::OpenPopup("Controls");

	if (ImGui::BeginPopup("Controls")) {
		if (ImGui::Selectable("Play")) {

		}

		if (ImGui::Selectable("Pause")) {

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

// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
	float direction[] = { 0.7f, 0.7f, 1.0f, 0.0f };
	float diffintensity[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);


	glEnable(GL_LIGHT0);
}


// An example of how to load a texure from a hardcoded location
//
void initTexture() {
	Image tex("./work/res/textures/brick.jpg");

	glActiveTexture(GL_TEXTURE0); // Use slot 0, need to use GL_TEXTURE1 ... etc if using more than one texture PER OBJECT
	glGenTextures(1, &g_texture); // Generate texture ID
	glBindTexture(GL_TEXTURE_2D, g_texture); // Bind it as a 2D texture

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

	glTranslatef(0, 0, -50 * g_zoom);
	glRotatef(g_pitch, 1, 0, 0);
	glRotatef(g_yaw, 0, 1, 0);
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


// Draw function
//
void render(int width, int height) {
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;


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

	//-------------------------
	//Attach depth buffer to FBO

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);


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

	glViewport(0,0,width,height);
	// Grey/Blueish background
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	setupCamera(width, height);


		// Use the shader we made
		if (g_useShader){
			glUseProgram(g_toonShader);
		} else {
			glUseProgram(0);
		}
		float direction[] = { 0.7f, 0.7f, 1.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, direction);

	glDisable(GL_LIGHTING);
	//draw unlit stuff here
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

	if(draw_school) g_school->renderSchool();
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

	//for fps calculation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(g_window)) {

		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ) {
		    frameSpeed = 1000.0/double(nbFrames);
		    fps = nbFrames;
		    nbFrames = 0;
		    lastTime += 1.0;
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