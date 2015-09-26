/*
 * file: dll_entity408.cpp
 *
 * Copyright 2015, Dr. Cirilo Bernardo (cirilo.bernardo@gmail.com)
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

#include <dll_entity408.h>
#include <dll_iges.h>
#include <iges.h>
#include <entity408.h>


DLL_IGES_ENTITY_408::DLL_IGES_ENTITY_408( IGES* aParent, bool create )
{
    m_type = ENT_SINGULAR_SUBFIGURE_INSTANCE;

    if( create )
    {
        if( NULL != aParent )
            aParent->NewEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, &m_entity );
        else
            m_entity = new IGES_ENTITY_408( NULL );

        if( NULL != m_entity )
            m_entity->AttachValidFlag( &m_valid );
    }

    return;
}


DLL_IGES_ENTITY_408::DLL_IGES_ENTITY_408( DLL_IGES& aParent, bool create )
{
    m_type = ENT_SINGULAR_SUBFIGURE_INSTANCE;
    IGES* ip = aParent.GetRawPtr();

    if( !create || NULL == ip )
        return;

    ip->NewEntity( ENT_SINGULAR_SUBFIGURE_INSTANCE, &m_entity );

    if( NULL != m_entity )
        m_entity->AttachValidFlag( &m_valid );

    return;
}


DLL_IGES_ENTITY_408::~DLL_IGES_ENTITY_408()
{
    return;
}


bool DLL_IGES_ENTITY_408::GetSubfigure( IGES_ENTITY_308*& aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_408*)m_entity)->GetDE( aPtr );
}


bool DLL_IGES_ENTITY_408::SetSubfigure( IGES_ENTITY_308* aPtr )
{
    if( !m_valid || NULL == m_entity )
        return false;

    return ((IGES_ENTITY_408*)m_entity)->SetDE( aPtr );
}


bool DLL_IGES_ENTITY_408::GetSubfigParams( double& aX, double& aY, double& aZ, double& aScale )
{
    if( !m_valid || NULL == m_entity )
        return false;

    IGES_ENTITY_408* ip = (IGES_ENTITY_408*)m_entity;
    aX = ip->X;
    aY = ip->Y;
    aZ = ip->Z;
    aScale = ip->S;
    return true;
}


bool DLL_IGES_ENTITY_408::SetSubfigParams( double aX, double aY, double aZ, double aScale )
{
    if( !m_valid || NULL == m_entity )
        return false;

    IGES_ENTITY_408* ip = (IGES_ENTITY_408*)m_entity;
    ip->X = aX;
    ip->Y = aY;
    ip->Z = aZ;
    ip->S = aScale;
    return true;
}
