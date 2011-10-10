//-----------------------------------------------------------------------------
// File: LibraryUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 22.6.2011
//
// Description:
// Library utility functions.
//-----------------------------------------------------------------------------

#include "LibraryUtils.h"

#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/component.h>
#include <models/model.h>

#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: parseProgrammableElements()
//-----------------------------------------------------------------------------
void parseProgrammableElements(LibraryInterface* lh, Component& component,
                               QList<Design::ComponentInstance>& elements)
{
    // Find the design that is attached to the component.
    VLNV designVLNV = component.getHierRef();

    // The received type is always VLNV::DESIGN so it must be asked from the
    // library handler to make sure the type is correct.
    designVLNV.setType(lh->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        return;
    }

    QSharedPointer<Design> compDesign;

    // Check if the component contains a direct reference to a design.
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);	
        compDesign = libComp.staticCast<Design>();
    }
    // Otherwise check if the component had reference to a design configuration.
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
        QSharedPointer<DesignConfiguration> designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
            compDesign = libComp.staticCast<Design>();
        }

        // If design configuration did not contain a reference to a design.
        if (!compDesign)
        {
            return;
        }
    }
    // If referenced view was not found.
    else
    {
        return;
    }

    // Go through all component instances and search for programmable elements.
    foreach (Design::ComponentInstance const& instance, compDesign->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(instance.componentRef);
        QSharedPointer<Component> childComp = libComp.staticCast<Component>();

		if (childComp)
		{
			// Add the component to the system design only if it is a leaf component and has a CPU
			// (TODO: or endpoints).
			if (!childComp->isHierarchical())
			{
				if (childComp->isCpu())
				{
					elements.append(instance);
				}
			}
			else
			{
				// Otherwise parse the hierarchical components recursively.
				parseProgrammableElements(lh, *childComp, elements);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: generateSystemDesign()
//-----------------------------------------------------------------------------
void generateSystemDesign(LibraryInterface* lh, QString const& directory,
                          Component& component, Design& sysDesign)
{
    // Parse all programmable elements from the HW component.
    QList<Design::ComponentInstance> elements;
    parseProgrammableElements(lh, component, elements);

    // Create SW mapping components for each programmable element.
    QList<Design::ComponentInstance> swInstances;

    foreach (Design::ComponentInstance const& element, elements)
    {
        // Generate a VLNV.
        VLNV swVLNV(VLNV::COMPONENT, sysDesign.getVlnv()->getVendor(),
                    sysDesign.getVlnv()->getLibrary(),
                    sysDesign.getVlnv()->getName().remove(".design") + "_swmapping_" + element.instanceName + ".comp",
                    sysDesign.getVlnv()->getVersion());

        // Create the SW mapping component.
        QSharedPointer<Component> swComp(new Component(swVLNV));
        swComp->setComponentHierarchy(KactusAttribute::KTS_PRODUCT);
        swComp->setComponentFirmness(KactusAttribute::KTS_FIXED);
        swComp->setComponentImplementation(KactusAttribute::KTS_SW);
        swComp->setComponentSWType(KactusAttribute::KTS_SW_MAPPING);

        // Create the model parameter for mapping HW to SW.
        QSharedPointer<ModelParameter> modelParam(new ModelParameter());
        modelParam->setName("kts_hw_ref");
        modelParam->setValue(element.instanceName);
        
        QMap<QString, QSharedPointer<ModelParameter> > modelParameters;
        modelParameters.insert("kts_hw_ref", modelParam);

        // Create a design for the SW mapping component.
        VLNV designVLNV(VLNV::DESIGN, swVLNV.getVendor(), swVLNV.getLibrary(),
                        swVLNV.getName().remove(".comp") + ".design", swVLNV.getVersion());
        QSharedPointer<Design> swDesign(new Design(designVLNV));

        View* hierView = new View("kts_sys_ref");
        hierView->setHierarchyRef(designVLNV);
        hierView->addEnvIdentifier("");

        // Initialize the model.
        Model* model = new Model();
        model->setModelParameters(modelParameters);
        model->addView(hierView);
        swComp->setModel(model);

        // Write object files.
        lh->writeModelToFile(directory, swDesign);
        lh->writeModelToFile(directory, swComp);

        //  Create the corresponding component instance and set its kts_hw_ref.
        Design::ComponentInstance swInstance(element.instanceName, element.displayName,
                                             QString(), swVLNV, QPointF(0, 0));
        swInstance.configurableElementValues.insert("kts_hw_ref", element.instanceName);


        // Add the newly created SW component to the list of SW instances.
        swInstances.append(swInstance);
    }

    sysDesign.setComponentInstances(swInstances);
}
