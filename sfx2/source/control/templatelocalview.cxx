/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sfx2/templatelocalview.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/string.hxx>
#include <sfx2/doctempl.hxx>
#include <sfx2/inputdlg.hxx>
#include <sfx2/sfxresid.hxx>
#include <sfx2/templatecontaineritem.hxx>
#include <sfx2/templateviewitem.hxx>
#include <svl/inettype.hxx>
#include <tools/urlobj.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <vcl/builderfactory.hxx>
#include <vcl/pngread.hxx>
#include <vcl/layout.hxx>

#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/frame/DocumentTemplates.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XDocumentTemplates.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#include "../doc/doc.hrc"

#define TemplateCache 1

#if TemplateCache

#include <tools/stream.hxx>
#include <vcl/pngwrite.hxx>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#if defined(_WIN32)
#include <direct.h>
#include <systools/win32/uwinapi.h>
#else
#include <rtl/bootstrap.hxx>
#include <osl/file.h>
#include <osl/file.hxx>
#include <unotools/pathoptions.hxx>
#endif

sal_uInt16 firstrun = 0, exist_listfile = 0;
using namespace std;

struct ThumbnailCache {
    int no;
    char date[40];
    char mapname[20];
    char filename[120];
};

vector<struct ThumbnailCache> cachedata ;

#define X_OFFSET 15
#define Y_OFFSET 15
class WaitWindow_Impl : public WorkWindow
{
    Rectangle     maRect;
    DrawTextFlags mnTextStyle;
    OUString      maText;

public:
    WaitWindow_Impl(sal_uInt16);
    virtual ~WaitWindow_Impl();
    virtual void dispose() override;
    virtual void Paint(vcl::RenderContext& rRenderContext, const Rectangle& rRect) override;
};

WaitWindow_Impl::WaitWindow_Impl(sal_uInt16 first_run) : WorkWindow(nullptr, WB_BORDER | WB_3DLOOK)
{
    Rectangle aRect = Rectangle(0, 0, 400, 30000);
    mnTextStyle = DrawTextFlags::Center | DrawTextFlags::VCenter | DrawTextFlags::WordBreak | DrawTextFlags::MultiLine;
    if(first_run)
        maText = SfxResId(RID_CNT_STR_CACHE).toString();
    else
        maText = SfxResId(RID_CNT_STR_LOAD).toString();
    maRect = GetTextRect(aRect, maText, mnTextStyle);
    aRect = maRect;
    aRect.Right() += 2 * X_OFFSET;
    aRect.Bottom() += 2 * Y_OFFSET;
    maRect.SetPos(Point(X_OFFSET, Y_OFFSET));
    SetOutputSizePixel(aRect.GetSize());

    Show();
    Update();
    Flush();
}

WaitWindow_Impl::~WaitWindow_Impl()
{
    disposeOnce();
}

void  WaitWindow_Impl::dispose()
{
    Hide();
    WorkWindow::dispose();
}

void WaitWindow_Impl::Paint(vcl::RenderContext& rRenderContext, const Rectangle& /*rRect*/)
{
    rRenderContext.DrawText(maRect, maText, mnTextStyle);
}

#endif

#define MNI_OPEN               1
#define MNI_EDIT               2
#define MNI_DEFAULT_TEMPLATE   3
#define MNI_DELETE             4
#define MNI_RENAME             5

using namespace ::com::sun::star;
using namespace ::com::sun::star::frame;

TemplateLocalView::TemplateLocalView ( vcl::Window* pParent)
    : TemplateAbstractView(pParent),
      mpDocTemplates(new SfxDocumentTemplates)
{
}

VCL_BUILDER_FACTORY(TemplateLocalView)

TemplateLocalView::~TemplateLocalView()
{
    disposeOnce();
}

void TemplateLocalView::dispose()
{
    for ( TemplateContainerItem* pRegion : maRegions)
        delete pRegion;

    maRegions.clear();

    maAllTemplates.clear();

    delete mpDocTemplates;
    TemplateAbstractView::dispose();
}

#if TemplateCache
OUString getCacheFolder()
{
    #if defined(_WIN32)
        OUString url("${$BRAND_BASE_DIR/" LIBO_ETC_FOLDER "/" SAL_CONFIGFILE("bootstrap") ":UserInstallation}/cache/");
    #else
        //~ OUString url("$${$$BRAND_BASE_DIR/$(LIBO_ETC_FOLDER)/$(call gb_Helper_get_rcfile,bootstrap):UserInstallation}/cache");
        SvtPathOptions aPathOpt;
        OUString url = aPathOpt.GetUserConfigPath() + "/../../cache";
    #endif
    rtl::Bootstrap::expandMacros(url);
    osl::Directory::create(url);

    if( url.startsWith( "file://" ) )
    {
        OUString aSysPath;
        if( osl_getSystemPathFromFileURL( url.pData, &aSysPath.pData ) == osl_File_E_None )
            url = aSysPath;
    }
    return url;
}

