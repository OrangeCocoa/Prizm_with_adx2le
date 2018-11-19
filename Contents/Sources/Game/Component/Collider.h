#pragma once

#include<DirectXCollision.h>
#include"DirectXTK\SimpleMath.h"

namespace Prizm
{
	enum OBJECT_TAG
	{
		PLAYER,
		TAG_MAX
	};

	class BoundingSphere
	{
	private:
		DirectX::BoundingSphere aabs_;
		OBJECT_TAG tag_;

	public:
		BoundingSphere(){}
		~BoundingSphere(){}
	};

	class AxisAlighnedBoundingBox
	{
	private:
		DirectX::BoundingBox aabb_;
		OBJECT_TAG tag_;

	public:
		AxisAlighnedBoundingBox(){}
		~AxisAlighnedBoundingBox(){}
	};

	class OrientedBoundingBox
	{
	private:
		DirectX::BoundingOrientedBox obb_;
		OBJECT_TAG tag_;

	public:
		OrientedBoundingBox() {}
		~OrientedBoundingBox(){}

		void Create(DirectX::SimpleMath::Vector3 center, DirectX::SimpleMath::Vector3 extents)
		{
			obb_ = DirectX::BoundingOrientedBox(center, extents, DirectX::SimpleMath::Quaternion::Identity);
		}

		void UpdateOrientation(DirectX::SimpleMath::Quaternion orientation)
		{
			obb_.Orientation = orientation;
		}

		void DrawWireFrame(void)
		{
			DirectX::SimpleMath::Vector3 corner[8];

			obb_.GetCorners(corner);


		}
	};


}