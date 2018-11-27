
#include"BulletWorld.h"

namespace Prizm
{
	BulletWorld::BulletWorld(void){}

	BulletWorld::~BulletWorld(void)
	{
		Finalize();
	}

	bool BulletWorld::Initialize(void)
	{

		return true;
	}

	void BulletWorld::Update(void)
	{

	}

	void BulletWorld::Finalize(void)
	{

	}

	void BulletWorld::SetDebugDraw(btIDebugDraw* draw)
	{

	}

	void BulletWorld::DebugDraw(void)
	{

	}
}