//********************************************************************************************
//*
//*    This file is part of Egoboo.
//*
//*    Egoboo is free software: you can redistribute it and/or modify it
//*    under the terms of the GNU General Public License as published by
//*    the Free Software Foundation, either version 3 of the License, or
//*    (at your option) any later version.
//*
//*    Egoboo is distributed in the hope that it will be useful, but
//*    WITHOUT ANY WARRANTY; without even the implied warranty of
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*    General Public License for more details.
//*
//*    You should have received a copy of the GNU General Public License
//*    along with Egoboo.  If not, see <http://www.gnu.org/licenses/>.
//*
//********************************************************************************************

/// @file  egolib/math/Plane.h
/// @brief Planes.

#pragma once

#include "egolib/vec.h"
#if 0
#include "egolib/typedef.h"
#include "egolib/_math.h"
#endif

// The base type of the plane data.
// @todo Remove this.
typedef fvec4_base_t plane_base_t;

/// @todo Remove this.
bool plane_base_normalize(plane_base_t * plane);

#if 0
struct plane_t
{
	plane_t(const fvec3_t& a, const fvec3_t& b, const fvec3_t& c);
	plane_t(const fvec3_t& p, const fvec3_t& n);
	plane_t(const plane_t& other);
};
#endif