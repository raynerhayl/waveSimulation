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
uniform vec4 cameraPos;
varying vec3 vertex_light_position;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
varying vec4 direction;
void main() {            
	vec4 vertexWorld = gl_ModelViewMatrix * gl_Vertex;
	vec4 cameraWorld = gl_ModelViewMatrix * cameraPos;
	direction = normalize(cameraWorld-vertexWorld);
    vertex_normal = normalize(gl_NormalMatrix * gl_Normal);
    // Calculate the light position for this vertex
    vertex_light_position = normalize(gl_LightSource[0].position.xyz);
    // Calculate the light’s half vector
    vertex_light_half_vector = normalize(gl_LightSource[0].halfVector.xyz);
    // Set the front color to the color passed through with glColor*f
    gl_FrontColor = gl_Color;
    // Set the position of the current vertex 
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}