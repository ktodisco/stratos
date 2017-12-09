/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_intersection.h"

#include "st_shape.h"

#include <cassert>
#include <float.h>
#include <vector>

float distance_to_plane(const st_vec3f& point, const st_plane* plane)
{
	return plane->_normal.dot(point) - plane->_normal.dot(plane->_point);
}

float distance_to_line_segment(const st_vec3f& point, const st_vec3f& a, const st_vec3f& b)
{
	st_vec3f cross = st_vec3f_cross((point - a), (point - b));
	st_vec3f segment = (b - a);

	return (cross.mag() / segment.mag());
}

bool closest_points_on_lines(
	const st_vec3f& start_a,
	const st_vec3f& end_a,
	const st_vec3f& start_b,
	const st_vec3f& end_b,
	st_vec3f& point_a,
	st_vec3f& point_b)
{
	// Algorithm from http://geomalgorithms.com/a07-_distance.html
	st_vec3f u_o = (end_a - start_a);
	st_vec3f u = u_o.normal();
	st_vec3f v_o = (end_b - start_b);
	st_vec3f v = v_o.normal();
	st_vec3f w0 = (start_a - start_b);

	float a = u.dot(u);
	float b = u.dot(v);
	float c = v.dot(v);
	float d = u.dot(w0);
	float e = v.dot(w0);

	float sc = (b * e - c * d) / (a * c - b * b);
	float tc = (a * e - b * d) / (a * c - b * b);

	point_a = start_a + u.scale_result(sc);
	point_b = start_b + v.scale_result(tc);

	return (sc >= 0.0f) && (sc <= u_o.mag()) && (tc >= 0.0f) && (tc <= v_o.mag());
}

st_vec3f farthest_along_vector(const std::vector<st_vec3f>& points, const st_vec3f& vector)
{
	float max_dot = -FLT_MAX;
	st_vec3f best;

	for (uint32_t i = 0; i < points.size(); ++i)
	{
		st_vec3f point = points[i];
		if (point.dot(vector) > max_dot)
		{
			max_dot = point.dot(vector);
			best = point;
		}
	}

	return best;
}

bool intersection_unimplemented(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	assert(false);
	return false;
}

bool sphere_vs_plane(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	// Figure out which shape is which.
	st_sphere sphere;
	st_plane plane;

	if (a->get_type() == k_shape_sphere)
	{
		sphere = *reinterpret_cast<const st_sphere*>(a);
		sphere._center += transform_a.get_translation();
		plane = *reinterpret_cast<const st_plane*>(b);
		plane._normal = transform_b.transform_vector(plane._normal);
		plane._point += transform_b.get_translation();
	}
	else
	{
		sphere = *reinterpret_cast<const st_sphere*>(b);
		sphere._center += transform_b.get_translation();
		plane = *reinterpret_cast<const st_plane*>(a);
		plane._normal = transform_a.transform_vector(plane._normal);
		plane._point += transform_a.get_translation();
	}

	float distance = distance_to_plane(sphere._center, &plane);

	return distance < sphere._radius;
}

