#pragma once

#include<memory>
#include<string>

namespace Prizm
{
	class BaseSystem
	{
		friend class BaseScene;

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		BaseSystem();
		~BaseSystem();

		bool Initialize(void);
		void Run(void);
		void Finalize(void);

		static std::string workspace_directory_;
	};
}