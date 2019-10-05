#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshGPU.hpp"

//--------------------------------------------------------------------------
/**
* Model
*/
Model::Model( RenderContext* ctx, char const* meshName )
{
	m_meshs = ctx->CreateOrGetMeshWithSubmeshes( meshName ); 
	for( int meshidx = 0; meshidx < m_meshs.size(); ++meshidx )
	{
		if ( m_meshs[meshidx] != nullptr )
		{
			m_materials.push_back( ctx->CreateOrGetMaterialFromXML( m_meshs[meshidx]->GetDefaultMaterialName().c_str() ) ); 
		}
		else
		{
			m_materials.push_back( nullptr );
		}
	}
}

//--------------------------------------------------------------------------
/**
* ~Model
*/
Model::~Model()
{

}

//--------------------------------------------------------------------------
/**
* SetMaterial
*/
void Model::SetMaterial( Material* material, unsigned int meshIndex /*= 0*/ )
{
	if( meshIndex < (unsigned int) m_materials.size() )
	{
		m_materials[meshIndex] = material;
	}
}

