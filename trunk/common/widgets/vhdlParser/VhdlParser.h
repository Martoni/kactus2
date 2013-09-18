//-----------------------------------------------------------------------------
// File: VhdlParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser reads a given vhdl file, creates ports and generics accordingly
// and displays the file highlighting the created ports and generics. 
//-----------------------------------------------------------------------------
#ifndef VhdlParser_H
#define VhdlParser_H

#include <QPlainTextEdit>
#include <QMouseEvent>
#include <QList>
#include <QMap>

#include <common/widgets/vhdlParser/VhdlSyntax.h>

class ModelParameter;
class Port;


//-----------------------------------------------------------------------------
//! Class VhdlParser.
//-----------------------------------------------------------------------------
class VhdlParser : public QPlainTextEdit 
{
    Q_OBJECT
public:

     /*!
     *  Constructor.
	 *
     *      @param [in] parent The parent widget.
     */
    VhdlParser(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~VhdlParser();

    /*!
     *   Reads a vhdl file and creates generics and ports.
     *
     *      @param [in] absolutePath The absolute path of the file.
	 *
	 *      @return False, if file could not be read, otherwise true.
     */
     virtual void  parseFile(QString absolutePath);

    /*!
     *   Scrolls the texteditor to the beginning of a valid entity declaration.
     */
    void scrollToEntityBegin();

public slots:

    //! Called when a model parameter is outside the text editor.
    virtual void editorChangedModelParameter(QSharedPointer<ModelParameter> changedParameter);

    //! Called when a model parameter is removed outside the text editor.
    virtual void editorRemovedModelParameter(QSharedPointer<ModelParameter> removedParameter);

    //! Called when a port is removed outside the text editor.
    virtual void editorRemovedPort(QSharedPointer<Port> removedPort);

signals:
    
    //! Emitted when a port is created or selected.
    void addPort(QSharedPointer<Port> port);

    //! Emitted when a port is removed or deselected.
    void removePort(QSharedPointer<Port> port);

    //! Emitted when a generic is created or selected.
    void addGeneric(QSharedPointer<ModelParameter> modelParam);

    //! Emitted when a generic is removed or deselected.
    void removeGeneric(QSharedPointer<ModelParameter> modelParam);

protected:
 
    //! Called when mouse is double clicked.
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

private:

    //! No copying.
    VhdlParser(const VhdlParser&);

    //! No assignment.
    VhdlParser& operator=(const VhdlParser&);  
  
    //! Position and enable of a port/generic in the document.
    struct SelectionInfo
    {
        int beginPos;
        int endPos;
        bool enabled;      
        bool operator<(const SelectionInfo& other) const{return beginPos < other.beginPos;}
    };

	/*!
     *  Signals add for all ports.
     */
    void importPorts();

	/*!
     *  Signals remove for all ports and empties lists to ports.
     */
    void removePorts();

	/*!
     *  Signals add for all model parameters.
     */
    void importGenerics();

	/*!
     *  Signals remove for all model parameters and empties lists to model parameters.
     */
    void removeGenerics();

	/*!
     *  Parses, formats, checks and displays a vhdl file.
     */
    void createDocument(QFile& vhdlFile);

    /*!
     *   Checks if given vhdl file is valid.
     *
     *      @param [in] fileString The vhdl file as string.
	 *
	 *      @return True if the file is valid, otherwise false.
     */
    bool checkEntityStructure(QString const& fileString) const;


    /*!
     *   Sets the entity name in the ending expression.
     *
     *      @param [in] fileString The vhdl file as string.
     */
    void setEntityEndExp(QString const& fileString);

    /*!
     *   Cuts out a section of a text leaving delimiting expressions.
     *
     *      @param [in] begin The beginning of the cut section.
	 *
     *      @param [in] end The end of the cut section.
	 *
     *      @param [in] text The text where to cut from.
	 *
	 *      @return The text section inside begin and end. 
     *              The beginning and the end are included.
     */
    QString parseSection(QRegExp const& begin, QRegExp const& end, 
        QString const& text) const;

    /*!
     *   Cuts out a section of a text omitting delimiting expressions.
     *
     *      @param [in] begin The beginning of the cut section.
	 *
     *      @param [in] end The end of the cut section.
	 *
     *      @param [in] text The text where to cut from.
	 *
	 *      @return The text section inside begin and end. 
     *              The beginning and the end are omitted.
     */
    QString parseSectionContent(QRegExp const& begin, QRegExp const& end, 
        QString const& text) const;

    /*!
     *   Finds the port declarations in a vhdl file and creates ports accordingly 
     *   with createPort().
     *
     */
    void parsePorts();

    /*!
     *   Finds the generic declarations in a vhdl file and creates model parameters
     *   accordingly with createGeneric().
     *
     */
    void parseGenerics();

    /*!
     *   Parses the values for vector bounds.
     *
     *      @param [in] rangeDeclaration The range declaration e.g "(32 downto 0)".
     *
     *      @param [out] leftBound The left bound of the vector.
     *
     *      @param [out] rightBound The right bound of the vector.
     */
    void parseBounds(QString const& rangeDeclaration, int& leftBound, int& rightBound) const;

    /*!
     *  Parses the value of a simple equation. The equation may contain literals and
     *  generics but not parathesis.
     *
     *      @param [in] equation The equation to solve.
     *
     *      @return The result of the equation.
     */
    int parseEquation(QString const& equation) const;

    /*!
     *   Creates a port and maps it to corresponding selection info.
     *
     *      @param [in] info The selection info for the created port.
     */
    void createPort(SelectionInfo const& info);

    /*!
     *   Creates a model parameter and maps it to corresponding selection info.
	 *
     *      @param [in] info The selection info for the created generic.
     */
    void createGeneric(SelectionInfo const& info);
    
    /*!
     *   Assigns the generic values used in port declaration for left and right bound
     *   and default value in a port.
	 *
     *      @param [in] port The port to assign to.
     */
    void assignGenerics(QSharedPointer<Port> port);
   
    /*!
     *   Assigns the generic values used in parameter declaration for default value.
	 *
     *      @param [in] param The parameter to assign to.
     */
    void assignGenerics(QSharedPointer<ModelParameter> param);


    /*!
     *   Changes the state of selection from selected to not selected and vice versa.
     *
     *      @param [in] info The selection whose state to change.
     */
    void toggleSelection(SelectionInfo& info);

    /*!
     *   Converts a generic or a number to an integer.
	 *
     *      @param [in] value The string to convert.
	 *
     *      @param [out] ok Flag for successful conversion.
     *
  	 *      @return The value as integer or -1 if value cannot be converted.
     */
    int valueForString(QString const& string, bool& ok) const;
    
    void formatSection(int const pos, int const lenght, QTextCharFormat const& format);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Maps ports to a declaration.
    QMap< SelectionInfo, QList< QSharedPointer<Port> > > ports_;

    //! Maps generics to a declaration.
    QMap< SelectionInfo, QList< QSharedPointer<ModelParameter> > > generics_;

    //! Maps a generic to ports using it.
    QMap< QSharedPointer<ModelParameter>, QList< QSharedPointer<Port> > > genericsInPorts_;

    //! Maps a generic to other generics using it.
    QMap< QSharedPointer<ModelParameter>, QList< QSharedPointer<ModelParameter> > > genericsInGenerics_;

    //! The entity end definition in vhdl.
    QRegExp entityEnd_;

    //! The type declaration definition in vhdl.
    QRegExp typeExp_;

    //! Pattern for equations in default values and vector bounds.
    QRegExp equationExp_;
    
    //! Formatting for text inside entity.
    QTextCharFormat insideEntityFormat_;

    //! Formatting for text outside entity.
    QTextCharFormat outsideEntityFormat_;

    //! Formatting for selected port.
    QTextCharFormat selectedPortFormat_;

    //! Formatting for selected generic.
    QTextCharFormat selectedGenericFormat_;

    //! Formatting for not selected port or generic.
    QTextCharFormat notSelectedFormat_;
};

#endif // VhdlParser_H