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

#ifndef INCLUDED_CONNECTIVITY_SOURCE_INC_FLAT_EDATABASEMETADATA_HXX
#define INCLUDED_CONNECTIVITY_SOURCE_INC_FLAT_EDATABASEMETADATA_HXX

#include "file/FDatabaseMetaData.hxx"

namespace connectivity
{
    namespace flat
    {

        //************ Class: java.sql.DatabaseMetaDataDate


        class OFlatDatabaseMetaData :   public file::ODatabaseMetaData
        {
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > impl_getTypeInfo_throw() override;
        protected:
            virtual ~OFlatDatabaseMetaData();
        public:
            OFlatDatabaseMetaData(file::OConnection* _pCon);

            virtual OUString SAL_CALL getURL(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) override;
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getColumns( const ::com::sun::star::uno::Any& catalog, const OUString& schemaPattern, const OUString& tableNamePattern, const OUString& columnNamePattern ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException, std::exception) override;
        };
    }
}
#endif // _CONNECTIVITY_FLAT_ODATABASEMETADATA_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
