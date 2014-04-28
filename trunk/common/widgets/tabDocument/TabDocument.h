//-----------------------------------------------------------------------------
// File: TabDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 4.5.2011
//
// Description:
// Tab document base class.
//-----------------------------------------------------------------------------

#ifndef TABDOCUMENT_H
#define TABDOCUMENT_H

#include <library/LibraryManager/vlnv.h>

#include <designEditors/common/DrawMode.h>
#include <common/IEditProvider.h>

#include <QWidget>
#include <QTabWidget>
#include <QString>
#include <QSettings>
#include <QMap>

//-----------------------------------------------------------------------------
//! TabDocument class.
//-----------------------------------------------------------------------------
class TabDocument : public QWidget
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! DocSupportFlags enumeration.
    //-----------------------------------------------------------------------------
    enum DocSupportFlags
    {
        DOC_ZOOM_SUPPORT = 0x01,
        DOC_DRAW_MODE_SUPPORT = 0x02,
        DOC_PRINT_SUPPORT = 0x04,
        DOC_PROTECTION_SUPPORT = 0x08,
        DOC_EDIT_SUPPORT = 0x10,
        DOC_VISIBILITY_CONTROL_SUPPORT = 0x20,
    };

	enum SupportedWindows {
		OUTPUTWINDOW            = 0x001,
		PREVIEWWINDOW           = 0x002,
		LIBRARYWINDOW           = 0x004,
		CONFIGURATIONWINDOW     = 0x008,
		CONNECTIONWINDOW        = 0x010,
		INTERFACEWINDOW         = 0x020,
		INSTANCEWINDOW          = 0x040,
        ADHOC_WINDOW            = 0x080,
        SYSTEM_DETAILS_WINDOW   = 0x100,
        CONTEXT_HELP_WINDOW     = 0x200,
        ADDRESS_WINDOW          = 0x400,
	};

    /*!
     *  Constructor.
     *
     *      @param [in] parent        The parent widget.
     *      @param [in] flags         The document support flags.
     *      @param [in] minZoomLevel  The minimum zoom level.
	 *      @param [in] maxZoomLevel  The maximum zoom level.
	 */
	TabDocument(QWidget* parent, unsigned int flags = 0,
		        int minZoomLevel = 100, int maxZoomLevel = 100);

	/*!
     *  Destructor.
     */
    virtual ~TabDocument();

	/*! \brief Get the VLNV that can be used to identify the document.
	 *
	 * \return The VLNV that identifies the document.
	*/
	virtual VLNV getIdentifyingVLNV() const = 0;

    /*!
     *  Applies the current settings into use.
     */
    virtual void applySettings(QSettings const& settings);

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

    /*!
     *  Refreshes the document view.
     */
    virtual void refresh();

    /*!
     *  Adds the tab document to the given tab widget.
     *
     *      @param [in] tabWidget The tab widget.
     */
    void setTabWidget(QTabWidget* tabWidget);

    /*!
     *  Fits the document fully in view. Supported only if the document can be zoomed.
     */
    virtual void fitInView();

    /*!
     *  Sets the zoom level.
     *
     *      @param [in] level The zoom level in percents.
     */
    virtual void setZoomLevel(int level);

    /*!
     *  Sets the draw mode of the document.
     *
     *      @param [in] mode The draw mode.
     *
     *      @remarks Proper implementation is required if the document is a diagram.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

    /*!
     *  Sets the state that the document has previously been unlocked.
     */
    void setPreviouslyUnlocked();

    /*!
     *  Returns the name of the document.
     */
    QString const& getDocumentName() const;

    /*!
     *  Returns the current zoom level in percents.
     */
    int getZoomLevel() const;

    /*!
     *  Returns the maximum allowed zoom level.
     */
    int getMaxZoomLevel() const;

    /*!
     *  Returns the minimum allowed zoom level.
     */
    int getMinZoomLevel() const;

    /*!
     *  Returns the supported draw modes.
     */
    virtual unsigned int getSupportedDrawModes() const;

    /*!
     *  Returns true if the document is protected (i.e. in locked state).
     */
    bool isProtected() const;

    /*!
     *  Returns the document flags.
     */
    unsigned int getFlags() const;
    
    /*!
     *  Returns true if the document has been modified but not saved.
     */
    bool isModified() const;

	/*! \brief Ask the user if file should be saved or not
	 *
	 * \return bool True if user wants to save a file.
	*/
	bool askSaveFile() const;

	/*! \brief Get the vlnv of the current component
	 *
	 * \return Base class returns invalid vlnv.
	*/
	virtual VLNV getDocumentVLNV() const;

    /*!
     *  Adds a related VLNV.
     *
     *      @param [in] vlnv The VLNV to add.
     *
     *      @remarks The occurrences of each unique VLNV are calculated internally.
     */
    void addRelatedVLNV(VLNV const& vlnv);

    /*!
     *  Removes a related VLNV.
     *
     *      @param [in] vlnv The VLNV to remove.
     */
    void removeRelatedVLNV(VLNV const& vlnv);    

    /*!
     *  Clears the list of related VLNVs.
     */
    void clearRelatedVLNVs();

    /*!
     *  Returns the list of related VLNVs.
     */
    QList<VLNV> getRelatedVLNVs() const;

    /*!
     *  Returns the edit provider.
     *
     *      @return Base class implementation returns null.
     *
     *      @remarks Edit support should be queried with getFlags().
     */
    virtual IEditProvider* getEditProvider();

	/*! \brief Get the bit fields that define which windows are supported by this tab.
	 *
	 * \return unsigned int which contains the bit fields specifying supported windows.
	*/
	virtual unsigned int getSupportedWindows() const;

    /*!
     *  Returns true if the document has previously been unlocked.
     */
    bool isPreviouslyUnlocked() const;

    /*!
     *  Returns the supported visibility controls.
     */
    QMap<QString, bool> const& getVisibilityControls() const;

