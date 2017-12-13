# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalPackage_ExternalPackage,ossiimenus,OSSIIMenus))

$(eval $(call gb_ExternalPackage_add_unpacked_files_with_dir,ossiimenus,share/extensions/OSSIIMenus,\
    Addons.xcu \
	icons/image1_16h.bmp \
	META-INF/manifest.xml \
	Office/UI/BaseWindowState.xcu \
	Office/UI/BasicIDEWindowState.xcu \
	Office/UI/CalcWindowState.xcu \
	Office/UI/DrawWindowState.xcu \
	Office/UI/ImpressWindowState.xcu \
	Office/UI/MathWindowState.xcu \
	Office/UI/StartModuleWindowState.xcu \
	Office/UI/WriterWindowState.xcu \
	OSSII_Symbol/dialog.xlb \
	OSSII_Symbol/InsertSymbol.pba \
	OSSII_Symbol/Module1.pba \
	OSSII_Symbol/script.xlb \
	pkg-desc/pkg-description.txt \
))

# vim: set noet sw=4 ts=4:
