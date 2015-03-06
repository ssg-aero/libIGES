/*
 * file: entity110.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
 *
 * Description: IGES Entity 110: Line, Section 4.13, p.96+ (124+)
 *
 * This file is part of libIGES.
 *
 * libIGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libIGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libIGES.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <sstream>
#include <error_macros.h>
#include <iges.h>
#include <iges_io.h>
#include <entity110.h>
#include <entity124.h>

using namespace std;


IGES_ENTITY_110::IGES_ENTITY_110( IGES* aParent ) : IGES_CURVE( aParent )
{
    entityType = 110;
    form = 0;
    return;
}


IGES_ENTITY_110::~IGES_ENTITY_110()
{
    return;
}


bool IGES_ENTITY_110::associate( std::vector<IGES_ENTITY*>* entities )
{
    if( !IGES_ENTITY::associate( entities ) )
    {
        ERRMSG << "\n + [INFO] failed to establish associations\n";
        return false;
    }

    if( pStructure )
    {
        ERRMSG << "\n + [VIOLATION] Structure entity is set\n";
        pStructure->DelReference( this );
        pStructure = NULL;
    }
    
    return true;
}


bool IGES_ENTITY_110::format( int &index )
{
    pdout.clear();
    iExtras.clear();

    if( index < 1 || index > 9999999 )
    {
        ERRMSG << "\n + [INFO] invalid Parameter Data Sequence Number\n";
        return false;
    }

    parameterData = index;

    if( !parent )
    {
        ERRMSG << "\n + [INFO] method invoked with no parent IGES object\n";
        return false;
    }

    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;
    double uir = parent->globalData.minResolution;

    ostringstream ostr;
    ostr << entityType << pd;
    string lstr = ostr.str();
    string tstr;

    double* pt[6] = { &X1, &Y1, &Z1, &X2, &Y2, &Z2 };

    for( int i = 0; i < 6; ++i )
    {
        if( i == 5 && extras.empty() )
        {
            if( !FormatPDREal( tstr, *pt[i], rd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format Line datum [";
                cerr << i << "]\n";
                return false;
            }
        }
        else
        {
            if( !FormatPDREal( tstr, *pt[i], pd, uir ) )
            {
                ERRMSG << "\n + [INFO] could not format Line datum [";
                cerr << i << "]\n";
                return false;
            }
        }

        AddPDItem( tstr, lstr, pdout, index, sequenceNumber, pd, rd );

    }

    if( !extras.empty() && !formatExtraParams( lstr, index, pd, rd ) )
    {
        ERRMSG << "\n + [INFO] could not format optional parameters\n";
        pdout.clear();
        iExtras.clear();
        return false;
    }

    if( !formatComments( index ) )
    {
        ERRMSG << "\n + [INFO] could not format comments\n";
        pdout.clear();
        return false;
    }

    paramLineCount = index - parameterData;

    return true;
}


bool IGES_ENTITY_110::rescale( double sf )
{
    X1 *= sf;
    Y1 *= sf;
    Z1 *= sf;
    X2 *= sf;
    Y2 *= sf;
    Z2 *= sf;

    return true;
}


bool IGES_ENTITY_110::Unlink( IGES_ENTITY* aChildEntity )
{
    return IGES_ENTITY::Unlink( aChildEntity );
}


bool IGES_ENTITY_110::IsOrphaned( void )
{
    if( refs.empty() && depends != STAT_INDEPENDENT )
        return true;

    return false;
}


bool IGES_ENTITY_110::IGES_ENTITY_110::AddReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::AddReference( aParentEntity );
}


bool IGES_ENTITY_110::DelReference( IGES_ENTITY* aParentEntity )
{
    return IGES_ENTITY::DelReference( aParentEntity );
}


bool IGES_ENTITY_110::ReadDE( IGES_RECORD* aRecord, std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadDE( aRecord, aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] failed to read Directory Entry\n";
        return false;
    }

    structure = 0;                  // N.A.
    hierarchy = STAT_HIER_ALL_SUB;  // field ignored

    if( form != 0 && form != 1 && form != 2 )
    {
        ERRMSG << "\n + [CORRUPT FILE] invalid Form Number (" << form << ") in Line\n";
        cerr << " + DE: " << aRecord->index << "\n";
        return false;
    }

    return true;
}


bool IGES_ENTITY_110::ReadPD( std::ifstream& aFile, int& aSequenceVar )
{
    if( !IGES_ENTITY::ReadPD( aFile, aSequenceVar ) )
    {
        ERRMSG << "\n + [INFO] could not read data for Line Entity\n";
        return false;
    }

    int idx;
    bool eor = false;
    char pd = parent->globalData.pdelim;
    char rd = parent->globalData.rdelim;

    idx = pdout.find( pd );

    if( idx < 1 || idx > 8 )
    {
        ERRMSG << "\n + [BAD FILE] strange index for first parameter delimeter (";
        cerr << idx << ")\n";
        return false;
    }

    ++idx;

    if( !ParseReal( pdout, idx, X1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no X1 value for Line Entity\n";
        return false;
    }

    if( !ParseReal( pdout, idx, Y1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no Y1 value for Line Entity\n";
        return false;
    }

    if( !ParseReal( pdout, idx, Z1, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no Z1 value for Line Entity\n";
        return false;
    }

    if( !ParseReal( pdout, idx, X2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no X2 value for Line Entity\n";
        return false;
    }

    if( !ParseReal( pdout, idx, Y2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no Y2 value for Line Entity\n";
        return false;
    }

    if( !ParseReal( pdout, idx, Z2, eor, pd, rd ) )
    {
        ERRMSG << "\n + [BAD FILE] no Z2 value for Line Entity\n";
        return false;
    }

    if( !eor && !readExtraParams( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read optional pointers\n";
        return false;
    }

    if( !readComments( idx ) )
    {
        ERRMSG << "\n + [BAD FILE] could not read extra comments\n";
        return false;
    }

    if( parent->globalData.convert )
    {
        double cf = parent->globalData.cf;
        X1 *= cf;
        Y1 *= cf;
        Z1 *= cf;
        X2 *= cf;
        Y2 *= cf;
        Z2 *= cf;
    }

    pdout.clear();
    return true;
}


bool IGES_ENTITY_110::SetEntityForm( int aForm )
{
    if( aForm != 0 && aForm != 1 && aForm != 2 )
    {
        ERRMSG << "\n + [BUG] invalid Form Number (" << aForm << ") specified\n";
        return false;
    }

    form = aForm;
    return true;
}


bool IGES_ENTITY_110::SetHierarchy( IGES_STAT_HIER aHierarchy )
{
    ERRMSG << "\n + [WARNING] hierarchy is not supported by the Line Entity\n";
    return true;
}


IGES_POINT IGES_ENTITY_110::GetStartPoint( bool xform )
{
    IGES_POINT p( X1, Y1, Z1 );

    if( xform && pTransform )
    {
        IGES_ENTITY_124* pT = (IGES_ENTITY_124*)pTransform;
        p = pT->GetTransformMatrix() * p;
    }

    return p;
}


IGES_POINT IGES_ENTITY_110::GetEndPoint( bool xform )
{
    IGES_POINT p( X2, Y2, Z2 );

    if( xform && pTransform )
    {
        IGES_ENTITY_124* pT = (IGES_ENTITY_124*)pTransform;
        p = pT->GetTransformMatrix() * p;
    }

    return p;
}


int IGES_ENTITY_110::GetNSegments( void )
{
    return 1;
}


bool IGES_ENTITY_110::IsClosed()
{
    return false;
}


int IGES_ENTITY_110::GetNCurves( void )
{
    return 0;
}


IGES_CURVE* IGES_ENTITY_110::GetCurve( int index )
{
    return NULL;
}


bool IGES_ENTITY_110::Interpolate( IGES_POINT& pt, int nSeg, double var, bool xform )
{
    // XXX - TO BE IMPLEMENTED
    return false;
}
