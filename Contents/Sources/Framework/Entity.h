#pragma once

#include<unordered_map>
#include<string>

namespace Prizm
{
	class Entity
	{
	private:
		
	public:
		Entity(){}
		~Entity(){}

		virtual bool Initialize(void) = 0;
		virtual void Run(void) = 0;
		virtual void Draw(void) = 0;
		virtual void Finalize(void) = 0;
	};
}