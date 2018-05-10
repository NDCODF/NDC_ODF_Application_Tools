#!/bin/bash
# postepm for ubuntu
# used for insert 'Section:' to every .deb file
# run at top of source directory
MACHINE=`uname -m`
ARCH="_x86"
test $MACHINE = "x86_64" && ARCH="_x86-64"
test $MACHINE = "armv7l" && ARCH=""

DPKG=`which dpkg`
PKG=(NDCODFApplicationTools_Linux${ARCH}_deb NDCODFApplicationTools_Linux${ARCH}_deb_langpack_zh-TW)
PRJ=(ndcodfapplicationtools ndcodfapplicationtools_languagepack)

for idx in "${!PRJ[@]}" ; do
    prj="${PRJ[$idx]}" ;
    pkg="${PKG[$idx]}" ;
    pkgdir="workdir/installation/$prj/deb/install/$pkg/DEBS" ;
    for debfile in $pkgdir/*.deb ; do
        debdir="${debfile}_dir" ;
        debinfodir="${debdir}/DEBIAN" ;
        mkdir -p $debinfodir ;
        basename $debfile ;
        # 1) unpack *.deb and it's info dir: /DEBIAN
        $DPKG -x $debfile $debdir ;
        $DPKG -e $debfile $debinfodir ;
        echo "Section: OxOffice" >> $debinfodir/control ;
        # 2) re-pack to origin *.deb
        $DPKG -b $debdir $debfile ;
        rm $debdir -rf ;
    done ;
    # 3) re-tar to origin.
    pushd "workdir/installation/$prj/deb/install" ;
    pkgdowndir="${pkg}_download" ;
    tar zcvf "${pkgdowndir}/${pkg}.tar.gz" $pkg
    popd
done
