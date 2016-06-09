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
varying float depth;
varying float fogFactor; 
void main()
{
	float intensity;
	vec4 color;
	vec3 n = normalize(vertex_normal);
	intensity = dot(vec3(gl_LightSource[0].position),n);
	vec4 ambient_color = gl_FrontMaterial.ambient * gl_LightSource[0].ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;
	vec4 diffuse_color = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	float sf =pow(max(dot(vertex_normal, vertex_light_half_vector), 0.0) , gl_FrontMaterial.shininess);
	sf = step(0.5, sf);
	vec4 specular_color = gl_FrontMaterial.specular * gl_LightSource[0].specular * sf;
	vec4 final = ambient_color + (gl_Color * ceil(intensity*2)/2)+ specular_color;
	float fogFactor = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
    fogFactor = clamp(fogFactor, 0.0, 1.0);
	gl_FragData[0] = mix(vec4(52/255.0,104/255.0,125/255.0,1.0), final, fogFactor);
	gl_FragData[1] = vec4(n,1);



}
