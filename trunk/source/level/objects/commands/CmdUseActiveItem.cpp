/***
 * hesperus: CmdUseActiveItem.cpp
 * Copyright Stuart Golodetz, 2009. All rights reserved.
 ***/

#include "CmdUseActiveItem.h"

#include <source/level/objects/components/ICmpInventory.h>
#include <source/level/objects/components/ICmpUsable.h>

namespace hesp {

//#################### CONSTRUCTORS ####################
CmdUseActiveItem::CmdUseActiveItem(const ObjectID& objectID)
:	m_objectID(objectID)
{}

//#################### PUBLIC METHODS ####################
void CmdUseActiveItem::execute(const ObjectManager_Ptr& objectManager, const std::vector<CollisionPolygon_Ptr>& polygons, const OnionTree_Ptr& tree,
							   const std::vector<NavDataset_Ptr>& navDatasets, int milliseconds)
{
	ICmpInventory_Ptr cmpInventory = objectManager->get_component(m_objectID, cmpInventory);	if(!cmpInventory) return;
	ObjectID activeItem = cmpInventory->active_item();											if(!activeItem.valid()) return;
	ICmpUsable_Ptr cmpItemUsable = objectManager->get_component(activeItem, cmpItemUsable);		if(!cmpItemUsable) return;
	cmpItemUsable->use();
}

}