void TemplateLocalView::readlistdata()
{
    cachedata.clear();
    // read list.txt data to struct
    char line[200];
    int nAllCount = 0;
    struct ThumbnailCache tmpcachedata = {0,"","",""};
    OUString Listurl = getCacheFolder() + "\\pic\\list.txt";

    FILE *pf = NULL;

    if ((pf = fopen (OUStringToOString( Listurl, RTL_TEXTENCODING_UTF8 ).getStr(), "r")) == NULL) {
        printf ( "could not open file\n");
    }else{
        printf ( "Success open file\n");
        while ((fgets (line, sizeof (line), pf))) {
            if ((sscanf (line, "%d %24[^\n] %10s %120[^\n]"
            ,&tmpcachedata.no ,tmpcachedata.date ,tmpcachedata.mapname ,tmpcachedata.filename)) == 4) {
                cachedata.push_back(tmpcachedata);
                exist_listfile = 1;
            }
        }

        sal_uInt16 nCount = mpDocTemplates->GetRegionCount();
        for (sal_uInt16 i = 0; i < nCount; ++i)
        {
            sal_uInt16 nEntries = mpDocTemplates->GetCount(i);

            for (sal_uInt16 j = 0; j < nEntries; ++j)
            {
                //~ OUString aName = mpDocTemplates->GetName(i,j);
                OUString OUS_mapname;
                if(exist_listfile)
                    OUS_mapname += OStringToOUString(string(cachedata[nAllCount].filename).c_str(), RTL_TEXTENCODING_UTF8);
                nAllCount++;
            }
        }
        fclose(pf);
    }

    if ((nAllCount != cachedata.size()) || cachedata.size() == 0)
        firstrun = 1;
}
#endif

