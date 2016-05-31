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

/// @file egolib/Math/LinearInterpolate_Real.hpp
/// @brief Linear interpolation for real types.
/// @author Michael Heilmann

#pragma once

#include "egolib/Math/LinearInterpolate.hpp"

namespace Ego {
namespace Math {

/**
 * @ingroup math
 * @brief
 *  Interpolation functor specialization for real types.
 * @param x, y
 *  the values to interpolate between
 * @param t
 *  the parameter.
 *  Must be within the bounds of @a 0 (inclusive) and @a 1 (inclusive).
 * @return
 *  the interpolated value
 * @remark
 *  Template specializations of this template do in general not perform
 *  linear extrapolation, that is, @a t has to stay within the bound of
 *  @a 0 (inclusive) and @a 1 (inclusive). It is required that a
 *  specialization raises an std::domain_error if an argument
 *  value for @a t is outside of those bounds.
 * @throws Id::OutOfBoundsException
 *  if @a t is smaller than @a 0 and greater than @a 1
 */
template <typename _OperatorType>
struct Lerp<_OperatorType, std::enable_if_t<IsReal<_OperatorType>::value>> {
    _OperatorType operator()(_OperatorType x, _OperatorType y, _OperatorType t) const {
        if (t < Zero<_OperatorType>()() || t > One<_OperatorType>()()) {
            Log::Entry e(Log::Level::Error, __FILE__, __LINE__);
            e << "parameter t = " << t << " not within the interval of [0,1]" << Log::EndOfEntry;
            Log::get() << e;
            throw Id::OutOfBoundsException(__FILE__, __LINE__, e.getText());
        }
        return x * (One<_OperatorType>()() - t) + y * t;
    }
};

} // namespace Math
} // namespace Ego