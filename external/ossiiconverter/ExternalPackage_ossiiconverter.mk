# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalPackage_ExternalPackage,ossiiconverter,OSSIIConverter))

$(eval $(call gb_ExternalPackage_add_unpacked_files_with_dir,ossiiconverter,share/extensions/OSSIIConverter,\
    Addons.xcu \
	description.xml \
	META-INF/manifest.xml \
	Office/UI/BaseWindowState.xcu \
	Office/UI/BasicIDEWindowState.xcu \
	Office/UI/CalcWindowState.xcu \
	Office/UI/DrawWindowState.xcu \
	Office/UI/ImpressWindowState.xcu \
	Office/UI/MathWindowState.xcu \
	Office/UI/StartModuleWindowState.xcu \
	Office/UI/WriterWindowState.xcu \
	OSSIIConverter/ConvertDialog.xdl \
	OSSIIConverter/ConvertMain.pba \
	OSSIIConverter/dialog.xlb \
	OSSIIConverter/ProgressDialog.xdl \
	OSSIIConverter/script.xlb \
	OSSIIConverter/StartConvert.pba \
	pkg-desc/pkg-description.txt  \
))

# vim: set noet sw=4 ts=4:
