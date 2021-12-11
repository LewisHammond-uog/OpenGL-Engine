#ifndef __SHADOW_PROGRAM_H__
#define __SHADOW_PROGRAM_H__


#include <glm/detail/type_mat.hpp>

#include "ShaderProgram.h"

class ShadowProgram : public ShaderProgram
{
public:
	ShadowProgram() = default;

	bool Initialise() override;

	void SetLightViewPoint(glm::mat4 a_matrix);

private:
	GLint m_lightViewPointLocation;//Uniform Location for WVP Matrix
	GLint m_depthSamplerLocation;  //Uniform Location for the shadow texture
}; 

#endif // __SHADOW_PROGRAM_H__
