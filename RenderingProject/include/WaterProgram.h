#ifndef __WATER_PROGRAM_H__
#define __WATER_PROGRAM_H__

#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>

#include "ShaderProgram.h"
class WaterProgram : public ShaderProgram
{
public:
	WaterProgram() = default;
	bool Initialise() override;

	void SetWorldViewPoint(const glm::mat4 a_worldViewPoint) const;
	void SetWorldCameraPos(const glm::vec3 a_camPos) const;
	void SetTime(const float a_time);
	void SetLightDirection(const glm::vec3 a_lightDirection) const;
	void SetLightColour(const glm::vec3 a_lightColour) const;

private:

	GLint m_worldViewPointLocation; //Uniform Location for WVP Matrix
	GLint m_worldCamPosLocation; //Uniform Location for the position of the camer
	GLint m_timeLocation; //Uniform Location for time
	GLint m_lightDirectionLocation; //Uniform location for the direction of the light lighting the water
	GLint m_lightColourLocation; //Uniform location for the colour of the light lighting the water


};

#endif //__WATER_PROGRAM_H__
