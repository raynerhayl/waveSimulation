//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
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

#version 120

#define M_PI 3.1415926535897932384626433832795




// Constant across both shaders
uniform sampler2D texture0;
uniform float time;



// Values to pass to the fragment shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;

void main() {

// wave properties

 	float wavelength = 20;
	float amplitude = 2;
	float waveSpeed = 1.0;
	float phase = (waveSpeed * 2.0 * M_PI)/ wavelength; // wave speed
	float frequency = (2 * M_PI) /wavelength ; // angular frequency
 	vec2 d = vec2(-1,0); // direction of wave propagation

	// Transform and pass on the normal/position/texture to fragment shader
	vec4 worldPos = gl_Vertex;

	vNormal = normalize(gl_NormalMatrix * gl_Normal);
	vPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	vTextureCoord0 = gl_MultiTexCoord0.xy;

	// transform worldPos

	worldPos.y = amplitude * sin(dot(d , vec2(worldPos.x, worldPos.y) * frequency + time * phase));


	// IMPORTANT tell OpenGL where the vertex is
	gl_Position = gl_ModelViewProjectionMatrix * worldPos;
}
