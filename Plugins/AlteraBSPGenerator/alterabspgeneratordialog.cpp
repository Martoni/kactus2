/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	alterabspgeneratordialog.cpp
 *	Project:		Kactus 2
*/

#include "alterabspgeneratordialog.h"
#include <LibraryManager/libraryinterface.h>
#include <models/bspbuildcommand.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>

#include <QDebug>

// windows runs bat script
#ifdef Q_OS_WIN32
const QString AlteraBSPGeneratorDialog::PROCESS_START_COMMAND = QString("\"Nios II Command Shell.bat\"");

// other run the shell script
#else
const QString AlteraBSPGeneratorDialog::PROCESS_START_COMMAND = QString("nios2_command_shell.sh");
#endif

AlteraBSPGeneratorDialog::AlteraBSPGeneratorDialog(LibraryInterface* handler, 
	QSharedPointer<Component> component, 
	QWidget *parent):
CommandLineGeneratorDialog(parent),
	handler_(handler),
	component_(component),
viewSelector_(NULL),
dirButton_(NULL),
commandLabel_(NULL), 
targetDir_(),
currentView_() {

	Q_ASSERT(handler_);
	Q_ASSERT(component_);

	viewSelector_ = new ViewSelector(ViewSelector::SW_VIEWS, component, this, false);
	viewSelector_->refresh();
	connect(viewSelector_, SIGNAL(viewSelected(const QString&)),
		this, SLOT(onViewChange(const QString&)), Qt::UniqueConnection);
	dirButton_ = new QPushButton(tr("Select..."), this);
	connect(dirButton_, SIGNAL(clicked(bool)),
		this, SLOT(onSelectOutput()), Qt::UniqueConnection);
	QFormLayout* settingsLayout = new QFormLayout();
	settingsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	settingsLayout->addRow(tr("1. Select SW view"), viewSelector_);
	settingsLayout->addRow(tr("2. Select output BSP directory"), dirButton_);

	QLabel* commandName = new QLabel(tr("Command:"), this);
	commandLabel_ = new QLabel(tr("Command to be run"), this);
	//commandLabel_->setWordWrap(true);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(settingsLayout);
	topLayout->addWidget(commandName, 0, Qt::AlignLeft);
	topLayout->addWidget(commandLabel_, 0, Qt::AlignLeft);
	topLayout->addWidget(outputBox_);
	topLayout->addLayout(statusLayout_);
	topLayout->addLayout(buttonLayout_);

	process_ = new QProcess(this);
	QStringList environment = QProcess::systemEnvironment();
	process_->setEnvironment(environment);
	connectProcessToOutput();

	// by default the target directory is on the root of the component
	targetDir_ = handler_->getDirectoryPath(*component_->getVlnv());

	onViewChange(viewSelector_->currentText());
}

AlteraBSPGeneratorDialog::~AlteraBSPGeneratorDialog() {
}

void AlteraBSPGeneratorDialog::onRunClicked() {
	// clear previous prints
	output_->clear();

	// on windows the command prompt is run and commands written to it
	#ifdef Q_OS_WIN32
	
	process_->start("cmd");

	// TODO the path is set elsewhere
	process_->write("\"c:\\ALTERA\\12.1\\nios2eds\\Nios II Command Shell.bat\"\n");
	process_->waitForReadyRead();

	// other run the shell script
	#else
	
	qDebug() << "Linux not supported yet";
	return;
	#endif

	

	// if process can not be started successfully
	if (!process_->waitForStarted()) {
		output_->printError(tr("Process could not be started successfully."));
		statusLabel_->setText(tr("Could not start."));
		process_->close();
		return;
	}
	qDebug() << "started";

	// TODO run the actual generation command


	process_->write("exit\n");
	process_->waitForReadyRead();

	process_->write("exit\n");
	process_->waitForBytesWritten();
	if (!process_->waitForFinished()) {
		output_->printError(tr("Process could not be finished successfully."));
		statusLabel_->setText(tr("Could not finish."));
		process_->close();
		return;
	}

	process_->close();
	qDebug() << "closed";
	
}

void AlteraBSPGeneratorDialog::onViewChange( const QString& viewName ) {
	currentView_ = component_->findSWView(viewName);
	Q_ASSERT(currentView_);

	updateCommand();
}

void AlteraBSPGeneratorDialog::onSelectOutput() {
	QString newTarget = QFileDialog::getExistingDirectory(this, tr("Select directory to generate BSP to."),
		targetDir_);

	// if user selected a directory
	if (!newTarget.isEmpty()) {
		targetDir_ = newTarget;
		updateCommand();
	}
}

void AlteraBSPGeneratorDialog::updateCommand() {
	QString command("[COMMAND MISSING]");
	QString sourceFile("[SOURCE_FILE MISSING]");
	QString args("[ARGUMENTS MISSING]");
	QString cpuName("[CPU_NAME MISSING]");

	Q_ASSERT(currentView_);
	QSharedPointer<BSPBuildCommand> com = currentView_->getBSPBuildCommand();

	if (com) {

		// only the defined elements are 

		if (!com->getCommand().isEmpty()) {
			command = com->getCommand();
		}

		// arguments may not be necessary
		args = com->getArguments();
		
		if (!com->getCPUName().isEmpty()) {
			cpuName = com->getCPUName();
		}
	}

	QString wholeCommand = QString("%1 %2 %3 %4 --cpu-name %5").arg(command).arg(targetDir_).arg(
		sourceFile).arg(args).arg(cpuName);
	commandLabel_->setText(wholeCommand);
}