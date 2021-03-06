/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "svgdialog.hxx"
#include "impsvgdialog.hxx"

#include <vcl/svapp.hxx>
#include <vcl/dialog.hxx>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/view/XRenderable.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>

#define SVG_DIALOG_SERVICE_NAME         "com.sun.star.comp.Draw.SVGFilterDialog"
#define SVG_DIALOG_IMPLEMENTATION_NAME  SVG_DIALOG_SERVICE_NAME
#define SVG_FILTER_DATA_NAME            "FilterData"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::view;
using namespace ::com::sun::star::document;


OUString SVGDialog_getImplementationName ()
{
    return OUString ( SVG_DIALOG_IMPLEMENTATION_NAME );
}


Sequence< OUString > SVGDialog_getSupportedServiceNames()
{
    Sequence< OUString > aRet { SVG_DIALOG_SERVICE_NAME };
    return aRet;
}


Reference< XInterface > SVGDialog_createInstance( const Reference< XMultiServiceFactory > & rSMgr )
{
    return static_cast< cppu::OWeakObject* >( new SVGDialog( comphelper::getComponentContext(rSMgr) ) );
}


SVGDialog::SVGDialog( const Reference< XComponentContext > &rxContext ) :
    OGenericUnoDialog( rxContext )
{
}


SVGDialog::~SVGDialog()
{
}


Any SAL_CALL SVGDialog::queryInterface( const Type& rType )
{
    Any aReturn( OGenericUnoDialog::queryInterface( rType ) );

    if( !aReturn.hasValue() )
    {
        aReturn = ::cppu::queryInterface( rType, static_cast< XPropertyAccess* >( this ),
                                                 static_cast< XExporter* >( this ) );
    }

    return aReturn;
}


void SAL_CALL SVGDialog::acquire()
    throw ()
{
    OWeakObject::acquire();
}


void SAL_CALL SVGDialog::release()
    throw ()
{
    OWeakObject::release();
}


Sequence< sal_Int8 > SAL_CALL SVGDialog::getImplementationId()
{
    return css::uno::Sequence<sal_Int8>();
}


OUString SAL_CALL SVGDialog::getImplementationName()
{
    return SVGDialog_getImplementationName();
}


Sequence< OUString > SAL_CALL SVGDialog::getSupportedServiceNames()
{
    return SVGDialog_getSupportedServiceNames();
}

svt::OGenericUnoDialog::Dialog SVGDialog::createDialog( vcl::Window* pParent )
{
    if( mxSrcDoc.is() )
        return svt::OGenericUnoDialog::Dialog(VclPtr<ImpSVGDialog>::Create( pParent, maFilterData ));
    else
        return svt::OGenericUnoDialog::Dialog(VclPtr<::Dialog>());
}

void SVGDialog::executedDialog( sal_Int16 nExecutionResult )
{
    if (nExecutionResult && m_aDialog)
        maFilterData = static_cast< ImpSVGDialog* >(m_aDialog.m_xVclDialog.get())->GetFilterData();

    destroyDialog();
}


Reference< XPropertySetInfo > SAL_CALL SVGDialog::getPropertySetInfo()
{
    return createPropertySetInfo( getInfoHelper() );
}


::cppu::IPropertyArrayHelper& SVGDialog::getInfoHelper()
{
    return *getArrayHelper();
}


::cppu::IPropertyArrayHelper* SVGDialog::createArrayHelper() const
{
    Sequence< Property > aProps;

    describeProperties(aProps);

    return new ::cppu::OPropertyArrayHelper( aProps );
}


Sequence< PropertyValue > SAL_CALL SVGDialog::getPropertyValues()
{
    sal_Int32 i, nCount;

    for( i = 0, nCount = maMediaDescriptor.getLength(); i < nCount; ++i )
    {
        if( maMediaDescriptor[ i ].Name == SVG_FILTER_DATA_NAME )
            break;
    }

    if( i == nCount )
    {
        maMediaDescriptor.realloc( ++nCount );
        maMediaDescriptor[ i ].Name = SVG_FILTER_DATA_NAME;
    }

    maMediaDescriptor[ i ].Value <<= maFilterData;

    return maMediaDescriptor;
}


void SAL_CALL SVGDialog::setPropertyValues( const Sequence< PropertyValue >& rProps )
{
    maMediaDescriptor = rProps;

    for( sal_Int32 i = 0, nCount = maMediaDescriptor.getLength(); i < nCount; i++ )
    {
        if( maMediaDescriptor[ i ].Name == SVG_FILTER_DATA_NAME )
        {
            maMediaDescriptor[ i ].Value >>= maFilterData;
            break;
        }
    }
}


void SAL_CALL SVGDialog::setSourceDocument( const Reference< XComponent >& xDoc )
{
    mxSrcDoc = xDoc;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