void TemplateLocalView::Populate ()
{
    for (TemplateContainerItem* pRegion : maRegions)
        delete pRegion;

    maRegions.clear();

    maAllTemplates.clear();

#if TemplateCache

    VclPtrInstance< WaitWindow_Impl > pWin(firstrun);
    #define PATH_SIZE 256
    #if defined(_WIN32)
    OUString PicPath = getCacheFolder() + "\\pic";
    #else
    OUString PicPath = getCacheFolder() + "/pic";
    #endif
    FILE* fstream = NULL;
    sal_uInt16 nAllCount = 1, nAllCount2 = 0;
    char cList[PATH_SIZE],cIndex[PATH_SIZE],cDir[PATH_SIZE],cDirB[PATH_SIZE],cDirS[PATH_SIZE];
    char cFile[PATH_SIZE],csFile[PATH_SIZE];
    snprintf(cDir,PATH_SIZE,"%s",OUStringToOString( PicPath, RTL_TEXTENCODING_UTF8 ).getStr());
    #if defined(_WIN32)
    snprintf(cDirB,PATH_SIZE,"%s\\b",cDir);
    snprintf(cDirS,PATH_SIZE,"%s\\s",cDir);
    snprintf(cList,PATH_SIZE,"%s\\list.txt",cDir);
    #else
    snprintf(cDirB,PATH_SIZE,"%s/b",cDir);
    snprintf(cDirS,PATH_SIZE,"%s/s",cDir);
    snprintf(cList,PATH_SIZE,"%s/list.txt",cDir);
    #endif
    /* if directory does not exist, create it */
    struct stat *cDirbuf = (struct stat *) malloc(sizeof(struct stat));
    struct stat *cFilebuf = (struct stat *) malloc(sizeof(struct stat));
    if (stat(cDirB, cDirbuf) != 0 || stat(cDirS, cDirbuf) != 0){
        #if defined(_WIN32)
        CreateDirectory(cDir, NULL);
        CreateDirectory(cDirB, NULL);
        CreateDirectory(cDirS, NULL);
        #else
        mkdir(cDir,S_IRWXU | S_IRWXG |S_IRWXO);
        mkdir(cDirB,S_IRWXU | S_IRWXG |S_IRWXO);
        mkdir(cDirS,S_IRWXU | S_IRWXG |S_IRWXO);
        #endif
    }else{
        if( remove( cList ) != 0 )
            perror( "Error deleting file" );
        else
            printf("remove list.txt OK!\n");
    }

    fstream = fopen(cList,"a");

#endif

    sal_uInt16 nCount = mpDocTemplates->GetRegionCount();
    for (sal_uInt16 i = 0; i < nCount; ++i)
    {
        OUString aRegionName(mpDocTemplates->GetFullRegionName(i));

        TemplateContainerItem* pItem = new TemplateContainerItem( i+1 );
        pItem->mnRegionId = i;
        pItem->maTitle = aRegionName;

        sal_uInt16 nEntries = mpDocTemplates->GetCount(i);

        for (sal_uInt16 j = 0; j < nEntries; ++j)
        {
            OUString aName = mpDocTemplates->GetName(i,j);
            OUString aURL = mpDocTemplates->GetPath(i,j);

            TemplateItemProperties aProperties;
            aProperties.nId = j+1;
            aProperties.nDocId = j;
            aProperties.nRegionId = i;
            aProperties.aName = aName;
            aProperties.aPath = aURL;
            aProperties.aRegionName = aRegionName;
#if TemplateCache
            std::fill_n(cIndex, 256, 0);
            std::fill_n(cFile, 256, 0);
            std::fill_n(csFile, 256, 0);
            BitmapEx cImg;
            OUString imgurl/*,imgurl2*/;
            OUString ii = OStringToOUString(std::to_string(i).c_str(), RTL_TEXTENCODING_UTF8);
            OUString jj = OStringToOUString(std::to_string(j).c_str(), RTL_TEXTENCODING_UTF8);
            #if defined(_WIN32)
            if(mnThumbnailWidth < 180){
                imgurl += PicPath + "\\s\\" + ii + jj +".png";
                firstrun = 0;
            }else{
                imgurl += PicPath + "\\b\\" + ii + jj +".png";
            }
            #else
            if(mnThumbnailWidth < 180){
                imgurl += PicPath + "/s/" + ii + jj +".png";
                firstrun = 0;
            }else{
                imgurl += PicPath + "/b/" + ii + jj +".png";
            }
            #endif
            snprintf(cFile,PATH_SIZE,OUStringToOString( imgurl, RTL_TEXTENCODING_UTF8 ).getStr());

            // check png file exist
            OUString OUS_mapname;
            if(exist_listfile)
                OUS_mapname += OStringToOUString(string(cachedata[nAllCount2].filename).c_str(), RTL_TEXTENCODING_UTF8);
            if((aName.compareTo(OUS_mapname) != 0) || (stat(cFile, cFilebuf) != 0))
            {
                snprintf(csFile,PATH_SIZE,OUStringToOString( OUString(PicPath + "/s/" + ii + jj +".png"), RTL_TEXTENCODING_UTF8 ).getStr());
                remove(csFile);

                // wirte list.txt
                snprintf(cIndex,256,"%-4d %-20s  %d%d  %-s\n",nAllCount++,"Sun Oct 29 15:03:07 2015",i,j,OUStringToOString( aName, RTL_TEXTENCODING_UTF8 ).getStr());
                fwrite(cIndex,1,strlen(cIndex),fstream);

                cImg = TemplateAbstractView::fetchThumbnail(aURL, getThumbnailWidth(), getThumbnailHeight());
                // write cache png
                SvFileStream aNew(imgurl, StreamMode::WRITE|StreamMode::TRUNC);
                vcl::PNGWriter aPNGWriter(cImg);
                aPNGWriter.Write(aNew);
            }else{
                // wirte list.txt
                snprintf(cIndex,256,"%-4d %-20s  %d%d  %-s\n",nAllCount++,"Sun Oct 29 15:03:07 2017",i,j,cachedata[nAllCount2].filename);
                fwrite(cIndex,1,strlen(cIndex),fstream);

                OUString PngURL;
                #if defined(_WIN32)
                if(mnThumbnailHeight < 180)
                {
                    PngURL += PicPath + "\\s\\" + OStringToOUString(string(cachedata[nAllCount2].mapname).c_str(), RTL_TEXTENCODING_UTF8) +".png";
                }else{
                    PngURL += PicPath + "\\b\\" + OStringToOUString(string(cachedata[nAllCount2].mapname).c_str(), RTL_TEXTENCODING_UTF8) +".png";
                }
                #else
                if(mnThumbnailHeight < 180)
                {
                    PngURL += PicPath + "/s/" + OStringToOUString(string(cachedata[nAllCount2].mapname).c_str(), RTL_TEXTENCODING_UTF8) +".png";
                }else{
                    PngURL += PicPath + "/b/" + OStringToOUString(string(cachedata[nAllCount2].mapname).c_str(), RTL_TEXTENCODING_UTF8) +".png";
                }
                #endif
                SvFileStream aFileStream(PngURL, StreamMode::READ);
                vcl::PNGReader aPNGReader(aFileStream);
                cImg = aPNGReader.Read();
            }
            nAllCount2++;
            aProperties.aThumbnail = cImg;
#else
            aProperties.aThumbnail = TemplateAbstractView::fetchThumbnail(aURL,
                                                                          getThumbnailWidth(),
                                                                          getThumbnailHeight());
#endif

            pItem->maTemplates.push_back(aProperties);
            maAllTemplates.push_back(aProperties);
        }

        maRegions.push_back(pItem);
    }
#if TemplateCache
    pWin.disposeAndClear();
    free(cDirbuf);
    free(cFilebuf);
    fclose(fstream);
#endif
}

