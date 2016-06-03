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
uniform float [100] waveProperties;
uniform int numWaves;

vec4 worldPos;

// Values to pass to the fragment shader
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTextureCoord0;


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

float height(sWave wave);

vec4 gerstnerFun(sWave wave);

vec3 gerstnerNorm(sWave wave, vec2 worldPlane);


void createWaves();

void main() {

// wave properties

	createWaves();

	// Transform and pass on the normal/position/texture to fragment shader
	worldPos = gl_Vertex;

	vNormal = normalize(gl_NormalMatrix * gl_Normal);
	vPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	vTextureCoord0 = gl_MultiTexCoord0.xy;

	// transform worldPos
	//vec4 normTemp = vec4(0,0,0,0);

	for(int i = 0; i < numWaves; i ++){
	//	worldPos.y = worldPos.y + height(waves[i]);
		vec4 gerstner = gerstnerFun(waves[i]);
		vec3 gerstnerNorm = gerstnerNorm(waves[i], vec2(gerstner.x,gerstner.z));

		worldPos.x = worldPos.x + gerstner.x;
		worldPos.y = worldPos.y + gerstner.y;
		worldPos.z = worldPos.z + gerstner.z;
		worldPos.w = worldPos.w + gerstner.w;

		//normTemp.x = normTemp.x + gerstnerNorm.x;
		//normTemp.y = normTemp.y + gerstnerNorm.y;
		//normTemp.z = normTemp.z + gerstnerNorm.z;
	}



	// IMPORTANT tell OpenGL where the vertex is
	gl_Position = gl_ModelViewProjectionMatrix * worldPos;
	//gl_Normal = normTemp;
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

		waves[i] = sWave(wavelength, amplitude, waveSpeed, phase, frequency, d, steepness);
	}
}

float height(sWave wave){
	float height = 0.0;

	height = wave.amplitude * sin(dot(wave.d , vec2(worldPos.x, worldPos.z) * wave.frequency + time * wave.phase));

	return height;
}

vec4 gerstnerFun(sWave wave){
	vec4 result = vec4(0,0,0,0);

	float steepF = wave.steepness * 1/(wave.frequency * wave.amplitude);

	//result.x = worldPos.x + steepF * wave.amplitude * wave.d.x * cos(wave.frequency * dot( wave.d , vec2(worldPos.x, worldPos.y) + time * wave.phase));
	//result.y = worldPos.y + steepF * wave.amplitude * wave.d.y * cos(wave.frequency *dot( wave.d , vec2(worldPos.x, worldPos.y)  + time * wave.phase));
	//result.z = wave.amplitude * sin( wave.frequency *dot(wave.d , vec2(worldPos.x, worldPos.y)  + time * wave.phase)); // cos gives a nice effect


	result.x = steepF * wave.amplitude * wave.d.x * cos(dot(wave.frequency *  wave.d , vec2(worldPos.x, worldPos.z) + time * wave.phase));
	result.y = wave.amplitude * sin( dot(wave.frequency *wave.d , vec2(worldPos.x, worldPos.z)  + time * wave.phase)); // cos gives a nice effect
	result.z = steepF * wave.amplitude * wave.d.y * cos(dot( wave.frequency *wave.d , vec2(worldPos.x, worldPos.z)  + time * wave.phase));

	result.w = worldPos.w;

	return result;
}

vec3 gerstnerNorm(sWave wave, vec2 worldPlane){
	vec3 result = vec3(0,0,0);

	float steepF = wave.steepness * 1/(wave.frequency * wave.amplitude);

	result.x = -wave.d.x * wave.frequency * wave.amplitude * cos(dot(wave.frequency * wave.d, worldPlane) + wave.phase * time);
	result.y = 1 - steepF * wave.frequency * wave.amplitude * sin(dot(wave.frequency * wave.d, worldPlane) + wave.phase * time);
	result.z = -wave.d.y * wave.frequency * wave.amplitude * cos(dot(wave.frequency * wave.d, worldPlane) + wave.phase * time);

	return result;

}
