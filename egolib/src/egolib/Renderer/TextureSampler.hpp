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

/// @file   egolib/Renderer/TextureSampler.hpp
/// @brief  A texture sampler.
/// @author Michael Heilmann

#pragma once

#include "egolib/Renderer/TextureFilter.hpp"
#include "egolib/Renderer/TextureAddressMode.hpp"

namespace Ego {
	
// We know we are dealing with filters and addressing modes for textures :)
using Filter = TextureFilter;
using AddressMode = TextureAddressMode;

/**
 * @brief A texture sampler is part of the state of a texture (Ego::Texture).
 */
struct TextureSampler {

private:
    /// @brief The minification filter
    Filter minFilter;

    /// @brief The magnification filter.
    Filter magFilter;

    /// @brief The mip map filter
    Filter mipMapFilter;

    /// @brief The addressing mode along the s-axis.
    AddressMode addressModeS;

    /// @brief The addressing mode along the t-axis.
    AddressMode addressModeT;

    /// @brief The level of anistropic filtering.
    float anisotropyLevel;

public:
    /**
     * @brief Construct this texture sampler.
     * @param minFilter the minification filter
     * @param magFilter the magnification filter
     * @param mipMapFilter the mipmap filter
     * @param addressModeS the address mode for the s-axis
     * @param addressModeT the address mode for the t-axis
     * @param anisotropyLevel the level of anisotropic filtering
     * When the sampler is applied, this value is clamped to  \f$[min,max]\f$ where \f$min\f$ is the
     * minimum level and \f$max\f$ is the maximum level of anistropic filtering supported.
     * The minimum level means anisotropic filtering is off i.e. isotropic filtering is performed.
     */
    TextureSampler(Filter minFilter, Filter magFilter, Filter mipMapFilter,
                   AddressMode addressModeS, AddressMode addressModeT, float anisotropyLevel);

    /**
     * @brief Copy-construct this texture sampler from another texture sampler.
     * @param other the other texture sampler
     */
    TextureSampler(const TextureSampler& other);

public:
    /**
     * @brief Assign this texture sampler from another texture sampler.
     * @param other the other texture sampler
     * @return this texture sampler
     */
    const TextureSampler& operator=(const TextureSampler& other);

public:
    /**
     * @brief Get the minification filter.
     * @return the minification filter
     */
    Filter getMinFilter() const;

    /**
     * @brief Get the magnification filter.
     * @return the magnification filter
     */
    Filter getMagFilter() const;

    /**
     * @brief Get the mipmap filter.
     * @return the mipmap filter
     */
    Filter getMipMapFilter() const;

    /**
     * @brief Get the address mode for the s-axis.
     * @return the address mode for the s-axis
     */
    AddressMode getAddressModeS() const;

    /**
     * @brief Get the address mode for the t-axis.
     * @return the address mode for the t-axis
     */
    AddressMode getAddressModeT() const;

    /**
     * @brief Get level of anistropic filtering.
     * @return the level of anisotropic filtering
     */
    float getAnisotropyLevel() const;

}; // struct TextureSampler

} // namespace Ego
