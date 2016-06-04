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

// Constant across both shaders
uniform sampler2D texture0;
uniform float time;


// Values passed in from the vertex shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;

void main() {

	// Can do all sorts of cool stuff here
	vec3 color = vec3(0,0,0);//texture2D(texture0, vTextureCoord0).rgb;

	color.x = vNormal.x;

	// IMPORTANT tell OpenGL what the final color of the fragment is (vec4)
	gl_FragColor = vec4(color, 1);
}
