#include "Light.h"

namespace Prizm
{
	class Light::Impl
	{
	public:
		PointLight point_light_;
	};

	Light::Light() : impl_(std::make_unique<Impl>()){}

	Light::~Light() = default;


}