bool oobb_vs_plane(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	// Figure out which shape is which.
	st_oobb oobb;
	st_plane plane;

	if (a->get_type() == k_shape_oobb)
	{
		oobb = *reinterpret_cast<const st_oobb*>(a);
		oobb._center += transform_a.get_translation();
		oobb._half_vectors[0] = transform_a.transform_vector(oobb._half_vectors[0]);
		oobb._half_vectors[1] = transform_a.transform_vector(oobb._half_vectors[1]);
		oobb._half_vectors[2] = transform_a.transform_vector(oobb._half_vectors[2]);
		plane = *reinterpret_cast<const st_plane*>(b);
		plane._normal = transform_b.transform_vector(plane._normal);
		plane._point += transform_b.get_translation();
	}
	else
	{
		oobb = *reinterpret_cast<const st_oobb*>(b);
		oobb._center += transform_b.get_translation();
		oobb._half_vectors[0] = transform_b.transform_vector(oobb._half_vectors[0]);
		oobb._half_vectors[1] = transform_b.transform_vector(oobb._half_vectors[1]);
		oobb._half_vectors[2] = transform_b.transform_vector(oobb._half_vectors[2]);
		plane = *reinterpret_cast<const st_plane*>(a);
		plane._normal = transform_a.transform_vector(plane._normal);
		plane._point += transform_a.get_translation();
	}

	// Project each of the half vectors onto the plane's normal to get its 'radius.'
	st_vec3f projection =
		oobb._half_vectors[0].project_onto_abs(plane._normal) +
		oobb._half_vectors[1].project_onto_abs(plane._normal) +
		oobb._half_vectors[2].project_onto_abs(plane._normal);
	float radius = projection.mag();

	float distance = distance_to_plane(oobb._center, &plane);

	bool collision = distance < radius;
	if (collision)
	{
		info->_penetration = radius - distance;
		info->_normal = plane._normal;

		const int32_t k_num_corners = 8;
		// We can find the collision point by finding the point penetrating farthest into the plane.
		st_vec3f corners[k_num_corners];
		corners[0] = (oobb._center + oobb._half_vectors[0] + oobb._half_vectors[1] + oobb._half_vectors[2]);
		corners[1] = (oobb._center + oobb._half_vectors[0] + oobb._half_vectors[1] - oobb._half_vectors[2]);
		corners[2] = (oobb._center + oobb._half_vectors[0] - oobb._half_vectors[1] + oobb._half_vectors[2]);
		corners[3] = (oobb._center + oobb._half_vectors[0] - oobb._half_vectors[1] - oobb._half_vectors[2]);
		corners[4] = (oobb._center - oobb._half_vectors[0] + oobb._half_vectors[1] + oobb._half_vectors[2]);
		corners[5] = (oobb._center - oobb._half_vectors[0] + oobb._half_vectors[1] - oobb._half_vectors[2]);
		corners[6] = (oobb._center - oobb._half_vectors[0] - oobb._half_vectors[1] + oobb._half_vectors[2]);
		corners[7] = (oobb._center - oobb._half_vectors[0] - oobb._half_vectors[1] - oobb._half_vectors[2]);

		float pens[k_num_corners];
		float max_pen = FLT_MAX;
		for (int i = 0; i < k_num_corners; ++i)
		{
			pens[i] = distance_to_plane(corners[i], &plane);
			// Maximum intersection is defined by the most negative, or most below the plane.
			if (pens[i] < max_pen)
			{
				max_pen = pens[i];
			}
		}

		// Gather all the corners that equal the maximum penetration.
		std::vector<st_vec3f> max_corners;
		for (int i = 0; i < k_num_corners; ++i)
		{
			if (st_equalf(pens[i], max_pen))
			{
				max_corners.push_back(corners[i]);
			}
		}

		// Now, average the corners with the maximum penetration to find the collision point.
		// If the point is an entire surface or edge, we should get a point in the middle of it.
		st_vec3f average = { 0.0f, 0.0f, 0.0f };
		for (auto& c : max_corners)
		{
			average += c;
		}
		average.scale(1.0f / static_cast<float>(max_corners.size()));

		info->_point = average + info->_normal.scale_result(info->_penetration);
	}

	return collision;
}

bool sphere_vs_sphere(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	const st_sphere* sphere_a = reinterpret_cast<const st_sphere*>(a);
	const st_sphere* sphere_b = reinterpret_cast<const st_sphere*>(b);

	st_vec3f center_a = sphere_a->_center + transform_a.get_translation();
	st_vec3f center_b = sphere_b->_center + transform_b.get_translation();

	float radii2 = powf(sphere_a->_radius + sphere_b->_radius, 2.0f);
	return center_a.dist2(center_b) < radii2;
}

bool capsule_vs_capsule(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	return false;
}

bool aabb_vs_aabb(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	const st_aabb* aabb_a = reinterpret_cast<const st_aabb*>(a);
	const st_aabb* aabb_b = reinterpret_cast<const st_aabb*>(b);

	st_vec3f min_a = transform_a.get_translation() + aabb_a->_min;
	st_vec3f max_a = transform_a.get_translation() + aabb_a->_max;
	st_vec3f min_b = transform_b.get_translation() + aabb_b->_min;
	st_vec3f max_b = transform_b.get_translation() + aabb_b->_max;

	// Find an axis separating the two.
	bool collide = !((min_a.x > max_b.x || max_a.x < min_b.x) ||
					 (min_a.y > max_b.y || max_a.y < min_b.y) ||
					 (min_a.z > max_b.z || max_a.z < min_b.z));
	return collide;
}

