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

uniform float viewportWidth;
uniform float viewportHeight;

uniform float time;


// Values passed in from the vertex shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;
varying float caustic;



void main() {
	vec3 CAMERA_POSITION = gl_ModelViewMatrixInverse[3].xyz;

	vec4 specular;
	vec4 diffuse;
	vec3 norm = normalize(vNormal); //Important: after interpolation normal modulus != 1.
	vec3 bnorm = texture2D(texture0, vTextureCoord0).rgb * 2.0 - 1.0;

	vec3 lightVector = gl_LightSource[0].position.xyz;
	float dist = length(lightVector);
	float attenuation = 1.0 / (gl_LightSource[0].constantAttenuation + gl_LightSource[0].linearAttenuation * dist + gl_LightSource[0].quadraticAttenuation * dist * dist);
	//attenuation = 1.0;
	lightVector = normalize(lightVector);
	float nxDir = max(0.0, dot(lightVector, norm));
	float bnxDir = max(0.0, dot(lightVector, bnorm));

	diffuse =  gl_LightSource[0].diffuse *( nxDir + bnxDir) ;

	if(nxDir != 0.0)
	{
			vec3 cameraVector = normalize(CAMERA_POSITION - vPosition.xyz);
			vec3 halfVector = normalize(lightVector + cameraVector);
			float nxHalf = max(0.0,dot(norm, halfVector));
			float specularPower = pow(nxHalf, gl_FrontMaterial.shininess);
			specular =  gl_LightSource[0].specular * specularPower ;
	}

	//vec2 screenspace = vec2(gl_FragCoord.x/viewportWidth + 1,gl_FragCoord.y/viewportHeight + 1);
	//vec4 texColor = vec4(texture2D(texture0, screenspace).rgb,1.0);
	vec4 texColor = vec4(1.0);

	//diffuse.xyz = diffuse.xyz + 0.2  * max(vPosition.y/3.0,0.0);

	gl_FragColor = gl_FrontMaterial.ambient *gl_LightSource[0].ambient + (gl_FrontMaterial.diffuse *diffuse * vec4(texColor.rgb,1.0)) + (gl_FrontMaterial.specular *specular * texColor.a);
	}
