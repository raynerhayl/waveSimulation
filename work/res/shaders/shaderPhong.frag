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
uniform sampler2D texture1;

uniform float time;


// Values passed in from the vertex shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;



void main() {


	vec4 color = vec4(0.0);
	vec4 diffuse = gl_LightSource[0].diffuse * max(dot(normalize(gl_NormalMatrix * vNormal),normalize(gl_LightSource[0].position.xyz)),0.0);

	color = diffuse * gl_FrontMaterial.diffuse;

	//color = color * 0.3 * abs(1.0-vPosition.y);
	//vec4 texColor = vec4(texture2D(texture0, vec2(vPosition.x/500.0,vPosition.y/500.0)).rgb,1.0);

	//color = vec4(0.0);
	//color.x = texColor.y;

	gl_FragColor = (color);



	}
