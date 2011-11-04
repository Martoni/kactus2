//-----------------------------------------------------------------------------
// File: SoftwareMappingsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 31.10.2011
//
// Description:
// Editor for editing software mappings for a specific HW component.
//-----------------------------------------------------------------------------

#include "SoftwareMappingsEditor.h"

#include <models/design.h>

#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: SoftwareMappingsEditor()
//-----------------------------------------------------------------------------
SoftwareMappingsEditor::SoftwareMappingsEditor(LibraryInterface* libHandler,
                                               QSharedPointer<Component> component,
                                               void* dataPointer, QWidget *parent,
                                               QWidget* parentWnd)
    : ItemEditor(component, parent), libInterface_(libHandler), component_(component),
      platformMapping_(VLNV::COMPONENT, libHandler, parentWnd, this),
      endpointGroup_(tr("Mapped Endpoints"), this), endpointList_(&endpointGroup_),
      addButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
      removeButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
      endpointType_(VLNV::COMPONENT, libHandler, parentWnd, &endpointGroup_),
      linkedAppType_(VLNV::COMPONENT, libHandler, parentWnd, &endpointGroup_)
{

    endpointList_.setFixedWidth(200);

    platformMapping_.setTitle(tr("Mapped Platform"));
    endpointType_.setTitle(tr("Endpoints"));
    linkedAppType_.setTitle(tr("Fixed Application (optional)"));

    QVBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(&addButton_);
    buttonLayout->addWidget(&removeButton_);
    buttonLayout->addStretch(1);

    QGridLayout* endpointLayout = new QGridLayout(&endpointGroup_);
    endpointLayout->addWidget(&endpointList_, 0, 0, 2, 1);
    endpointLayout->addLayout(buttonLayout, 0, 1, 2, 1);
    endpointLayout->addWidget(&endpointType_, 0, 2, 1, 1);
    endpointLayout->addWidget(&linkedAppType_, 1, 2, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&platformMapping_);
    layout->addWidget(&endpointGroup_);
    layout->addStretch(1);

    // Set endpoint and application VLNV editor disabled by default.
    endpointType_.setEnabled(false);
    linkedAppType_.setEnabled(false);

    // Connect signals.
    connect(&platformMapping_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    connect(&addButton_, SIGNAL(clicked()), this, SLOT(addEndpointsMapping()));
    connect(&removeButton_, SIGNAL(clicked()), this, SLOT(removeEndpointsMapping()));
    connect(&endpointType_, SIGNAL(vlnvEdited()), this, SLOT(updateListItem()));
    connect(&endpointList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onListItemChanged(QListWidgetItem*, QListWidgetItem*)));

    connect(&endpointType_, SIGNAL(vlnvEdited()), this, SLOT(updateData()));
    connect(&linkedAppType_, SIGNAL(vlnvEdited()), this, SLOT(updateData()));

    // Read the existing SW design if found.
    VLNV designVLNV = *component_->getVlnv();
    designVLNV.setType(VLNV::DESIGN);
    designVLNV.setName(designVLNV.getName().remove(".comp") + "_swmap.design");

    if (libInterface_->contains(designVLNV))
    {
        QSharedPointer<LibraryComponent> libComp = libInterface_->getModel(designVLNV);
        QSharedPointer<Design> design = libComp.staticCast<Design>();
        QMap<QString, EndpointsMappingData*> endpointMap;
        unsigned int connIndex = 0;

        foreach (Design::ComponentInstance const& instance, design->getComponentInstances())
        {
            // Retrieve the component model.
            QSharedPointer<LibraryComponent> libComp = libInterface_->getModel(instance.componentRef);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            if (comp == 0)
            {
//                 emit errorMessage(tr("The component %1 was not found in the library").arg(
//                                   instance.componentRef.name()).arg(design->getVlnv()->name()));
                continue;
            }

            // Create the correct graphics item based on the SW type.
            switch (comp->getComponentSWType())
            {
            case KactusAttribute::KTS_SW_ENDPOINTS:
                {
                    EndpointsMappingData data;
                    data.endpointsVLNV = instance.componentRef;
                    endpointsMappingDatas_.push_back(data);

                    QListWidgetItem* item = new QListWidgetItem(QIcon(":/icons/graphics/endpoints.png"),
                                                                data.endpointsVLNV.getVendor() + ":" +
                                                                data.endpointsVLNV.getLibrary() + "\n" +
                                                                data.endpointsVLNV.getName() + ":" +
                                                                data.endpointsVLNV.getVersion());
                    item->setSizeHint(QSize(item->sizeHint().width(), 35));
                    endpointList_.addItem(item);
                    endpointMap.insert(instance.instanceName, &endpointsMappingDatas_.back());
                    break;
                }

            case KactusAttribute::KTS_SW_APPLICATION:
                {
                    // Find the correct program entity item based on the interconnection.
                    Design::Interconnection const& conn = design->getInterconnections().at(connIndex);
                    Q_ASSERT(conn.interface2.componentRef == instance.instanceName);
                    endpointMap.value(conn.interface1.componentRef)->linkedAppVLNV = instance.componentRef;
                    ++connIndex;
                    break;
                }

            case KactusAttribute::KTS_SW_PLATFORM:
                {
                    platformMapping_.setVLNV(instance.componentRef);
                    break;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SoftwareMappingsEditor()
//-----------------------------------------------------------------------------
SoftwareMappingsEditor::~SoftwareMappingsEditor()
{
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool SoftwareMappingsEditor::isValid() const
{
    // The mapped platform must be either totally empty or valid.
    if (!platformMapping_.isEmpty() && !platformMapping_.isValid())
    {
        return false;
    }

    // Check all mapped endpoints that their endpoints VLNV is valid and
    // linked application is either empty or valid.
    for (int i = 0; i < endpointsMappingDatas_.size(); ++i)
    {
        EndpointsMappingData const& data = endpointsMappingDatas_[i];

        if (!data.endpointsVLNV.isValid())
        {
            return false;
        }

        if (!data.linkedAppVLNV.isEmpty() && !data.linkedAppVLNV.isValid())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: makeChanges()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::makeChanges()
{
    VLNV designVLNV = *component_->getVlnv();
    designVLNV.setType(VLNV::DESIGN);
    designVLNV.setName(designVLNV.getName().remove(".comp") + "_swmap.design");

    // Create the SW design if there is something to save.
    if (platformMapping_.isValid() || !endpointsMappingDatas_.empty())
    {
        // Create the view for the component if it does not exist.
        if (component_->findView("kts_sw_ref") == 0)
        {
            View* view = new View("kts_sw_ref");
            view->addEnvIdentifier("");
            view->setHierarchyRef(designVLNV);
            component_->addView(view);
        }

        QSharedPointer<Design> design(new Design(designVLNV));

        QList<Design::ComponentInstance> instances;
        QList<Design::Interconnection> interconnections;

        // Add the platform component item to the instance list if set.
        if (platformMapping_.isValid())
        {
            Design::ComponentInstance instance(platformMapping_.getVLNV().getName().remove(".comp"),
                                               QString(), QString(), platformMapping_.getVLNV(), QPointF());
            instances.append(instance);
        }

        for (int i = 0; i < endpointsMappingDatas_.size(); ++i)
        {
            EndpointsMappingData const& data = endpointsMappingDatas_[i];

            Design::ComponentInstance instance(data.endpointsVLNV.getName().remove(".comp"),
                                               QString(), QString(), data.endpointsVLNV, QPointF());
            instances.append(instance);

            // Save the linked application if found.
            if (data.linkedAppVLNV.isValid())
            {
                Design::ComponentInstance appInstance(data.linkedAppVLNV.getName().remove(".comp"),
                                                      QString(), QString(), data.linkedAppVLNV, QPointF());
                instances.append(appInstance);

                // Add the applink interconnection.
                Design::Interface iface1(instance.instanceName, "app_link");
                Design::Interface iface2(appInstance.instanceName, "app_link");
                interconnections.append(Design::Interconnection(instance.instanceName + "_link_" + appInstance.instanceName,
                                                                iface1, iface2, QList<QPointF>()));
            }
        }

        design->setComponentInstances(instances);
        design->setInterconnections(interconnections);

        if (libInterface_->contains(designVLNV))
        {
            libInterface_->writeModelToFile(design);
        }
        else
        {
            QFileInfo fileInfo(libInterface_->getPath(*component_->getVlnv()));
            libInterface_->writeModelToFile(fileInfo.path(), design);
        }
    } else if (libInterface_->contains(designVLNV))
    {
        libInterface_->removeObject(designVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: addEndpointsMapping()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::addEndpointsMapping()
{
    // Create a new data entry.
    endpointsMappingDatas_.push_back(EndpointsMappingData());

    // Create a list widget item and select it.
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/icons/graphics/endpoints.png"),
                                                tr("<Vendor>:<Library>\n<Name:>:<Version>"));
    item->setSizeHint(QSize(item->sizeHint().width(), 35));
    item->setTextColor(Qt::red);
    endpointList_.addItem(item);
    endpointList_.setCurrentItem(item);

    // Enable all related widgets.
    removeButton_.setEnabled(true);
    endpointType_.setEnabled(true);
    linkedAppType_.setEnabled(true);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: removeEndpointsMapping()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::removeEndpointsMapping()
{
    if (endpointList_.currentItem() != 0)
    {
        // Remove the data entry and the item.
        int index = endpointList_.currentRow();
        endpointList_.takeItem(endpointList_.currentRow());
        endpointsMappingDatas_.remove(index);

        // Disable the remove button and VLNV editors if there are no more list items.
        if (endpointList_.count() == 0)
        {
            removeButton_.setEnabled(false);
            endpointType_.setEnabled(false);
            linkedAppType_.setEnabled(false);
        }

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: updateListItem()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::updateListItem()
{
    if (endpointList_.currentItem() != 0)
    {
        QListWidgetItem* item = endpointList_.currentItem();

        // Update the item's text.
        QString vendor = "<Vendor>";
        QString library = "<Library>";
        QString name = "<Name>";
        QString version = "<Version>";

        VLNV vlnv = endpointType_.getVLNV();

        if (!vlnv.getVendor().isEmpty())
        {
            vendor = vlnv.getVendor();
        }

        if (!vlnv.getLibrary().isEmpty())
        {
            library = vlnv.getLibrary();
        }

        if (!vlnv.getName().isEmpty())
        {
            name = vlnv.getName();
        }

        if (!vlnv.getVersion().isEmpty())
        {
            version = vlnv.getVersion();
        }

        item->setText(vendor + ":" + library + "\n" + name + ":" + version);

        // Update the item's color based on the validity of the VLNV.
        if (vlnv.isValid())
        {
            item->setTextColor(Qt::black);
        }
        else
        {
            item->setTextColor(Qt::red);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: updateData()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::updateData()
{
    QListWidgetItem* item = endpointList_.currentItem();

    if (item != 0)
    {
        endpointsMappingDatas_[endpointList_.row(item)].endpointsVLNV = endpointType_.getVLNV();
        endpointsMappingDatas_[endpointList_.row(item)].linkedAppVLNV = linkedAppType_.getVLNV();
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: onListItemChanged()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::onListItemChanged(QListWidgetItem* cur, QListWidgetItem*)
{
    // If there is no new selection, empty and disable the VLNV editors.
    if (cur == 0)
    {
        endpointType_.setVLNV(VLNV());
        endpointType_.setEnabled(false);
        
        linkedAppType_.setEnabled(false);
        linkedAppType_.setVLNV(VLNV());
    }
    else
    {
        // If the selection was valid, update the editors with the matching VLNVs.
        int row = endpointList_.row(cur);

        endpointType_.setVLNV(endpointsMappingDatas_[row].endpointsVLNV);
        endpointType_.setEnabled(true);

        linkedAppType_.setVLNV(endpointsMappingDatas_[row].linkedAppVLNV);
        linkedAppType_.setEnabled(true);
    }
}
