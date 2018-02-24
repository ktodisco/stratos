#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cstdint>

#include <Windows.h>
#include <wrl.h>

class st_dx12_pipeline_state
{
public:
	st_dx12_pipeline_state(const struct st_pipeline_state_desc& desc);
	~st_dx12_pipeline_state();

	ID3D12PipelineState* get_state() const { return _pipeline_state.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipeline_state;
};

#endif
