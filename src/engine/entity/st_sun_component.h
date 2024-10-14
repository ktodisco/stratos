#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_component.h>

#include <graphics/light/st_directional_light.h>

#include <memory>

/*
 * A component describing a sun as part of the world surrounding the scene.
 */
class st_sun_component : public st_component
{
public:
	st_sun_component(
		class st_entity* ent,
		float azimuth,
		float angle,
		struct st_vec3f color,
		float power);
	~st_sun_component();

	void update(struct st_frame_params* params) override;

	void debug() override;

	float get_azimuth() const { return _azimuth; }
	float get_angle() const { return _angle; }
	st_vec3f get_color() const { return _light->_color; }
	float get_power() const { return _light->_power; }

	void set_azimuth(float azimuth) { _azimuth = azimuth; }
	void set_angle(float angle) { _angle = angle; }
	void set_color(const st_vec3f& color) { _light->_color; }
	void set_power(float power) { _light->_power; }

private:
	st_vec3f calculate_direction() const;

	float _azimuth = 0.0f;
	float _angle = 0.0f;

	std::unique_ptr<st_directional_light> _light = nullptr;
};
