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

#include "wave.hpp"
#include "simple_gui.hpp"

#include "shady_geometry.hpp"
#include "opengl.hpp"
#include "school.hpp"
#include "helpers.hpp"

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

BoundingBox scene_bounds = BoundingBox(vec3(-100, -100, -100), vec3(100, 100, 100));

//school related
School * g_school;
bool draw_school = true;

//wave related
Wave * wave;
float waveTime = 0.0;


//performance
float frameSpeed = 0;
int fps = 0;

// Values and fields to showcase the use of shaders
// Remove when modifying main.cpp for Assignment 3
//
bool g_useShader = true;
GLuint g_texture = 0;
GLuint g_shader = 0;

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


// Keyboard callback
// Called for every key event on since the last glfwPollEvents
//
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
	// cout << "Key Callback :: key=" << key << "scancode=" << scancode
	// 	<< "action=" << action << "mods=" << mods << endl;
	// YOUR CODE GOES HERE
	// ...
	switch (key) {
	case 'F':
		if (action == 1)draw_school = !draw_school;
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



	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::Begin("Fixed Overlay", nullptr, ImVec2(0, 0), 0.3f,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ostringstream ss;
	// Replace this with your code
	ss << frameSpeed << " ms/frame  " << fps << " fps";
	ImGui::Text(ss.str().c_str());
	ImGui::End();

	// Flush components and render
	SimpleGUI::render();
}

void initSchool() {
	g_school = new School(200, scene_bounds);
}

void initWave() {
	wave = new Wave();
}

// Sets up where and what the light is
// Called once on start up
//
void initLight() {
	float direction[] = { 0.0f, 0.0f, 1.0f, 0.0f };
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
	g_shader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "./work/res/shaders/shaderDemo.vert", "./work/res/shaders/shaderDemo.frag" });
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



void drawOrigin() {
	//x
	glColor3f(1, 0, 0);
	glPushMatrix(); {
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 50);
		cgraLine(100);
	}glPopMatrix();
	//y
	glColor3f(0, 1, 0);
	glPushMatrix(); {
		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 0, 50);
		cgraLine(100);
	}glPopMatrix();
	//z
	glColor3f(0, 0, 1);
	glPushMatrix(); {
		glTranslatef(0, 0, 50);
		cgraLine(100);
	}glPopMatrix();
}


// Draw function
//
void render(int width, int height) {
	glViewport(0, 0, width, height);
	// Grey/Blueish background
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	setupCamera(width, height);

	glDisable(GL_LIGHTING);
	//draw unlit stuff here
	drawOrigin();
	glColor3f(1, 1, 1);
	cgraCube(
		vec3((scene_bounds.max.x + scene_bounds.min.x) / 2,
			(scene_bounds.max.y + scene_bounds.min.y) / 2,
			(scene_bounds.max.z + scene_bounds.min.z) / 2), vec3(
				abs(scene_bounds.max.x - scene_bounds.min.x),
				abs(scene_bounds.max.y - scene_bounds.min.y),
				abs(scene_bounds.max.z - scene_bounds.min.z)
				));
	if (draw_school) {
		g_school->renderSchool();
	}

	glEnable(GL_LIGHTING);

	// Without shaders
	// Uses the default OpenGL pipeline
	//
	if (!g_useShader) {

		// // Texture setup
		// //

		// // Enable Drawing texures
		// glEnable(GL_TEXTURE_2D);
		// // Use Texture as the color
		// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		// // Set the location for binding the texture
		// glActiveTexture(GL_TEXTURE0);
		// // Bind the texture
		// glBindTexture(GL_TEXTURE_2D, g_texture);

		// // Render a single square as our geometry
		// // You would normally render your geometry here
		// glBegin(GL_QUADS);
		// glNormal3f(0.0, 0.0, 1.0);
		// glTexCoord2f(0.0, 0.0);
		// glVertex3f(-5.0, -5.0, 0.0);
		// glTexCoord2f(0.0, 1.0);
		// glVertex3f(-5.0, 5.0, 0.0);
		// glTexCoord2f(1.0, 1.0);
		// glVertex3f(5.0, 5.0, 0.0);
		// glTexCoord2f(1.0, 0.0);
		// glVertex3f(5.0, -5.0, 0.0);
		// glEnd();
		// glFlush();
	}


	// With shaders (no lighting)
	// Uses the shaders that you bind for the graphics pipeline
	//
	else {

		// Texture setup

		// Enable Drawing texures
		glEnable(GL_TEXTURE_2D);
		// Set the location for binding the texture
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, g_texture);

		// Use the shader we made
		glUseProgram(g_shader);

		// Set our sampler (texture0) to use GL_TEXTURE0 as the source
		glUniform1i(glGetUniformLocation(g_shader, "texture0"), 0);

		glUniform1f(glGetUniformLocation(g_shader, "time"), waveTime);

		int waves = 1 * 5; // number of wave properties

		GLfloat props[100];

		// populate wave property array

		/*for (int i = 0; i < waves; i = i + 5) {
			props[i] = 20;
			props[i + 1] = 2.0;
			props[i + 2] = 1.0;
			props[i + 3] = -1.0;
			props[i + 4] = 0.0;
		}*/

		int i = 0;

		props[i] = 20;	     // wavelength
		props[i + 1] = 2.0;  // amplitude
		props[i + 2] = 1.0;  // velocity
		props[i + 3] = -1.0; // direction x
		props[i + 4] = 0.0;  // direction y

		i = i + 5;

		props[i] = 20;	    
		props[i + 1] = 2.0;  
		props[i + 2] = -1.0; 
		props[i + 3] = 1.0; 
		props[i + 4] = 1.0;  
		
		// repeate for next wave

		glUniform1fv(glGetUniformLocation(g_shader, "waveProperties"), 100, props);

		glUniform1i(glGetUniformLocation(g_shader, "numWaves"), 1);  // specify the number of waves




		// Render a single square as our geometry
		// You would normally render your geometry here

		if (draw_school == false) {
			wave->render();

		}

		glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-5.0, -5.0, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-5.0, 5.0, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(5.0, 5.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(5.0, -5.0, 0.0);
		glEnd();
		glFlush();

		// Unbind our shader
		glUseProgram(0);
	}


	// Disable flags for cleanup (optional)
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
}


// Forward decleration for cleanliness (Ignore)
void APIENTRY debugCallbackARB(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);


//Main program
//
int main(int argc, char **argv) {

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
	initWave();


	//for fps calculation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

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

		waveTime=waveTime + 0.1;

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
