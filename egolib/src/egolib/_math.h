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

/// @file egolib/_math.h
/// @details The name's pretty self explanatory, doncha think?

#pragma once

#include "egolib/typedef.h"
#include "egolib/vec.h"
#include "egolib/platform.h"
#include "egolib/log.h"
#include "egolib/math/Math.hpp"

#include "egolib/extensions/ogl_include.h"
#include "egolib/extensions/ogl_debug.h"

#if !defined(ABS)
	#define ABS(x) std::abs(x)
#endif

/**
 * @brief
 *	Clip a value.
 * @param _value
 *	the value
 * @param _minimum
 *	the minimum
 * @param _maximum
 *	the maximum
 * @return
 *	the clipped value
 */
template<typename T> const T& CLIP(const T& _value, const T& _minimum, const T& _maximum)
{
	return std::min(std::max(_value, _minimum), _maximum);
}

//--------------------------------------------------------------------------------------------
// IEEE 32-BIT FLOATING POINT NUMBER FUNCTIONS
//--------------------------------------------------------------------------------------------


#if defined(TEST_NAN_RESULT)
#    define LOG_NAN(XX)      if( ieee32_bad(XX) ) log_error( "**** A math operation resulted in an invalid result (NAN) ****\n    (\"%s\" - %d)\n", __FILE__, __LINE__ );
#else
#    define LOG_NAN(XX)
#endif

namespace Ego
{
    namespace Math
    {
        inline float degToRad(float x)
        {
            return x * 0.017453292519943295769236907684886f;
        }

        inline double degToRad(double x)
        {
            return x * 0.017453292519943295769236907684886;
        }
    }
}

namespace Ego
{
    namespace Math
    {
        /**
         * @brief
         *  Get \f$\pi\f$.
         * @return
         *  \f$\pi\f$
         */
        template <typename Type>
        Type pi();

        /**
         * @brief
         *  Get \f$\pi\f$.
         * @return
         *  \f$\pi\f$
         */
        template <>
        inline float pi<float>()
        {
            return 3.1415926535897932384626433832795f;
        }

        /**
         * @brief
         *  Get \f$\pi\f$.
         * @return
         *  \f$\pi\f$
         */
        template <>
        inline double pi<double>()
        {
            return 3.1415926535897932384626433832795;
        }

        /**
         * @brief
         *  Get \f$\frac{1}{\pi}\f$.
         * @return
         *  \f$\frac{1}{\pi}\f$
         */
        template <typename Type>
        Type invPi();

        /**
        * @brief
        *  Get \f$\frac{1}{\pi}\f$.
        * @return
        *  \f$\frac{1}{\pi}\f$
        */
        template <>
        inline float invPi<float>()
        {
            return 0.31830988618379067153776752674503f;
        }

        /**
        * @brief
        *  Get \f$\frac{1}{\pi}\f$.
        * @return
        *  \f$\frac{1}{\pi}\f$
        */
        template <>
        inline double invPi<double>()
        {
            return 0.31830988618379067153776752674503;
        }

        /**
         * @brief
         *  Get \f$2 \cdot \pi\f$.
         * @return
         *  \f$2 \cdot \pi\f$
         */
        template <typename Type>
        Type twoPi();

        /**
         * @brief
         *  Get \f$2 \cdot \pi\f$.
         * @return
         *  \f$2 \cdot \pi\f$
         */
        template <>
        inline float twoPi<float>()
        {
            return 6.283185307179586476925286766559f;
        }

        /**
         * @brief
         *  Get \f$2 \cdot \pi\f$.
         * @return
         *  \f$2 \cdot \pi\f$
         */
        template <>
        inline double twoPi<double>()
        {
            return 6.283185307179586476925286766559;
        }

        /**
         * @brief
         *  Get \f$\frac{1}{2 \cdot \pi}\f$.
         * @return
         *  \f$\frac{1}{2 \cdot \pi}\f$
         */
        template <typename Type>
        Type invTwoPi();

        /**
         * @brief
         *  Get \f$\frac{1}{2 \cdot \pi}\f$.
         * @return
         *  \f$\frac{1}{2 \cdot \pi}\f$
         */
        template <>
        inline float invTwoPi<float>()
        {
            return 0.15915494309189533576888376337251f;
        }

