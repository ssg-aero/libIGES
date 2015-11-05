/*
 * file: tutorial1.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description:
 *  This program attempts to create a 3D compound curve in which
 *  a complete circular arc is represented by a single 9-point
 *  NURBS curve. If the result is generally accepted by MCADS,
 *  this may simplify the IGES models generated by the geom_pcb
 *  classes due to the compact form of the NURBS in comparison
 *  to the many-point Polynomial B-Spline currently used to
 *  represent circular arcs.
 *
 * This file is part of libIGES.
 *
 * libIGES is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, If not, see
 * <http://www.gnu.org/licenses/> or write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 * In this sample code we shall:
 * a. create a compound NURBS curve which traces the boundaries of
 *    a segment of a cylinder in Model Space
 * b. create a cylinder as a surface of revolution
 * c. create a Trimmed Parametric Surface consisting of the cylinder
 *    (b) and the bounding curve (a).
 * d. apply a magenta color to the surface (c)
 * e. close the trimmed cylinder by applying an untrimmed NURBS plane
 *    to fill the partial arc
 * f. cap the top with a trimmed NURBS plane
 * g. cap the bottom with a trimmed NURBS plane
 */

#include <iostream>
#include <cmath>
#include <api/dll_iges.h>
#include <api/all_api_entities.h>