void TemplateLocalView::reload ()
{
    mpDocTemplates->Update();

    Populate();

    // Check if we are currently browsing a region or root folder
    if (mnCurRegionId)
    {
        sal_uInt16 nRegionId = mnCurRegionId - 1;   //Is offset by 1

        for (TemplateContainerItem* pRegion : maRegions)
        {
            if (pRegion->mnRegionId == nRegionId)
            {
                showRegion(pRegion);
                break;
            }
        }
    }
    else
        showAllTemplates();

    //No items should be selected by default
    deselectItems();
}

void TemplateLocalView::showAllTemplates()
{
    mnCurRegionId = 0;
    maCurRegionName.clear();

    insertItems(maAllTemplates, false, true);

    maOpenRegionHdl.Call(nullptr);
}

void TemplateLocalView::showRegion(TemplateContainerItem *pItem)
{
    mnCurRegionId = pItem->mnRegionId+1;
    maCurRegionName = pItem->maTitle;

    insertItems((pItem)->maTemplates);

    maOpenRegionHdl.Call(nullptr);
}

void TemplateLocalView::showRegion(const OUString &rName)
{
    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->maTitle == rName)
        {
            showRegion(pRegion);
            break;
        }
    }
}

TemplateContainerItem* TemplateLocalView::getRegion(OUString const & rName)
{
    for (TemplateContainerItem* pRegion : maRegions)
        if (pRegion->maTitle == rName)
            return pRegion;

    return nullptr;
}

void TemplateLocalView::createContextMenu(const bool bIsDefault)
{
    std::unique_ptr<PopupMenu> pItemMenu(new PopupMenu);
    pItemMenu->InsertItem(MNI_OPEN,SfxResId(STR_OPEN).toString());
    pItemMenu->InsertItem(MNI_EDIT,SfxResId(STR_EDIT_TEMPLATE).toString());

    if(!bIsDefault)
        pItemMenu->InsertItem(MNI_DEFAULT_TEMPLATE,SfxResId(STR_DEFAULT_TEMPLATE).toString());
    else
        pItemMenu->InsertItem(MNI_DEFAULT_TEMPLATE,SfxResId(STR_RESET_DEFAULT).toString());

    pItemMenu->InsertSeparator();
    pItemMenu->InsertItem(MNI_RENAME,SfxResId(STR_RENAME).toString());
    pItemMenu->InsertItem(MNI_DELETE,SfxResId(STR_DELETE).toString());
    pItemMenu->InsertSeparator();
    deselectItems();
    maSelectedItem->setSelection(true);
    maItemStateHdl.Call(maSelectedItem);
    pItemMenu->SetSelectHdl(LINK(this, TemplateLocalView, ContextMenuSelectHdl));
    pItemMenu->Execute(this, Rectangle(maPosition,Size(1,1)), PopupMenuFlags::ExecuteDown);
    Invalidate();
}

IMPL_LINK_TYPED(TemplateLocalView, ContextMenuSelectHdl, Menu*, pMenu, bool)
{
    sal_uInt16 nMenuId = pMenu->GetCurItemId();

    switch(nMenuId)
    {
    case MNI_OPEN:
        maOpenTemplateHdl.Call(maSelectedItem);
        break;
    case MNI_EDIT:
        maEditTemplateHdl.Call(maSelectedItem);
        break;
    case MNI_RENAME:
    {
        ScopedVclPtrInstance< InputDialog > m_pTitleEditDlg( SfxResId(STR_RENAME_TEMPLATE).toString(), this);
        OUString sOldTitle = maSelectedItem->getTitle();
        m_pTitleEditDlg->SetEntryText( sOldTitle );
        m_pTitleEditDlg->HideHelpBtn();

        if(!m_pTitleEditDlg->Execute())
            break;
        OUString sNewTitle = comphelper::string::strip( m_pTitleEditDlg->GetEntryText(), ' ');

        if ( !sNewTitle.isEmpty() && sNewTitle != sOldTitle )
        {
            maSelectedItem->setTitle(sNewTitle);
        }
    }
        break;
    case MNI_DELETE:
    {
        ScopedVclPtrInstance< MessageDialog > aQueryDlg(this, SfxResId(STR_QMSG_SEL_TEMPLATE_DELETE), VclMessageType::VCL_MESSAGE_QUESTION, VCL_BUTTONS_YES_NO);
        if ( aQueryDlg->Execute() != RET_YES )
            break;

        maDeleteTemplateHdl.Call(maSelectedItem);
        reload();
    }
        break;
    case MNI_DEFAULT_TEMPLATE:
        maDefaultTemplateHdl.Call(maSelectedItem);
        break;
    default:
        break;
    }

    return false;
}

