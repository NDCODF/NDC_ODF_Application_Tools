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

#include <o3tl/make_unique.hxx>

#include <svl/itemset.hxx>
#include <svl/style.hxx>
#include <svl/smplhint.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdpool.hxx>

#include "unchss.hxx"

#include "strings.hrc"
#include "glob.hxx"
#include "sdresid.hxx"
#include "drawdoc.hxx"
#include "stlsheet.hxx"
#include "glob.hrc"


StyleSheetUndoAction::StyleSheetUndoAction(SdDrawDocument* pTheDoc,
                                           SfxStyleSheet* pTheStyleSheet,
                                           const SfxItemSet* pTheNewItemSet) :
                      SdUndoAction(pTheDoc)
{
    DBG_ASSERT(pTheStyleSheet, "Undo without StyleSheet ???");
    mpStyleSheet = pTheStyleSheet;

    // Create ItemSets; Attention, it is possible that the new one is from a,
    // different pool. Therefore we clone it with its items.
    mpNewSet = o3tl::make_unique<SfxItemSet>(static_cast<SfxItemPool&>(SdrObject::GetGlobalDrawObjectItemPool()), pTheNewItemSet->GetRanges());
    SdrModel::MigrateItemSet( pTheNewItemSet, mpNewSet.get(), pTheDoc );

    mpOldSet = o3tl::make_unique<SfxItemSet>(static_cast<SfxItemPool&>(SdrObject::GetGlobalDrawObjectItemPool()), mpStyleSheet->GetItemSet().GetRanges());
    SdrModel::MigrateItemSet( &mpStyleSheet->GetItemSet(), mpOldSet.get(), pTheDoc );

    maComment = SD_RESSTR(STR_UNDO_CHANGE_PRES_OBJECT);
    OUString aName(mpStyleSheet->GetName());

    // delete layout name and separator
    sal_Int32 nPos = aName.indexOf(SD_LT_SEPARATOR);
    if (nPos != -1)
        aName = aName.copy(nPos + strlen(SD_LT_SEPARATOR));

    if (aName == SD_RESSTR(STR_LAYOUT_TITLE))
    {
        aName = SD_RESSTR(STR_PSEUDOSHEET_TITLE);
    }
    else if (aName == SD_RESSTR(STR_LAYOUT_SUBTITLE))
    {
        aName = SD_RESSTR(STR_PSEUDOSHEET_SUBTITLE);
    }
    else if (aName == SD_RESSTR(STR_LAYOUT_BACKGROUND))
    {
        aName = SD_RESSTR(STR_PSEUDOSHEET_BACKGROUND);
    }
    else if (aName == SD_RESSTR(STR_LAYOUT_BACKGROUNDOBJECTS))
    {
        aName = SD_RESSTR(STR_PSEUDOSHEET_BACKGROUNDOBJECTS);
    }
    else if (aName == SD_RESSTR(STR_LAYOUT_NOTES))
    {
        aName = SD_RESSTR(STR_PSEUDOSHEET_NOTES);
    }
    else
    {
        OUString aOutlineStr(SD_RESSTR(STR_PSEUDOSHEET_OUTLINE));
        nPos = aName.indexOf(aOutlineStr);
        if (nPos != -1)
        {
            OUString aNumStr(aName.copy(aOutlineStr.getLength()));
            aName = SD_RESSTR(STR_LAYOUT_OUTLINE) + aNumStr;
        }
    }

    // replace placeholder with template name
    maComment = maComment.replaceFirst("$", aName);
}

void StyleSheetUndoAction::Undo()
{
    SfxItemSet aNewSet( mpDoc->GetItemPool(), mpOldSet->GetRanges() );
    SdrModel::MigrateItemSet( mpOldSet.get(), &aNewSet, mpDoc );

    mpStyleSheet->GetItemSet().Set(aNewSet);
    if( mpStyleSheet->GetFamily() == SD_STYLE_FAMILY_PSEUDO )
        static_cast<SdStyleSheet*>(mpStyleSheet)->GetRealStyleSheet()->Broadcast(SfxSimpleHint(SFX_HINT_DATACHANGED));
    else
        mpStyleSheet->Broadcast(SfxSimpleHint(SFX_HINT_DATACHANGED));
}

void StyleSheetUndoAction::Redo()
{
    SfxItemSet aNewSet( mpDoc->GetItemPool(), mpOldSet->GetRanges() );
    SdrModel::MigrateItemSet( mpNewSet.get(), &aNewSet, mpDoc );

    mpStyleSheet->GetItemSet().Set(aNewSet);
    if( mpStyleSheet->GetFamily() == SD_STYLE_FAMILY_PSEUDO )
        static_cast<SdStyleSheet*>(mpStyleSheet)->GetRealStyleSheet()->Broadcast(SfxSimpleHint(SFX_HINT_DATACHANGED));
    else
        mpStyleSheet->Broadcast(SfxSimpleHint(SFX_HINT_DATACHANGED));
}

OUString StyleSheetUndoAction::GetComment() const
{
    return maComment;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