public slots:
    /*!
     *  Validates the document against the IP-XACT standard.
     *
     *      @param [out] errorList Error message list for reporting standard violations.
     *
     *      @return True if the document is valid. False if there were any violations.
     */
    virtual bool validate(QStringList& errorList);

    //! Saves the document and resets the modified state.
    virtual bool save();

	//! Saves the document as new object and resets the modified state.
	virtual bool saveAs();

    //! Prints the document, if the document supports printing. Support can be queried with getFlags().
    virtual void print();

    /*!
     *  Sets the document modified/unmodified.
     *
     *      @param [in] modified True if the document should be set modified; otherwise false.
     */
    void setModified(bool modified = true);

    /*!
     *  Requests the document to be refreshed when shown the next time.
     */
    void requestRefresh();

	/*! \brief Display an error dialog to user with given text.
	 *
	 * \param message Contains the text to display in the error dialog.
	 *
	*/
	void onErrorDialog(const QString& message);

signals:
	//! \brief Emitted when contents of the widget change
	void contentChanged();

	//! \brief Send an error message to be printed to user.
	void errorMessage(const QString& msg);

	//! \brief Send a notification to be printed to user.
	void noticeMessage(const QString& msg);

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

	//! \brief Emitted when the tab document is refreshed.
	void refreshed();

	//! \brief Emitted when the modified state changes.
	void modifiedChanged(bool modified);

    //! Emitted when the document has been saved.
    void documentSaved(TabDocument* doc);

	//! \brief Emitted when a design editor should be opened for given hierarchical view.
	void openDesign(const VLNV& componentVLNV, const QString& viewName);

	//! \brief Emitted when a bus editor should be opened for given bus.
	void openBus(const VLNV& busdefVLNV, const VLNV& absdefVLNV);

	//! \brief Emitted when a COM definition should be opened in an editor.
	void openComDefinition(const VLNV& comdefVLNV);

	//! \brief Emitted when a SW design should be opened in an editor.
	void openSWDesign(const VLNV& componentVLNV, const QString& viewName);

	//! \brief Emitted when a system design should be opened in an editor.
	void openSystemDesign(const VLNV& componentVLNV, const QString& viewName);

protected:
    /*!
     *  Sets the name of the document.
     *
     *      @param [in] name The name of the document.
     */
    void setDocumentName(QString const& name);

    /*!
     *  Sets the type of the document. If not empty, the name of the tab will be shown as: DocName [DocType]
     *
     *      @param [in] type The type name.
     */
    void setDocumentType(QString const& type);

    /*!
     *  Adds a new visibility control to the document.
     *
     *      @param [in] name   The name of the new visibility control.
     *      @param [in] state  The initial state of the visibility control.
     */
    void addVisibilityControl(QString const& name, bool state);
    
    /*!
     *  Shows the document and asks to refresh it if refresh was requested.
     */
    virtual void showEvent(QShowEvent* event);

	//! \brief Contains the bit fields that define which windows are supported for this tab.
	unsigned int supportedWindows_;

private slots:
    //! Handles the refresh request.
    void handleRefreshRequest();

private:
    // Disable copying.
    TabDocument(TabDocument const& rhs);
    TabDocument& operator=(TabDocument const& rhs);

    /*!
     *  Updates the tab title.
     */
    void updateTabTitle();

    /*!
     *  Sets the tab title of this document.
     *
     *      @param [in] title The title text.
     */
    void setTabTitle(QString const& title);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Document flags.
    unsigned int flags_;

    //! Boolean flag for 'modified' state.
    bool modified_;

    //! The protection state (locked/unlocked).
    bool locked_;

    //! The zoom level as a percentage.
    int zoomLevel_;

    //! Maximum zoom level.
    int maxZoomLevel_;

    //! Minimum zoom level.
    int minZoomLevel_;

    //! The document title.
    QString title_;

    //! The name of the document.
    QString docName_;

    //! The document type name.
    QString docType_;

    //! The parent tab widget.
    QTabWidget* tabWidget_;

    //! If true, the document has been previously unlocked.
    bool previouslyUnlocked_;

    //! Visibility control map.
    QMap<QString, bool> visibilityControls_;

    //! Related VLNV registry.
    QMap<VLNV, unsigned int> relatedVLNVs_;

    //! If true, the document must be refreshed when shown the next time.
    bool refreshRequested_;
};

//-----------------------------------------------------------------------------

#endif // TABDOCUMENT_H