sal_uInt16 TemplateLocalView::getCurRegionItemId() const
{
    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->mnRegionId == mnCurRegionId-1)
            return pRegion->mnId;
    }

    return 0;
}

sal_uInt16 TemplateLocalView::getRegionId(size_t pos) const
{
    assert(pos < maRegions.size());

    return maRegions[pos]->mnId;
}

sal_uInt16 TemplateLocalView::getRegionId(OUString const & sRegion) const
{
    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->maTitle == sRegion)
            return pRegion->mnId;
    }

    return 0;
}

OUString TemplateLocalView::getRegionName(const sal_uInt16 nRegionId) const
{
    return mpDocTemplates->GetRegionName(nRegionId);
}

OUString TemplateLocalView::getRegionItemName(const sal_uInt16 nItemId) const
{
    for (const TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->mnId == nItemId)
            return pRegion->maTitle;
    }

    return OUString();
}

std::vector<OUString> TemplateLocalView::getFolderNames()
{
    size_t n = maRegions.size();
    std::vector<OUString> ret(n);

    for (size_t i = 0; i < n; ++i)
        ret[i] = maRegions[i]->maTitle;

    return ret;
}

std::vector<TemplateItemProperties>
TemplateLocalView::getFilteredItems(const std::function<bool (const TemplateItemProperties&)> &rFunc) const
{
    std::vector<TemplateItemProperties> aItems;

    if (mnCurRegionId)
    {
        TemplateContainerItem *pFolderItem = maRegions[mnCurRegionId-1];

        for (TemplateItemProperties & rItemProps : pFolderItem->maTemplates)
        {
            if (rFunc(rItemProps))
                aItems.push_back(rItemProps);
        }
    }
    else
    {
        for (const TemplateContainerItem* pFolderItem : maRegions)
        {
            for (const TemplateItemProperties & rItemProps : pFolderItem->maTemplates)
            {
                if (rFunc(rItemProps))
                    aItems.push_back(rItemProps);
            }
        }
    }

    return aItems;
}

sal_uInt16 TemplateLocalView::createRegion(const OUString &rName)
{
    sal_uInt16 nRegionId = mpDocTemplates->GetRegionCount();    // Next regionId
    sal_uInt16 nItemId = getNextItemId();

    if (!mpDocTemplates->InsertDir(rName,nRegionId))
        return 0;

    // Insert to the region cache list and to the thumbnail item list
    TemplateContainerItem* pItem = new TemplateContainerItem( nItemId );
    pItem->mnRegionId = nRegionId;
    pItem->maTitle = rName;

    maRegions.push_back(pItem);

    return pItem->mnId;
}

bool TemplateLocalView::renameRegion(const OUString &rTitle, const OUString &rNewTitle)
{
    sal_uInt16 nDocId = USHRT_MAX;
    TemplateContainerItem *pRegion = getRegion(rTitle);

    if(pRegion)
    {
        sal_uInt16 nRegionId = pRegion->mnRegionId;
        return mpDocTemplates->SetName( rNewTitle, nRegionId, nDocId );
    }
    return false;
}

bool TemplateLocalView::removeRegion(const sal_uInt16 nItemId)
{
    sal_uInt16 nRegionId = USHRT_MAX;

    // Remove from the region cache list
    std::vector<TemplateContainerItem*>::iterator pRegionIt;
    for ( pRegionIt = maRegions.begin(); pRegionIt != maRegions.end();)
    {
        if ( (*pRegionIt)->mnId == nItemId )
        {
            if (!mpDocTemplates->Delete((*pRegionIt)->mnRegionId,USHRT_MAX))
                return false;

            nRegionId = (*pRegionIt)->mnRegionId;

            delete *pRegionIt;
            pRegionIt = maRegions.erase(pRegionIt);
        }
        else
        {
            // Synchronize regions cache ids with SfxDocumentTemplates
            if (nRegionId != USHRT_MAX && (*pRegionIt)->mnRegionId > nRegionId)
                --(*pRegionIt)->mnRegionId;

            ++pRegionIt;
        }
    }

    if (nRegionId == USHRT_MAX)
        return false;

    // Synchronize view regions ids with SfxDocumentTemplates
    std::vector<TemplateContainerItem*>::iterator pRegionIter = maRegions.begin();
    for ( ; pRegionIter != maRegions.end(); ++pRegionIter)
    {
        if ((*pRegionIter)->mnRegionId > nRegionId)
            --(*pRegionIter)->mnRegionId;
    }

    return true;
}

