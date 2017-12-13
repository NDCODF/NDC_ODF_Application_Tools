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
#ifndef INCLUDED_EDITENG_UDLNITEM_HXX
#define INCLUDED_EDITENG_UDLNITEM_HXX

#include <svl/eitem.hxx>
#include <vcl/vclenum.hxx>
#include <tools/color.hxx>
#include <editeng/editengdllapi.h>

class SvXMLUnitConverter;

// class SvxTextLineItem ------------------------------------------------

/* Value container for underline and overline font effects */

class EDITENG_DLLPUBLIC SvxTextLineItem : public SfxEnumItem
{
    Color mColor;
public:
    static SfxPoolItem* CreateDefault();

    SvxTextLineItem( const FontLineStyle eSt,
                     const sal_uInt16 nId );

    // "pure virtual Methods" from SfxPoolItem
    virtual bool GetPresentation( SfxItemPresentation ePres,
                            SfxMapUnit eCoreMetric,
                            SfxMapUnit ePresMetric,
                            OUString &rText, const IntlWrapper * = nullptr ) const override;

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = nullptr ) const override;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const override;
    virtual SvStream&       Store(SvStream &, sal_uInt16 nItemVersion) const override;
    virtual OUString   GetValueTextByPos( sal_uInt16 nPos ) const override;
    virtual sal_uInt16      GetValueCount() const override;

    virtual bool            QueryValue( css::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const override;
    virtual bool            PutValue( const css::uno::Any& rVal, sal_uInt8 nMemberId ) override;

    // MS VC4.0 messes things up
    void                    SetValue( sal_uInt16 nNewVal )
                                {SfxEnumItem::SetValue(nNewVal); }
    virtual bool            HasBoolValue() const override;
    virtual bool            GetBoolValue() const override;
    virtual void            SetBoolValue( bool bVal ) override;

    virtual bool            operator==( const SfxPoolItem& ) const override;

    inline SvxTextLineItem& operator=(const SvxTextLineItem& rTextLine)
        {
            SetValue( rTextLine.GetValue() );
            SetColor( rTextLine.GetColor() );
            return *this;
        }

    // enum cast
    FontLineStyle           GetLineStyle() const
                                { return (FontLineStyle)GetValue(); }
    void                    SetLineStyle( FontLineStyle eNew )
                                { SetValue((sal_uInt16) eNew); }

    const Color&            GetColor() const                { return mColor; }
    void                    SetColor( const Color& rCol )   { mColor = rCol; }
};

// class SvxUnderlineItem ------------------------------------------------

/* Value container for underline font effects */

class EDITENG_DLLPUBLIC SvxUnderlineItem : public SvxTextLineItem
{
public:
    static SfxPoolItem* CreateDefault();

    SvxUnderlineItem( const FontLineStyle eSt,
                      const sal_uInt16 nId );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = nullptr ) const override;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const override;
    virtual OUString   GetValueTextByPos( sal_uInt16 nPos ) const override;
};

// class SvxOverlineItem ------------------------------------------------

/* Value container for overline font effects */

class EDITENG_DLLPUBLIC SvxOverlineItem : public SvxTextLineItem
{
public:
    static SfxPoolItem* CreateDefault();

    SvxOverlineItem( const FontLineStyle eSt,
                     const sal_uInt16 nId );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = nullptr ) const override;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const override;
    virtual OUString   GetValueTextByPos( sal_uInt16 nPos ) const override;
};

#endif // INCLUDED_EDITENG_UDLNITEM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