st_vec3f separating_axis_point_of_collision(const st_oobb* oobb_a, const st_oobb* oobb_b, uint32_t min_penetration_index)
{
	// This is not the ideal way of doing this, but it should arrive at the correct result.
	st_vec3f point_of_intersection;

	std::vector<st_vec3f> corners_a;
	std::vector<st_vec3f> corners_b;
	oobb_a->get_corners(corners_a);
	oobb_b->get_corners(corners_b);
		
	st_vec3f a_to_b = oobb_b->_center - oobb_a->_center;

	// Find the two points of a closest to b.
	st_vec3f primary_a = farthest_along_vector(corners_a, a_to_b);
	corners_a.erase(std::find(corners_a.begin(), corners_a.end(), primary_a));
	st_vec3f secondary_a = farthest_along_vector(corners_a, a_to_b);
	corners_a.erase(std::find(corners_a.begin(), corners_a.end(), secondary_a));
	st_vec3f tertiary_a = farthest_along_vector(corners_a, a_to_b);
	corners_a.erase(std::find(corners_a.begin(), corners_a.end(), tertiary_a));
	st_vec3f quarternary_a = farthest_along_vector(corners_a, a_to_b);

	// Find the two points of b closest to a.
	st_vec3f primary_b = farthest_along_vector(corners_b, -a_to_b);
	corners_b.erase(std::find(corners_b.begin(), corners_b.end(), primary_b));
	st_vec3f secondary_b = farthest_along_vector(corners_b, -a_to_b);
	corners_b.erase(std::find(corners_b.begin(), corners_b.end(), secondary_b));
	st_vec3f tertiary_b = farthest_along_vector(corners_b, -a_to_b);
	corners_b.erase(std::find(corners_b.begin(), corners_b.end(), tertiary_b));
	st_vec3f quarternary_b = farthest_along_vector(corners_b, -a_to_b);

	// If the normal is one of the boxes' axes, use the closest point from the other box.
	if (min_penetration_index < 3)
	{
		// If the faces of the boxes are colliding, average the points.
		if (st_absf(oobb_a->_half_vectors[min_penetration_index].normal().dot(oobb_b->_half_vectors[0].normal())) > 0.95f ||
			st_absf(oobb_a->_half_vectors[min_penetration_index].normal().dot(oobb_b->_half_vectors[1].normal())) > 0.95f ||
			st_absf(oobb_a->_half_vectors[min_penetration_index].normal().dot(oobb_b->_half_vectors[2].normal())) > 0.95f)
		{
			point_of_intersection = (primary_b + secondary_b + tertiary_b + quarternary_b).scale_result(0.25f);
		}
		else
		{
			point_of_intersection = primary_b;
		}
	}
	else if(min_penetration_index < 6)
	{
		if (st_absf(oobb_a->_half_vectors[0].normal().dot(oobb_b->_half_vectors[min_penetration_index - 3].normal())) > 0.95f ||
			st_absf(oobb_a->_half_vectors[1].normal().dot(oobb_b->_half_vectors[min_penetration_index - 3].normal())) > 0.95f ||
			st_absf(oobb_a->_half_vectors[2].normal().dot(oobb_b->_half_vectors[min_penetration_index - 3].normal())) > 0.95f)
		{
			point_of_intersection = (primary_a + secondary_a + tertiary_a + quarternary_a).scale_result(0.25f);
		}
		else
		{
			point_of_intersection = primary_a;
		}
	}
	else
	{
		// Using the two half vectors crossed to get the axis of minimum penetration,
		// and the closest points, formulate two candidate edges per box.
		uint32_t cross_index = min_penetration_index - 6;
		uint32_t a_hvec_index = cross_index / 3;
		uint32_t b_hvec_index = cross_index % 3;

		st_vec3f a_hvec = oobb_a->_half_vectors[a_hvec_index];
		st_vec3f b_hvec = oobb_b->_half_vectors[b_hvec_index];

		st_vec3f edges_a[2][2];
		st_vec3f edges_b[2][2];

		bool align = st_equalf(st_absf((primary_a - secondary_a).normal().dot(a_hvec.normal())), 1.0f);
		edges_a[0][0] = primary_a;
		edges_a[0][1] = align ? secondary_a : tertiary_a;
		edges_a[1][0] = align ? tertiary_a : secondary_a;
		edges_a[1][1] = quarternary_a;

		align = st_equalf(st_absf((primary_b - secondary_b).normal().dot(b_hvec.normal())), 1.0f);
		edges_b[0][0] = primary_b;
		edges_b[0][1] = align ? secondary_b : tertiary_b;
		edges_b[1][0] = align ? tertiary_b : secondary_b;
		edges_b[1][1] = quarternary_b;

		st_vec3f point_a;
		st_vec3f point_b;
		float min_dist = FLT_MAX;
		for (uint32_t i = 0; i < 2; ++i)
		{
			for (uint32_t j = 0; j < 2; ++j)
			{
				st_vec3f temp_a;
				st_vec3f temp_b;
				if (closest_points_on_lines(edges_a[i][0], edges_a[i][1], edges_b[j][0], edges_b[j][1], temp_a, temp_b))
				{
					float dist = (temp_a - temp_b).mag();
					if (dist < min_dist)
					{
						min_dist = dist;
						point_a = temp_a;
						point_b = temp_b;
					}
				}
			}
		}

		point_of_intersection = (point_a + point_b).scale_result(0.5f);
	}

	return point_of_intersection;
}