bool TemplateLocalView::removeTemplate (const sal_uInt16 nItemId, const sal_uInt16 nSrcItemId)
{
    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->mnId == nSrcItemId)
        {
            TemplateContainerItem *pItem = pRegion;
            std::vector<TemplateItemProperties>::iterator pIter;
            for (pIter = pItem->maTemplates.begin(); pIter != pItem->maTemplates.end(); ++pIter)
            {
                if (pIter->nId == nItemId)
                {
                    if (!mpDocTemplates->Delete(pItem->mnRegionId,pIter->nDocId))
                        return false;

                    pIter = pItem->maTemplates.erase(pIter);

                    if (pRegion->mnRegionId == mnCurRegionId-1)
                    {
                        RemoveItem(nItemId);
                        Invalidate();
                    }

                    // Update Doc Idx for all templates that follow
                    for (; pIter != pItem->maTemplates.end(); ++pIter)
                        pIter->nDocId = pIter->nDocId - 1;

                    break;
                }
            }

            CalculateItemPositions();
            break;
        }
    }

    return true;
}

bool TemplateLocalView::moveTemplate (const ThumbnailViewItem *pItem, const sal_uInt16 nSrcItem,
                                       const sal_uInt16 nTargetItem)
{
    TemplateContainerItem *pTarget = nullptr;
    TemplateContainerItem *pSrc = nullptr;

    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->mnId == nTargetItem)
            pTarget = pRegion;
        else if (pRegion->mnId == nSrcItem)
            pSrc = pRegion;
    }

    if (pTarget && pSrc)
    {
        sal_uInt16 nSrcRegionId = pSrc->mnRegionId;
        sal_uInt16 nTargetRegion = pTarget->mnRegionId;
        sal_uInt16 nTargetIdx = mpDocTemplates->GetCount(nTargetRegion);    // Next Idx

        const TemplateViewItem *pViewItem = static_cast<const TemplateViewItem*>(pItem);

        bool bCopy = !mpDocTemplates->Move(nTargetRegion,nTargetIdx,nSrcRegionId,pViewItem->mnDocId);

        if (bCopy)
        {
            OUString sQuery = (OUString(SfxResId(STR_MSG_QUERY_COPY).toString()).replaceFirst("$1", pViewItem->maTitle)).replaceFirst("$2",
                getRegionName(nTargetRegion));
            ScopedVclPtrInstance< MessageDialog > aQueryDlg(this, sQuery, VclMessageType::VCL_MESSAGE_QUESTION, VCL_BUTTONS_YES_NO);
            if ( aQueryDlg->Execute() != RET_YES )
                return false;

            if (!mpDocTemplates->Copy(nTargetRegion,nTargetIdx,nSrcRegionId,pViewItem->mnDocId))
                return false;
        }
        // move template to destination

        TemplateItemProperties aTemplateItem;
        aTemplateItem.nId = nTargetIdx + 1;
        aTemplateItem.nDocId = nTargetIdx;
        aTemplateItem.nRegionId = nTargetRegion;
        aTemplateItem.aName = pViewItem->maTitle;
        aTemplateItem.aPath = mpDocTemplates->GetPath(nTargetRegion,nTargetIdx);
        aTemplateItem.aRegionName = pViewItem->maHelpText;
        aTemplateItem.aThumbnail = pViewItem->maPreview1;

        pTarget->maTemplates.push_back(aTemplateItem);

        if (!bCopy)
        {
            // remove template from region cached data

            std::vector<TemplateItemProperties>::iterator aIter;
            for (aIter = pSrc->maTemplates.begin(); aIter != pSrc->maTemplates.end();)
            {
                if (aIter->nDocId == pViewItem->mnDocId)
                {
                    aIter = pSrc->maTemplates.erase(aIter);
                }
                else
                {
                    // Keep region document id synchronized with SfxDocumentTemplates
                    if (aIter->nDocId > pViewItem->mnDocId)
                        --aIter->nDocId;

                    ++aIter;
                }
            }

            // Keep view document id synchronized with SfxDocumentTemplates
            std::vector<ThumbnailViewItem*>::iterator pItemIter = mItemList.begin();
            for (; pItemIter != mItemList.end(); ++pItemIter)
            {
                if (static_cast<TemplateViewItem*>(*pItemIter)->mnDocId > pViewItem->mnDocId)
                    --static_cast<TemplateViewItem*>(*pItemIter)->mnDocId;
            }
        }

        CalculateItemPositions();
        Invalidate();

        return true;
    }

    return false;
}

