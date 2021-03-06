/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"
#include "PortMap.h"
#include "vector.h"
#include "XmlUtils.h"

#include <QDomNode>
#include <QString>
#include <QDomNodeList>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QMap>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <qmath.h>

#include <QDebug>

General::BitSteering General::str2BitSteering(const QString& str) {
	if (str == QString("on")) {
		return General::ON;
	}
	else if (str == QString("off")) {
		return General::OFF;
	}
	else {
		return General::BITSTEERING_UNSPECIFIED;
	}
}

QString General::bitSteering2Str(const General::BitSteering bitSteering) {
	switch (bitSteering) {
	case General::ON: {
		return QString("on");
	}
	case General::OFF: {
		return QString("off");
	}
	default: {
		return QString();
	}
	}
}

General::PortBounds::PortBounds():
portName_(),
left_(0),
right_(0) {
}

General::PortBounds::PortBounds( const QString& portName ):
portName_(portName),
left_(0),
right_(0) {
}

General::PortBounds::PortBounds( const QString& portName, const int left, const int right ):
portName_(portName),
left_(left),
right_(right) {
}

General::PortBounds::PortBounds( const PortBounds& other ):
portName_(other.portName_),
left_(other.left_),
right_(other.right_) {
}

General::PortBounds& General::PortBounds::operator=( const PortBounds& other ) {
	if (&other != this) {
		portName_ = other.portName_;
		left_ = other.left_;
		right_ = other.right_;
	}
	return *this;
}

bool General::PortBounds::operator<( const PortBounds& other ) const {
	return portName_ < other.portName_;
}

bool General::PortBounds::operator==( const PortBounds& other ) const {
	return portName_ == other.portName_;
}

bool General::PortBounds::operator!=( const PortBounds& other ) const {
	return portName_ != other.portName_;
}

General::PortAlignment::PortAlignment():
port1Left_(-1),
port1Right_(-1),
port2Left_(-1),
port2Right_(-1),
invalidAlignment_(true) {
}

General::PortAlignment::PortAlignment( const PortAlignment& other ):
port1Left_(other.port1Left_),
port1Right_(other.port1Right_),
port2Left_(other.port2Left_),
port2Right_(other.port2Right_),
invalidAlignment_(other.invalidAlignment_) {
}

General::PortAlignment& General::PortAlignment::operator=( const PortAlignment& other ) {
	if (this != &other) {
		port1Left_ = other.port1Left_;
		port1Right_ = other.port1Right_;
		port2Left_ = other.port2Left_;
		port2Right_ = other.port2Right_;
		invalidAlignment_ = other.invalidAlignment_;
	}
	return *this;
}

