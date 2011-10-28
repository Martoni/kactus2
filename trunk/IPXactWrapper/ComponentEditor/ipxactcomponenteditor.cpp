/* 
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "ipxactcomponenteditor.h"
#include "componenttreeitem.h"
#include "itemeditor.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/KactusAttribute.h>

#include <models/component.h>

#include <exceptions/write_error.h>

#include "fileSet/fileseteditor.h"
#include "fileSet/fileeditor.h"
#include "fileSet/filebuildereditor.h"
#include "general/generaleditor.h"
#include "modelParameters/modelparametereditor.h"
#include "endpoints/EndpointEditor.h"
#include "ports/portseditor.h"
#include "parameters/parameterseditor.h"
#include "otherClockDrivers/otherclockdriverseditor.h"
#include "channels/channeleditor.h"
#include "busInterfaces/businterfaceeditor.h"
#include "busInterfaces/apieditor.h"
#include "views/vieweditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QSettings>
#include <QSplitter>
#include <QIcon>
#include <QFile>

#include <QDebug>

static const int MIN_COMPONENT_EDITOR_HEIGHT = 500;

IPXactComponentEditor::IPXactComponentEditor(LibraryInterface* lh, const QFileInfo& xmlLocation, 
											 QSharedPointer<Component> component, 
											 QWidget *parent): 
TabDocument(parent, DOC_PROTECTION_SUPPORT), 
xmlFile_(xmlLocation), 
splitter_(this),
handler_(lh),
component_(component),
editableComponent_(QSharedPointer<Component>(new Component(*component_))),
navigator_(editableComponent_, lh, this), 
widgetStack_(this), indexes_(),
currentIndex_() {

	Q_ASSERT_X(component_, "IPXactComponentEditor constructor",
		"Null Component-pointer was given to IPXactComponentEditor");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&splitter_, 1);

	// add the widget that displays the navigation tree
	splitter_.addWidget(&navigator_);

	// add the stacked widget which shows the item-editors
	splitter_.addWidget(&widgetStack_);

	// set the stretch factors for the widgets
	splitter_.setStretchFactor(0, 0);
	splitter_.setStretchFactor(1, 1);

	// set the size policy for the widget
	//setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
	setMinimumHeight(MIN_COMPONENT_EDITOR_HEIGHT);

	// instance for retrieving the previously set options for the Component
	// editor
	QSettings settings("tut.fi", "Kactus2");

	// if geometry is saved then restore it
	if (settings.contains("ComponentEditor/geometry")) {
		restoreGeometry(settings.value("ComponentEditor/geometry").toByteArray());
	}

	// connect the signal from the navigator that informs when user has selected
	// another item to be edited.
	connect(&navigator_, SIGNAL(itemSelected(const QModelIndex&)),
		this, SLOT(selectedItemChanged(const QModelIndex&)), Qt::UniqueConnection);
	
	// the signal that informs when user has decided to remove on item in the
	// tree
	connect(&navigator_, SIGNAL(itemRemoved(const QModelIndex&)),
		this, SLOT(onItemRemoved(const QModelIndex&)), Qt::UniqueConnection);

	connect(&navigator_, SIGNAL(itemMoved()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// set up an empty editor to be displayed at startup when no item is 
	// selected.
	setupNullEditor();

	VLNV const* vlnv = component_->getVlnv();
	setDocumentName(vlnv->getName() + " (" + vlnv->getVersion() + ")");

    if (component_->getComponentImplementation() == KactusAttribute::KTS_SW)
    {
        switch (component_->getComponentSWType())
        {
        case KactusAttribute::KTS_SW_PLATFORM:
            {
                setDocumentType("SW Platform");
                break;
            }

        case KactusAttribute::KTS_SW_APPLICATION:
            {
                setDocumentType("SW Application");
                break;
            }
        
        case KactusAttribute::KTS_SW_ENDPOINTS:
            {
                setDocumentType("MCAPI Endpoints");
                break;
            }
		case KactusAttribute::KTS_SW_MAPPING: 
			{
				setDocumentType("SW mapping");
				break;
			}

        default:
            {
                Q_ASSERT(false);
                break;
            }
        }
    }
    else
    {
	    setDocumentType("Component");
    }

    // Open in unlocked mode by default only if the version is draft.
    setProtection(vlnv->getVersion() != "draft");
}

IPXactComponentEditor::~IPXactComponentEditor() {
	// instance to save the mainWindow settings
	QSettings settings("tut.fi", "Kactus2");

	// remove all the editors from the widget stack and delete them
	QMap<void*, ItemEditor*>::iterator i = indexes_.begin();
	while (i != indexes_.end()) {

		// remove the widget from the stack
		widgetStack_.removeWidget(i.value());
		
		// delete the editor
		delete i.value();

		// remove the item from the map and move iterator to next item
		i = indexes_.erase(i);
	}
	indexes_.clear();

	// save the geometry
	settings.setValue("ComponentEditor/geometry", saveGeometry());
}

void IPXactComponentEditor::selectedItemChanged( const QModelIndex& newIndex ) {

	// make sure new index is valid and is different from old one
	if (!newIndex.isValid() || newIndex == currentIndex_) {
		return;
	}

    ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		newIndex.internalPointer());

	void* dataPointer = item->getDataPointer();

	updateOldIndexItem();

	// update the current ComponentTreeItem before changing to new one
//     if (!updateOldIndexItem()) {
//         //navigator_.selectItem(currentIndex_);
//         return;
//     }

	// if there exists an editor for this element.
	if (indexes_.contains(dataPointer)) {

		// set the correct editor to be displayed
		widgetStack_.setCurrentWidget(indexes_.value(dataPointer));
	}
	// if an element exists but there is not yet an editor created for it
	else {
		createNewEditor(item);
	}

	// finally if editor changes the currentIndex_ is updated the new one.
	currentIndex_ = newIndex;
}

void IPXactComponentEditor::onItemNameChanged( const QString& newName ) {
	
	// make sure that some item is selected and the new name is not empty string.
	if (!currentIndex_.isValid() || newName.isEmpty()) {
		return;
	}

	navigator_.updateItemName(newName, currentIndex_);
}

void IPXactComponentEditor::setupNullEditor()
{
	// create an empty editor
	ItemEditor* editor = new ItemEditor(QSharedPointer<Component>(), &widgetStack_);

	// add the pointer to the map
	indexes_.insert(0, editor);

	// add the empty editor to widgetStack and set it as currently visible
	widgetStack_.insertWidget(0, editor);
	widgetStack_.setCurrentIndex(0);
}

void IPXactComponentEditor::createNewEditor( ComponentTreeItem* item ) {

	Q_ASSERT_X(item, "IPXactComponentEditor::createNewEditor",
		"Null pointer was given as a parameter");

	ItemEditor* editor = 0;

	switch (item->type()) {

		case ComponentTreeItem::FILESET: {
			
			// create the editor
			editor = new FileSetEditor(xmlFile_, editableComponent_, 
				item->getDataPointer(),	&widgetStack_);

			break;
										 }
		case ComponentTreeItem::FILE: {

			// create the editor
			editor = new FileEditor(xmlFile_, editableComponent_, 
				item->getDataPointer(),	&widgetStack_);
			break;
									  }

        case ComponentTreeItem::GENERAL: {
            // Create the general editor.
            editor = new GeneralEditor(this, handler_, editableComponent_, &widgetStack_);
            break;
                                        }

		case ComponentTreeItem::MODELPARAMETERS: {
            if (component_->getComponentImplementation() == KactusAttribute::KTS_SW &&
                component_->getComponentSWType() == KactusAttribute::KTS_SW_ENDPOINTS)
            {
			    editor = new EndpointEditor(editableComponent_, 
				                            item->getDataPointer(), this);
            }
            else
            {
                editor = new ModelParameterEditor(editableComponent_, 
                                                  item->getDataPointer(), this);
            }
			break;
												 }
		case ComponentTreeItem::PORTS: {
            editor = new PortsEditor(editableComponent_, 
 			                         item->getDataPointer(), handler_, this);
			break;
									   }
		case ComponentTreeItem::PARAMETERS: {
			editor = new ParametersEditor(editableComponent_, this);
			break;
									   }
		case ComponentTreeItem::OTHERCLOCKDRIVERS: {
			editor = new OtherClockDriversEditor(editableComponent_, this);
			break;
												   }
		case ComponentTreeItem::CHANNEL: {
			editor = new ChannelEditor(editableComponent_, item->getDataPointer(), this);
			break;
										 }
		case ComponentTreeItem::BUSINTERFACE: {
            if (component_->getComponentImplementation() == KactusAttribute::KTS_SW &&
                component_->getComponentSWType() == KactusAttribute::KTS_SW_PLATFORM)
            {
                editor = new APIEditor(handler_, editableComponent_, 
                                       item->getDataPointer(), this);
            }
            else
            {
			    editor = new BusInterfaceEditor(handler_, editableComponent_, 
				    item->getDataPointer(), this);
            }
			break;
											  }
		case ComponentTreeItem::VIEW: {
			editor = new ViewEditor(editableComponent_, item->getDataPointer(), handler_, this);
			break;
									  }
		case ComponentTreeItem::DEFAULTFILEBUILDERS: {
			editor = new FileBuilderEditor(editableComponent_, item->getDataPointer(), this);
			break;
													 }

		// item type that does not have a direct editor
		default: {
			return;
				 }
	}

	connect(editor, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	

	// add the editor to the widgetStack and connect it's signals
	addEditor(item, editor);
}

void IPXactComponentEditor::onOk() {

	emit noticeMessage(tr("Ok pressed"));

	// apply all the changes in the editors
	onApply();

	// if some editors were in invalid state and could not close then dont 
	// write the xml and dont close the editor
	if (indexes_.size() != 0) {
		return;
	}

	// close the IPXactEditor if everything went fine
	close();
}

void IPXactComponentEditor::onApply() {

	bool everythingValid = true;

	// check the validity of the current editor
	updateOldIndexItem();

	// iterator to the beginning of the map
	QMap<void*, ItemEditor*>::iterator i = indexes_.begin();
	// tell each editor to apply the changes if they are in valid state
	while (i != indexes_.end()) {

		// make sure the editor is in a valid state.
		if (i.value()->confirmEditorChange() && i.value()->isValid()) {
			// apply the changes to the editor
			i.value()->makeChanges();

			// remove the editor from the widget stack
			widgetStack_.removeWidget(i.value());

			// delete the editor
			delete i.value();

			// remove the pointer to editor from the map and move i to the
			// next editor
			i = indexes_.erase(i);
		}
		// if one editor is not in valid state
		else {
			everythingValid = false;
			++i;
		}
	}

	// clear the selection in the navigator
	navigator_.clearSelection();

	// if everything went ok then apply button is disabled and the settings 
	// written into file
	if (everythingValid) {
		
		// write the file
		QFile file(xmlFile_.absoluteFilePath());
		if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			emit errorMessage(tr("Could not open file %1 for writing").arg(
				xmlFile_.absoluteFilePath()));
			return;
		}

		// write the parsed model
		try {
			component_->write(file);
		}
		catch (Write_error error) {
			QString msg(error.what());
			msg += error.errorMsg();
			emit errorMessage(msg);
			return;
		}

		// close the file
		file.close();
	}
}

void IPXactComponentEditor::addEditor( ComponentTreeItem* item, 
									  ItemEditor* editor ) {

	// add the editor to the widget stack and get it's index
	widgetStack_.addWidget(editor);

	// save the pointer to the editor in the map with dataPointer as key
	indexes_.insert(item->getDataPointer(), editor);

	// set the newly created editor to be displayed
	widgetStack_.setCurrentWidget(editor);

	// connect the editor's nameChanged signal to event handler so the name
	// change can be informed to the model
	connect(editor, SIGNAL(nameChanged(const QString)),
		this, SLOT(onItemNameChanged(const QString)), Qt::UniqueConnection);
}

bool IPXactComponentEditor::updateOldIndexItem() {
	
	// if no current index is yet selected
	if (!currentIndex_.isValid()) {
		return true;
	}

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		currentIndex_.internalPointer());

	// find the editor for current model index
	ItemEditor* currentEditor = indexes_.value(item->getDataPointer(), 0);

	// if there was a valid editor
	if (currentEditor) {

        // Check if the change is allowed by the current editor.
        if (!currentEditor->confirmEditorChange())
        {
            return false;
        }

		// tell editor to save the settings
		currentEditor->makeChanges();

		// set items validity according to editor's status
		item->setValidity(currentEditor->isValid());
	}

	// tell tree navigator to update the current model index so changes become
	// visible instantly
	navigator_.updateItem(currentIndex_);

    return true;
}

void IPXactComponentEditor::onItemRemoved( const QModelIndex& index ) {
	// make sure index is valid
	if (!index.isValid()) {
		return;
	}
	// if the index is the current index then set current index to be invalid
	// to avoid problems in other functions.
	else if (index == currentIndex_) {
		currentIndex_ = QModelIndex();
	}

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		index.internalPointer());
	Q_ASSERT_X(item, "IPXactComponentEditor::onItemRemoved",
		"static_cast failed to give valid ComponentTreeItem-pointer");

	// get all the dataPointers that can be used to remove the matching editors
	QList<void*> dataPointers;
	item->getDataPointers(dataPointers);

	// remove all the widgets for the elements but start at the end of the
	// list because the elements must be removed so that lower-hierarchy ones 
	// are removed first. The list is created recursively so the low-hierarchy
	// elements are at the end of the list.
	for (int i = dataPointers.size() - 1; i >= 0; --i) {
		
		// find the editor for the dataPointer, if editor does not exist then
		// return a null pointer
		ItemEditor* editor = indexes_.value(dataPointers.at(i), 0);
		
		// if editor didn't exits
		if (!editor) {

			// there is no need to remove anything because the editor did not
			// exist and we can move to next editor to be removed
			continue;
		}

		// erase the key-value pair from the map
		indexes_.erase(indexes_.find(dataPointers.at(i)));

		// remove editor from stackedWidget
		widgetStack_.removeWidget(editor);

		// tell the editor to destroy the model
		editor->removeModel();

		// free the allocated memory for the editor
		delete editor;		
	}

    emit contentChanged();
}

VLNV IPXactComponentEditor::getComponentVLNV() const {
	return *component_->getVlnv();
}

bool IPXactComponentEditor::save() {

	// if all editors were valid then document can be written to disk
	if (saveEditors()) {

		*component_ = *editableComponent_;
		handler_->writeModelToFile(component_);

		return TabDocument::save();
	}
	// there was errors so do not write
	else {
		emit errorMessage(tr("Component was not valid and was not saved to disk."));
		return false;
	}
}

bool IPXactComponentEditor::saveEditors() {
	bool everythingValid = true;

	// check the validity of the current editor
	updateOldIndexItem();

	// iterator to the beginning of the map
	QMap<void*, ItemEditor*>::iterator i = indexes_.begin();
	// tell each editor to apply the changes if they are in valid state
	while (i != indexes_.end()) {

		// make sure the editor is in a valid state.
		if (i.value()->confirmEditorChange() && i.value()->isValid()) {
			// apply the changes to the editor
			i.value()->makeChanges();

			// remove the editor from the widget stack
			widgetStack_.removeWidget(i.value());

			// delete the editor
			delete i.value();

			// remove the pointer to editor from the map and move i to the
			// next editor
			i = indexes_.erase(i);
		}
		// if one editor is not in valid state
		else {
			everythingValid = false;
			++i;
		}
	}

	// clear the selection in the navigator
	navigator_.clearSelection();

	// create a null editor because it is needed to display empty editor
	setupNullEditor();

	return everythingValid;
}

bool IPXactComponentEditor::saveAs()
{
    // Ask the user for a new VLNV along with attributes and directory.
    KactusAttribute::ProductHierarchy prodHier = component_->getComponentHierarchy();
    KactusAttribute::Firmness firmness = component_->getComponentFirmness();

    VLNV vlnv;
    QString directory;

    if (!NewObjectDialog::saveAsDialog(parentWidget(), handler_, *component_->getVlnv(),
                                       prodHier, firmness, vlnv, directory))
    {
        // If the user canceled, there was no error.
        return true;
    }

	// if all editors were valid then document can be written to disk
	if (saveEditors()) {

        // Update the kactus2 attributes.
		editableComponent_->setComponentHierarchy(prodHier);
		editableComponent_->setComponentFirmness(firmness);

		// create copies of the objects so saving is not done to the original component
		component_ = QSharedPointer<Component>(new Component(*editableComponent_));

		// make sure the vlnv type is correct
		VLNV compVLNV = vlnv;
		compVLNV.setType(VLNV::COMPONENT);

		// update the vlnv
		component_->setVlnv(vlnv);
		editableComponent_->setVlnv(vlnv);

        // Write the component to a file.
        handler_->writeModelToFile(directory, component_);

		setDocumentName(compVLNV.getName() + " (" + compVLNV.getVersion() + ")");

		return TabDocument::saveAs();
	}
	
	// there was errors so do not write
	else {
		emit errorMessage(tr("Component was not valid and was not saved to disk."));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void IPXactComponentEditor::setProtection(bool locked)
{
    TabDocument::setProtection(locked);

    // TODO: Enable/disable all editing.
    widgetStack_.setEnabled(!locked);
	navigator_.setLocked(locked);
}

bool IPXactComponentEditor::isHWImplementation() const {
	return component_->getComponentImplementation() == KactusAttribute::KTS_HW;
}
