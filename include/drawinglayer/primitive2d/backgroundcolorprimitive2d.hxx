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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_BACKGROUNDCOLORPRIMITIVE2D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE2D_BACKGROUNDCOLORPRIMITIVE2D_HXX

#include <drawinglayer/drawinglayerdllapi.h>

#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <basegfx/color/bcolor.hxx>


// BackgroundColorPrimitive2D class

namespace drawinglayer
{
    namespace primitive2d
    {
        /** BackgroundColorPrimitive2D class

            This primitive is defined to fill the whole visible Viewport with
            the given color (and thus decomposes to a filled polygon). This
            makes it a view-dependent primitive by definition. It only has
            a valid decomposition if a valid Viewport is given in the
            ViewInformation2D at decomposition time.

            It will try to buffer its last decomposition using maLastViewport
            to detect changes in the get2DDecomposition call.
         */
        class DRAWINGLAYER_DLLPUBLIC BackgroundColorPrimitive2D : public BufferedDecompositionPrimitive2D
        {
        private:
            /// the fill color to use
            basegfx::BColor                             maBColor;
            double mfTransparency;

            /// the last used viewInformation, used from getDecomposition for buffering
            basegfx::B2DRange                           maLastViewport;

        protected:
            /// create local decomposition
            virtual Primitive2DContainer create2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const override;

        public:
            /// constructor
            explicit BackgroundColorPrimitive2D(
                const basegfx::BColor& rBColor,
                double fTransparency = 0);

            /// data read access
            const basegfx::BColor& getBColor() const { return maBColor; }
            double getTransparency() const { return mfTransparency; }

            /// compare operator
            virtual bool operator==(const BasePrimitive2D& rPrimitive) const override;

            /// get B2Drange
            virtual basegfx::B2DRange getB2DRange(const geometry::ViewInformation2D& rViewInformation) const override;

            /// provide unique ID
            DeclPrimitive2DIDBlock()

            /// Override standard getDecomposition call to be view-dependent here
            virtual Primitive2DContainer get2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const override;
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer


#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE2D_BACKGROUNDCOLORPRIMITIVE2D_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
