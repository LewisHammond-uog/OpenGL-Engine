#include "Material.h"

Material::~Material()
{
	//Deallocate Textures
	delete m_pDiffuseTexture;
	delete m_pSpecularTexture;
	delete m_pNormalTexture;
}
