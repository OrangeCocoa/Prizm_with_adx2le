
#include"Component.h"

namespace Prizm
{
	void Component::SetOwner(std::shared_ptr<Entity>& entity)
	{
		owner_ = entity;
	}

	std::shared_ptr<Entity> Component::GetOwner(void)
	{
		auto owner = owner_.lock();
		return owner;
	}
}