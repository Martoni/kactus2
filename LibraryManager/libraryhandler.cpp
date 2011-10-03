/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "libraryhandler.h"
#include "vlnv.h"

#include <mainwindow/mainwindow.h>

// the dialog files
#include <common/dialogs/renameDialog/renamedialog.h>
#include <common/dialogs/createVLNVDialog/createvlnvdialog.h>
#include <common/dialogs/comboSelector/comboselector.h>
#include <common/dialogs/ObjectRemoveDialog/objectremovedialog.h>
#include <common/dialogs/ObjectRemoveDialog/objectremovemodel.h>

// the model files
#include <models/busdefinition.h>
#include <models/component.h>
#include <models/generatorchain.h>
#include <models/abstractiondefinition.h>
#include <models/designconfiguration.h>
#include <models/librarycomponent.h>
#include <models/design.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

// the exception files
#include <exceptions/parse_error.h>
#include <exceptions/invalid_file.h>
#include <exceptions/write_error.h>
#include <exceptions/vhdl_error.h>

#include <QLabel>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QFileDialog>
#include <QTreeView>
#include <QWidget>
#include <QFile>
#include <QDomDocument>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QFileInfo>
#include <QMessageBox>
#include <stdexcept>
#include <QObject>
#include <QCoreApplication>
#include <QSettings>

#include <QDebug>

