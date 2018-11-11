#pragma once

#include<vector>
#include<memory>

namespace Prizm
{
	template<class _ResourceType>
	class ResourcePool
	{
	private:
		std::vector<std::shared_ptr<_ResourceType>> resources_;
		std::vector<unsigned int> reuce_; // reuce a resource index queue

	public:
		const std::shared_ptr<_ResourceType>& Get(unsigned int index)
		{
			return resources_[index];
		}

		const unsigned int Load(const std::shared_ptr<_ResourceType>& resource)
		{
			if (reuce_.size())
			{
				auto& reuse_index = reuce_.back();
				reuce_.pop_back();

				resources_[reuse_index] = std::move(resource);
				return reuse_index;
			}

			resources_.emplace_back(std::move(resource));
			return static_cast<unsigned int>(resources_.size() - 1);
		}

		void Release(const unsigned int& index)
		{
			resources_[index] = nullptr;
			reuce_.emplace_back(index);
		}

		const unsigned int Size(void)
		{
			return [&]()
			{
				unsigned int i = 0;
				for (auto& resource : resources_)
				{
					if (resource) ++i;
				}
				return i;
			}();
		}
	};
}