General::PortAlignment General::calculatePortAlignment( 
	const PortMap* portMap1, 
	int phys1LeftBound, 
	int phys1RightBound, 
	const PortMap* portMap2,
	int phys2LeftBound, 
	int phys2RightBound ) {

	General::PortAlignment alignment;
	
	// if the port maps are for different logical ports
	if (portMap1->logicalPort() != portMap2->logicalPort()) {
		return alignment;
	}

	// if port 1 is vectored on the port map
	if (portMap1->physicalVector()) {
		phys1LeftBound = portMap1->getPhysicalLeft();
		phys1RightBound = portMap1->physicalVector()->getRight();
	}

	// if port 2 is vectored on the port map
	if (portMap2->physicalVector()) {
		phys2LeftBound = portMap2->getPhysicalLeft();
		phys2RightBound = portMap2->physicalVector()->getRight();
	}

	// if both have vectored logical signals
	if (portMap1->logicalVector() && portMap2->logicalVector()) {

		// if the vectored ports don't have any common bounds
		if (portMap1->getLogicalRight() > portMap2->getLogicalLeft() ||
			portMap1->getLogicalLeft() < portMap2->getLogicalRight()) {
				return alignment;
		}

		int logicalLeft = qMin(portMap1->getLogicalLeft(), 
			portMap2->getLogicalLeft());
		int logicalRight = qMax(portMap1->getLogicalRight(),
			portMap2->getLogicalRight());

		{
			// count how much the left bound of port 1 has to be adjusted down
			int downSize = abs(portMap1->getLogicalLeft() - logicalLeft);
			// count how must the right bound of  port 1 has to be adjusted up
			int upSize = abs(logicalRight - portMap1->getLogicalRight());

			alignment.port1Left_ = phys1LeftBound - downSize;
			alignment.port1Right_ = phys1RightBound + upSize;
		}
		{
			// count how much the left bound of port 2 has to be adjusted down
			int downSize = abs(portMap2->getLogicalLeft() - logicalLeft);
			// count how must the right bound of  port 2 has to be adjusted up
			int upSize = abs(logicalRight - portMap2->getLogicalRight());

			alignment.port2Left_ = phys2LeftBound - downSize;
			alignment.port2Right_ = phys2RightBound + upSize;
		}
	}
	// if port map1 has vectored logical signal
	else if (portMap1->logicalVector() && !portMap2->logicalVector()) {

		// port 1 uses the original physical bounds
		alignment.port1Left_ = phys1LeftBound;
		alignment.port1Right_ = phys1RightBound;

		// port 2 uses the bounds of the logical port of port 1
		alignment.port2Left_ = portMap1->getLogicalLeft();
		alignment.port2Right_ = portMap1->getLogicalRight();
	}
	// if port map2 has vectored logical signal
	else if (!portMap1->logicalVector() && portMap2->logicalVector()) {

		// port 1 uses the bounds of the logical port of port 2
		alignment.port1Left_ = portMap2->getLogicalLeft();
		alignment.port1Right_ = portMap2->getLogicalRight();

		// port 2 uses the original physical bounds
		alignment.port2Left_ = phys2LeftBound;
		alignment.port2Right_ = phys2RightBound;
	}
	// if neither has vectored logical signal
	else {

		// both ports use the original physical bounds
		alignment.port1Left_ = phys1LeftBound;
		alignment.port1Right_ = phys1RightBound;
		alignment.port2Left_ = phys2LeftBound;
		alignment.port2Right_ = phys2RightBound;
	}

	// check if the sizes of the ports match
	int port1Size = alignment.port1Left_ - alignment.port1Right_ + 1;
	int port2Size = alignment.port2Left_ - alignment.port2Right_ + 1;
	if (port1Size != port2Size) {
		alignment.invalidAlignment_ = true;
	}
	else {
		alignment.invalidAlignment_ = false;
	}
	return alignment;
}

QString General::port2String(const QString& portName, int leftBound, int rightBound) {
	QString str(portName);
	str += QString("[%1..%2]").arg(leftBound).arg(rightBound);
	return str;
}

QString General::bool2Str(bool value) {
	if (value) {
		return QString("true");
	}
	else {
		return QString("false");
	}
}

General::Endianness General::str2Endianness(QString str,
		General::Endianness defaultValue) {

	if (str == QString("big")) {
		return General::BIG;
	}
	else if (str == QString("little")) {
		return General::LITTLE;
	}
	else {
		return defaultValue;
	}
}

QString General::endianness2Str(const General::Endianness endianness) {
	switch (endianness) {
	case General::BIG: {
		return QString("big");
	}
	case General::LITTLE: {
		return QString("little");
	}
	default: { // this should never happen
		return QString();
	}
	}
}

General::Initiative General::str2Initiative(QString str,
		General::Initiative defaultValue) {

	// select the correct enum value that matches the string
	if (str == QString("requires")) {
		return General::REQUIRES;
	}
	else if (str == QString("provides")) {
		return General::PROVIDES;
	}
	else if (str == QString("both")) {
		return General::BOTH;
	}
	else if (str == QString("phantom")) {
		return General::INITIATIVE_PHANTOM;
	}
	else {
		return defaultValue;
	}
}

