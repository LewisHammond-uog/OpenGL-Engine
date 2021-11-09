#include "Material.h"

#include "Texture.h"

Material::~Material()
{
	//Deallocate Textures
	delete m_pDiffuseTexture;
	delete m_pSpecularTexture;
}
