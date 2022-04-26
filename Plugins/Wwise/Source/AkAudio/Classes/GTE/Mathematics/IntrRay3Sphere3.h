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

#include <Mathematics/IntrIntervals.h>
#include <Mathematics/IntrLine3Sphere3.h>
#include <Mathematics/Ray.h>

namespace gte
{
    template <typename Real>
    class TIQuery<Real, Ray3<Real>, Sphere3<Real>>
    {
    public:
        struct Result
        {
            bool intersect;
        };

        Result operator()(Ray3<Real> const& ray, Sphere3<Real> const& sphere)
        {
            // The sphere is (X-C)^T*(X-C)-1 = 0 and the line is X = P+t*D.
            // Substitute the line equation into the sphere equation to
            // obtain a quadratic equation Q(t) = t^2 + 2*a1*t + a0 = 0, where
            // a1 = D^T*(P-C) and a0 = (P-C)^T*(P-C)-1.
            Result result;

            Vector3<Real> diff = ray.origin - sphere.center;
            Real a0 = Dot(diff, diff) - sphere.radius * sphere.radius;
            if (a0 <= (Real)0)
            {
                // P is inside the sphere.
                result.intersect = true;
                return result;
            }
            // else: P is outside the sphere

            Real a1 = Dot(ray.direction, diff);
            if (a1 >= (Real)0)
            {
                result.intersect = false;
                return result;
            }

            // Intersection occurs when Q(t) has real roots.
            Real discr = a1 * a1 - a0;
            result.intersect = (discr >= (Real)0);
            return result;
        }
    };

    template <typename Real>
    class FIQuery<Real, Ray3<Real>, Sphere3<Real>>
        :
        public FIQuery<Real, Line3<Real>, Sphere3<Real>>
    {
    public:
        struct Result
            :
            public FIQuery<Real, Line3<Real>, Sphere3<Real>>::Result
        {
            // No additional information to compute.
        };

        Result operator()(Ray3<Real> const& ray, Sphere3<Real> const& sphere)
        {
            Result result;
            DoQuery(ray.origin, ray.direction, sphere, result);
            for (int i = 0; i < result.numIntersections; ++i)
            {
                result.point[i] = ray.origin + result.parameter[i] * ray.direction;
            }
            return result;
        }

    protected:
        void DoQuery(Vector3<Real> const& rayOrigin,
            Vector3<Real> const& rayDirection, Sphere3<Real> const& sphere,
            Result& result)
        {
            FIQuery<Real, Line3<Real>, Sphere3<Real>>::DoQuery(rayOrigin,
                rayDirection, sphere, result);

            if (result.intersect)
            {
                // The line containing the ray intersects the sphere; the
                // t-interval is [t0,t1].  The ray intersects the sphere as
                // long as [t0,t1] overlaps the ray t-interval [0,+infinity).
                std::array<Real, 2> rayInterval = { (Real)0, std::numeric_limits<Real>::max() };
                FIQuery<Real, std::array<Real, 2>, std::array<Real, 2>> iiQuery;
                auto iiResult = iiQuery(result.parameter, rayInterval);
                if (iiResult.intersect)
                {
                    result.numIntersections = iiResult.numIntersections;
                    result.parameter = iiResult.overlap;
                }
                else
                {
                    result.intersect = false;
                    result.numIntersections = 0;
                }
            }
        }
    };
}
