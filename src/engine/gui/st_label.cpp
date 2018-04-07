/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_label.h>

#include <framework/st_frame_params.h>

#include <gui/st_font.h>

st_label::st_label(const char* text, float x, float y, st_frame_params* params)
{
	extern std::unique_ptr<st_font> g_font;
	g_font->print(params, text, x, y, k_text_color);
}

st_label::~st_label()
{
}
