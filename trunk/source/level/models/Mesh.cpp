/***
 * hesperus: Mesh.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "Mesh.h"

namespace hesp {

//#################### CONSTRUCTORS ####################
Mesh::Mesh(const std::vector<Submesh_Ptr>& submeshes)
:	m_submeshes(submeshes)
{}

//#################### PUBLIC METHODS ####################
void Mesh::render() const
{
	for(size_t i=0, size=m_submeshes.size(); i<size; ++i)
	{
		m_submeshes[i]->render();
	}
}

void Mesh::skin(const BoneConfiguration_Ptr& boneConfiguration)
{
	for(size_t i=0, size=m_submeshes.size(); i<size; ++i)
	{
		m_submeshes[i]->skin(boneConfiguration);
	}
}

}
