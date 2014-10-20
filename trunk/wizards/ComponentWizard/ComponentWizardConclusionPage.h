//-----------------------------------------------------------------------------
// File: ComponentWizardConclusionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 06.06.2013
//
// Description:
// Conclusion page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDCONCLUSIONPAGE_H
#define COMPONENTWIZARDCONCLUSIONPAGE_H

#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <QWizardPage>
#include <QLabel>
#include <QTextEdit>

class LibraryInterface;
class ComponentDiffWidget;

//-----------------------------------------------------------------------------
//! Conclusion page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardConclusionPage : public QWizardPage
{
    Q_OBJECT
public:
    /*!
    *  Constructor.
    *
    *      @param [in] component    The initial component to create.
    *      @param [in] lh           The library interface.
    *      @param [in] parent       The parent wizard.
    */
    ComponentWizardConclusionPage(QSharedPointer<Component> component, LibraryInterface* lh, 
        QWidget* parent);

    /*!
    *  Destructor.
    */
    ~ComponentWizardConclusionPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    virtual void initializePage();

public slots:

    virtual void onComponentChanged(QSharedPointer<Component> component);

private:
    // Disable copying.
    ComponentWizardConclusionPage(ComponentWizardConclusionPage const& rhs);
    ComponentWizardConclusionPage& operator=(ComponentWizardConclusionPage const& rhs);

    void setupLayout();
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The original component.
    QSharedPointer<Component> originalComponent_;

    //! The created component.
    QSharedPointer<Component> workingComponent_;

    //! Pointer to the library manager.
    LibraryInterface* handler_;

    //! Widget for summary labels.
    QWidget summaryWidget_;

    //! Label for VLNV vendor.
    QLabel* vendorLabel_;

    //! Label for VLNV library.
    QLabel* libraryLabel_;

    //! Label for VLNV name.
    QLabel* nameLabel_;

    //! Label for VLNV version.
    QLabel* versionLabel_;

    //! Label for Kactus attribute hierarchy.
    QLabel* hierarchyLabel_;

    //! Label for Kactus attribute firmness.
    QLabel* firmnessLabel_;

    //! Label for the xml path.
    QLabel* directoryLabel_;

    //! Label for author.
    QLabel* authorLabel_;

    //! Label for the component filesets.
    QLabel* filesetsLabel_;

    //! Label for the created parameters.
    QLabel* parametersLabel_;

    //! Label for the created ports.
    QLabel* portsLabel_;

    //! Label for the created views.
    QLabel* viewsLabel_;

    //! Label for the component description.
    QLabel* descriptionLabel_;

    //! The component preview box.
    ComponentPreviewBox previewBox_;

    ComponentDiffWidget* diffView_;
};

#endif // COMPONENTWIZARDCONCLUSIONPAGE_H
