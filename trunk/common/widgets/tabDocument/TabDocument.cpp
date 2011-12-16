//-----------------------------------------------------------------------------
// File: TabDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 4.5.2011
//
// Description:
// Tab document base class.
//-----------------------------------------------------------------------------

#include "TabDocument.h"

#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: TabDocument()
//-----------------------------------------------------------------------------
TabDocument::TabDocument(QWidget* parent, unsigned int flags, 
						 int minZoomLevel, int maxZoomLevel) : QWidget(parent),
                                                               flags_(flags), modified_(false),
                                                               locked_(false), zoomLevel_(100),
                                                               maxZoomLevel_(maxZoomLevel),
                                                               minZoomLevel_(minZoomLevel), title_(""),
                                                               docName_(""), tabWidget_(0),
                                                               previouslyUnlocked_(false)

{
    connect(this, SIGNAL(contentChanged()), this, SLOT(setModified()));
}

//-----------------------------------------------------------------------------
// Function: ~TabDocument()
//-----------------------------------------------------------------------------
TabDocument::~TabDocument()
{
}

//-----------------------------------------------------------------------------
// Function: applySettings()
//-----------------------------------------------------------------------------
void TabDocument::applySettings(QSettings const&)
{
}

//-----------------------------------------------------------------------------
// Function: setTabWidget()
//-----------------------------------------------------------------------------
void TabDocument::setTabWidget(QTabWidget* tabWidget)
{
    // Remove from the old tab.
    if (tabWidget_ != 0)
    {
        tabWidget_->removeTab(tabWidget_->indexOf(this));
    }

    tabWidget_ = tabWidget;
    tabWidget_->addTab(this, title_);
    tabWidget_->setCurrentWidget(this);
}

//-----------------------------------------------------------------------------
// Function: setDocumentName()
//-----------------------------------------------------------------------------
void TabDocument::setDocumentName(QString const& name)
{
    docName_ = name;
    updateTabTitle();
}

//-----------------------------------------------------------------------------
// Function: setDocumentType()
//-----------------------------------------------------------------------------
void TabDocument::setDocumentType(QString const& type)
{
    if (type.isEmpty())
    {
        docType_ = "";
    }
    else
    {
        docType_ = " [" + type + "]";
    }

    updateTabTitle();
}

//-----------------------------------------------------------------------------
// Function: fitInView()
//-----------------------------------------------------------------------------
void TabDocument::fitInView()
{
}

//-----------------------------------------------------------------------------
// Function: setZoomLevel()
//-----------------------------------------------------------------------------
void TabDocument::setZoomLevel(int level)
{
    level = std::min(std::max(level, minZoomLevel_), maxZoomLevel_);

    if (flags_ & DOC_ZOOM_SUPPORT)
    {
        zoomLevel_ = level;
    }
}

//-----------------------------------------------------------------------------
// Function: setMode()
//-----------------------------------------------------------------------------
void TabDocument::setMode(DrawMode)
{
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void TabDocument::setProtection(bool locked)
{
    locked_ = locked;

    if (!locked_)
    {
        previouslyUnlocked_ = true;
    }
}

//-----------------------------------------------------------------------------
// Function: setModified()
//-----------------------------------------------------------------------------
void TabDocument::setModified(bool modified)
{
	if (modified == modified_) 
		return;

    if (modified)
    {
        setTabTitle(docName_ + docType_ + "*");
    }
    else
    {
        setTabTitle(docName_ + docType_);
    }

    modified_ = modified;
	emit modifiedChanged(modified_);
}

//-----------------------------------------------------------------------------
// Function: getDocumentName()
//-----------------------------------------------------------------------------
QString const& TabDocument::getDocumentName() const
{
    return docName_;
}

//-----------------------------------------------------------------------------
// Function: getZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getZoomLevel() const
{
    return zoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: getSupportedDrawModes()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getSupportedDrawModes() const
{
    return MODE_NONE;
}

//-----------------------------------------------------------------------------
// Function: isProtected()
//-----------------------------------------------------------------------------
bool TabDocument::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: getFlags()
//-----------------------------------------------------------------------------
unsigned int TabDocument::getFlags() const
{
    return flags_;
}

//-----------------------------------------------------------------------------
// Function: isModified()
//-----------------------------------------------------------------------------
bool TabDocument::isModified() const
{
    return modified_;
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool TabDocument::save()
{
    setModified(false);
    return true;
}

//-----------------------------------------------------------------------------
// Function: saveAs()
//-----------------------------------------------------------------------------
bool TabDocument::saveAs()
{
	setModified(false);
	return true;
}

//-----------------------------------------------------------------------------
// Function: print()
//-----------------------------------------------------------------------------
void TabDocument::print()
{
}

//-----------------------------------------------------------------------------
// Function: getMaxZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getMaxZoomLevel() const
{
    return maxZoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: getMinZoomLevel()
//-----------------------------------------------------------------------------
int TabDocument::getMinZoomLevel() const
{
    return minZoomLevel_;
}

//-----------------------------------------------------------------------------
// Function: askSaveFile()
//-----------------------------------------------------------------------------
bool TabDocument::askSaveFile() const {

	QMessageBox::StandardButton button = QMessageBox::question(parentWidget(), 
		tr("Save changes to file?"), 
		tr("Would you like to save the changes you have made to the file?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	// return true if user clicked yes
	return QMessageBox::Yes == button;
}

//-----------------------------------------------------------------------------
// Function: updateTabTitle()
//-----------------------------------------------------------------------------
void TabDocument::updateTabTitle()
{
    // Update also the title.
    if (isModified())
    {
        setTabTitle(docName_ + docType_ + "*");
    }
    else
    {
        setTabTitle(docName_ + docType_);
    }
}

//-----------------------------------------------------------------------------
// Function: setTabTitle()
//-----------------------------------------------------------------------------
void TabDocument::setTabTitle(QString const& title)
{
    // Update the tab title if the document has been added to a tab widget.
    if (tabWidget_ != 0)
    {
        int index = tabWidget_->indexOf(this);

        if (index >= 0)
        {
            tabWidget_->setTabText(index, title);
        }
    }

    title_ = title;
}

VLNV TabDocument::getComponentVLNV() const {
	return VLNV();
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
IEditProvider* TabDocument::getEditProvider()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void TabDocument::refresh() {
	emit refreshed();
}

bool TabDocument::isHWImplementation() const {
	return false;
}

//-----------------------------------------------------------------------------
// Function: setPreviouslyUnlocked()
//-----------------------------------------------------------------------------
void TabDocument::setPreviouslyUnlocked()
{
    previouslyUnlocked_ = true;
}

//-----------------------------------------------------------------------------
// Function: isPreviouslyUnlocked()
//-----------------------------------------------------------------------------
bool TabDocument::isPreviouslyUnlocked() const
{
    return previouslyUnlocked_;
}