QString General::Initiative2Str(const General::Initiative initiative) {
	switch (initiative) {
	case General::REQUIRES: {
		return QString("requires");
	}
	case General::PROVIDES: {
		return QString("provides");
	}
	case General::BOTH: {
		return QString("both");
	}
	case General::INITIATIVE_PHANTOM: {
		return QString("phantom");
	}
	// if initiative = NONE
	default: {
		return QString();
	}
	}
}

General::Presence General::str2Presence(QString str,
		General::Presence defaultValue) {

	// identify the correct enum value
	if (str == QString("illegal")) {
		return General::ILLEGAL;
	}

	else if (str == QString("required")) {
		return General::REQUIRED;
	}

	else if (str == QString("optional")) {
		return General::OPTIONAL;
	}
	else {
		return defaultValue;
	}
}

QString General::presence2Str(General::Presence presence) {
	switch (presence) {
	case General::ILLEGAL: {
		return QString("illegal");
	}
	case General::REQUIRED: {
		return QString("required");
	}
	case General::OPTIONAL: {
		return QString("optional");
	}
	default: {
		return QString();
	}
	}
}

General::Direction General::str2Direction(QString str,
		General::Direction defaultValue) {

	// convert QString into Direction
	if (str == QString("in")) {
		return General::IN;
	}
	else if (str == QString("out")) {
		return General::OUT;
	}
	else if (str == QString("inout")) {
		return General::INOUT;
	}
	else if (str == QString("phantom")) {
		return General::DIRECTION_PHANTOM;
	}
	else {
		return defaultValue;
	}
}

QString General::direction2Str(const General::Direction direction) {
	switch (direction) {
	case General::IN: {
		return QString("in");
	}
	case General::OUT: {
		return QString("out");
	}
	case General::INOUT: {
		return QString("inout");
	}
	case General::DIRECTION_PHANTOM: {
		return QString("phantom");
	}
	// if DIRECTION_INVALID
	default: {
		return QString();
	}
	}
}

General::Direction General::convert2Mirrored(const Direction originalDirection ) {
	switch (originalDirection) {
		case General::IN: 
			return General::OUT;
		case General::OUT:
			return General::IN;
		case General::INOUT:
			return General::INOUT;
		case General::DIRECTION_PHANTOM:
			return General::DIRECTION_PHANTOM;
		default:
			return General::DIRECTION_INVALID;
	}
}

General::Access General::str2Access(QString str,
		General::Access defaultValue) {
	if (str == QString("read-write")) {
		return General::READ_WRITE;
	}
	else if (str == QString("read-only")) {
		return General::READ_ONLY;
	}
	else if (str ==	QString("write-only")) {
		return General::WRITE_ONLY;
	}
	else if (str ==	QString("read-writeOnce")) {
		return General::READ_WRITEONCE;
	}
	else if (str == QString("writeOnce")) {
		return General::WRITEONCE;
	}
	else {
		return defaultValue;
	}
}

QString General::access2Str(const General::Access access) {
	switch (access) {
	case General::READ_WRITE: {
		return QString("read-write");
	}
	case General::READ_ONLY: {
		return QString("read-only");
	}
	case General::WRITE_ONLY: {
		return QString("write-only");
	}
	case General::READ_WRITEONCE: {
		return QString("read-writeOnce");
	}
	case General::WRITEONCE: {
		return QString("writeOnce");
	}
	// if UNSPECIFIED_ACCESS
	default: {
		return QString();
	}
	}
}

General::Usage General::str2Usage(QString str,
		General::Usage defaultValue) {
	if (str == QString("memory")) {
		return General::MEMORY;
	}
	else if (str == QString("register")) {
		return General::REGISTER;
	}
	else if (str ==	QString("reserved")) {
		return General::RESERVED;
	}
	else {
		return defaultValue;
	}
}

QString General::usage2Str(const General::Usage usage) {
	switch (usage) {
	case General::MEMORY: {
		return QString("memory");
	}
	case General::REGISTER: {
		return QString("register");
	}
	case General::RESERVED: {
		return QString("reserved");
	}
	// if UNSPECIFIED_USAGE
	default: {
		return QString();
	}
	}
}

