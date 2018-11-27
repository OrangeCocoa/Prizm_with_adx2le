
#include"Entity.h"
#include"Texture.h"

namespace Prizm
{
	void Entity::RunComponets(void)
	{
		for (const auto& component : components_)
		{
			if (component.second)
				component.second->Run();
			else
				components_.erase(component.first);
		}
	}

	void Entity::DrawComponents(void)
	{
		for (const auto& component : components_)
		{
			if (component.second)
				component.second->Draw();
			else
				components_.erase(component.first);
		}
	}

	void Entity::FinalizeComponets(void)
	{
		for (const auto& component : components_)
		{
			if (component.second)
				component.second->Finalize();
			else
				components_.erase(component.first);
		}
	}
}