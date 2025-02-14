/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_atmosphere_component.h>

#include <imgui.h>

st_atmosphere_component::st_atmosphere_component(
	st_entity* ent,
	st_vec2f radii,
	st_vec4f rayleigh_params,
	st_vec4f mie_params,
	st_vec4f ozone_params) : st_component(ent)
{
	_params._planet_radius_km = radii.x;
	_params._atmosphere_radius_km = radii.y;
	_params._rayleigh_coefficients = st_vec3f { rayleigh_params.x, rayleigh_params.y, rayleigh_params.z };
	_params._rayleigh_average = rayleigh_params.w;
	_params._mie_coefficients = st_vec3f { mie_params.x, mie_params.y, mie_params.z };
	_params._mie_average = mie_params.w;
	_params._ozone_coefficients = st_vec3f { ozone_params.x, ozone_params.y, ozone_params.z };
	_params._ozone_average = ozone_params.w;
}

void st_atmosphere_component::update(st_frame_params* params)
{
	params->_atmosphere = &_params;
}

void st_atmosphere_component::debug()
{	
	if (ImGui::TreeNode("st_atmosphere_component"))
	{
		ImGui::SliderFloat("Planet Radius (km)", &_params._planet_radius_km, 3000.0f, 10000.0f, "%f");
		ImGui::SliderFloat("Atmosphere Radius (km)", &_params._atmosphere_radius_km, 3001.0f, 12000.0f, "%f");

		st_vec3f rc = _params._rayleigh_coefficients.scale_result(1e6f);
		ImGui::DragFloat3("Rayleigh Coefficients (e-6)", rc.axes, 0.05f, 0.0f, 50.0f);
		_params._rayleigh_coefficients = rc.scale_result(1e-6f);

		ImGui::DragFloat("Rayleigh Average Density (km)", &_params._rayleigh_average, 0.01f, 1.0f, 30.0f);

		st_vec3f mc = _params._mie_coefficients.scale_result(1e6f);
		ImGui::DragFloat3("Mie Coefficients (e-6)", mc.axes, 0.05f, 0.0f, 50.0f);
		_params._mie_coefficients = mc.scale_result(1e-6f);

		ImGui::DragFloat("Mie Average Density (km)", &_params._mie_average, 0.01f, 1.0f, 30.0f);

		st_vec3f oc = _params._ozone_coefficients.scale_result(1e6f);
		ImGui::DragFloat3("Ozone Coefficients (e-6)", oc.axes, 0.05f, 0.0f, 50.0f);
		_params._ozone_coefficients = oc.scale_result(1e-6f);

		ImGui::DragFloat("Ozone Average Density (km)", &_params._ozone_average, 0.01f, 1.0f, 30.0f);

		ImGui::TreePop();
	}
}