General::GroupSelectorOperator General::str2GroupSelector(QString str,
		General::GroupSelectorOperator defaultValue) {
	if (str == QString("and")) {
		return General::AND;
	}
	else if (str == QString("or")) {
		return General::OR;
	}
	else {
		return defaultValue;
	}
}

General::TimeUnit General::str2TimeUnit(QString str,
		General::TimeUnit defaultValue) {

	if (str.compare(QString("ps"), Qt::CaseInsensitive) == 0) {
		return General::PS;
	}
	else if (str.compare(QString("ns"), Qt::CaseInsensitive) == 0) {
		return General::NS;
	}
	else {
		return defaultValue;
	}
}

QString General::timeUnit2Str(General::TimeUnit& timeUnit) {
	switch (timeUnit) {
	case General::PS: {
		return QString("ps");
	}
	default: {
		return QString("ns");
	}
	}
}

General::DriverType General::str2DriverType(QString str,
                General::DriverType defaultValue) {

	if (str == QString("any")) {
		return General::ANY;
	}
	else if (str == QString("clock")) {
		return General::CLOCK;
	}
	else if (str == QString("singleshot")) {
		return General::SINGLESHOT;
	}
	else {
		return defaultValue;
	}
}

QString General::driverType2Str(General::DriverType type) {

	switch (type) {
	case General::ANY: {
		return QString("any");
	}
	case General::CLOCK: {
		return QString("clock");
	}
	case General::SINGLESHOT: {
		return QString("singleshot");
	}
	default: {
		return QString();
	}
	}
}

bool General::str2Bool(const QString str, bool defaultValue) {
	if (str == QString("true")) {
		return true;
	}
	else if (str == QString("false")) {
		return false;
	}
	else {
		return defaultValue;
	}
}

bool General::BooleanValue2Bool(const General::BooleanValue value,
		const bool defaultValue) {
	switch (value) {
	case General::BOOL_TRUE: {
		return true;
	}
	case General::BOOL_FALSE: {
		return false;
	}
	default: {
		return defaultValue;
	}
	}
}

General::BooleanValue General::bool2BooleanValue(const bool value) {
	if (value)
		return General::BOOL_TRUE;
	else
		return General::BOOL_FALSE;
}

General::BooleanValue General::str2BooleanValue(const QString& str) {
	if (str == QString("true")) {
		return General::BOOL_TRUE;
	}
	else if (str == QString("false")) {
		return General::BOOL_FALSE;
	}
	else {
		return General::BOOL_UNSPECIFIED;
	}
}

QString General::booleanValue2Str(const General::BooleanValue value) {
	switch (value) {
	case General::BOOL_TRUE: {
		return QString("true");
	}
	case General::BOOL_FALSE: {
		return QString("false");
	}
	default: {
		return QString();
	}
	}
}

General::ClockStruct::ClockStruct(QDomNode &clockNode):
value_(0),
timeUnit_(General::NS),
attributes_() {

	QString value = clockNode.childNodes().at(0).nodeValue();
	value = XmlUtils::removeWhiteSpace(value);
	bool ok = false;
	value_ = value.toDouble(&ok);


	// get the attributes
	QDomNamedNodeMap attributeMap = clockNode.attributes();
	for (int i = 0; i < attributeMap.size(); ++i) {
		QDomNode tempNode = attributeMap.item(i);

		// get spirit:units attribute value
		if (tempNode.nodeName() == QString("spirit:units")) {
			timeUnit_ = General::str2TimeUnit(tempNode.childNodes().at(i).
					nodeValue(),General::NS);
		}

		// other attributes go to QMap
		else {
			QString name = tempNode.nodeName();
			QString value = tempNode.nodeValue();
			attributes_[name] = value;
		}
	}
}

General::ClockStruct::ClockStruct( double value ):
value_(value),
timeUnit_(General::NS),
attributes_() {
}

