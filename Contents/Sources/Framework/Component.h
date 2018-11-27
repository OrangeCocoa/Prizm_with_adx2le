#pragma once

#include<memory>

namespace Prizm
{
	class Entity;

	class Component
	{
	private:
		std::weak_ptr<Entity> owner_;

	public:
		Component(){}
		virtual ~Component() = 0;

		virtual bool Initialize(void) = 0;
		virtual void Run(void) = 0;
		virtual void Draw(void) = 0;
		virtual void Finalize(void) = 0;
		
		void SetOwner(std::shared_ptr<Entity>& entity);
		std::shared_ptr<Entity> GetOwner(void);
	};
}