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

	//vNormal = normalize(vNormal);

	vec4 color = vec4(0.0);
	
	vec3 L = normalize(gl_LightSource[0].position.xyz);
	vec3 norm = normalize(vNormal);
	vec4 diff = gl_FrontLightProduct[0].diffuse * max(dot(norm,L),0.0);

	vec4 amb = gl_FrontLightProduct[0].ambient;
	color = diff + amb;

	gl_FragColor = color;


/*	// Can do all sorts of cool stuff here
	vec3 color = texture2D(texture1, vTextureCoord0).rgb;

	//color = colorFromLight(0, vPosition, vNormal);
	// IMPORTANT tell OpenGL what the final color of the fragment is (vec4)
	//gl_FragColor = vec4(color, 0.1);

	vec4 finalColor = vec4(color.rgb,0.0);


   for (int i=0;i<1;i++)
   {
      vec3 L = normalize(gl_LightSource[i].position.xyz); 
      vec3 E = normalize(-vPosition); // we are in Eye Coordinates, so EyePos is (0,0,0) 
      vec3 R = normalize(-reflect(L,vNormal)); 
   
      //calculate Ambient Term: 
      vec4 Iamb = gl_FrontLightProduct[i].ambient; 
      //calculate Diffuse Term: 
      vec4 Idiff = gl_FrontLightProduct[i].diffuse * max(dot(vNormal,L), 0.0);
      Idiff = clamp(Idiff, 0.0, 1.0); 
   
      // calculate Specular Term:
      vec4 Ispec = gl_FrontLightProduct[i].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
      Ispec = clamp(Ispec, 0.0, 1.0); 
   
      finalColor += Iamb + Idiff + Ispec;
   }
   
   // write Total Color: 
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor; 			
	

	vec3 CAMERA_POSITION = gl_ModelViewMatrixInverse[3].xyz;

	vec4 specular = vec4(0.0);
	vec4 diffuse;
	vec3 norm = normalize(vNormal); //Important: after interpolation normal modulus != 1.
	vec3 lightVector = gl_LightSource[0].position.xyz;
	float dist = length(lightVector);
	float attenuation = 1.0 / (gl_LightSource[0].constantAttenuation + gl_LightSource[0].linearAttenuation * dist + gl_LightSource[0].quadraticAttenuation * dist * dist);
	attenuation = 1.0;
	lightVector = normalize(lightVector);
	float nxDir = max(0.0, dot(lightVector, norm));
	diffuse =  gl_LightSource[0].diffuse * nxDir ;

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

	gl_FragColor = gl_FrontMaterial.ambient *gl_LightSource[0].ambient + (gl_FrontMaterial.diffuse *diffuse * vec4(texColor.rgb,1.0)) + (gl_FrontMaterial.specular *specular * texColor.a);	*/

	}