bool separating_axis_test(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	bool collision = true;
	std::vector<st_vec3f> axes;
	st_oobb oobb_a, oobb_b;

	oobb_a = *reinterpret_cast<const st_oobb*>(a);
	oobb_a._center += transform_a.get_translation();
	oobb_a._half_vectors[0] = transform_a.transform_vector(oobb_a._half_vectors[0]);
	oobb_a._half_vectors[1] = transform_a.transform_vector(oobb_a._half_vectors[1]);
	oobb_a._half_vectors[2] = transform_a.transform_vector(oobb_a._half_vectors[2]);

	oobb_b = *reinterpret_cast<const st_oobb*>(b);
	oobb_b._center += transform_b.get_translation();
	oobb_b._half_vectors[0] = transform_b.transform_vector(oobb_b._half_vectors[0]);
	oobb_b._half_vectors[1] = transform_b.transform_vector(oobb_b._half_vectors[1]);
	oobb_b._half_vectors[2] = transform_b.transform_vector(oobb_b._half_vectors[2]);

	// The axes we need to check are the primary axes of each oriented box.
	axes.push_back(oobb_a._half_vectors[0]);
	axes.push_back(oobb_a._half_vectors[1]);
	axes.push_back(oobb_a._half_vectors[2]);
	axes.push_back(oobb_b._half_vectors[0]);
	axes.push_back(oobb_b._half_vectors[1]);
	axes.push_back(oobb_b._half_vectors[2]);

	// We also need to check nine additional axes to cover corner-to-corner and edge-to-edge cases.
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			axes.push_back(st_vec3f_cross(oobb_a._half_vectors[i], oobb_b._half_vectors[j]));
		}
	}

	float min_penetration = FLT_MAX;
	st_vec3f min_penetration_axis;
	uint32_t min_penetration_index = INT_MAX;

	for (uint32_t i = 0; i < axes.size(); ++i)
	{
		st_vec3f axis = axes[i];
		axis.normalize();

		// Project the half vectors to get half the projected shape.
		// We use absolution value projection otherwise one projection may subtract from the half projection.
		float project_a =
			oobb_a._half_vectors[0].project_onto(axis).mag() +
			oobb_a._half_vectors[1].project_onto(axis).mag() +
			oobb_a._half_vectors[2].project_onto(axis).mag();
		float project_b =
			oobb_b._half_vectors[0].project_onto(axis).mag() +
			oobb_b._half_vectors[1].project_onto(axis).mag() +
			oobb_b._half_vectors[2].project_onto(axis).mag();

		// Find the min and max points for the full projections of a and b.
		float min_a = oobb_a._center.dot(axis) - project_a;
		float max_a = oobb_a._center.dot(axis) + project_a;
		float min_b = oobb_b._center.dot(axis) - project_b;
		float max_b = oobb_b._center.dot(axis) + project_b;

		// This is a separating axis if there is no overlap in the projections.
		// There is no overlap if the minimum point of each projection does not fall between
		// the max and min of the other projection.
		float penetration = st_min((max_b - min_a), (max_a - min_b));

		if (penetration < 0.0f)
		{
			collision = false;
			break;
		}

		// Update the minimum penetration value.
		if (penetration < min_penetration)
		{
			min_penetration = penetration;
			min_penetration_axis = axis;
			min_penetration_index = i;
		} 
	}

	if (collision && min_penetration_index < INT_MAX)
	{
		// The normal of the collision is the axis of minimum penetration.
		info->_normal = min_penetration_axis;
		info->_penetration = min_penetration;
		info->_point = separating_axis_point_of_collision(&oobb_a, &oobb_b, min_penetration_index);
	}

	return collision;
}

