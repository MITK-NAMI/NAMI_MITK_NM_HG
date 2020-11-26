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

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "MyPlugin.h"
#include <mitkPointSet.h>
#include <mitkRenderWindow.h>
#include <mitkStandaloneDataStorage.h>

// Qt
#include "QmitkRenderWindow.h"
#include <QMessageBox>
#include <QmitkNavigationToolStorageSelectionWidget.h>

// mitk image
#include <mitkImage.h>

const std::string MyPlugin::VIEW_ID = "org.mitk.views.myplugin";

void MyPlugin::SetFocus()
{
  m_Controls.buttonPerformImageProcessing->setFocus();
}

void MyPlugin::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &MyPlugin::DoImageProcessing);
}

void MyPlugin::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                  const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
  foreach (mitk::DataNode::Pointer node, nodes)
  {
    if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
    {
      m_Controls.labelWarning->setVisible(false);
      m_Controls.buttonPerformImageProcessing->setEnabled(true);
      return;
    }
  }

  m_Controls.labelWarning->setVisible(true);
  m_Controls.buttonPerformImageProcessing->setEnabled(false);
}

void MyPlugin::DoImageProcessing()
{
  QList<mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
  if (nodes.empty())
    return;

  mitk::DataNode *node = nodes.front();

  if (!node)
  {
    // Nothing selected. Inform the user and return
    QMessageBox::information(nullptr, "Template", "Please load and select an image before starting image processing.");
    return;
  }

  // here we have a valid mitk::DataNode

  // a node itself is not very useful, we need its data item (the image)
  mitk::BaseData *data = node->GetData();
  if (data)
  {
    // test if this data item is an image or not (could also be a surface or something totally different)
    mitk::Image *image = dynamic_cast<mitk::Image *>(data);
    if (image)
    {
      std::stringstream message;
      std::string name;
      message << "Performing image processing for image ";
      if (node->GetName(name))
      {
        // a property called "name" was found for this DataNode
        message << "'" << name << "'";
      }
      message << ".";
      MITK_INFO << message.str();

      // actually do something here...

	  //Initialisierung
      mitk::StandaloneDataStorage::Pointer dataStorage = mitk::StandaloneDataStorage::New();
      mitk::RenderWindow::Pointer renderWindow = mitk::RenderWindow::New();
      renderWindow->GetRenderer()->SetMapperID(mitk::BaseRenderer::Standard3D);
      renderWindow->GetRenderer()->SetDataStorage(dataStorage);

      // 3D Punkt erstellen
      double coordinates[] = {10.2, 5.5, 2.1};
      double coordinatesTwo[] = {15, 1, 6.5};
      mitk::Point3D myPoint(coordinates);
      mitk::Point3D my2point(coordinatesTwo);

      // PointSet erstellen und 3D Punkt hinzufügen
      mitk::PointSet::Pointer myPointSet = mitk::PointSet::New();
      myPointSet->InsertPoint(myPoint);
      myPointSet->InsertPoint(my2point);

      // DataNode erstellen und PointSet als Daten setzen
      mitk::DataNode::Pointer myDataNode = mitk::DataNode::New();
      myDataNode->SetName("MyNodeTest");
      myDataNode->SetData(myPointSet);

      // DataNode der DataStorage hinzufügen
      this->GetDataStorage()->Add(myDataNode);
      dataStorage->Add(myDataNode);

     // PointSet visualisieren
      auto geo = dataStorage->ComputeBoundingGeometry3D(dataStorage->GetAll());
      mitk::RenderingManager::GetInstance()->InitializeViews(geo);
	  
    }
  }
}
