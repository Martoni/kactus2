/* 
 *
 *  Created on: 25.5.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyrefwidget.cpp
 */

#include "hierarchyrefwidget.h"

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/component.h>
#include <mainwindow/mainwindow.h>

#include <QVBoxLayout>
#include <QStringList>
#include <QFormLayout>
#include <QApplication>

HierarchyRefWidget::HierarchyRefWidget(QSharedPointer<View> view, 
									   QSharedPointer<Component> component,
										LibraryInterface* libHandler,
									   QWidget *parent): 
QWidget(parent),
view_(view),
component_(component),
hierarchyRef_(NULL),
topLevelRef_(this) {

	// find the main window for VLNV editor
	QWidget* parentW = NULL;
	foreach (QWidget* widget, QApplication::topLevelWidgets()) {
		MainWindow* mainWnd = dynamic_cast<MainWindow*>(widget);
		if (mainWnd) {
			parentW = mainWnd;
			break;
		}
	}

	hierarchyRef_ = new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parentW, this);

	hierarchyRef_->setTitle(tr("Hierarchy reference"));
	hierarchyRef_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    hierarchyRef_->addContentType(VLNV::DESIGN);

	QFormLayout* extensionLayout = new QFormLayout();
	extensionLayout->addRow(tr("VendorExtension: Reference to a top-level "
		"implementation view"), &topLevelRef_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(hierarchyRef_);
	layout->addLayout(extensionLayout);
	layout->addStretch();
	layout->setContentsMargins(0, 0, 0, 0);

	connect(hierarchyRef_, SIGNAL(vlnvEdited()),
		this, SLOT(onVLNVChanged()), Qt::UniqueConnection);
	connect(&topLevelRef_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onTopRefChanged(const QString&)), Qt::UniqueConnection);
}

HierarchyRefWidget::~HierarchyRefWidget() {
}

bool HierarchyRefWidget::isValid() const {
	return hierarchyRef_->isValid();
}

void HierarchyRefWidget::refresh() {
	
	// ask the vlnv from the model
	hierarchyRef_->setVLNV(view_->getHierarchyRef());

	// before changing the index, the editor must be disconnected
	disconnect(&topLevelRef_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onTopRefChanged(const QString&)));

	// remove the previous values from the combo box
	topLevelRef_.clear();

	// ask the available views from the component
	QStringList list;
	// if user does not want to reference any view
	list.append(QString(""));

	list.append(component_->getViewNames());

	topLevelRef_.addItems(list);

	// find the text in the combo box and select it
	QString topRef = view_->getTopLevelView();
	int index = topLevelRef_.findText(topRef);
	topLevelRef_.setCurrentIndex(index);

	// after change reconnect the editor
	connect(&topLevelRef_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onTopRefChanged(const QString&)), Qt::UniqueConnection);
}

void HierarchyRefWidget::onVLNVChanged() {
	view_->setHierarchyRef(hierarchyRef_->getVLNV());
	emit contentChanged();
}

void HierarchyRefWidget::onTopRefChanged( const QString& newRef ) {
	view_->setTopLevelView(newRef);
	emit contentChanged();
}

void HierarchyRefWidget::clear() {
	
	// clear the hierarchy ref
	hierarchyRef_->setVLNV(VLNV());

	// before changing the index, the editor must be disconnected
	disconnect(&topLevelRef_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onTopRefChanged(const QString&)));

	// clear the top level ref
	topLevelRef_.clear();

	// after change reconnect the editor
	connect(&topLevelRef_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onTopRefChanged(const QString&)), Qt::UniqueConnection);

	view_->clearHierarchy();
}

void HierarchyRefWidget::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/hierview.html");
}