LibraryHandler::LibraryHandler(VLNVDialer* dialer, MainWindow* parent): 
QTabWidget(parent), data_(), 
treeModel_(),
hierarchyModel_(),
searchWidget_(),
treeWidget_(),
hierarchyWidget_(),
objects_() {

	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	setWindowTitle(tr("LibraryHandler"));

	data_ = QSharedPointer<LibraryData>(new LibraryData(this));
	data_->openLibraryFile();

	treeModel_ = QSharedPointer<LibraryTreeModel>(
			new LibraryTreeModel(this, data_.data(), this));

	hierarchyModel_ = QSharedPointer<HierarchyModel>(
		new HierarchyModel(data_.data(), this, this));

	// create the connections between models and library handler
	syncronizeModels();

	treeWidget_ = QSharedPointer<LibraryTreeWidget>(
			new LibraryTreeWidget(dialer, this, treeModel_.data(), this));

	connect(treeWidget_.data(), SIGNAL(itemSelected(const VLNV&)),
		this, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

	hierarchyWidget_ = QSharedPointer<HierarchyWidget>(
		new HierarchyWidget(dialer, this, this, hierarchyModel_.data()));

	connect(hierarchyWidget_.data(), SIGNAL(componentSelected(const VLNV&)),
		this, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

	addTab(hierarchyWidget_.data(), tr("Hierarchy"));
	addTab(treeWidget_.data(), tr("Plain IP-Xact"));
}

LibraryHandler::~LibraryHandler() {
}

void LibraryHandler::syncronizeModels() {

	// connect the signals from the data model
	connect(data_.data(), SIGNAL(removeVLNV(VLNV*)),
		treeModel_.data(), SLOT(onRemoveVLNV(VLNV*)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(removeVLNV(VLNV*)),
		hierarchyModel_.data(), SLOT(onRemoveVLNV(VLNV*)), Qt::UniqueConnection);

	connect(data_.data(), SIGNAL(resetModel()),
		treeModel_.data(), SLOT(onResetModel()), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(resetModel()),
		hierarchyModel_.data(), SLOT(onResetModel()), Qt::UniqueConnection);

	// signals from data model to library handler
	connect(data_.data(), SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(openDesign(const VLNV&)),
		this, SLOT(onOpenDesign(const VLNV&)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(editItem(const VLNV&)),
		this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(createBusDef(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(createComponent(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(data_.data(), SIGNAL(createDesign(const VLNV&)),
		this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);

	/**************************************************************************/
	// connect the signals from the tree model
	connect(treeModel_.data(), SIGNAL(removeVLNV(QList<VLNV*>)),
		this, SLOT(onRemoveVLNV(QList<VLNV*>)), Qt::UniqueConnection);

	// signals from tree model to library handler
	connect(treeModel_.data(), SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(openDesign(const VLNV&)),
		this, SLOT(onOpenDesign(const VLNV&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(editItem(const VLNV&)),
		this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(createBus(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(createAbsDef(const VLNV&)),
		this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(createComponent(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(createDesign(const VLNV&)),
		this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(exportItems(const QList<VLNV*>&)),
		this, SLOT(onExportItems(const QList<VLNV*>&)), Qt::UniqueConnection);
	connect(treeModel_.data(), SIGNAL(refreshDialer()),
		this, SIGNAL(refreshDialer()), Qt::UniqueConnection);

	/*************************************************************************/
	// connect the signals from the hierarchy model

	// signals from hierarchy model to library handler
	connect(hierarchyModel_.data(), SIGNAL(openDesign(const VLNV&)),
		this, SLOT(onOpenDesign(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(editItem(const VLNV&)),
		this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(createBusDef(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(createComponent(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(createBus(const VLNV&)),
		this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(createAbsDef(const VLNV&)),
		this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(createDesign(const VLNV&)),
		this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);
	connect(hierarchyModel_.data(), SIGNAL(exportItem(const VLNV&)),
		this, SLOT(onExportItem(const VLNV&)), Qt::UniqueConnection);
}

void LibraryHandler::onExportItem( const VLNV& vlnv ) {
	
	// invalid vlnvs are not exported
	if (!vlnv.isValid())
		return;

	// get the current working directory and save it to be restored in the end
	// of the function
	QDir savedWorkingDirectory = QDir::current();

	QSettings settings;
	QString defaultPath = settings.value("general/defaultProjDir", QDir::homePath()).toString();

	// ask the target directory where the package is to be exported
	QString targetPath = QFileDialog::getExistingDirectory(this, 
		tr("Select the location to export library to"),
		defaultPath);

	// if no target path was specified
	if (targetPath.isEmpty())
		return;

	// create a QDir instance of the target directory
	QDir target(targetPath);

	// The user hasn't chosen "yes to all" or "no to all" yet
	bool yesToAll = false;
	bool noToAll = false;
	// info of copied files is stored so same file is not copied multiple times
	fileList handledFiles;
	copyFiles(target, vlnv, handledFiles, yesToAll, noToAll);

	QDir::setCurrent(savedWorkingDirectory.absolutePath());
	return;
}

void LibraryHandler::onExportItems(const QList<VLNV*>& vlnvs) {
	
	// if no VLNVs to export
	if (vlnvs.isEmpty())
		return;

	// get the current working directory and save it to be restored in the end
	// of the function
	QDir savedWorkingDirectory = QDir::current();

	QSettings settings;
	QString defaultPath = settings.value("general/defaultProjDir", QDir::homePath()).toString();

	// ask the target directory where the package is to be exported
	QString targetPath = QFileDialog::getExistingDirectory(this, 
		tr("Select the location to export library to"),
		defaultPath);

	// if no target path was specified
	if (targetPath.isEmpty())
		return;

	// create a QDir instance of the target directory
	QDir target(targetPath);

	// The user hasn't chosen "yes to all" or "no to all" yet
	bool yesToAll = false;
	bool noToAll = false;

	// info of copied files is stored so same file is not copied multiple times
	fileList handledFiles;

	foreach (VLNV* vlnv, vlnvs) {

		// invalid vlnvs are not exported
		if (!vlnv->isValid())
			continue;

		// copy the files to the specified location from each vlnv
		copyFiles(target, *vlnv, handledFiles, yesToAll, noToAll);
	}

	QDir::setCurrent(savedWorkingDirectory.absolutePath());
	return;
}

void LibraryHandler::copyFiles(QDir& target, const VLNV& vlnv, 
fileList& handledFiles, bool& yesToAll, bool& noToAll) {

	// parse the vlnv to get the dependencies and also copy them
	QSharedPointer<LibraryComponent> item = getModel(vlnv);

	// if pointer is null the vlnv did not exist or the document was not
	// of supported type
	if (!item) {	
		return;
	}

	// save the current working directory to be restored later
	QDir savedWorkingDir = QDir::current();

	QDir::setCurrent(target.absolutePath());
	QDir vlnvTarget;

	// get the path that matches the VLNV structure
	QString dirPath = vlnv.createDirPath();

	// and create the path to the target directory
	if (!vlnvTarget.mkpath(dirPath)) {
		emit errorMessage(tr(
			"Could not create directory structure, aborting."));

		// restore the previous working directory to avoid messing with
		// caller function
		QDir::setCurrent(savedWorkingDir.absolutePath());
		return;
	}
	// set the directory to the place to copy the vlnv file to
	vlnvTarget.setPath(dirPath);

	QString documentPath = data_->getPath(vlnv);
	if (documentPath.isNull()) {

		// restore the previous working directory to avoid messing with
		// caller function
		QDir::setCurrent(savedWorkingDir.absolutePath());
		return;
	}

	// contains the info of the source file
	QFileInfo sourceFileInfo(documentPath);
	// points to the source directory where all relative paths must be
	// compared to
	QDir sourceDir(sourceFileInfo.absoluteDir());

	// copy the file from source to destination
	copyFile(sourceFileInfo, vlnvTarget, handledFiles, yesToAll, noToAll);

	// get the list of the dependency files for this vlnv
	QStringList fileList = item->getDependentFiles();
	// copy the files
	copyFiles(fileList, sourceDir, vlnvTarget, handledFiles, yesToAll, noToAll);

	// get the vlnvs that are needed by this current vlnv and also copy them
	QList<VLNV> vlnvList = item->getDependentVLNVs();

	for (int i = 0; i < vlnvList.size(); ++i) {
		copyFiles(target, vlnvList.at(i), handledFiles, yesToAll, noToAll);
	}

	// restore the previous working directory to avoid messing with
	// caller function
	QDir::setCurrent(savedWorkingDir.absolutePath());
	return;
}

void LibraryHandler::copyFile(QFileInfo& source, QDir& target,
fileList& handledFiles, bool& yesToAll, bool& noToAll) {

	// if the source file has already been handled previously within
	// this copy operation
	if (handledFiles.contains(source)) {
		return;
	}

	// if the source doesn't exist
	if (!source.exists()) {
		emit errorMessage(tr("Could not find file: %1").arg(source.fileName()));
		return;
	}

	// save the current directory to be restored at the end of the function
	QDir savedCurrentDir = QDir::current();

	// set the current working directory to the destination
	QDir::setCurrent(target.absolutePath());

	// file existed so add it to handledFiles list to indicate that
	// we don't need to check later anymore in possible recursive
	// function calls
	handledFiles.append(source);

	// set QFile instance of the source file
	QFile sourceFile(source.filePath());

	// if a file with same name as source already exists in the
	// target directory
	if (QFile::exists(source.fileName())) {

		QMessageBox::StandardButton answer = QMessageBox::Yes;

		// if no to all has been clicked, return false and dont copy
		if (noToAll) {
			answer = QMessageBox::No;
		}
		// if no "yes to all" or "no to all" has been clicked then ask user
		// what to do
		else if (!yesToAll && !noToAll) {
			QString title = tr("overwrite file?");
			QString text = tr("The file ");
			text += source.fileName();
			text += tr(" already exists, would you like to overwrite"
				" the file?");

			// ask the user to overwrite the file, if user says no we
			// quit the function and return false

			answer = QMessageBox::question(
				this, title, text, QMessageBox::Yes | QMessageBox::No |
				QMessageBox::YesToAll | QMessageBox::NoToAll ,
				QMessageBox::No);
		}

		// if user says "no" to overwrite
		if ( answer	== QMessageBox::No) {
			// the user said no to overwrite so return

			// restore the current directory to the state it was before this
			// function
			QDir::setCurrent(savedCurrentDir.absolutePath());
			return;
		}
		// if user clicked "yes to all"
		else if ( answer == QMessageBox::YesToAll ) {
			yesToAll = true;
			noToAll = false;
		}
		// if user clicked "no to all"
		else if ( answer == QMessageBox::NoToAll ) {
			yesToAll = false;
			noToAll = true;

			// restore the current directory to the state it was before this function
			QDir::setCurrent(savedCurrentDir.absolutePath());
			return;
		}

		// if answer was "yes" or "yes to all" we remove the old file to
		// copy the new in its place
		if (answer == QMessageBox::Yes || answer == QMessageBox::YesToAll) {
			QFile target(source.fileName());

			// if a file with same name already exists it is first removed
			// if user wanted to overwrite
			target.remove();
		}
	}
	// if the copying of the file can not be done
	if (!sourceFile.copy(source.fileName())) {
		emit errorMessage(tr("File %1 couldn't be copied").arg(source.fileName()));
	}

	// restore the current directory to the state it was before this function
	QDir::setCurrent(savedCurrentDir.absolutePath());
	return;
}

void LibraryHandler::copyFiles(const QStringList& files, QDir& sourceDir,
QDir& targetDir, fileList& handledFiles, bool& yesToAll, bool& noToAll) {

	// save the current directory to be restored later
	QDir savedCurrentDir = QDir::current();

	// set the target as current directory
	QDir::setCurrent(targetDir.absolutePath());

	for (int i = 0; i < files.size(); ++i) {

		QFileInfo sourceInfo(sourceDir.absoluteFilePath(files.at(i)));

		// if the file can't be found, find the next file
		if (!sourceInfo.exists()) {
			continue;
		}

		QFileInfo targetInfo(files.at(i));

		// create directories to store the file in a correct relative path
		if (!QDir::current().mkpath(targetInfo.absolutePath())) {
			emit errorMessage(tr("Could not create directory structure for file %1").arg(
				targetInfo.fileName()));
		}

		// set the target directory to copy the file into
		QDir targetDir(targetInfo.absolutePath());

		copyFile(sourceInfo, targetDir, handledFiles, yesToAll, noToAll);
	}

	// set the current directory to the state it was before this function
	QDir::setCurrent(savedCurrentDir.absolutePath());
	return;
}

void LibraryHandler::importLibraryFile( const QString filePath /*= QString()*/ ) {
	data_->openLibraryFile(filePath);
}

void LibraryHandler::exportLibraryFile( const QString filePath /*= QString()*/ ) {
	data_->saveLibraryFile(filePath);
}

QSharedPointer<LibraryComponent> LibraryHandler::getModel( const VLNV& vlnv ) {

	// if object has already been previously parsed
	if (objects_.contains(vlnv)) {
		return objects_.value(vlnv);
	}
	else {
		if (!data_->contains(vlnv)) {
			emit noticeMessage(tr("VLNV %1:%2:%3:%4 was not found in library").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion()));
			return QSharedPointer<LibraryComponent>();
		}
		
		VLNV toCreate = vlnv;
		// make sure the vlnv is of correct type
		toCreate.setType(data_->getType(vlnv));

		// get path to the document
		QString path = data_->getPath(toCreate);

		// if the file was not found
		if (path.isEmpty()) {
			return QSharedPointer<LibraryComponent>();
		}

		// create file handle and use it to read the IP-Xact document into memory
		QFile file(path);
		QDomDocument doc;
		doc.setContent(&file);
		file.close();

		QSharedPointer<LibraryComponent> libComp;

		try {
			// create correct type of object and cast the pointer
			switch (toCreate.getType()) {
			case VLNV::BUSDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new BusDefinition(doc));
				break;
									  }
			case VLNV::COMPONENT: {
				libComp = QSharedPointer<LibraryComponent>(new Component(doc));
				break;
								  }
			case VLNV::DESIGN: {
				libComp = QSharedPointer<LibraryComponent>(new Design(doc));
				break;
							   }

			case VLNV::GENERATORCHAIN: {
				libComp = QSharedPointer<LibraryComponent>(new GeneratorChain(doc));
				break;
									   }
			case VLNV::DESIGNCONFIGURATION: {
				libComp = QSharedPointer<LibraryComponent>(new DesignConfiguration(doc));
				break;
											}

			case VLNV::ABSTRACTIONDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new AbstractionDefinition(doc));
				break;
											  }
			default: {
				emit noticeMessage(tr("Document was not supported type"));
				return QSharedPointer<LibraryComponent>();
					 }
			}
		}
		// if an exception occurred during the parsing
		catch (Parse_error error) {
			QString errorMsg(error.what() + QString(" ") + error.errorMsg() +
				tr(" within file: %1").arg(path));
			emit errorMessage(errorMsg);
			return QSharedPointer<LibraryComponent>();
		}
		catch (...) {
			emit errorMessage(
				tr("Error occurred during parsing of the document %1").arg(path));
			return QSharedPointer<LibraryComponent>();
		}
		
		// save the parsed item to the map and return pointer to it
		objects_.insert(vlnv, libComp);
		return libComp;
	}
}

bool LibraryHandler::registerVLNV( const VLNV& vlnv, const QString& path ) {

	return data_->addVLNV(vlnv, path);

}

bool LibraryHandler::contains( const VLNV& vlnv ) {
	return data_->contains(vlnv);
}

const QString LibraryHandler::getPath( const VLNV& vlnv ) const {
	if (!vlnv.isValid())
		return QString();
	else
		return data_->getPath(vlnv);
}

void LibraryHandler::writeModelToFile( const QString path, 
									  QSharedPointer<LibraryComponent> model ) {
	// if the given file path is not valid
	if (path.isEmpty()) {
		return;
	}

	VLNV vlnv = *model->getVlnv();
	Q_ASSERT(!data_->contains(vlnv));

    // Create the path if it does not exist.
    if (!QDir(path).exists())
    {
        if (!QDir().mkpath(path))
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("Could not create directory \"%1\".").arg(path));
            msgBox.exec();
            return;
        }
    }

	QString filePath = path + "/" + vlnv.getName() + "." + vlnv.getVersion() + ".xml";

	// create file info instance representing the file in the path
	QFileInfo oldFileInfo;
	oldFileInfo.setFile(filePath);

	// remove the old file
	if (oldFileInfo.exists()) {
		QFile oldFile(oldFileInfo.filePath());
		oldFile.remove();
	}

	// create a new file
	QFile newFile(filePath);
	if (!newFile.open(QFile::WriteOnly | QFile::Truncate)) {
		emit errorMessage(tr("Could not open file %1 for writing").arg(filePath));
		return;
	}

	// write the parsed model
	try {
		model->write(newFile);
	}
	catch (Write_error error) {
		QString errorMsg = QString("%1 %2 within file: %3").arg(
			error.what()).arg(
			error.errorMsg()).arg(
			newFile.fileName());
		emit errorMessage(errorMsg);

		// close the file
		newFile.close();
		return;
	}

    // close the file
    newFile.close();

	// tell library to register the vlnv
	data_->addVLNV(vlnv, filePath);
}

void LibraryHandler::writeModelToFile( QSharedPointer<LibraryComponent> model ) {
	
	Q_ASSERT(data_->contains(*model->getVlnv()));

	// make sure the object is parsed again next time
	VLNV objectVLNV = *model->getVlnv();
	objects_.remove(objectVLNV);

	QString filePath = data_->getPath(*model->getVlnv());

	// create file info instance representing the file in the path
	QFileInfo oldFileInfo;
	oldFileInfo.setFile(filePath);

	// remove the old file
	if (oldFileInfo.exists()) {
		QFile oldFile(oldFileInfo.filePath());
		oldFile.remove();
	}

	// create a new file
	QFile newFile(filePath);
	if (!newFile.open(QFile::WriteOnly | QFile::Truncate)) {
		emit errorMessage(tr("Could not open file %1 for writing").arg(filePath));
		return;
	}

	// write the parsed model
	try {
		model->write(newFile);
	}
	catch (Write_error error) {
		QString errorMsg = QString("%1 %2 within file: %3").arg(
			error.what()).arg(
			error.errorMsg()).arg(
			newFile.fileName());
		emit errorMessage(errorMsg);
		
		// close the file
		newFile.close();
		return;
	}

	// close the file
	newFile.close();

	onItemSaved(*model->getVlnv());
}


void LibraryHandler::searchForIPXactFiles() {

	QSettings settings;
	QString defaultPath = settings.value("general/defaultProjDir", QDir::homePath()).toString();

	QString path = QFileDialog::getExistingDirectory(this, 
		tr("Select a directory to start searching for IP-Xact files"),
		defaultPath, QFileDialog::ReadOnly);

    if (!path.isEmpty())
    {
    	data_->searchForFiles(path);
    }
}

void LibraryHandler::onCheckLibraryIntegrity() {
	data_->checkIntegrity();
}

void LibraryHandler::getNeededVLNVs( const VLNV& vlnv, QList<VLNV>& list ) {
	// if invalid vlnv
	if (!vlnv.isValid())
		return;

	// get the vlnvs needed by this vlnv
	QSharedPointer<LibraryComponent> libComponent = getModel(vlnv);

	// if there was no match for the vlnv
	if (!libComponent) {
		emit errorMessage(tr("No item with following info was found in library: \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4").arg(vlnv.getVendor()).arg(vlnv.getLibrary()).arg(
			vlnv.getName()).arg(vlnv.getVersion()));
		return;
	}

	QList<VLNV> tempList = libComponent->getDependentVLNVs();

	// this document is searched so add it to the list
	list.append(vlnv);

	// search all documents that are not yet added to the list
	for (int i = 0; i < tempList.size(); ++i) {

		// if the vlnv is not yet added to the list
		if (!list.contains(tempList.at(i))) {

			// search recursively for vlnvs
			getNeededVLNVs(tempList.at(i), list);
		}
	}
}

void LibraryHandler::getDependencyFiles( const VLNV& vlnv, QStringList& list ) {

	// create the model of the vlnv
	QSharedPointer<LibraryComponent> libComponent = getModel(vlnv);
	QStringList ownList = libComponent->getDependentFiles();

	// go through all files and convert the relative file path to an absolute
	// one to make the file accessible in the caller function
	for (int i = 0; i < ownList.size(); ++i) {

		// get path to the IP-Xact document that is used as base for relative
		// file paths
		QString ipPath = data_->getPath(vlnv);

		QFileInfo ipInfo(ipPath);

		// if the file can't be found print error to the user
		if (!ipInfo.exists()) {
			emit errorMessage(tr("File %1 can't be found in file system").arg(
				ipInfo.filePath()));

			// and continue to next file
			continue;
		}

		QDir ipDir(ipInfo.absolutePath());

		// path to the single dependecy file
		QString fullFilePath = ipDir.absoluteFilePath(ownList.at(i));

		// make sure the file exists
		QFileInfo fileInfo(fullFilePath);
		if (fileInfo.exists() && !list.contains(fileInfo.canonicalFilePath())) {

			// if file existed and it wasn't yet on the list, add it to the
			// list of files
			list.append(fileInfo.canonicalFilePath());
		}
	}
}

LibraryItem const* LibraryHandler::getTreeRoot() const {
	
	return treeModel_->getRoot();
}

void LibraryHandler::onSelectionChanged( const VLNV& vlnv ) {
	// if vlnv was invalid
	if (!vlnv.isValid())
		return;

	treeWidget_->selectItem(vlnv);
	//hierarchyWidget_->selectItems(vlnv);
	return;
}

void LibraryHandler::onClearSelection() {

	// tell tree widget to select an invalid vlnv
	treeWidget_->selectItem(VLNV());
}

void LibraryHandler::onUpdateObject( const QSharedPointer<LibraryComponent> object,
									const QString& path) {
	
	// get the current vlnv of the object
	VLNV vlnv = *object->getVlnv();

	data_->updateVLNV(path, vlnv);
	
	// after updating the vlnv nothing is selected in the library view so
	// select again the same object that was updated
	onSelectionChanged(vlnv);
}

VLNV::IPXactType LibraryHandler::getDocumentType(const VLNV& vlnv) {

	if (!vlnv.isValid())
		return VLNV::INVALID;

	return data_->getType(vlnv);
}

void LibraryHandler::onEditItem( const VLNV& vlnv ) {

	if (!vlnv.isValid())
		return;

	// if vlnv is for bus
	switch (data_->getType(vlnv)) {
		case VLNV::COMPONENT: {
			QSharedPointer<LibraryComponent> libComp = getModel(vlnv);
			QSharedPointer<Component> component = libComp.staticCast<Component>();

			switch (component->getComponentImplementation()) {
		case KactusAttribute::KTS_SYS: {
			emit openSystem(vlnv);
			break;
									   }
		case KactusAttribute::KTS_SW: {

			// TODO remove this when mapping can be opened
			if (component->getComponentSWType() == KactusAttribute::KTS_SW_MAPPING)
				return;

			emit openComponent(vlnv);
			break;
									  }
		default: {
			emit openComponent(vlnv);
			break;
				 }
			}
			return;
							  }
		case VLNV::BUSDEFINITION: {
						
			// find the child items of the bus definition
			QList<VLNV> absDefVLNVs;
			hierarchyModel_->getChildren(absDefVLNVs, vlnv);
			
			// if theres exactly one abstraction definition per bus definition
			if (absDefVLNVs.size() == 1) {
				emit openBus(vlnv, absDefVLNVs.first(), false);
			}

			// if the abstraction definition can not be identified by the bus definition.
			else
				emit openBus(vlnv, VLNV(), false);
			return;
								  }
		case VLNV::ABSTRACTIONDEFINITION: {

			QSharedPointer<LibraryComponent> libComp = getModel(vlnv);
			QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

			VLNV busDefVLNV = absDef->getBusType();

			if (!data_->contains(busDefVLNV)) {
				QString message(tr("Library does not contain bus definition " 
					"%1:%2:%3:%4").arg(busDefVLNV.getVendor()).arg(
					busDefVLNV.getLibrary()).arg(
					busDefVLNV.getName()).arg(
					busDefVLNV.getVersion()));
				message += tr(" required by abstraction definition %1:%2:%3:%4").arg(
					vlnv.getVendor()).arg(
					vlnv.getLibrary()).arg(
					vlnv.getName()).arg(
					vlnv.getVersion());
				emit errorMessage(message);
				return;
			}
			
			// find the child items of the bus definition
			QList<VLNV> absDefVLNVs;
			hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

			// if theres exactly one abstraction definition per bus definition
			if (absDefVLNVs.size() == 1 && absDefVLNVs.first() == vlnv) {
				emit openBus(busDefVLNV, vlnv, false);
			}

			// if there are multiple abstraction definitions for that one bus def
			else {
				emit openBus(busDefVLNV, vlnv, true);
			}
			return;
										  }
		default: {
			emit noticeMessage(tr("Item can not be edited."));
			return;
				 }
	}	
}

void LibraryHandler::onOpenDesign( const VLNV& vlnv ) {
	QSharedPointer<LibraryComponent> libComb = getModel(vlnv);
	if (!libComb) {
		emit errorMessage(tr("Component was not found"));
		return;
	}

	// make type cast
	QSharedPointer<Component> component;
	if (libComb->getVlnv()->getType() == VLNV::COMPONENT) {
		QSharedPointer<Component> component = libComb.staticCast<Component>();

		QStringList views = component->getHierViews();

		// if component does not have any views
		if (views.size() == 0) {
			return;
		}
		emit openDesign(vlnv, views.first());
	}
}

void LibraryHandler::onCreateNewItem( const VLNV& vlnv ) {

	bool showAttributes = true;
	if (vlnv.getType() != VLNV::COMPONENT)
		showAttributes = false;

    NewObjectDialog newDesignDialog(this, vlnv.getType(), showAttributes, this);

	if (contains(vlnv) && getDocumentType(vlnv) == VLNV::COMPONENT) {
        QSharedPointer<LibraryComponent> libComp = getModel(vlnv);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

        KactusAttribute::ProductHierarchy prodHier = comp->getComponentHierarchy();
        KactusAttribute::Firmness firmness = comp->getComponentFirmness();

        newDesignDialog.setKactusAttributes(prodHier, firmness);
    }

    newDesignDialog.setVLNV(vlnv);
	
	switch (vlnv.getType()) {
		case VLNV::ABSTRACTIONDEFINITION:
		case VLNV::BUSDEFINITION: {
			newDesignDialog.setWindowTitle("Set new bus VLNV");
			break;
								  }
		case VLNV::COMPONENT: {
			newDesignDialog.setWindowTitle("Set new component VLNV");
			break;
							  }
	}

	newDesignDialog.exec();

	// if user clicked cancel
	if (newDesignDialog.result() == QDialog::Rejected) {
		return;
	}

	VLNV newVlnv = newDesignDialog.getVLNV();
	QString directory = newDesignDialog.getPath();
	QString filePath = directory + "/" + newVlnv.getName() + 
		"." + newVlnv.getVersion() + ".xml";

	QSharedPointer<LibraryComponent> libComp;

	// create an object of correct type
	switch (vlnv.getType()) {
		case VLNV::ABSTRACTIONDEFINITION:
		case VLNV::BUSDEFINITION: {
			emit createBus(newVlnv, directory);
			return;
								  }
		case VLNV::COMPONENT: {
			emit createComponent(newDesignDialog.getProductHierarchy(),
                                 newDesignDialog.getFirmness(),
                                 newVlnv, directory);
			return;
							  }
		default: {
			emit noticeMessage(tr("The item type is not supported"));
			return;
				 }
	}
}

void LibraryHandler::onCreateAbsDef( const VLNV& busDefVLNV ) {
	if (!busDefVLNV.isValid())
		return;
	
	Q_ASSERT(getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION);
	Q_ASSERT(data_->contains(busDefVLNV));

	QString busDefPath = data_->getPath(busDefVLNV);
	QFileInfo busDefInfo(busDefPath);
	QString directory = busDefInfo.absolutePath();

	// find the child items of the bus definition
	QList<VLNV> absDefVLNVs;
	hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

	// if theres no previous abstraction definition for given bus def
	if (absDefVLNVs.isEmpty()) {
		emit createAbsDef(busDefVLNV, directory, false);
	}

	// if the abstraction definition can not be identified by the bus definition.
	else
		emit createAbsDef(busDefVLNV, directory, true);
}

void LibraryHandler::onCreateDesign( const VLNV& vlnv ) {
	emit openDesign(vlnv, QString());
}

void LibraryHandler::updatePath( const VLNV& vlnv, const QString& path ) {
	data_->updatePath(vlnv, path);
}

void LibraryHandler::updateComponentFiles( const VLNV& oldComponent, 
										  const VLNV& newComponent ) {

	if (!data_->contains(oldComponent) || !data_->contains(newComponent)) {
		emit errorMessage(tr("Component was not found within library"));
		return;
	}
	else if (data_->getType(oldComponent) != VLNV::COMPONENT ||
		data_->getType(newComponent) != VLNV::COMPONENT) {
			emit errorMessage(tr("Given VLNV was not component"));
			return;
	}

	// parse the components
	QSharedPointer<LibraryComponent> old = getModel(oldComponent);
	QSharedPointer<Component> oldComp = old.staticCast<Component>();

	QSharedPointer<LibraryComponent> newC = getModel(newComponent);
	QSharedPointer<Component> newComp = newC.staticCast<Component>();

	// get the paths to the xml files
	QFileInfo sourceInfo(data_->getPath(oldComponent));
	QString sourcePath = sourceInfo.absolutePath();
	QFileInfo targetInfo(data_->getPath(newComponent));
	QString targetPath = targetInfo.absolutePath();

	// update the file paths and copy necessary files
	newComp->updateFiles(*oldComp, sourcePath, targetPath);

	// write the new component to file system
	writeModelToFile(newComp);
}

VLNV* LibraryHandler::getOriginalPointer( const VLNV& vlnv ) const {
	return data_->getOriginalPointer(vlnv);
}

int LibraryHandler::referenceCount( const VLNV& vlnv ) const {
	QList<VLNV> list;
	hierarchyModel_->getOwners(list, vlnv);
	return list.size();
}

int LibraryHandler::getOwners( QList<VLNV>& list, const VLNV& vlnvToSearch ) const {
	hierarchyModel_->getOwners(list, vlnvToSearch);
	return list.size();
}

void LibraryHandler::removeObject( const VLNV& vlnv ) {

	if (!vlnv.isValid())
		return;
	else if (!data_->contains(vlnv))
		return;

	objects_.remove(vlnv);

	QString vlnvPath = data_->getPath(vlnv);

	// tell each model to remove the object
	VLNV* vlnvPointer = data_->getOriginalPointer(vlnv);
	treeModel_->onRemoveVLNV(vlnvPointer);
	hierarchyModel_->onRemoveVLNV(vlnvPointer);
	data_->onRemoveVLNV(vlnvPointer);

	QFile xmlFile(vlnvPath);
	if (xmlFile.exists()) {
	
		if (!xmlFile.remove()) {
			emit errorMessage(tr("Could not remove file %1").arg(vlnvPath));
		}
	}
	else {
		emit errorMessage(tr("File %1 was not found in file system").arg(vlnvPath));
	}
}

void LibraryHandler::removeObjects( const QList<VLNV>& vlnvList ) {

	QList<VLNV*> originalPointers;
	foreach (VLNV vlnv, vlnvList) {
		VLNV* vlnvP = data_->getOriginalPointer(vlnv);
		if (vlnvP)
			originalPointers.append(vlnvP);
	}

	if (!originalPointers.isEmpty())
		onRemoveVLNV(originalPointers);
}

void LibraryHandler::onRemoveVLNV( QList<VLNV*> vlnvs ) {

	// create the dialog to select which items to remove
	ObjectRemoveDialog removeDialog(this);

	QStringList vlnvPaths;

	foreach (VLNV* vlnv, vlnvs) {

		if (!vlnv)
			continue;

		// if the vlnv is not found in the library
		if (!data_->contains(*vlnv)) {
			objects_.remove(*vlnv);
			continue;
		}

		// path to the given vlnv file
		vlnvPaths.append(data_->getPath(*vlnv));

		// the vlnv and it's file to the dialog
		removeDialog.createItem(*vlnv, true);

		switch (data_->getType(*vlnv)) {
		case VLNV::COMPONENT: {
			QSharedPointer<LibraryComponent> libComp = getModel(*vlnv);
			QSharedPointer<Component> component = libComp.staticCast<Component>();

			// ask the component for all it's hierarchical references
			QList<VLNV> hierRefs = component->getHierRefs();
			foreach (VLNV ref, hierRefs) {

				// if the vlnv is not found in library
				if (!data_->contains(ref))
					continue;

				switch (data_->getType(ref)) {
					case VLNV::DESIGNCONFIGURATION: {
						if (data_->contains(ref)) {

							QSharedPointer<LibraryComponent> libDes = getModel(ref);
							QSharedPointer<DesignConfiguration> desConf = libDes.staticCast<DesignConfiguration>();

							removeDialog.createItem(ref);

							// if library contains the design reference then add it to the dialog list.
							VLNV designVLNV = desConf->getDesignRef();
							if (data_->contains(designVLNV)) {
								removeDialog.createItem(designVLNV);
							}
						}
						break;
													}
					case VLNV::DESIGN: {
						if (data_->contains(ref)) {
							removeDialog.createItem(ref);
						}
						break;
									   }
					default:
						continue;
				}
			}

			// ask the component for all it's file references
			QStringList files = component->getFiles();
			QString compPath = data_->getPath(*vlnv);
			foreach (QString relPath, files) {
				QString absPath = General::getAbsolutePath(compPath, relPath);
				if (!absPath.isEmpty())
					removeDialog.createItem(absPath);
			}
			break;
							  }
		case VLNV::DESIGNCONFIGURATION: {
			QSharedPointer<LibraryComponent> libComp = getModel(*vlnv);
			QSharedPointer<DesignConfiguration> desConf = libComp.staticCast<DesignConfiguration>();

			// if library contains the design reference then add it to the dialog list.
			VLNV designVLNV = desConf->getDesignRef();
			if (data_->contains(designVLNV)) {
				removeDialog.createItem(designVLNV);
			}
			break;
										}
		case VLNV::BUSDEFINITION: {
			
			// find the child items of the bus definition
			QList<VLNV> absDefVLNVs;
			hierarchyModel_->getChildren(absDefVLNVs, *vlnv);

			// if bus definition is removed then ask to remove all it's 
			// abstraction definitions also.
			foreach (VLNV absDefVLNV, absDefVLNVs) {
				// if abstraction definition exists in the library
				if (data_->contains(absDefVLNV)) {

					// add the abstraction definition to the dialog list
					removeDialog.createItem(absDefVLNV);
				}
			}
			
			break;
								  }
		case VLNV::ABSTRACTIONDEFINITION: {

			// parse the abstraction definition
			QSharedPointer<LibraryComponent> libComp = getModel(*vlnv);
			QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

			// find abs def's bus definition
			VLNV busDefVLNV = absDef->getBusType();
			
			// if bus definition is not found
			if (!data_->contains(busDefVLNV))
				break;

			// find the child items of the bus definition
			QList<VLNV> absDefVLNVs;
			hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

			// if theres only this abs def for the bus def
			if (absDefVLNVs.size() == 1 && absDefVLNVs.first() == *vlnv)
				// also add the bus definition to the list of removable items
				removeDialog.createItem(busDefVLNV);

			break;
										  }
		default: {
			break;
				 }
		}
	}

	int result = removeDialog.exec();
	
	// if user clicked cancel
	if (result == QDialog::Rejected)
		return;

	QList<ObjectRemoveModel::Item> itemsToRemove = removeDialog.getItemsToRemove();
	foreach (ObjectRemoveModel::Item item, itemsToRemove) {
		
		// if this is vlnv object then remove it from library
		if (item.type_ == ObjectRemoveModel::VLNVOBJECT) {

			// remove the item from the parsed objects
			objects_.remove(item.vlnv_);

			// get the path to the xml-file
			QString objectPath = data_->getPath(item.vlnv_);

			// tell each model to remove the object
			VLNV* vlnvPointer = data_->getOriginalPointer(item.vlnv_);
			treeModel_->onRemoveVLNV(vlnvPointer);
			hierarchyModel_->onRemoveVLNV(vlnvPointer);
			data_->onRemoveVLNV(vlnvPointer);

			QFile xmlFile(objectPath);
			if (!xmlFile.remove())
				emit errorMessage(tr("File %1 could not be removed from file system.").arg(
				objectPath));
		}
		// if this is file then remove it from file system
		else {
			QFileInfo fileInfo(item.path_);
			if (fileInfo.exists()) {

				// remove the file specified by path
				QFile file(item.path_);
				if (!file.remove())
					emit errorMessage(tr("File %1 could not be removed from file system.").arg(
					item.path_));

				// check if the directory contains other files
				QDir dir = fileInfo.absoluteDir();
				dir.rmdir(fileInfo.absolutePath());
			}
		}
	}

	// remove the directory that contains the object name if the directory is empty
	foreach (QString vlnvPath, vlnvPaths) {
		QFileInfo vlnvInfo(vlnvPath);
		QDir vlnvDir(vlnvInfo.absoluteDir());
		if (vlnvDir.rmdir(vlnvDir.absolutePath())) {
			vlnvDir.cdUp();
			vlnvDir.rmdir(vlnvDir.absolutePath());
		}
	}
}

void LibraryHandler::onItemSaved( const VLNV& vlnv ) {
	if (!vlnv.isValid())
		return;

	VLNV savedItem = vlnv;
	savedItem.setType(data_->getType(vlnv));

	// make sure the model is parsed again next time to make all changes available
	objects_.remove(vlnv);

	switch (savedItem.getType()) {
		case VLNV::COMPONENT: {
			hierarchyModel_->onComponentSaved(savedItem);
			return;
							  }
		default: 
			return;
	}
}

VLNV LibraryHandler::getDesignVLNV( const VLNV& hierarchyRef ) {
	if (!data_->contains(hierarchyRef)) {
		emit errorMessage(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
			hierarchyRef.getVendor()).arg(
			hierarchyRef.getLibrary()).arg(
			hierarchyRef.getName()).arg(
			hierarchyRef.getVersion()));
		return VLNV();
	}

	QSharedPointer<LibraryComponent> libComp = getModel(hierarchyRef);

	// if the hierarchical reference was for design configuration
	if (data_->getType(hierarchyRef) == VLNV::DESIGNCONFIGURATION) {
		QSharedPointer<DesignConfiguration> desConf = libComp.staticCast<DesignConfiguration>();

		VLNV designVLNV = desConf->getDesignRef();
		// if design vlnv was not found in library
		if (!data_->contains(designVLNV)) {
			QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
				designVLNV.getVendor()).arg(designVLNV.getLibrary()).arg(designVLNV.getName()).arg(
				designVLNV.getVersion()));
			emit errorMessage(errorMsg);
			return VLNV();
		}
		// if the vlnv is not for design
		else if (data_->getType(designVLNV) != VLNV::DESIGN) {
			QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not for design.").arg(
				designVLNV.getVendor()).arg(designVLNV.getLibrary()).arg(designVLNV.getName()).arg(
				designVLNV.getVersion()));
			emit errorMessage(errorMsg);
			return VLNV();
		}
		designVLNV.setType(VLNV::DESIGN);
		return designVLNV;
	}

	// if the hierarchical reference was directly for a design
	else if (data_->getType(hierarchyRef) == VLNV::DESIGN) {
		return hierarchyRef;
	}
	// if hierarchy reference is not for design or design configuration
	else {
		emit errorMessage(tr("VLNV: %1:%2:%3:%4 was not valid hierarchical reference.").arg(
			hierarchyRef.getVendor()).arg(
			hierarchyRef.getLibrary()).arg(
			hierarchyRef.getName()).arg(
			hierarchyRef.getVersion()));
		return VLNV();
	}
}

QSharedPointer<Design> LibraryHandler::getDesign( const VLNV& hierarchyRef ) {
	if (!data_->contains(hierarchyRef)) {
		emit errorMessage(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
			hierarchyRef.getVendor()).arg(
			hierarchyRef.getLibrary()).arg(
			hierarchyRef.getName()).arg(
			hierarchyRef.getVersion()));
		return QSharedPointer<Design>();
	}

	// get the vlnv for the design
	VLNV designVlnv = getDesignVLNV(hierarchyRef);
	
	// if design was found
	if (designVlnv.isValid()) {
		QSharedPointer<LibraryComponent> libComp = getModel(designVlnv);
		return libComp.staticCast<Design>();
	}
	// design was not found
	else {
		return QSharedPointer<Design>();
	}
}
