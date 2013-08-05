//-----------------------------------------------------------------------------
// File: NewComDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 17.4.2012
//
// Description:
// New COM definition page.
//-----------------------------------------------------------------------------

#ifndef NEWCOMDEFINITIONPAGE_H
#define NEWCOMDEFINITIONPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/LibraryPathSelector/librarypathselector.h>

#include <QPushButton>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! NewComDefinitionPage class.
//-----------------------------------------------------------------------------
class NewComDefinitionPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewComDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewComDefinitionPage();

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
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    //! Called when the user clicks browse button.
    void onBrowse();

signals:
    //! Signaled when a COM definition should be created.
    void createComDefinition(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewComDefinitionPage(NewComDefinitionPage const& rhs);
    NewComDefinitionPage& operator=(NewComDefinitionPage const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibraryPathSelector* directoryEdit_;

    //! Pushbutton for browsing the save directory.
    QPushButton* browseButton_;

    //! Flag for indicating whether the directory has been chosen using browse.
    bool directorySet_;
};

//-----------------------------------------------------------------------------

#endif // NEWCOMDEFINITIONPAGE_H