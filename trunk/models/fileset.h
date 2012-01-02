/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#ifndef FILESET_H_
#define FILESET_H_

#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QDomNode>
#include <QSharedPointer>
#include <QMap>
#include <QStringList>
#include <QXmlStreamWriter>

class Function;
class FileBuilder;
class File;

/*! \brief Equals the spirit:fileSet element in IP-Xact specification.
 *
 * Contains a list of files and directories associated with a component.
 */
class FileSet {

public:

	/*! \brief The constructor
	 *
	 * \param fileSetNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	FileSet(QDomNode &fileSetNode);

	/*! \brief The constructor
	 *
	 * \param name Name of the file set
	 * \param group A group where this file set belongs to.
	 */
	FileSet(const QString name, const QString group);

	/*! \brief The default constructor
	 *
	 * Constructs a completely empty fileSet. NOTE this file set is not in
	 * a valid condition and must be edited to make it valid.
	*/
	FileSet();

	//! \brief Copy constructor
	FileSet(const FileSet &other);

	//! \brief Assignment operator
	FileSet &operator=(const FileSet &other);

	/*! \brief The destructor
	 *
	 */
	~FileSet();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*
	* Exception guarantee: basic
	* \exception Write_error Occurs if class or one of it's member classes is
	* not valid IP-Xact in the moment of writing.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the File set is in a valid state.
	*
	* \param errorList The list to add the possible error messages to.
	* \param parentIdentifier String from parent to help to identify the location of the error.
	* \param checkChildren If true then the child-items are also checked. If
	* false then only this model is checked.
	*
	* \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, 
		const QString& parentIdentifier, 
		bool checkChildren = true) const;

	/*! \brief Get the default file builders for this file set
	 *
	 * \return QList containing pointers to the file builders.
	 */
	QList<QSharedPointer<FileBuilder> >& getDefaultFileBuilders();

	/*! \brief Get the dependencies for this file set
	 *
	 * \return QList containing paths for the directories containing files
	 * on which this file set depends.
	 */
	const QStringList& getDependencies();

	/*! \brief Get the files associated with this file set
	 *
	 * \return QList containing pointers to the files associated with this
	 * file set.
	 */
	const QList<QSharedPointer<File> >& getFiles();

	/*! \brief Get list of the functions for this file set
	 *
	 * \return QList containing pointers to the functions for this file set.
	 */
	const QList<QSharedPointer<Function> >& getFunctions();

	/*! \brief Get the groups of this file set
	 *
	 * \return QList containing the groups.
	 */
	const QStringList& getGroups();

	/*! \brief Get the name of the file set.
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! \brief Get the displayName of the file set.
	*
	* \return QString containing the displayName.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the file set.
	*
	* \return The description of the file set.
	*/
	QString getDescription() const;

	/*! \brief Set the default file builders for this file set.
	 *
	 * Calling this function will delete old file builders.
	 *
	 * \param defaultFileBuilders QList containing pointers to the new
	 * default file builders.
	 */
	void setDefaultFileBuilders(
			QList<QSharedPointer<FileBuilder> > &defaultFileBuilders);

	/*! \brief Set the dependencies for this file set
	 *
	 * Calling this function will delete all old dependencies.
	 *
	 * \param dependencies QList containing the new dependencies.
	 */
	void setDependencies(const QStringList &dependencies);

	/*! \brief Set the files for this file set
	 *
	 * Calling this function will delete old files.
	 *
	 * \param files QList containing pointers to the new files for this
	 * file set.
	 */
	void setFiles(QList<QSharedPointer<File> > &files);

	/*! \brief Set the functions for this file set
	 *
	 * Calling this function will delete old functions
	 *
	 * \param functions QList containing pointers to the new functions.
	 */
	void setFunctions(QList<QSharedPointer<Function> > &functions);

	/*! \brief Set the groups for this file  set
	 *
	 * Calling this function will delete the old groups
	 *
	 * \param groups QList containing the new group names.
	 */
	void setGroups(const QStringList &groups);

	/*! \brief Set the name for this file set
	 *
	 * \param name QString containing the name.
	 */
	void setName(const QString &name);

	/*! \brief Set the display name for the file set.
	*
	* \param displayName QString containing the displayName.
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Set the description for the file set.
	*
	* \param description QString containing the description.
	*/
	void setDescription(const QString& description);

	const QList<QString> getFilePaths();

	/*! \brief Get the vhdl files and libraries used in this fileSet.
	 *
	 * \return QList containing the file names and library names.
	 *
	 *	If the library name is not defined then the returned library name is set
	 *	If to "work".
	 *
	 * Exception guarantee: basic
	 * \exception Invalid_file Occurs if file type is not defined for a file.
	 */
	QList<General::LibraryFilePair> getVhdlLibraries() const;

	/*! \brief Get the names of the vhdl libraries on this file set.
	 *
	 *
	 * \return QStringList contains the library names.
	*/
	QStringList getVhdlLibraryNames() const;

	/*! \brief Get the verilog files and libraries used in this fileSet.
	 *
	 * \return QStringList containing the verilog files.
	 * 
	 * Exception guarantee: basic
	 * \exception Invalid_file Occurs if file type is not defined for a file.
	*/
	QList<General::LibraryFilePair> getVerilogLibraries() const;

	/*! \brief Add a new file to the file set.
	 *
	 * If a file with a same name already exists within the fileset it is
	 * deleted and replaces by the new file.
	 *
	 * \param file Pointer to the new file to be added.
	 */
	void addFile(File* file);

	/*! \brief Get pointer to the file with given logical name.
	 *
	 * \param logicalName The logical name of the wanted file. If multiple
	 * files with same logical name exist, the first one is returned.
	 *
	 * \return Pointer to the File instance, null pointer if none is found.
	 */
	File* getFile(const QString logicalName) const;

	/*! \brief Get the name elements of the files stored within this file set.
	 *
	 * Note: The file paths returned by this function are relative to the
	 * containing xml-file.
	 *
	 * \return QStringList containing the file names.
	 */
	QStringList getFileNames() const;

	/*! \brief Remove the specified file from the file set.
	 *
	 * \param fileName The name of the file to be removed
	 *
	*/
	void removeFile(const QString& fileName);

	/*! \brief Remove the specified file from the file set.
	 *
	 * \param file Pointer to the file that is to be removed
	 *
	*/
	void removeFile(const File* file);

	/*! \brief Create a new file to the file set.
	 * 
	 * This function constructs an empty file to the file set.
	 *
	 * \return Pointer to the file that was created.
	*/
	File* createFile();

	/*! \brief Create a new file to the file set.
	 *
	 * \param path File path for the file.
	 *
	 * \return File* Pointer to the file that was created.
	*/
	File* createFile(const QString& path);

	/*! \brief Get the default build command's command part for given file.
	 *
	 * \param file Pointer to the file that's command is wanted.
	 *
	 * \return QString containing the command part.
	*/
	QString getDefaultCommand(const File* file) const;

	/*! \brief Get the default build command's flags for given file.
	 *
	 * \param file Pointer ot the file that's default flags are wanted.
	 *
	 * \return QString containing the default flags.
	*/
	QString getDefaultFlags(const File* file) const;

	/*! \brief Get list of the RTL files contained in this file set.
	 *
	 * \return QList containing pointers to the files.
	*/
	QList<QSharedPointer<File> > getRTLFiles() const;

	/*! \brief Does the file set contain file with given name.
	 *
	 * \param fileName The name of the file searched for.
	 *
	 * \return True if file is found.
	*/
	bool contains(const QString& fileName) const;

	/*! \brief Change the name of a file within this file set.
	 * 
	 * If file is not found within file set then nothing is done.
	 * 
	 * \param from QString containing the current file name.
	 * \param to QString containing the file name to set.
	 *
	*/
	void changeFileName(const QString& from, const QString& to);

	/*! \brief Get pointer to the file that contains quartus pinmap
	 *
	 * \return Pointer to the file, if not found then null pointer is returned.
	*/
	File* getQuartusPinMap() const;

	/*! \brief Sort the files in the file set to given order.
	 *
	 * \param fileNames Contains the names of files in the order to sort them into.
	 *
	*/
	void sortFiles(const QStringList& fileNames);

	/*! \brief Add the default file build commands for vhdl files.
	 *
	*/
	void useDefaultVhdlBuilders();

private:

	/*! \brief Contains the name and description information for file set.
	 * 
	 * MANDATORY spirit:name
	 * OPTIONAL spirit:displayName
	 * OPTIONAL spirit:description
	 */
	General::NameGroup nameGroup_;

	/*! \brief Describes the function or purpose of the file set.
	 * OPTIONAL spirit:group
	 */
	QStringList groups_;

	/*! \brief Contains the files.
	 * OPTIONAL spirit:file
	 * Contains pointers to single files or directories associated with the
	 * file set.
	 */
	QList<QSharedPointer<File> > files_;

	/*! \brief Contains the defaultFileBuilders
	 * OPTIONAL spirit:defaultFileBuilder
	 * Specifies the unbounded default build commands for the files within
	 * this file set.
	 */
	QList<QSharedPointer<FileBuilder> > defaultFileBuilders_;

	/*! \brief Contains paths to directories containing dependency files
	 * OPTIONAL spirit:dependency
	 */
	QStringList dependencies_;

	/*! \brief Contains the functions.
	 * OPTIONAL spirit:function
	 * Specifies the unboundeed information about a software function for a
	 * generator.
	 */
	QList<QSharedPointer<Function> > functions_;
};

#endif /* FILESET_H_ */
