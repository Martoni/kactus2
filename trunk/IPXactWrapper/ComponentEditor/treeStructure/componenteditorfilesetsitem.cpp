/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetsitem.h"
#include "componenteditorfilesetitem.h"

ComponentEditorFileSetsItem::ComponentEditorFileSetsItem( ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler,
                                                         PluginManager& pluginMgr,
														 QSharedPointer<Component> component,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileSets_(component->getFileSets()),
editor_(component, libHandler, pluginMgr) {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		QSharedPointer<ComponentEditorFileSetItem> fileSetItem(
			new ComponentEditorFileSetItem(fileSet, model, libHandler, component, this));
		childItems_.append(fileSetItem);
	}

    connect(&editor_, SIGNAL(fileAdded(File*)),
            this, SLOT(onFileAdded(File*)), Qt::UniqueConnection);
    connect(&editor_, SIGNAL(filesUpdated()),
            this, SLOT(updateFileItems()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorFileSetsItem::~ComponentEditorFileSetsItem() {
}

QString ComponentEditorFileSetsItem::text() const { 
	return tr("File sets");
}

ItemEditor* ComponentEditorFileSetsItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorFileSetsItem::editor() const {
	return &editor_;
}

QString ComponentEditorFileSetsItem::getTooltip() const {
	return tr("Contains the file sets of the component");
}

void ComponentEditorFileSetsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorFileSetItem> fileSetItem(
		new ComponentEditorFileSetItem(fileSets_.at(index), model_, libHandler_, component_, this));
	
	childItems_.insert(index, fileSetItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetsItem::onFileAdded()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetsItem::onFileAdded(File* file)
{
    foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
        QSharedPointer<ComponentEditorFileSetItem> fileSetItem = item.staticCast<ComponentEditorFileSetItem>();

        if (fileSetItem->getFileSet() == file->getParent())
        {
            fileSetItem->onFileAdded(file);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetsItem::onFilesUpdated()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetsItem::updateFileItems()
{
    foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
        QSharedPointer<ComponentEditorFileSetItem> fileSetItem = item.staticCast<ComponentEditorFileSetItem>();
        fileSetItem->updateFileItems();
    }
}