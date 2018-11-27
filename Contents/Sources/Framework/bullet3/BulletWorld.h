#pragma once

#include<memory>
#include<vector>

#include<btBulletDynamicsCommon.h>
#include"BulletDebugDraw.h"

namespace Prizm
{
	class BulletWorld
	{
	public:
		BulletWorld(void);
		~BulletWorld(void);

		bool Initialize(void);
		void Update(void);
		void Finalize(void);

		void SetDebugDraw(btIDebugDraw* draw);
		void DebugDraw(void);

		struct RBInfo
		{
			btScalar mass;
			btVector3 pos;
			btVector3 scale;

			RBInfo() 
				: mass(.0f)
				, pos(btVector3(.0f, .0f, .0f)){}
		};

		void AddAABB(RBInfo& info);

		enum CollisionType
		{
			UNKNOWN,
			AABB,		// btBoxShape
			OBB,		// btConvexHullShape
			SPHERE,		// btSphereShape
			CAPSULE,	// btCapsuleShape
			CYLINDER,	// btCylinderShape
			CONE,		// btConeShape
		};

		struct ObjectSync
		{
			CollisionType type;

			ObjectSync()
				: type(UNKNOWN){}
		};

		std::shared_ptr<ObjectSync> object_sync_;

		

	private:
		void AddRigidbody(std::shared_ptr<btCollisionShape>& shape, btScalar mass);

	public:

	};
}