bool TemplateLocalView::moveTemplates(const std::set<const ThumbnailViewItem*, selection_cmp_fn> &rItems,
                                      const sal_uInt16 nTargetItem)
{
    bool ret = true;

    TemplateContainerItem *pTarget = nullptr;
    TemplateContainerItem *pSrc = nullptr;

    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->mnId == nTargetItem)
            pTarget = pRegion;
    }

    if (pTarget)
    {
        bool refresh = false;

        sal_uInt16 nTargetRegion = pTarget->mnRegionId;
        sal_uInt16 nTargetIdx = mpDocTemplates->GetCount(nTargetRegion);    // Next Idx
        std::vector<sal_uInt16> aItemIds;    // List of moved items ids (also prevents the invalidation of rItems iterators when we remove them as we go)

        std::set<const ThumbnailViewItem*,selection_cmp_fn>::const_iterator aSelIter;
        for ( aSelIter = rItems.begin(); aSelIter != rItems.end(); ++aSelIter, ++nTargetIdx )
        {
            const TemplateViewItem *pViewItem = static_cast<const TemplateViewItem*>(*aSelIter);
            sal_uInt16 nSrcRegionId = pViewItem->mnRegionId;

            for (TemplateContainerItem* pRegion : maRegions)
            {
                if (pRegion->mnRegionId == nSrcRegionId)
                    pSrc = pRegion;
            }

            if(pSrc)
            {
                bool bCopy = !mpDocTemplates->Move(nTargetRegion,nTargetIdx,nSrcRegionId,pViewItem->mnDocId);

                if (bCopy)
                {
                    OUString sQuery = (OUString(SfxResId(STR_MSG_QUERY_COPY).toString()).replaceFirst("$1", pViewItem->maTitle)).replaceFirst("$2",
                        getRegionName(nTargetRegion));
                    ScopedVclPtrInstance< MessageDialog > aQueryDlg(this, sQuery, VclMessageType::VCL_MESSAGE_QUESTION, VCL_BUTTONS_YES_NO);

                    if ( aQueryDlg->Execute() != RET_YES )
                    {
                        OUString sMsg(SfxResId(STR_MSG_ERROR_LOCAL_MOVE).toString());
                        sMsg = sMsg.replaceFirst("$1",getRegionName(nTargetRegion));
                        ScopedVclPtrInstance<MessageDialog>(this, sMsg.replaceFirst( "$2",pViewItem->maTitle))->Execute();

                        return false; //return if any single move operation fails
                    }

                    if (!mpDocTemplates->Copy(nTargetRegion,nTargetIdx,nSrcRegionId,pViewItem->mnDocId))
                    {
                        ret = false;
                        continue;
                    }
                }

                // move template to destination

                TemplateItemProperties aTemplateItem;
                aTemplateItem.nId = nTargetIdx + 1;
                aTemplateItem.nDocId = nTargetIdx;
                aTemplateItem.nRegionId = nTargetRegion;
                aTemplateItem.aName = pViewItem->maTitle;
                aTemplateItem.aPath = mpDocTemplates->GetPath(nTargetRegion,nTargetIdx);
                aTemplateItem.aRegionName = pViewItem->maHelpText;
                aTemplateItem.aThumbnail = pViewItem->maPreview1;

                pTarget->maTemplates.push_back(aTemplateItem);

                if (!bCopy)
                {
                    // remove template from region cached data

                    std::vector<TemplateItemProperties>::iterator pPropIter;
                    for (pPropIter = pSrc->maTemplates.begin(); pPropIter != pSrc->maTemplates.end();)
                    {
                        if (pPropIter->nDocId == pViewItem->mnDocId)
                        {
                            pPropIter = pSrc->maTemplates.erase(pPropIter);
                            aItemIds.push_back(pViewItem->mnDocId + 1);//mnid
                        }
                        else
                        {
                            // Keep region document id synchronized with SfxDocumentTemplates
                            if (pPropIter->nDocId > pViewItem->mnDocId)
                                --pPropIter->nDocId;

                            ++pPropIter;
                        }
                    }

                    // Keep view document id synchronized with SfxDocumentTemplates
                    std::vector<ThumbnailViewItem*>::iterator pItemIter = mItemList.begin();
                    for (; pItemIter != mItemList.end(); ++pItemIter)
                    {
                        if (static_cast<TemplateViewItem*>(*pItemIter)->mnDocId > pViewItem->mnDocId)
                            --static_cast<TemplateViewItem*>(*pItemIter)->mnDocId;
                    }
                }
            }

            refresh = true;
        }

        // Remove items from the current view
        for (std::vector<sal_uInt16>::iterator it = aItemIds.begin(); it != aItemIds.end(); ++it)
            RemoveItem(*it);

        if (refresh)
        {
            CalculateItemPositions();
            Invalidate();
        }
    }
    else
        ret = false;

    return ret;
}