General::ClockStruct::ClockStruct( const ClockStruct& other ):
value_(other.value_),
timeUnit_(other.timeUnit_),
attributes_(other.attributes_) {
}

General::ClockStruct& General::ClockStruct::operator=( const ClockStruct& other ) {
	if (this != &other) {
		value_ = other.value_;
		timeUnit_ = other.timeUnit_;
		attributes_ = other.attributes_;
	}
	return *this;
}

General::ClockPulseValue::ClockPulseValue(QDomNode &clockNode): 
value_(2),
attributes_() {

	QString value = clockNode.childNodes().at(0).nodeValue();
	bool ok = false;
	value_ = value.toInt(&ok);

	// get the attributes
	//XmlUtils::parseAttributes(clockNode, attributes_);
}

General::ClockPulseValue::ClockPulseValue( unsigned int value ):
value_(value), 
attributes_() {

}

General::ClockPulseValue::ClockPulseValue( const ClockPulseValue& other ):
value_(other.value_),
attributes_(other.attributes_) {
}

General::ClockPulseValue& General::ClockPulseValue::operator=( const ClockPulseValue& other ) {
	if (this != &other) {
		value_ = other.value_;
		attributes_ = other.attributes_;
	}
	return *this;
}


General::InterfaceMode General::str2Interfacemode(const QString& str, InterfaceMode defaultValue) {
	
	// check all known interface mode names
	for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; ++i) {
		
		// if match is found
		if (str.compare(General::INTERFACE_MODE_NAMES[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::InterfaceMode>(i);
		}
	}

	// if there was no match
	return defaultValue;
}

QString General::interfaceMode2Str(const General::InterfaceMode mode) {
	return General::INTERFACE_MODE_NAMES[mode];
}

//-----------------------------------------------------------------------------
// Function: getCompatibleInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode General::getCompatibleInterfaceMode(InterfaceMode mode)
{
    switch (mode)
    {
    case General::MASTER: {
            return MIRROREDMASTER;
        }
    case General::SLAVE: {
            return MIRROREDSLAVE;
        }
    case General::SYSTEM: {
            return MIRROREDSYSTEM;
        }
    case General::MIRROREDMASTER: {
            return MASTER;
        }
    case General::MIRROREDSLAVE: {
            return SLAVE;
        }
    case General::MIRROREDSYSTEM: {
            return SYSTEM;
        }
    case General::MONITOR: {
            return MONITOR;
        }
    default: {
            Q_ASSERT(false);
            return MASTER;
        }
    }
}

General::Qualifier::Qualifier(QDomNode& qualifierNode): isAddress_(false),
		isData_(false), isClock_(false), isReset_(false) {
	for (int i = 0; i < qualifierNode.childNodes().count(); ++i) {
		QDomNode tempNode = qualifierNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:isAddress")) {
			QString isAddress = tempNode.childNodes().at(0).nodeValue();
			isAddress_ = General::str2Bool(isAddress, false);
		}

		else if (tempNode.nodeName() == QString("spirit:isData")) {
			QString isData = tempNode.childNodes().at(0).nodeValue();
			isData_ = General::str2Bool(isData, false);
		}

		else if (tempNode.nodeName() == QString("spirit:isClock")) {
			QString isClock = tempNode.childNodes().at(0).nodeValue();
			isClock_ = General::str2Bool(isClock, false);
		}

		else if (tempNode.nodeName() == QString("spirit:isReset")) {
			QString isReset = tempNode.childNodes().at(0).nodeValue();
			isReset_ = General::str2Bool(isReset, false);
		}
	}
	return;
}

General::Qualifier::Qualifier( const Qualifier& other ):
isAddress_(other.isAddress_),
isData_(other.isData_),
isClock_(other.isClock_),
isReset_(other.isReset_) {
}

General::Qualifier::Qualifier():
isAddress_(false),
isData_(false),
isClock_(false),
isReset_(false) {
}

