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
uniform sampler2D texture1;
uniform float time;
uniform float [200] waveProperties;
uniform int numWaves;
uniform float frameWidth = 200;

uniform float viewportWidth;
uniform float viewportHeight;


vec4 worldPos;

// Values to pass to the fragment shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;
varying float caustic;


struct sWave
{
	float wavelength;
	float amplitude;
	float waveSpeed;
	float phase;
	float frequency;
	vec2 d;
	float steepness;
};

sWave[100] waves;

vec4 gerstnerFun(sWave wave);

vec3 gerstnerNorm(sWave wave, vec2 worldPlane);

void createWaves();

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {

// wave properties


	// Transform and pass on the normal/position/texture to fragment shader
	worldPos = vec4(gl_Vertex.xyzw);

	createWaves();
	vNormal = vec3(0.0);


	// calculate vertex position
	for(int i = 0; i < numWaves; i ++){
		vec4 gerstner = gerstnerFun(waves[i]);

		worldPos.x = worldPos.x + gerstner.x;
		worldPos.y = worldPos.y + gerstner.y;
		worldPos.z = worldPos.z + gerstner.z;

	}

	worldPos.y = worldPos.y +  rand(vec2(0.0,1.0));

	// calculate normal 
	for(int i = 0; i < numWaves; i ++){
		vec3 gerstnerNorm = (gerstnerNorm(waves[i], vec2(worldPos.x,worldPos.z)));

		vNormal.x = gerstnerNorm.x + vNormal.x;
		vNormal.y = gerstnerNorm.y + vNormal.y;
		vNormal.z = gerstnerNorm.z + vNormal.z;

	}


	vNormal.y = 1 - vNormal.y;
	vNormal = vNormal;

	vPosition = worldPos.xyz;

	vTextureCoord0 = vPosition.xz/50.0;
	
	// IMPORTANT tell OpenGL where the vertex is
	gl_Position = gl_ModelViewProjectionMatrix * worldPos;
	
}

/*
Creates a wave struct from the given properties, the vector contains the 
wavelength, amplitude and waveSpeed in that order
*/

void createWaves(){

	for(int i = 0; i < numWaves; i ++){
		int index = i * 6;

		float wavelength = waveProperties[index];
		float amplitude = waveProperties[index + 1];
		float waveSpeed = waveProperties[index + 2];
		float phase = (waveSpeed * 2.0 * M_PI)/ wavelength; // wave speed
		float frequency = (2 * M_PI) /wavelength ; // angular frequency
 		vec2 d = normalize(vec2(waveProperties[index + 3],waveProperties[index + 4])); // direction of wave propagation
		float steepness = waveProperties[index + 5];
 
		// circular waves
		//d = ((vec2(worldPos.x, worldPos.z) - vec2(1,0))/length((vec2(worldPos.x, worldPos.z) - vec2(1,0))));
		

		waves[i] = sWave(wavelength, amplitude, waveSpeed, phase, frequency, d, steepness);
	}
}


vec4 gerstnerFun(sWave wave){
	vec4 result = vec4(0,0,0,0);

	float term = wave.frequency * dot(wave.d, vec2(worldPos.x, worldPos.z)) + time * wave.phase;


	float steepF = wave.steepness * 1/(wave.frequency * wave.amplitude);

	result.x = steepF * wave.amplitude * wave.d.x * cos(term);
	result.y = wave.amplitude * sin(term); // cos gives a nice effect
	result.z = steepF * wave.amplitude * wave.d.y * cos(term);

	result.w = worldPos.w;

	return result;
}

vec3 gerstnerNorm(sWave wave, vec2 worldPlane){
	vec3 result = vec3(0,0,0);

	float steepF = wave.steepness * 1/(wave.frequency * wave.amplitude);

	float term =wave.frequency * dot( wave.d, vec2(worldPos.x, worldPos.z)) + wave.phase * time;

	result.x = -wave.d.x * wave.frequency * wave.amplitude * cos(term);
	result.y = steepF * wave.frequency * wave.amplitude * sin(term);
	result.z = -wave.d.y * wave.frequency * wave.amplitude * cos(term);

	return result;
}
