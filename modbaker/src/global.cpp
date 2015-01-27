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
#include "global.h"

#include <cmath>
#include <sstream>

using namespace std;

c_config*         g_config;
c_frustum         g_frustum;

vector<c_tile_definition> g_tiledict;


//---------------------------------------------------------------------
//-   Variables for mouse handling
//---------------------------------------------------------------------
float g_mouse_gl_x;
float g_mouse_gl_y;
float g_mouse_gl_z;


//---------------------------------------------------------------------
//-   Selection
//---------------------------------------------------------------------
int g_nearest_vertex;
int g_nearest_tile;
int g_nearest_object;


//---------------------------------------------------------------------
//-   Calculate the distance between two points
//---------------------------------------------------------------------
float calculate_distance(vect3 start, vect3 end)
{
	float dist;
	dist = sqrtf( powf((start.x - end.x), 2.0f) +  powf((start.y - end.y), 2.0f) + powf((start.z - end.z), 2.0f) );

	return dist;
}


//---------------------------------------------------------------------
//-   Function to handle cleanup
//---------------------------------------------------------------------
void Quit()
{
	SDL_Quit( ); // clean up the window
	exit(-1);    // and exit appropriately
}


//---------------------------------------------------------------------
//-   Constructor: Read the config
//---------------------------------------------------------------------
c_config::c_config()
{
	ifstream file;

	this->set_width(1024);
	this->set_height(786);

	// TODO: Also look in the home directories for a file called
	// ~/.egoboo/modbaker.cfg
	// C:\Documents and settings\egoboo\modbaker.cfg
	file.open("modbaker.cfg");

	if (!file)
	{
		cout << "Config file not found!" << endl;
		Quit();
	}

	string buffer;
	vector <string> tokens;

	while(!file.eof())
	{
		getline(file, buffer);

		tokens.clear();
		if(tokenize_colon(buffer, tokens))
		{
			if (tokens[0] == "egoboo_path")
				this->set_egoboo_path(tokens[1]);

			if (tokens[0] == "font_size")
			{
				// Convert the string to integer using stringstreams
				int itmp;
				stringstream sstr(tokens[1]);
				sstr >> itmp;
				this->set_font_size(itmp);
			}

			if (tokens[0] == "screen_width")
			{
				// Convert the string to integer using stringstreams
				int itmp;
				stringstream sstr(tokens[1]);
				sstr >> itmp;
				this->set_width(itmp);
			}

			if (tokens[0] == "screen_height")
			{
				// Convert the string to integer using stringstreams
				int itmp;
				stringstream sstr(tokens[1]);
				sstr >> itmp;
				this->set_height(itmp);
			}

			if (tokens[0] == "font_file")
				this->set_font_file(tokens[1]);
		}
	}

	file.close();
	cout << "Config file parsed successfully" << endl;
}


//---------------------------------------------------------------------
//-   Destructor
//---------------------------------------------------------------------
c_config::~c_config() {
	cout << "Config deleted" << endl;
}


//---------------------------------------------------------------------
//-   Setter and Getter for m_egoboo_path
//---------------------------------------------------------------------
string c_config::get_egoboo_path()
{
	return this->m_egoboo_path;
}

void c_config::set_egoboo_path(string p_egoboo_path)
{
	this->m_egoboo_path = p_egoboo_path;
}


//---------------------------------------------------------------------
//-   Setter and Getter for m_font_size
//---------------------------------------------------------------------
int c_config::get_font_size()
{
	return this->m_font_size;
}

void c_config::set_font_size(int p_font_size)
{
	this->m_font_size = p_font_size;
}


//---------------------------------------------------------------------
//-   Setter and Getter for m_font_file
//---------------------------------------------------------------------
string c_config::get_font_file()
{
	return this->m_font_file;
}

void c_config::set_font_file(string p_font_file)
{
	this->m_font_file = p_font_file;
}


//---------------------------------------------------------------------
//-   Setter and Getter for m_width
//---------------------------------------------------------------------
int c_config::get_width()
{
	return this->m_width;
}

void c_config::set_width(int p_width)
{
	this->m_width = p_width;
}


//---------------------------------------------------------------------
//-   Setter and Getter for m_height
//---------------------------------------------------------------------
int c_config::get_height()
{
	return this->m_height;
}

void c_config::set_height(int p_height)
{
	this->m_height = p_height;
}