General::Qualifier& General::Qualifier::operator=( const Qualifier& other ) {
	if (this != &other) {
		isAddress_ = other.isAddress_;
		isData_ = other.isData_;
		isClock_ = other.isClock_;
		isReset_ = other.isReset_;
	}
	return *this;
}

General::LibraryFilePair::LibraryFilePair(const QString filePath,
		const QString libraryName): filePath_(filePath),
		libraryName_(libraryName) {
}

General::LibraryFilePair::LibraryFilePair( const LibraryFilePair& other ):
filePath_(other.filePath_),
libraryName_(other.libraryName_) {
}

bool General::LibraryFilePair::operator==( const LibraryFilePair& other ) const {
	return other.filePath_ == this->filePath_ && 
		other.libraryName_ == this->libraryName_;
}

General::LibraryFilePair& General::LibraryFilePair::operator=( const LibraryFilePair& other ) {
	if (this != &other) {
		filePath_ = other.filePath_;
		libraryName_ = other.libraryName_;
	}
	return *this;
}

QString General::getRelativePath(const QString from, const QString to) {
	if (from.isEmpty() || to.isEmpty()) {
		return QString();
	}

	// create file info instance to make sure that only the directory of the
	// from parameter is used
	QFileInfo fromInfo(from);

    QString fromPath = fromInfo.absolutePath();

    if (fromInfo.isDir())
    {
        fromPath = fromInfo.absoluteFilePath();
    }

	// if the directory does not exist
	QDir ipXactDir(fromPath);

	if (!ipXactDir.exists()) {
		return QString();
	}

	// create file info instance to make sure the target file exists and to
	// use an absolute file path to calculate the relative path.
	QFileInfo toInfo(to);
	if (!toInfo.exists()) {
		return QString();
	}

	// calculate the relative path and return it.
	QString relPath = ipXactDir.relativeFilePath(toInfo.absoluteFilePath());

    // Strip the ending slash if found.
    if (relPath.size() > 0 && relPath.at(relPath.size() - 1) == '/')
    {
        relPath = relPath.left(relPath.size() - 1);
    }
    else if (relPath.isEmpty())
    {
        relPath = ".";
    }

    return relPath;
}

QString General::getRelativeSavePath( const QString& from, const QString& to ) {
	if (from.isEmpty() || to.isEmpty()) {
		return QString();
	}

	// create file info instance to make sure that only the directory of the
	// from parameter is used
	QFileInfo fromInfo(from);

	QString fromPath = fromInfo.absolutePath();

	if (fromInfo.isDir())
	{
		fromPath = fromInfo.absoluteFilePath();
	}

	// if the directory does not exist
	QDir ipXactDir(fromPath);

	if (!ipXactDir.exists()) {
		return QString();
	}

	// create file info instance to make sure the target file exists and to
	// use an absolute file path to calculate the relative path.
	QFileInfo toInfo(to);

	// calculate the relative path and return it.
	return ipXactDir.relativeFilePath(toInfo.absoluteFilePath());
}

QString General::getAbsolutePath(const QString& originalPath,
		const QString& relativePath) {

	// if one of the parameters is empty
	if (originalPath.isEmpty() || relativePath.isEmpty()) {
		return QString();
	}

	QFileInfo relativeInfo(relativePath);
	if (relativeInfo.isAbsolute()) {
		return relativePath;
	}

	// get the directory path of the original path
	QFileInfo original(originalPath);
	QDir originalDir(original.absolutePath());

	// make sure the path exists
	return originalDir.absoluteFilePath(relativePath);
}

