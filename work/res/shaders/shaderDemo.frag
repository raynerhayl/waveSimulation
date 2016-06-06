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

vec3 colorFromLight(int, vec3, vec3);


void main() {

	// Can do all sorts of cool stuff here
	vec3 color = vec3(0,0,0);//texture2D(texture0, vTextureCoord0).rgb;

	color = colorFromLight(0, vPosition, vNormal);


	// IMPORTANT tell OpenGL what the final color of the fragment is (vec4)
	gl_FragColor = vec4(color, 0.1);
	
}

vec3 colorFromLight(int light_idx, vec3 viewspace_position, vec3 viewspace_normal) {
	vec3 light_viewspace_position =
	gl_LightSource[light_idx].position.xyz;
	vec3 light_direction = normalize(light_viewspace_position -
	viewspace_position);

	// Ambient
	vec3 ambient = gl_LightSource[light_idx].ambient.rgb * gl_FrontMaterial.ambient.rgb;

	// Diffuse
	float s_dot_n = max(dot(light_direction, viewspace_normal), 0.0);
	vec3 diffuse = gl_LightSource[light_idx].diffuse.rgb *
	gl_FrontMaterial.diffuse.rgb * s_dot_n;
	return ambient + diffuse;
}