int main()
{
    // instantiate the IGES data object
    DLL_IGES model;

    // Task A: creating a compound NURBS curve

    // create a compound curve
    DLL_IGES_ENTITY_102 compound( model, true );

    // create a NURBS curve to add to the
    DLL_IGES_ENTITY_126 nc( model, true );

    double SQHALF = sqrt( 0.5 );

    // knots for a circle
    double cknots[] = { 0.0, 0.0, 0.0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0, 1.0, 1.0 };

    // first circle's control points
    double cpts[] = {   1.0, 0.0, 0.0, 1.0,
                        1.0, 1.0, 0.0, SQHALF,
                        0.0, 1.0, 0.0, 1,
                        -1.0, 1.0, 0.0, SQHALF,
                        -1.0, 0.0, 0.0, 1,
                        -1.0, -1.0, 0.0, SQHALF,
                        0.0, -1.0, 0.0, 1,
                        1.0, -1.0, 0.0, SQHALF,
                        1.0, 0.0, 0.0, 1
    };

    // second circle's control points; the X and Y values are
    // negated here so that the arc is drawn in a CW direction
    // and is a projected reflection of the first arc along the Y axis
    double cpts2[] = {   -1.0, 0.0, -5.0, 1.0,
                         -1.0, -1.0, -5.0, SQHALF,
                         0.0, -1.0, -5.0, 1,
                         1.0, -1.0, -5.0, SQHALF,
                         1.0, 0.0, -5.0, 1,
                         1.0, 1.0, -5.0, SQHALF,
                         0.0, 1.0, -5.0, 1,
                         -1.0, 1.0, -5.0, SQHALF,
                         -1.0, 0.0, -5.0, 1
    };

    // create the first arc and add it to the compund curve
    nc.SetNURBSData( 9, 3, cknots, cpts, true, 0.125, 1 );
    compound.AddSegment( nc );

    // create a NURBS line from the endpoint of the first circle to
    // the start point of the second circle
    double lknots[] = { 0.0, 0.0, 1.0, 1.0 };
    double lpts[] = { 1.0, 0.0, 0.0,
                      1.0, 0.0, -5.0 };
    nc.NewEntity();
    nc.SetNURBSData( 2, 2, lknots, lpts, false, 0, 1.0 );
    compound.AddSegment( nc );

    // create a NURBS curve for the second arc
    nc.NewEntity();
    nc.SetNURBSData( 9, 3, cknots, cpts2, true, 0, 0.875 );
    compound.AddSegment( nc );

    // the second curve must also be accompanied by a transform
    // so that the arc's start point coincides with the vertical
    // line drawn from the first curve
    DLL_IGES_ENTITY_124 tx0( model, true );
    MCAD_TRANSFORM T;
    T.T.x = 0.0;
    T.T.z = -10.0;  // 2 * Z(lower arc)
    // set diagonal matrix to -1, +1, -1
    T.R.v[0][0] = -1.0;
    T.R.v[2][2] = -1.0;
    tx0.SetRootTransform( T );

    nc.SetTransform( tx0 );

    // create the NURBS line from the end of ARC2 to
    // the start of ARC1
    double lpts2[] = { SQHALF, SQHALF, -5.0,
                       SQHALF, SQHALF, 0.0 };
    nc.NewEntity();
    nc.SetNURBSData( 2, 2, lknots, lpts2, false, 0, 1.0 );
    compound.AddSegment( nc );

    // create the Surface of Revolution;
    //   + create axis
    //   + create generatrix
    //   + set startAngle and endAngle
    DLL_IGES_ENTITY_120 rev( model, true );
    DLL_IGES_ENTITY_110 mline( model, true );
    // axis
    mline.SetLineStart( 0.0, 0.0, -5.0 );
    mline.SetLineEnd( 0.0, 0.0, 0.0 );
    rev.SetAxis( mline );
    // generatrix
    mline.NewEntity();
    mline.SetLineStart( 1.0, 0.0, 0.0 );
    mline.SetLineEnd( 1.0, 0.0, -5.0 );
    rev.SetGeneratrix( mline );
    rev.SetAngles( 0.0, 2.0 * M_PI );

    // use the compound curve to define a surface boundary in model space
    DLL_IGES_ENTITY_142 bound( model, true );
    bound.SetModelSpaceBound( compound );
    bound.SetCurveCreationFlag( CURVE_CREATE_PROJECTION );
    bound.SetCurvePreference( BOUND_PREF_MODELSPACE );
    bound.SetSurface( rev );

    // create the Trimmed Parametric Surface (TPS)
    DLL_IGES_ENTITY_144 surf( model, true );
    surf.SetSurface( rev );
    surf.SetBoundCurve( bound );

    // create a custom color (magenta)
    DLL_IGES_ENTITY_314 color( model, true );
    color.SetColor( 100.0, 0.0, 100.0 );
    // attach the color to the trimmed surface
    surf.SetColor( color );

    // create a NURBS plane
    DLL_IGES_ENTITY_128 plane( model, true );
    double ppts[] = { 1.0, 0.0, 0.0, 1.0, 0.0, -5.0,
                      SQHALF, SQHALF, 0.0, SQHALF, SQHALF, -5.0 };
    plane.SetNURBSData( 2, 2, 2, 2, lknots, lknots, ppts, false, false, false, 0.0, 1.0, 0.0, 1.0 );
    // add the plane to a TPS and add the color; we do not specify a
    // bounding curve since the bounds of the plane section already
    // form the desired boundary
    surf.NewEntity();
    surf.SetSurface( plane );
    surf.SetColor( color );

    // create a trimmed NURBS plane to cap the bottom of the cylinder
    compound.NewEntity();
    // arc segment
    double cpts3[] = {   1.0, 0.0, -5.0, 1.0,
                        1.0, 1.0, -5.0, SQHALF,
                        0.0, 1.0, -5.0, 1,
                        -1.0, 1.0, -5.0, SQHALF,
                        -1.0, 0.0, -5.0, 1,
                        -1.0, -1.0, -5.0, SQHALF,
                        0.0, -1.0, -5.0, 1,
                        1.0, -1.0, -5.0, SQHALF,
                        1.0, 0.0, -5.0, 1
    };
    nc.NewEntity();
    nc.SetNURBSData( 9, 3, cknots, cpts3, true, 0.125, 1 );
    compound.AddSegment( nc );
    // line segment, bottom
    nc.NewEntity();
    double lpts3[] = { 1.0, 0.0, -5.0,
                       SQHALF, SQHALF, -5.0 };
    nc.SetNURBSData( 2, 2, lknots, lpts3, true, 0, 1 );
    compound.AddSegment( nc );
    // plane
    plane.NewEntity();
    double ppts2[] = { -1.0, -1.0, -5.0, 1.0, -1.0, -5.0,
                       -1.0, 1.0, -5.0, 1.0, 1.0, -5.0 };
    plane.SetNURBSData( 2, 2, 2, 2, lknots, lknots, ppts2, false, false, false, 0.0, 1.0, 0.0, 1.0 );
    // boundary
    bound.NewEntity();
    bound.SetModelSpaceBound( compound );
    bound.SetSurface( plane );
    // define the PTS
    surf.NewEntity();
    surf.SetSurface( plane );
    surf.SetBoundCurve( bound );
    surf.SetColor( color );

    // create a trimmed NURBS plane to cap the top of the cylinder
    compound.NewEntity();
    // arc segment
    nc.NewEntity();
    nc.SetNURBSData( 9, 3, cknots, cpts, true, 0.125, 1 );
    compound.AddSegment( nc );
    // line segment, bottom
    nc.NewEntity();
    double lpts4[] = { 1.0, 0.0, 0.0,
                       SQHALF, SQHALF, 0.0 };
    nc.SetNURBSData( 2, 2, lknots, lpts3, true, 0, 1 );
    compound.AddSegment( nc );
    // plane
    plane.NewEntity();
    double ppts3[] = { -1.0, -1.0, 0.0, 1.0, -1.0, 0.0,
                       -1.0, 1.0, 0.0, 1.0, 1.0, 0.0 };
    plane.SetNURBSData( 2, 2, 2, 2, lknots, lknots, ppts3, false, false, false, 0.0, 1.0, 0.0, 1.0 );
    // boundary
    bound.NewEntity();
    bound.SetModelSpaceBound( compound );
    bound.SetSurface( plane );
    // define the PTS
    surf.NewEntity();
    surf.SetSurface( plane );
    surf.SetBoundCurve( bound );
    surf.SetColor( color );

    model.Write( "tutorial1.igs", true );

    return 0;
}

using namespace std;