        /**
         * @brief
         *  Get \f$\frac{1}{2 \cdot \pi}\f$.
         * @return
         *  \f$\frac{1}{2 \cdot \pi}\f$
         */
        template <>
        inline double invTwoPi<double>()
        {
            return 0.15915494309189533576888376337251;
        }

        inline float radToDeg(float x)
        {
            return x * 57.295779513082320876798154814105f;
        }

        inline double radToDeg(double x)
        {
            return x * 57.295779513082320876798154814105;
        }
    }
}


#if defined(__cplusplus)
extern "C"
{
#endif

#define FACE_RANDOM  ((FACING_T)generate_randmask(0, 0xFFFF))

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// basic constants

#if !defined(PI)
#   define PI                  3.1415926535897932384626433832795f
#endif

#if !defined(INV_PI)
#   define INV_PI              0.31830988618379067153776752674503f
#endif

#if !defined(TWO_PI)
#   define TWO_PI              6.283185307179586476925286766559f
#endif

#if !defined(INV_TWO_PI)
#   define INV_TWO_PI          0.15915494309189533576888376337251f
#endif

#if !defined(PI_OVER_TWO)
#   define PI_OVER_TWO         1.5707963267948966192313216916398f
#endif

#if !defined(PI_OVER_FOUR)
#   define PI_OVER_FOUR         0.78539816339744830961566084581988f
#endif

#if !defined(SQRT_TWO)
#   define SQRT_TWO            1.4142135623730950488016887242097f
#endif

#if !defined(INV_SQRT_TWO)
#   define INV_SQRT_TWO        0.70710678118654752440084436210485f
#endif

#if !defined(RAD_TO_FACING)
#   define RAD_TO_FACING(XX)     ( (XX) * 10430.378350470452724949566316381f )
#endif

#if !defined(FACING_TO_RAD)
#   define FACING_TO_RAD(XX)     ( (XX) * 0.000095873799242852576857380474343257f )
#endif

#if !defined(DEG_TO_RAD)
#   define DEG_TO_RAD(XX)         ( (XX) * 0.017453292519943295769236907684886f )
#endif

#if !defined(RAD_TO_DEG)
#   define RAD_TO_DEG(XX)         ( (XX) * 57.295779513082320876798154814105f )
#endif

#if !defined(ONE_TO_TURN)
#   define ONE_TO_TURN(XX)         CLIP_TO_16BITS(( int )( XX * ( float )0x00010000 ) )
#endif

#if !defined(TURN_TO_ONE)
#   define TURN_TO_ONE(XX)         ( (float) CLIP_TO_16BITS(XX) / ( float )0x00010000 )
#endif

#if !defined(RAD_TO_ONE)
#   define RAD_TO_ONE(XX)         ( (XX) * INV_TWO_PI )
#endif

#if !defined(ONE_TO_RAD)
#   define ONE_TO_RAD(XX)         ( (XX) * TWO_PI )
#endif

//--------------------------------------------------------------------------------------------
// the lookup tables for sine and cosine

#define TRIG_TABLE_BITS   14
#define TRIG_TABLE_SIZE   (1<<TRIG_TABLE_BITS)
#define TRIG_TABLE_MASK   (TRIG_TABLE_SIZE-1)
#define TRIG_TABLE_OFFSET (TRIG_TABLE_SIZE>>2)

/// @note - Aaron uses two terms without much attention to their meaning
///         I think that we should use "face" or "facing" to mean the fill 16-bit value
///         and use "turn" to be the TRIG_TABLE_BITS-bit value

    extern float turntosin[TRIG_TABLE_SIZE];           ///< Convert TURN_T == FACING_T>>2...  to sine
    extern float turntocos[TRIG_TABLE_SIZE];           ///< Convert TURN_T == FACING_T>>2...  to cosine

/// pre defined directions
#define FACE_WEST    0x0000
#define FACE_NORTH   0x4000                                 ///< Character facings
#define FACE_EAST    0x8000
#define FACE_SOUTH   0xC000

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#if !defined(SGN)
#    define SGN(X)  LAMBDA( 0 == (X), 0, LAMBDA( (X) > 0, 1, -1) )
#endif

#if !defined(SQR)
#    define SQR(A) ((A)*(A))
#endif

#if !defined(LOG)
#    define LOG(A) ((float)log((float)(A)))
#endif

#if !defined(SIN)
#    define SIN(A) ((float)sin((float)(A)))
#endif

#if !defined(COS)
#    define COS(A) ((float)cos((float)(A)))
#endif

#if !defined(ACOS)
#    define ACOS(A) ((float)acos((float)(A)))
#endif

#if !defined(TAN)
#    define TAN(A) ((float)tan((float)(A)))
#endif

#if !defined(ATAN)
#    define ATAN(A) ((float)atan((float)(A)))
#endif

#if !defined(POW)
#    define POW(A, B) ((float)pow((float)(A), (float)(B)))
#endif

#if !defined(ATAN2)
#    define ATAN2(A, B) ((float)atan2((float)(A), (float)(B)))
#endif

#if !defined(SWAP)
#    define SWAP(TYPE, A, B) { TYPE temp; memmove( &temp, &(A), sizeof(TYPE) ); memmove( &(A), &(B), sizeof(TYPE) ); memmove( &(B), &temp, sizeof(TYPE) ); }
#endif

#if !defined(CEIL)
#    define CEIL(VAL) ( (float)ceil((float)VAL) )
#endif

#if !defined(FLOOR)
#    define FLOOR(VAL) ( (float)floor((float)VAL) )
#endif

#define MAT_IDX(I,J) (4*(I)+(J))
#define CNV(I,J)     v[MAT_IDX(I,J)]
#define CopyMatrix( pMatrixDest, pMatrixSource ) memmove( pMatrixDest, pMatrixSource, sizeof( *pMatrixDest ) )

//--------------------------------------------------------------------------------------------
// FAST CONVERSIONS

#if !defined(INV_FF)
#   define INV_FF              0.003921568627450980392156862745098f
#endif

#if !defined(INV_0100)
#   define INV_0100            0.00390625f
#endif

#if !defined(INV_FFFF)
#   define INV_FFFF            0.000015259021896696421759365224689097f
#endif

#define FF_TO_FLOAT( V1 )  ( (float)(V1) * INV_FF )

#define FFFF_TO_FLOAT( V1 )  ( (float)(V1) * INV_FFFF )
#define FLOAT_TO_FFFF( V1 )  ( (int)((V1) * 0xFFFF) )

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// vector definitions


#if 0
    typedef double dvec2_base_t[2];          ///< the basic double precision 2-vector type
    typedef double dvec3_base_t[3];          ///< the basic double precision 3-vector type
    typedef double dvec4_base_t[4];          ///< the basic double precision 4-vector type
#endif
#if 0
	// turn off a really useless warning
	#if defined(_MSC_VER)
		#pragma warning(disable : 4201)
	#endif
	// turn it back on
	#if defined(_MSC_VER)
		#pragma warning(default : 4201)
	#endif
#endif



//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// My lil' random number table

// swig chokes on the definition below
#if defined(SWIG)
#    define RANDIE_BITS    12
#    define RANDIE_COUNT 4096
#else
#    define RANDIE_BITS   12
#    define RANDIE_COUNT (1 << RANDIE_BITS)
#endif

#define RANDIE_MASK  ((Uint32)(RANDIE_COUNT - 1))
#define RANDIE       randie[randindex & RANDIE_MASK ];  randindex++; randindex &= RANDIE_MASK

    extern Uint32  randindex;
    extern Uint16  randie[RANDIE_COUNT];   ///< My lil' random number table

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

// prototypes of other math functions

    void make_turntosin( void );
    void make_randie( void );

// conversion functions
    FACING_T vec_to_facing( const float dx, const float dy );
    void     facing_to_vec( const FACING_T facing, float * dx, float * dy );

// rotation functions
    int terp_dir( const FACING_T majordir, const FACING_T minordir, const int weight );

// limiting functions
    void getadd_int( const int min, const int value, const int max, int* valuetoadd );
    void getadd_flt( const float min, const float value, const float max, float* valuetoadd );

// random functions
    int generate_irand_pair( const IPair num );
    int generate_irand_range( const FRange num );
    int generate_randmask( const int base, const Uint32 mask );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#if defined(__cplusplus)
}

#endif
