/*******************************************************************************
The content of the files in this repository include portions of the
AUDIOKINETIC Wwise Technology released in source code form as part of the SDK
package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use these files in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Copyright (c) 2021 Audiokinetic Inc.
*******************************************************************************/

// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2020
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 4.0.2019.08.13

#pragma once

#include <Mathematics/TIQuery.h>
#include <Mathematics/Cylinder3.h>
#include <Mathematics/Halfspace.h>

// Queries for intersection of objects with halfspaces.  These are useful for
// containment testing, object culling, and clipping.

namespace gte
{
    template <typename Real>
    class TIQuery<Real, Halfspace3<Real>, Cylinder3<Real>>
    {
    public:
        struct Result
        {
            bool intersect;
        };

        Result operator()(Halfspace3<Real> const& halfspace, Cylinder3<Real> const& cylinder)
        {
            Result result;

            // Compute extremes of signed distance Dot(N,X)-d for points on
            // the cylinder.  These are
            //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
            //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
            Real center = Dot(halfspace.normal, cylinder.axis.origin) - halfspace.constant;
            Real absNdW = std::fabs(Dot(halfspace.normal, cylinder.axis.direction));
            Real root = std::sqrt(std::max((Real)1, (Real)1 - absNdW * absNdW));
            Real tmax = center + cylinder.radius * root + (Real)0.5 * cylinder.height * absNdW;

            // The cylinder and halfspace intersect when the projection
            // interval maximum is nonnegative.
            result.intersect = (tmax >= (Real)0);
            return result;
        }
    };
}
