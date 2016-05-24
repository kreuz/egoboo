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

/// @file egolib/Math/Translate_AABB.hpp
/// @brief Translation of axis-aligned bounding boxes.
/// @author Michael Heilmann

#pragma once

#include "egolib/Math/AABB.hpp"

namespace Ego {
namespace Math {

template <typename _EuclideanSpaceType>
struct Translate<AABB<_EuclideanSpaceType>> {
    typedef AABB<_EuclideanSpaceType> X;
    typedef typename _EuclideanSpaceType::VectorType T;
    X operator()(const X& x, const T& t) const {
        return X(x.getMin() + t, x.getMax() + t);
    }
};

template <typename _EuclideanSpaceType>
AABB<_EuclideanSpaceType> translate(const AABB<_EuclideanSpaceType>& x, const typename _EuclideanSpaceType::VectorType& t) {
    Translate<AABB<_EuclideanSpaceType>> f;
    return f(x, t);
}

} // namespace Math
} // namespace Ego
