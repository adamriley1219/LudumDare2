#pragma once
#include "Engine/Math/Matrix44.hpp"
#include <vector>

//--------------------------------------------------------------------------
class RenderContext;
class MeshGPU;
class Material;
//--------------------------------------------------------------------------

class Model
{
public: 
	Model( RenderContext* ctx, char const* meshName ); 
	~Model();

	void SetMaterial( Material* material, unsigned int meshIndex = 0 );

public:
	std::vector<MeshGPU*> m_meshs; 
	std::vector<Material*> m_materials; 
	Matrix44 m_transformMatrix = Matrix44::IDENTITY; 
};