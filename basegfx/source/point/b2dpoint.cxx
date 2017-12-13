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

#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/numeric/ftools.hxx>

namespace basegfx
{
    B2DPoint& B2DPoint::operator=( const ::basegfx::B2DTuple& rPoint )
    {
        mfX = rPoint.getX();
        mfY = rPoint.getY();
        return *this;
    }

    B2DPoint& B2DPoint::operator*=( const ::basegfx::B2DHomMatrix& rMat )
    {
        double fTempX(
            rMat.get(0, 0) * mfX +
            rMat.get(0, 1) * mfY +
            rMat.get(0, 2));
        double fTempY(
            rMat.get(1, 0) * mfX +
            rMat.get(1, 1) * mfY +
            rMat.get(1, 2));

        if(!rMat.isLastLineDefault())
        {
            const double fOne(1.0);
            const double fTempM(
                rMat.get(2, 0) * mfX +
                rMat.get(2, 1) * mfY +
                rMat.get(2, 2));

            if(!fTools::equalZero(fTempM) && !fTools::equal(fOne, fTempM))
            {
                fTempX /= fTempM;
                fTempY /= fTempM;
            }
        }

        mfX = fTempX;
        mfY = fTempY;

        return *this;
    }

    B2DPoint operator*( const ::basegfx::B2DHomMatrix& rMat, const B2DPoint& rPoint )
    {
        B2DPoint aRes( rPoint );
        return aRes *= rMat;
    }
} // end of namespace basegfx

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
