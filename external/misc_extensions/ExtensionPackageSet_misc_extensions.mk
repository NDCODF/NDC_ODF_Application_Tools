# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExtensionPackageSet_ExtensionPackageSet,misc_extensions))

ifeq ($(NDC_WITH_EXTENSION_NDCHELP),yes)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,NDCHelp,NDCHelp.oxt))
endif

ifneq ($(BARCODE_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Barcode,$(BARCODE_EXTENSION_PACK)))
endif

ifneq ($(DIAGRAM_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,SmART,$(DIAGRAM_EXTENSION_PACK)))
endif

ifneq ($(GOOGLE_DOCS_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,gdocs,$(GOOGLE_DOCS_EXTENSION_PACK)))
endif

ifneq ($(HUNART_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,hunart,$(HUNART_EXTENSION_PACK)))
endif

ifneq ($(NUMBERTEXT_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,numbertext,$(NUMBERTEXT_EXTENSION_PACK)))
endif

ifneq ($(SUNTEMPLATES_DE_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Sun_ODF_Template_Pack_de,$(SUNTEMPLATES_DE_PACK)))
endif

ifneq ($(SUNTEMPLATES_EN_US_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Sun_ODF_Template_Pack_en-US,$(SUNTEMPLATES_EN_US_PACK)))
endif

ifneq ($(SUNTEMPLATES_ES_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Sun_ODF_Template_Pack_es,$(SUNTEMPLATES_ES_PACK)))
endif

ifneq ($(SUNTEMPLATES_FR_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Sun_ODF_Template_Pack_fr,$(SUNTEMPLATES_FR_PACK)))
endif

ifneq ($(SUNTEMPLATES_HU_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Sun_ODF_Template_Pack_hu,$(SUNTEMPLATES_HU_PACK)))
endif

ifneq ($(SUNTEMPLATES_IT_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Sun_ODF_Template_Pack_it,$(SUNTEMPLATES_IT_PACK)))
endif

ifneq ($(TYPO_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,typo,$(TYPO_EXTENSION_PACK)))
endif

ifneq ($(VALIDATOR_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,Validator,$(VALIDATOR_EXTENSION_PACK)))
endif

ifneq ($(WATCH_WINDOW_EXTENSION_PACK),)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,WatchWindow,$(WATCH_WINDOW_EXTENSION_PACK)))
endif

ifeq ($(NDC_WITH_EXTENSION_CPMLIBRE),yes)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,cpmlibre,cpmlibre.oxt))
endif

ifeq ($(NDC_WITH_EXTENSION_HYPERLINK),yes)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,hyperlink,hyperlink.oxt))
endif

ifeq ($(NDC_WITH_EXTENSION_ONEKEY2ODF),yes)
$(eval $(call gb_ExtensionPackageSet_add_extension,misc_extensions,onekey2odf,onekey2odf.oxt))
endif

# vim: set noet sw=4 ts=4:
