#ifndef __GEOMETRY_PROGRAM_H__
#define __GEOMETRY_PROGRAM_H__

#include "ShaderProgram.h"


class GeometryProgram : public ShaderProgram
{
public:
	GeometryProgram() = default;

	bool Initialise() override;

private:

	GLint m_projectionViewLocation;
	GLint m_cameraWorldPosLocation;
	GLint m_textureLocation;

};

#endif //__GEOMETRY_PROGRAM_H__
