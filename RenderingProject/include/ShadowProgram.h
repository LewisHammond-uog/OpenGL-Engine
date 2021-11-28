#ifndef __SHADOW_PROGRAM_H__
#define __SHADOW_PROGRAM_H__


#include "ShaderProgram.h"

class ShadowProgram : public ShaderProgram
{
public:
	ShadowProgram() = default;

	bool Initialise() override;

private:
	GLint m_worldViewPointLocation;//Uniform Location for WVP Matrix
	GLint m_depthSamplerLocation;  //Uniform Location for the shadow texture
}; 

#endif // __SHADOW_PROGRAM_H__
