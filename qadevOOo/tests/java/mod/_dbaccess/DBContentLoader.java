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

package mod._dbaccess;

import java.io.PrintWriter;

import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.uno.XInterface;

/**
* Test for object which is represented by service
* <code>com.sun.star.sdb.ContentLoader</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
*  <li> <code>com::sun::star::frame::FrameLoader</code></li>
*  <li> <code>com::sun::star::frame::XFrameLoader</code></li>
*  <li> <code>com::sun::star::lang::XInitialization</code></li>
* </ul>
* This object test <b> is NOT </b> designed to be run in several
* threads concurrently.
* @see com.sun.star.beans.XPropertySet
* @see com.sun.star.frame.FrameLoader
* @see com.sun.star.frame.XSynchronousFrameLoader
* @see com.sun.star.lang.XInitialization
* @see ifc.beans._XPropertySet
* @see ifc.frame._FrameLoader
* @see ifc.frame._XSynchronousFrameLoader
* @see ifc.lang._XInitialization
*/
public class DBContentLoader extends TestCase {

    /**
    * Creating a TestEnvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.sdb.ContentLoader</code>. <p>
    *     Object relations created :
    * <ul>
    *  <li> <code>'FrameLoader.URL"'</code> for
    *      {@link ifc.frame._XFrameLoader} </li>
    * </ul>
    */
    @Override
    public TestEnvironment createTestEnvironment( TestParameters Param,
                                                  PrintWriter log )
                                                    throws Exception {

        Object oInterface = Param.getMSF().createInstance
            ("com.sun.star.sdb.ContentLoader");
        XInterface oObj = (XInterface) oInterface ;

        log.println("ImplementationName " + util.utils.getImplName(oObj));
        log.println( "creating a new environment for object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        // adding relation for XFrameLoader
        tEnv.addObjRelation("FrameLoader.URL",  ".component:DB/DataSourceBrowser") ;

        return tEnv;
    } // finish method getTestEnvironment

}

