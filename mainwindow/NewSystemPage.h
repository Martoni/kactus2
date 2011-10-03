//-----------------------------------------------------------------------------
// File: NewSystemPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 9.6.2011
//
// Description:
// New system property page.
//-----------------------------------------------------------------------------

#ifndef NEWSYSTEMPAGE_H
#define NEWSYSTEMPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>

#include <QTreeWidget>
#include <QLineEdit>

class LibraryInterface;
class LibraryItem;
class VLNVEditor;
class VLNV;

//-----------------------------------------------------------------------------
//! NewSystemPage class.
//-----------------------------------------------------------------------------
class NewSystemPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewSystemPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewSystemPage();

    /*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *      @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    bool prevalidate() const;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    bool onPageChange();

public slots:
    /*!
     *  Asks the user to select a directory.
     */
    void selectDirectory();

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    /*!
     *  Called when the item in the tree widget has changed.
     */
    void onTreeItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem*);

signals:
    //! Signaled when a system design should be created.
    void createSystem(VLNV const& compVLNV, VLNV const& sysVLNV, QString const& directory);

private:
    // Disable copying.
    NewSystemPage(NewSystemPage const& rhs);
    NewSystemPage& operator=(NewSystemPage const& rhs);

    /*!
     *  Adds child items to a tree item from a library item.
     *
     *      @param [in]      libItem   The library item ("source").
     *      @param [in/out]  treeItem  The tree item ("destination").
     */
    void addChildItems(LibraryItem const* libItem, QTreeWidgetItem* treeItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! Component selection tree widget.
    QTreeWidget* compTreeWidget_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

    //! Line edit for the directory.
    QLineEdit* directoryEdit_;
};

//-----------------------------------------------------------------------------

#endif // NEWSYSTEMPAGE_H
