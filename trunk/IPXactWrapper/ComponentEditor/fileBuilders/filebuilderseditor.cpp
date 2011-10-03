/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: filebuilderseditor.cpp
 */

#include "filebuilderseditor.h"

#include "filebuildersdelegate.h"

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

FileBuildersEditor::FileBuildersEditor(void* dataPointer, 
									   QWidget *parent):
QWidget(parent),
addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
view_(this), model_(dataPointer, this), 
proxy_(this){

	// set view to be sortable
	view_.setSortingEnabled(true);
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setAlternatingRowColors(true);
	view_.verticalHeader()->hide();
	view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
	view_.setWordWrap(true);

	// set the width for columns
	view_.horizontalHeader()->setMinimumSectionSize(180);

	view_.setItemDelegate(new FileBuildersDelegate(this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->addLayout(buttonLayout);

	connect(&addRowButton_, SIGNAL(clicked(bool)),
		&model_, SLOT(onAddRow()), Qt::UniqueConnection);
	connect(&removeRowButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRemove()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	restoreChanges();
}

FileBuildersEditor::~FileBuildersEditor() {
}

bool FileBuildersEditor::isValid() const {
	return model_.isValid();
}

void FileBuildersEditor::restoreChanges() {
	model_.restore();
}

void FileBuildersEditor::applyChanges() {
	model_.apply();
}

void FileBuildersEditor::onRemove() {
	QModelIndex index = proxy_.mapToSource(view_.currentIndex());

	// if index is valid then remove the row
	if (index.isValid())
		model_.onRemoveRow(index.row());
}