bool TemplateLocalView::copyFrom(const sal_uInt16 nRegionItemId, const BitmapEx &rThumbnail,
                                  const OUString &rPath)
{
    for (TemplateContainerItem* pRegion : maRegions)
    {
        if (pRegion->mnId == nRegionItemId)
        {
            sal_uInt16 nId = 0;
            sal_uInt16 nDocId = 0;

            TemplateContainerItem *pRegionItem =
                    pRegion;

            if (!pRegionItem->maTemplates.empty())
            {
                nId = (pRegionItem->maTemplates.back()).nId+1;
                nDocId = (pRegionItem->maTemplates.back()).nDocId+1;
            }

            OUString aPath(rPath);
            sal_uInt16 nRegionId = pRegion->mnRegionId;

            if (mpDocTemplates->CopyFrom(nRegionId,nDocId,aPath))
            {
                TemplateItemProperties aTemplate;
                aTemplate.nId = nId;
                aTemplate.nDocId = nDocId;
                aTemplate.nRegionId = nRegionId;
                aTemplate.aName = aPath;
                aTemplate.aRegionName = getRegionName(nRegionId);
                aTemplate.aThumbnail = rThumbnail;
                aTemplate.aPath = mpDocTemplates->GetPath(nRegionId,nDocId);

                TemplateContainerItem *pItem = pRegion;

                pItem->maTemplates.push_back(aTemplate);

                return true;
            }

            break;
        }
    }

    return false;
}

bool TemplateLocalView::copyFrom(const OUString &rPath)
{
    assert(mnCurRegionId);

    TemplateContainerItem *pRegItem = maRegions[mnCurRegionId-1];

    sal_uInt16 nId = getNextItemId();
    sal_uInt16 nDocId = 0;
    sal_uInt16 nRegionId = pRegItem->mnRegionId;

    OUString aPath(rPath);

    if (!pRegItem->maTemplates.empty())
        nDocId = (pRegItem->maTemplates.back()).nDocId+1;

    if (!mpDocTemplates->CopyFrom(nRegionId,nDocId,aPath))
        return false;

    TemplateItemProperties aTemplate;
    aTemplate.nId = nId;
    aTemplate.nDocId = nDocId;
    aTemplate.nRegionId = nRegionId;
    aTemplate.aName = aPath;
    aTemplate.aThumbnail = TemplateAbstractView::fetchThumbnail(rPath,
                                                                TEMPLATE_THUMBNAIL_MAX_WIDTH,
                                                                TEMPLATE_THUMBNAIL_MAX_HEIGHT);
    aTemplate.aPath = rPath;
    aTemplate.aRegionName = getRegionName(nRegionId);

    pRegItem->maTemplates.push_back(aTemplate);

    insertItem(aTemplate);

    return true;
}

bool TemplateLocalView::copyFrom (TemplateContainerItem *pItem, const OUString &rPath)
{
    sal_uInt16 nId = 1;
    sal_uInt16 nDocId = 0;
    sal_uInt16 nRegionId = pItem->mnRegionId;
    OUString aPath(rPath);

    if (!pItem->maTemplates.empty())
    {
        nId = (pItem->maTemplates.back()).nId+1;
        nDocId = (pItem->maTemplates.back()).nDocId+1;
    }

    if (mpDocTemplates->CopyFrom(nRegionId,nDocId,aPath))
    {
        TemplateItemProperties aTemplate;
        aTemplate.nId = nId;
        aTemplate.nDocId = nDocId;
        aTemplate.nRegionId = nRegionId;
        aTemplate.aName = aPath;
        aTemplate.aThumbnail = TemplateAbstractView::fetchThumbnail(rPath,
                                                                    TEMPLATE_THUMBNAIL_MAX_WIDTH,
                                                                    TEMPLATE_THUMBNAIL_MAX_HEIGHT);
        aTemplate.aPath = rPath;
        aTemplate.aRegionName = getRegionName(nRegionId);

        pItem->maTemplates.push_back(aTemplate);

        CalculateItemPositions();

        return true;
    }

    return false;
}

bool TemplateLocalView::exportTo(const sal_uInt16 nItemId, const sal_uInt16 nRegionItemId, const OUString &rName)
{
    for (TemplateContainerItem* pRegItem : maRegions)
    {
        if (pRegItem->mnId == nRegionItemId)
        {
            std::vector<TemplateItemProperties>::iterator aIter;
            for (aIter = pRegItem->maTemplates.begin(); aIter != pRegItem->maTemplates.end(); ++aIter)
            {
                if (aIter->nId == nItemId)
                {
                    if (!mpDocTemplates->CopyTo(pRegItem->mnRegionId,aIter->nDocId,rName))
                        return false;

                    return true;
                }
            }

            break;
        }
    }

    return false;
}

bool TemplateLocalView::renameItem(ThumbnailViewItem* pItem, const OUString& sNewTitle)
{
    sal_uInt16 nRegionId = 0;
    sal_uInt16 nDocId = USHRT_MAX;
    TemplateViewItem* pDocItem = dynamic_cast<TemplateViewItem*>( pItem );

    if ( pDocItem )
    {
        nRegionId = pDocItem->mnRegionId;
        nDocId = pDocItem->mnDocId;
    }

    return mpDocTemplates->SetName( sNewTitle, nRegionId, nDocId );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
