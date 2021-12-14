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
	void SetProjectionMatrix(const glm::mat4 a_projectionMatrix) const;
	void SetWorldCameraPos(const glm::vec3 a_camPos) const;
	void SetTime(const float a_time);

private:

	GLint m_worldViewPointLocation; //Uniform Location for WVP Matrix
	GLint m_worldProjectionMatrixLocation;
	GLint m_worldCamPosLocation;
	GLint m_timeLocation; //Uniform Location for time 

};

#endif //__WATER_PROGRAM_H__
