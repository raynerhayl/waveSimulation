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

varying vec3 normal;
varying vec3 vertex_light_position;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
varying vec4 direction;
void main()
{
	float intensity;
	vec4 color;
	vec3 n = normalize(vertex_normal);
	intensity = dot(vec3(gl_LightSource[0].position),n);
	vec4 ambient_color = gl_FrontMaterial.ambient * gl_LightSource[0].ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;
	vec4 diffuse_color = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	vec4 specular_color = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(max(dot(vertex_normal, vertex_light_half_vector), 0.0) , gl_FrontMaterial.shininess);
	float black = 1;
	if (dot(vec3(normalize(direction)), vertex_normal) < 0.25){
		black = 0;
	}
	gl_FragColor = (ambient_color + diffuse_color) * (floor(intensity*5)/5) * black;



}
