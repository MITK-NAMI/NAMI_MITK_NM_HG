/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "QmitkRegisterClasses.h"
#include "QmitkRenderWindow.h"

#include <mitkStandaloneDataStorage.h>
#include <mitkIOUtil.h>

#include <itksys/SystemTools.hxx>
#include <QApplication>

//##Documentation
//## @brief Load image (nrrd format) and display it in a 2D view
int main(int argc, char* argv[])
{
  QApplication qtapplication( argc, argv );

  if (argc < 2)
  {
    fprintf( stderr, "Usage:   %s [filename] \n\n", itksys::SystemTools::GetFilenameName(argv[0]).c_str() );
    return 1;
  }

  // Register Qmitk-dependent global instances
  QmitkRegisterClasses();

  //*************************************************************************
  // Part I: Basic initialization
  //*************************************************************************

  // Create a DataStorage
  // The DataStorage manages all data objects. It is used by the
  // rendering mechanism to render all data objects
  // We use the standard implementation mitk::StandaloneDataStorage.
  mitk::StandaloneDataStorage::Pointer ds = mitk::StandaloneDataStorage::New();


  //*************************************************************************
  // Part II: Create some data by reading a file
  //*************************************************************************

  // Read supported data formats by using the IOUtil helper class.
  const char * filename = argv[1];
  try
  {
    std::vector<mitk::BaseData::Pointer> baseData = mitk::IOUtil::Load(filename);

    //*********************************************************************
    // Part III: Put the data into the datastorage
    //*********************************************************************

    // First, put the data into a data node
    mitk::DataNode::Pointer node = mitk::DataNode::New();
    node->SetData(baseData.at(0));

    // Then, add the newly created data node into the datastorage
    ds->Add(node);
  }
  catch(...)
  {
    fprintf( stderr, "Could not open file %s \n\n", filename );
    exit(2);
  }

  //*************************************************************************
  // Part IV: Create window and pass the datastorage to it
  //*************************************************************************

  // Create a RenderWindow
  QmitkRenderWindow renderWindow;

  // Tell the RenderWindow which (part of) the datastorage to render
  renderWindow.GetRenderer()->SetDataStorage(ds);

  // Initialize the RenderWindow
  mitk::TimeGeometry::Pointer geo = ds->ComputeBoundingGeometry3D(ds->GetAll());
  mitk::RenderingManager::GetInstance()->InitializeViews( geo );
  //mitk::RenderingManager::GetInstance()->InitializeViews();

  // Select a slice
  mitk::SliceNavigationController::Pointer sliceNaviController = renderWindow.GetSliceNavigationController();
  if (sliceNaviController)
    sliceNaviController->GetSlice()->SetPos( 0 );

  //*************************************************************************
  // Part V: Qt-specific initialization
  //*************************************************************************
  renderWindow.show();
  renderWindow.resize( 256, 256 );

  // for testing
  #include "QtTesting.h"
  if (strcmp(argv[argc-1], "-testing") != 0)
    return qtapplication.exec();
  else
    return QtTesting();

  // cleanup: Remove References to DataStorage. This will delete the object
  ds = NULL;
}
/**
\example Step1.cpp
*/