bool General::isIpXactFileType( const QString& fileType ) {

	// check all the pre-specified file types
	for (unsigned int i = 0; i < General::FILE_TYPE_COUNT; ++i) {
		
		// if the file type is one of the specified ones
		if (QString(General::FILE_TYPES[i]) == fileType) {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Function: generaldeclarations::getFileTypes()
//-----------------------------------------------------------------------------
QStringList General::getFileTypes( QSettings& settings, const QString& fileSuffix )
{	
    QStringList types;

	settings.beginGroup("FileTypes");
	foreach (QString const& type, settings.childGroups())
    {
		// Get the extensions associated with the file type.
		QString extensions = settings.value(type + "/Extensions").toString();

		foreach (QString extension, extensions.split(";", QString::SkipEmptyParts))
        {
			if (extension.compare(fileSuffix, Qt::CaseInsensitive) == 0)
            {
				types.append(type);
			}
		}
	}
    settings.endGroup();

	return types;
}

//-----------------------------------------------------------------------------
// Function: generaldeclarations::getFileTypes()
//-----------------------------------------------------------------------------
QStringList General::getFileTypes( QSettings& settings, const QFileInfo& file )
{
	return General::getFileTypes(settings, file.suffix());
}

General::ModifiedWrite General::str2ModifiedWrite( const QString& str ) {
	
	// check all defined strings
	for (unsigned int i = 0; i < General::MODIFIED_WRITE_COUNT; ++i) {
		
		// if a match is found
		if (str.compare(General::MODIFIED_WRITE_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::ModifiedWrite>(i);
		}
	}

	// if none of the defined strings matched 
	return General::MODIFIED_WRITE_COUNT;
}

QString General::modifiedWrite2Str( const General::ModifiedWrite modWrite ) {
	return General::MODIFIED_WRITE_STRINGS[modWrite];
}

QString General::readAction2Str( const General::ReadAction readAction ) {
	return General::READ_ACTION_STRINGS[readAction];
}

General::ReadAction General::str2ReadAction( const QString& str ) {
	// check all defined strings
	for (unsigned int i = 0; i < General::READ_ACTION_COUNT; ++i) {
		
		// if a match is found
		if (str.compare(General::READ_ACTION_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::ReadAction>(i);
		}
	}

	// if none of the defined strings matched
	return General::READ_ACTION_COUNT;
}

QString General::testConstraint2Str( const General::TestConstraint testConstraint ) {
	return General::TEST_CONSTRAINT_STRINGS[testConstraint];
}

General::TestConstraint General::str2TestConstraint( const QString& str ) {
	// check all defined strings
	for (unsigned int i = 0; i < General::TESTCONSTRAINT_COUNT; ++i) {

		// if a match is found
		if (str.compare(General::TEST_CONSTRAINT_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::TestConstraint>(i);
		}
	}

	// if none of the defined strings matched
	return General::TESTCONSTRAINT_COUNT;
}



quint64 General::str2Uint( const QString& str ) {

    if (str.isEmpty()) {
        return 0;
    }

    // used to detect if the conversion was successful
    bool success = true;
    quint64 number = 0;

    // if starts with "0x" then it is hexadecimal digit
    if (str.startsWith("0x", Qt::CaseInsensitive)) {

        number = str.toULongLong(&success, 16);
        if (success) {
            return number;
        }
    }

    // needed because the last letter is chopped if one is found
    QString strNumber = str;

    // the multiple is the last letter if one exists
    const QChar multiple = strNumber.at(strNumber.size()-1);
    quint64 multiplier = 1;

    // get the correct multiplier and remove the letter from the string
    if (multiple == 'k' || multiple == 'K') {
        multiplier = qPow(2, 10);
        strNumber.chop(1);
    }
    else if (multiple == 'M') {
        multiplier = qPow(2, 20);
        strNumber.chop(1);
    }
    else if (multiple == 'G') {
        multiplier = qPow(2, 30);
        strNumber.chop(1);
    }
    else if (multiple == 'T') {
        multiplier = qPow(2, 40);
        strNumber.chop(1);
    }
    else if (multiple == 'P') {
        multiplier = qPow(2, 50);
        strNumber.chop(1);
    }

    // try to convert the number 
    number = strNumber.toULongLong(&success);

    // if the conversion failed
    if (!success) {
        return 0;
    }
    // otherwise return the correct int-format
    else {
        return number * multiplier;
    }
}