st_vec3f gjk_support(const st_convex_hull* hull, const st_vec3f& dir)
{
	float max_dot = -FLT_MAX;
	st_vec3f best;

	for (uint32_t i = 0; i < hull->_positions.size(); ++i)
	{
		st_vec3f point = hull->_positions[i];
		if (point.dot(dir) > max_dot)
		{
			max_dot = point.dot(dir);
			best = point;
		}
	}

	return best;
}

bool gjk_internal(
	const st_convex_hull* a,
	const st_convex_hull* b,
	std::vector<st_vec3f>& simplex)
{
	while (true)
	{
		float distance_to_simplex = FLT_MAX;
		st_vec3f new_point;

		if (simplex.size() == 0)
		{
			// Start by picking an arbitrary direction.
			st_vec3f dir = st_vec3f::x_vector();
			new_point = gjk_support(a, dir) - gjk_support(b, -dir);
			simplex.push_back(new_point);
		}
		else if (simplex.size() == 1)
		{
			// Pick the direction from the point to the origin.
			st_vec3f dir = -simplex[0];
			new_point = gjk_support(a, dir) - gjk_support(b, -dir);
			distance_to_simplex = (simplex[0] - new_point).mag();
		}
		else if (simplex.size() == 2)
		{
			float dist_to_line = distance_to_line_segment({0.0f, 0.0f, 0.0f}, simplex[0], simplex[1]);
			float dist_a = simplex[0].mag();
			float dist_b = simplex[1].mag();

			if (dist_to_line < dist_a && dist_to_line < dist_b)
			{
				// TODO: Direction is perpendicular to line in direction of origin.
			}
			else
			{
				// TODO: The farther point is deleted, then continue.
			}
		}
		else if (simplex.size() == 3)
		{
			// TODO: Triangle case.
		}
		else
		{
			// TODO: Tetrahedron case.
		}

		if (new_point.mag() > distance_to_simplex)
		{
			return false;
		}
		simplex.push_back(new_point);
	}

	return false;
}

bool gjk(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info)
{
	st_convex_hull hull_a = *reinterpret_cast<const st_convex_hull*>(a);
	st_convex_hull hull_b = *reinterpret_cast<const st_convex_hull*>(b);

	// Transform all of the positions into world space.
	for (uint32_t i = 0; i < hull_a._positions.size(); ++i)
	{
		st_vec3f point = hull_a._positions[i];
		hull_a._positions[i] = transform_a.transform_vector(point);
	}
	for (uint32_t i = 0; i < hull_b._positions.size(); ++i)
	{
		st_vec3f point = hull_b._positions[i];
		hull_b._positions[i] = transform_b.transform_vector(point);
	}

	// Create a new simplex structure, empty initially.
	std::vector<st_vec3f> simplex;
	return gjk_internal(&hull_a, &hull_b, simplex);
}
