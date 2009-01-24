//---------------------------------------------------------------------
// ModBaker - a module editor for Egoboo
//---------------------------------------------------------------------
// Copyright (C) 2009 Tobias Gall
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_types.h>
#include <SDL_endian.h>

#include "general.h"
#include "modbaker.h"
#include "global.h"

#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

//---------------------------------------------------------------------
//-   The main function
//---------------------------------------------------------------------
int main(int argc, char **argv)
{
	c_modbaker modbaker;

	string modname;

	modname = "advent.mod";

	// TODO: Read from argv
	if (argc >= 2)
	{
		modname = argv[1];
	}

	modbaker.init(modname);
	modbaker.main_loop();

	return 0;
}


//---------------------------------------------------------------------
//-   Constructor
//---------------------------------------------------------------------
c_modbaker::c_modbaker()
{
	this->done          = false;
	this->active        = true;
	this->selection_add = false;
}


//---------------------------------------------------------------------
//-   Init everything
//---------------------------------------------------------------------
void c_modbaker::init(string modname)
{
	g_mesh.getTileDictioary().load();
	g_mesh.load_mesh_mpd(g_config.get_egoboo_path() + "modules/" + modname + "/gamedat/level.mpd");

	g_renderer.load_basic_textures(modname);

	g_renderer.m_renderlist.build();
}


//---------------------------------------------------------------------
//-   The main loop
//---------------------------------------------------------------------
void c_modbaker::main_loop()
{
	g_renderer.getPCam()->reset();

	while ( !done )
	{
		if ( active )
		{
			g_renderer.begin_frame();
//			{ // TODO: move to crenderer::render()
			// 3D mode
			g_renderer.begin_3D_mode();
			g_renderer.getPCam()->move();
			g_renderer.render_mesh();
			g_renderer.render_positions();
			get_GL_pos(g_mouse_x, g_mouse_y);
			g_renderer.end_3D_mode();

			// 2D mode
			g_renderer.begin_2D_mode();
			g_renderer.render_text();
			g_renderer.end_2D_mode();

			g_renderer.end_frame();  // Finally render the scene
//			}
		}

		handle_game_events();
		handle_window_events();
	}
}