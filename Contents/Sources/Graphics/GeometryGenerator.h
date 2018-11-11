#pragma once

#include"Geometry.h"

namespace Prizm
{
	namespace GeometryGenerator
	{
		Geometry Triangle2D(float scale, float center_x, float center_y);
		Geometry Triangle3D(float scale);
		Geometry Quad2D(float width, float height, float center_x, float center_y);
		Geometry Quad3D(float scale);
		Geometry QuadFieldStrip(float width, float height);
		Geometry QuadFieldList(float width, float height);
		Geometry Cube(void);
		Geometry Sphere(float radius, unsigned ring_count, unsigned slice_count);
		Geometry Grid(float width, float depth, unsigned m, unsigned n);
		Geometry Cylinder(float height, float top_radius, float bottom_radius, unsigned slice_count, unsigned stack_count);
	};
}