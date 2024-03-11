#include "aidparser.h"
#include "aidmediator.h"
#include "aidtempdirmanager.h"

// miniz includes
#include "miniz.h"

namespace AID {

	Parser::Parser(QWidget* parent)
	{
        connect(&Mediator::getInstance(), &Mediator::parseXml, this, &Parser::parseXmlToObj);
        connect(&Mediator::getInstance(), &Mediator::readFile, this, &Parser::fileReader);
        //qDebug() << "Parser Connections: parseXmlToObj and fileReader set";
	}

	Parser::~Parser()
	{
	}

    QString Parser::create3dObj(const ObjModel& model, const QString& fileName, int sequenceNumber)
    {
        return "OK";
    }

    QString Parser::writeObjFile(const ObjModel& model, const QString& fileName, int sequenceNumber)
    {
        //QString tempPath = QDir::tempPath();
        int index = fileName.lastIndexOf('/');
        QString baseName = QFileInfo(fileName).completeBaseName();
        QString objPath = fileName.left(index) + '/' + baseName;

        // TODO: Better naming-match model names
        QString objFileName = QString("%1/%2.obj").arg(objPath).arg(sequenceNumber, 3, 10, QLatin1Char('0'));
        //qDebug() << "Temp file obj location:" << objFileName;
        
        // Path to the new directory
        QString dirPath = objPath;

        QDir dir(dirPath);

        if (!dir.exists())
        {
            bool success = dir.mkpath(".");
            if (!success)
            {
                qDebug() << "Failed to create directory.";
            }
        }

        QFile file(objFileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            qDebug() << "Failed to open file for writing:" << file.errorString();
            return "1";
        }

        QTextStream out(&file);
        
        if (!model.matGroups.isEmpty()) {
            out << "mtllib models.mtl";
            out << "\n\n";
        }

        if (!model.vertices.isEmpty()) {
            for (const QVector3D& vert : model.vertices) {
                QString formattedX = QString::number(vert.x(), 'f', 6);
                QString formattedY = QString::number(vert.y(), 'f', 6);
                QString formattedZ = QString::number(vert.z(), 'f', 6);
                out << "v " << formattedX << " " << formattedY << " " << formattedZ << "\n";
            }
        }
        out << "\n";
        /*
            for (const QVector3D& norm: model.normals) {
                out << "vn " << norm.x() << " " << norm.y() << " " << norm.z() << "\n";
            }
            out << "\n";
        */
        // TODO: Make all same precision
        if (!model.texCoords.isEmpty()) {
            for (const QPointF& texCoord : model.texCoords) {
                QString formattedX = QString::number(texCoord.x(), 'f', 6);
                QString formattedY = QString::number(texCoord.y(), 'f', 6);
                out << "vt " << formattedX << " " << formattedY << "\n";
            }
        }
        out << "\n";

        // TODO: This probably won't work, need more work
        if (!model.smoothingGroups.isEmpty()) {
            out << "# I have changed this code to print only first element\n";
            for (const unsigned int& group : model.smoothingGroups) {
                out << "s " << group << "\n";
            }
        }
        out << "\n";

        // TODO: This probably won't work, need more work. Just a stop gap to suppress errors
        if (!model.matGroups.isEmpty()) {
            // QVector<QPair<QString, QVector<int>>> matGroups;
            for (const QPair<QString, QVector<int>>& group : model.matGroups) {
                out << "usemtl " << group.second.first() << "\n";
            }
        }
        out << "\n";
/*
        for (const QVector3D& face : model.faces) {
            out << "f " << face.x() << " " << face.y() << " " << face.z() << "\n";
        }
*/
        if (model.texCoords.isEmpty())
        {
            if (model.normals.isEmpty())
            {
                for (const QVector3D& face : model.faces) {
                    out << "f " << face.x() << " " << face.y() << " " << face.z() << "\n";
                }

            }
            else
            {
                for (const QVector3D& face : model.faces) {
                    out << "f " << face.x() << "//" << face.x() << " " << face.y() << "//" << face.y() << " " << face.z() << "//" << face.z() << "\n";
                }
            }
        }
        else
        {
            if (model.normals.isEmpty())
            {
                for (const QVector3D& face : model.faces) {
                    out << "f " << face.x() << "/" << face.x() << " " << face.y() << "/" << face.y() << " " << face.z() << "/" << face.z() << "\n";
                }

            }
            else
            {
                for (const QVector3D& face : model.faces) {
                    out << "f " << face.x() << "/" << face.x() << "/" << face.x() << " " << face.y() << "/" << face.y() << "/" << face.y() << " " << face.z() << "/" << face.z() << "/" << face.z() << "\n";
                }
            }
        }
        


        //qDebug() << "Created OBJ file:" << objFileName;
        return objFileName;

    }

    void Parser::readXmlIntoModels(const QString& fileName, const Object3D& properties, const QMap<QString, ObjMaterial>& materials, const bool isAsset)
    {
        QFile xmlFile(fileName);
        if (!xmlFile.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Failed to open XML file:" << xmlFile.errorString();
            return;
        }
        QXmlStreamReader xmlReader(&xmlFile);

        QVector<ObjModel> objModels;
        ObjModel currentModel;

        int index = fileName.lastIndexOf("/models.xml");
        QFileInfo fi(fileName);
        QString absoluteDir = fileName.left(index);
        QString texturePath = absoluteDir + "/textures/";
        qDebug() << absoluteDir << "<<<absoluteDir";
        index = absoluteDir.lastIndexOf('/');
        QString objFolderPath = absoluteDir.remove(absoluteDir.left(index));


        int objToggle = 0;
        int subPartCount = 0;
        uint version = 0;
        if (properties.version.has_value())
            version = properties.version.value();
        
        int overallVertexCount = 0;
        //QVector3D center(0.0f, 0.0f, 0.0f);

        while (!xmlReader.atEnd() && !xmlReader.hasError()) {
            xmlReader.readNext();

            if (xmlReader.isStartElement()) {
                if (xmlReader.name() == (QString) "model") {
                    ObjModel currentModel;
                    if (!xmlReader.attributes().value("name").isEmpty())
                    {
                        currentModel.name = xmlReader.attributes().value("name").toString();
                    }
                    else
                    {
                        currentModel.name = objFolderPath + QString::number(subPartCount);
                    }
                    if (!xmlReader.attributes().value("type").isEmpty())
                    {
                        currentModel.type = xmlReader.attributes().value("type").toString();
                    }
                    else
                    {
                        currentModel.type = "unknown";
                    }
                    currentModel.count = subPartCount;
                    currentModel.isAsset = isAsset;
                    subPartCount++;
                    currentModel.assetPath = texturePath;
                    //parseModel(xmlReader, currentModel, version);
                    while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == (QString) "model")) {
                        xmlReader.readNext();

                        if (xmlReader.isStartElement()) {
                            if (xmlReader.name() == (QString) "vertices") {
                                int count = xmlReader.attributes().value("count").toInt();
                                QStringList vertexList = xmlReader.readElementText().split(';', Qt::SkipEmptyParts); // QString::SkipEmptyParts
                                for (const QString& vertex : vertexList) {
                                    QStringList coords = vertex.split(',');
                                    if (coords.size() == 3) {
                                        float x = coords[0].toFloat();
                                        float y = coords[1].toFloat();
                                        float z = coords[2].toFloat();
                                        if (version >= 2) {
                                            // Swap y and z for versions 2 and above
                                            std::swap(y, z);
                                        }
                                        else
                                        {
                                            x *= 100;
                                            y *= 100;
                                            z *= 100;
                                        }
                                        currentModel.vertices.append(QVector3D(x, y, z));
                                        currentModel.version = version;
                                    }
                                }
                                QVector3D min = currentModel.vertices.isEmpty() ? QVector3D() : currentModel.vertices.first();
                                QVector3D max = min;

                                for (const QVector3D& vertex : currentModel.vertices) {
                                    min.setX(qMin(min.x(), vertex.x()));
                                    min.setY(qMin(min.y(), vertex.y()));
                                    min.setZ(qMin(min.z(), vertex.z()));
                                    max.setX(qMax(max.x(), vertex.x()));
                                    max.setY(qMax(max.y(), vertex.y()));
                                    max.setZ(qMax(max.z(), vertex.z()));
                                }

                                currentModel.localCenter = (min + max) * 0.5;
                            }
                            else if (xmlReader.name() == (QString) "faces") {
                                QStringList faceList = xmlReader.readElementText().split(';', Qt::SkipEmptyParts);
                                for (const QString& face : faceList) {
                                    QStringList indices = face.split(',');
                                    if (indices.size() == 3) {
                                        currentModel.faces.append(QVector3D(
                                            indices[0].toInt() + objToggle,
                                            indices[1].toInt() + objToggle,
                                            indices[2].toInt() + objToggle));
                                    }
                                }
                            }
                            else if (xmlReader.name() == (QString) "smoothing") {
                                QStringList smoothingList = xmlReader.readElementText().split(';', Qt::SkipEmptyParts);
                                for (const QString& smoothing : smoothingList) {
                                    currentModel.smoothingGroups.append(smoothing.toUInt());
                                }
                            }
                            else if (xmlReader.name() == (QString) "map_coords") {
                                QStringList texCoordList = xmlReader.readElementText().split(';', Qt::SkipEmptyParts);
                                for (const QString& coord : texCoordList) {
                                    QStringList coords = coord.split(',');
                                    if (coords.size() == 2) {
                                        currentModel.texCoords.append(QPointF(coords[0].toFloat(), coords[1].toFloat()));
                                    }
                                }
                            }
                            else if (xmlReader.name() == (QString) "mat_groups") {
                                uint count = xmlReader.attributes().value("count").toInt();
                                currentModel.matGroupsCount = count;
                                while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == (QString) "mat_groups")) {
                                    xmlReader.readNext();
                                    if (xmlReader.isStartElement() && xmlReader.name() == (QString) "mat_group") {
                                        QString name = xmlReader.attributes().value("name").toString();
                                        QStringList faceIndices = xmlReader.readElementText().split(';', Qt::SkipEmptyParts);
                                        QVector<int> indices;
                                        for (const QString& index : faceIndices) {
                                            indices.append(index.toInt());
                                        }
                                        currentModel.matGroups.append(qMakePair(name, indices));
                                        for (auto mat : materials.keys())
                                        {
                                            if (mat == name)
                                            {
                                                currentModel.materials.append(qMakePair(name, materials.value(mat)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    objModels.push_back(currentModel);
                }
                else if (xmlReader.name() == (QString) "models") {
                    // Optional version attribute
                    if (!xmlReader.attributes().value("xmlns").isEmpty())
                    {
                        QString xmlNameSpace = xmlReader.attributes().value("xmlns").toString();
                        // TODO - what can we use this for?
                    }
                    
                }
            }
        }

        QPair<QVector3D, QVector3D> boundingBox = calculateBoundingBox(objModels);

        QVector3D boundingBoxCenter = calculateBoundingBoxCenter(boundingBox);

        

        for (ObjModel& model : objModels)
        {
            model.center = boundingBoxCenter;
        }

        xmlFile.close();

        QStringList objPath;
        int sequenceNumber = 1;

        // Remnants of .obj loader
        for (const ObjModel& model : objModels) {
            //QString objFileName = writeObjFile(model, fileName, sequenceNumber++);
            //objPath.append(objFileName);
        }
        uint i = 0;

        for (const ObjModel& model : objModels) {
            //qDebug() << "Creating Mesh from Data";
            emit Mediator::getInstance().createMeshFromData(model);
            //qDebug() << "Signal sent to create mesh";
        }

        emit Mediator::getInstance().setCameraFromData();
        
/*
        if (objPath.isEmpty())
        {
            qDebug() << "objPath string list is empty";
        }
        else
        {
            qDebug() << objPath.size() << objPath.first() << objPath.last();
        }
*/
        // 
        for (const QString& objFileName : objPath) {
            //emit Mediator::getInstance().addConvertedObjToScene("file:///" + objFileName, true);
            //qDebug() << "Add converted object to scene" << objFileName;

            //QFile::remove(objFileName); // Remove file
                
        }
        
    }

    QPair<QVector3D, QVector3D> Parser::calculateBoundingBox(const QVector<ObjModel>& objModels) {
        if (objModels.isEmpty()) {
            return qMakePair(QVector3D(), QVector3D());
        }

        QVector3D min = objModels.first().vertices.first();
        QVector3D max = objModels.first().vertices.first();

        for (const ObjModel& model : objModels) {
            for (const QVector3D& vertex : model.vertices) {
                min.setX(qMin(min.x(), vertex.x()));
                min.setY(qMin(min.y(), vertex.y()));
                min.setZ(qMin(min.z(), vertex.z()));
                max.setX(qMax(max.x(), vertex.x()));
                max.setY(qMax(max.y(), vertex.y()));
                max.setZ(qMax(max.z(), vertex.z()));
            }
        }

        return qMakePair(min, max);
    }

    QVector3D Parser::calculateBoundingBoxCenter(const QPair<QVector3D, QVector3D>& boundingBox) {
        return (boundingBox.first + boundingBox.second) * 0.5;
    }
    // Parses a single <material> element in the XML
    ObjMaterial Parser::parseMaterialElement(QXmlStreamReader& xmlReader, const uint version) {
        ObjMaterial material;

        while (!(xmlReader.isEndElement() && xmlReader.name() == (QString) "material")) {
            if (xmlReader.isStartElement()) {
                QString elementName = xmlReader.name().toString();
                QXmlStreamAttributes attributes = xmlReader.attributes();

                if (elementName == "ambient") {
                    QVector3D color = parseColor(attributes.value(version >= 2 ? "rgb" : "rgba").toString());
                    material.ka = color;
                }
                else if (elementName == "diffuse") {
                    QVector3D color = parseColor(attributes.value(version >= 2 ? "rgb" : "rgba").toString());
                    material.kd = color;
                }
                else if (elementName == "specular") {
                    QVector3D color = parseColor(attributes.value(version >= 2 ? "rgb" : "rgba").toString());
                    material.ks = color;
                }
                else if (elementName == "emission") {
                    QVector3D color = parseColor(attributes.value(version >= 2 ? "rgb" : "rgba").toString());
                    material.ke = color;
                }
                else if (elementName == "shininess") {
                    material.shininess = attributes.value("val").toFloat();
                }
                else if (elementName == "tex_map") {
                    material.textureMap = attributes.value("source").toString();
                    qDebug() << elementName << "is not empty" << attributes.value("source").toString();
                    
                }
                else if (elementName == "transparency") {
                    material.transparency = version >= 2 ? xmlReader.readElementText().toFloat() : attributes.value("val").toFloat();
                }
                else if (elementName == "two_sided") {
                    material.twoSided = true;
                }
                else if (elementName == "flipped") {
                    material.flipped = true;
                }
                else if (elementName == "extend") {
                    parseExtendElement(xmlReader, material);
                }
            }
            xmlReader.readNext();
        }
        return material;
    }

    QVector3D Parser::parseColor(const QString& colorString) {
        QStringList colors = colorString.split(',');
        if (colors.size() >= 3) {
            return QVector3D(colors[0].toFloat(), colors[1].toFloat(), colors[2].toFloat());
        }
        return QVector3D();
    }

    void Parser::parseExtendElement(QXmlStreamReader& xmlReader, ObjMaterial& material) {
        while (!(xmlReader.isEndElement() && xmlReader.name() == (QString) "extend")) {
            if (xmlReader.isStartElement()) {
                QString elementName = xmlReader.name().toString();
                QXmlStreamAttributes attributes = xmlReader.attributes();

                if (elementName == "metalness") {
                    material.metalness = attributes.value("val").toDouble();
                }
                else if (elementName == "roughness") {
                    material.roughness = attributes.value("val").toDouble();
                }
                else if (elementName == "specular") {
                    material.specularValue = attributes.value("val").toDouble();
                }
                else if (elementName == "emission") {
                    material.emissionColor = parseColor(attributes.value("rgb").toString());
                }
                else if (elementName == "normal") {
                    
                    material.normalMap = attributes.value("source").toString();
                    qDebug() << elementName << "is not empty" << attributes.value("source").toString();
                    
                }
                else if (elementName == "parallax") {
                    material.parallaxMap = attributes.value("source").toString();
                    qDebug() << elementName << "is not empty" << attributes.value("source").toString();

                    
                }
                else if (elementName == "ambientOcclusion") {
                    material.ambientOcclusionMap = attributes.value("source").toString();
                    qDebug() << elementName << "is not empty" << attributes.value("source").toString();
                    
                }
            }
            xmlReader.readNext();
        }
    }
    // Parses the materials.xml and returns a map of material name to material properties
    QMap<QString, ObjMaterial> Parser::parseMaterials(const QString& fileName, const Object3D& properties) {

        QMap<QString, ObjMaterial> materials;
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open the materials.xml file:" << fileName;
            return materials;
        }

        uint version = 0;
        if (properties.version.has_value())
            version = properties.version.value();

        QXmlStreamReader xmlReader(&file);
        xmlReader.readNext();
        while (!xmlReader.atEnd()) {
            if (xmlReader.isStartElement() && xmlReader.name() == (QString)"material") {
                QString matName = xmlReader.attributes().value("name").toString();
                ObjMaterial material = parseMaterialElement(xmlReader, version);
                material.name = matName;
                materials.insert(matName, material);
            }
            else {
                xmlReader.readNext();
            }
        }

        if (xmlReader.hasError()) {
            qWarning() << "XML Error: " << xmlReader.errorString();
        }

        file.close();
        return materials;
    }

    // Writes the .mtl (Material Template Library) file
    void Parser::writeMTLFile(const QMap<QString, ObjMaterial>& materials, const QString& fileName) {
        
        int index = fileName.lastIndexOf('/');
        QString baseName = QFileInfo(fileName).completeBaseName();
        QString objPath = fileName.left(index) + '/' + baseName;

        // TODO: Better naming-match model names
        QString outFileName = QString("%1/%2.mtl").arg(objPath).arg("models");
        //qDebug() << "Temp file obj location:" << outFileName;
        //qDebug() << outFileName;
        // Path to the new directory
        QString dirPath = objPath;

        QDir dir(dirPath);

        if (!dir.exists())
        {
            bool success = dir.mkpath(".");
            if (!success)
            {
                qDebug() << "Failed to create directory.";
            }
            
        }
        
        QFile file(outFileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            qDebug() << "Cannot write to file: " << outFileName;
            return;
        }

        QTextStream out(&file);

        if (materials.isEmpty())
        {
            qDebug() << "Materials map is empty";
        }
        else
        {
            for (const auto& keyValue : materials.toStdMap()) {
                const ObjMaterial& mat = keyValue.second;
                out << "newmtl " << mat.name << "\n";
                out << "Ka " << mat.ka.x() << " " << mat.ka.y() << " " << mat.ka.z() << "\n";
                out << "Kd " << mat.kd.x() << " " << mat.kd.y() << " " << mat.kd.z() << "\n";
                out << "Ks " << mat.ks.x() << " " << mat.ks.y() << " " << mat.ks.z() << "\n";
                out << "Ns " << mat.shininess << "\n";
                out << "d " << mat.transparency << "\n";
                if (!mat.textureMap.isEmpty()) {
                    out << "map_Kd Textures/" << mat.textureMap << "\n";
                }
                out << "\n";
            }
        }

        file.close();
    }
    
    Object3D Parser::parseObject3dFile(const QString& fileName)
    {
        Object3D object3D;
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open file for reading:" << fileName;
            return object3D;
        }

        QXmlStreamReader xml(&file);

        while (!xml.atEnd() && !xml.hasError()) {
            QXmlStreamReader::TokenType token = xml.readNext();

            if (token == QXmlStreamReader::StartElement) {
                if (xml.name() == (QString) "version") {
                    bool ok;
                    uint version = xml.readElementText().toUInt(&ok);
                    if (ok) {
                        object3D.version = version;
                    }
                }
                else if (xml.name() == (QString) "name") {
                    object3D.name = xml.readElementText();
                }
                else if (xml.name() == (QString) "description") {
                    object3D.description = xml.readElementText();
                }
                else if (xml.name() == (QString) "arrangement") {
                    QString arrangement = xml.readElementText();
                    if (arrangement.startsWith("floor:")) {
                        QStringList parts = arrangement.split(':');
                        if (parts.size() == 2) {
                            bool ok;
                            double distance = parts[1].toDouble(&ok);
                            if (ok) {
                                object3D.floorDistance = distance;
                                object3D.arrangement = "floor";
                            }
                        }
                    }
                    else {
                        object3D.arrangement = arrangement;
                    }
                }
                else if (xml.name() == (QString) "groupName") {
                    QString groupName = xml.readElementText();
                    if (!groupName.isEmpty()) {
                        object3D.groupName = groupName;
                    }
                }
            }
        }

        if (xml.hasError()) {
            qDebug() << "XML error:" << xml.errorString();
        }
        qDebug() << "\nObject3d.xml parsed " << object3D.name << object3D.version.value() << "\n";
        file.close();
        return object3D;
    }
    
    
	void Parser::parseXmlToObj(const QString& url, const bool isAsset)
	{   

        QString object3dFileName = url + "/Object3d.xml";
        Object3D objectProperties = parseObject3dFile(object3dFileName);

        QString xmlMaterialsFile = url + "/materials.xml";
        QMap<QString, ObjMaterial> materials = parseMaterials(xmlMaterialsFile, objectProperties);
        // Create MTL file
        const QString mtlFileName = url + "/models.mtl";
        // writeMTLFile(materials, mtlFileName); // Function to write mtl file
        
        // Appending models.xml to path
        QString xmlFileName = url + "/models.xml";
		readXmlIntoModels(xmlFileName, objectProperties, materials, isAsset);
	}
    
    void Parser::fileReader(const QString& importname, const bool isAsset)
    {
        // Get the TEMP folder path.
        //QString tempFolder = QDir::tempPath();
        QString tempFolder = TempDirManager::instance().createTempDir();
        QString extractedFolderName;

        if (importname.endsWith(".xobj3d", Qt::CaseInsensitive))
        {
            QByteArray resourceData;
            if (importname.startsWith(':')) // Resource path
            {
                QFile resourceFile(importname);
                if (!resourceFile.open(QIODevice::ReadOnly)) {
                    qWarning() << "Failed to open resource file:" << importname;
                    return;
                }
                resourceData = resourceFile.readAll();
                resourceFile.close();
            }
            else // Regular file path
            {
                QFile file(importname);
                if (!file.open(QIODevice::ReadOnly)) {
                    qWarning() << "Failed to open file:" << importname;
                    return;
                }
                resourceData = file.readAll();
                file.close();
            }

            mz_zip_archive zip_archive;
            memset(&zip_archive, 0, sizeof(zip_archive));

            mz_bool status = mz_zip_reader_init_mem(&zip_archive, resourceData.constData(), resourceData.size(), 0);
            if (!status)
            {
                mz_zip_error err = mz_zip_get_last_error(&zip_archive);
                const char* err_msg = mz_zip_get_error_string(err);
                qWarning() << "mz_zip_reader_init_mem() failed for resource data";
                qWarning() << "miniz error:" << err << err_msg;
                mz_zip_reader_end(&zip_archive);
                return;
            }
            mz_uint num_files = mz_zip_reader_get_num_files(&zip_archive);

            //qDebug() << "\nTotal number of files: " << num_files << "\n";

            QString zipnameQt(importname);
            extractedFolderName = QFileInfo(zipnameQt).completeBaseName();

            // Adjusting the extractionDir to work with the TEMP folder.
            QDir extractionDir(tempFolder + "/" + extractedFolderName);
            qDebug() << "Folder Name" << extractedFolderName;

            if (!extractionDir.exists())
            {
                extractionDir.mkpath(".");
            }

            for (mz_uint i = 0; i < num_files; i++)
            {
                mz_zip_archive_file_stat file_stat;
                if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
                {
                    qDebug() << "mz_zip_reader_file_stat() failed!\n";
                    mz_zip_reader_end(&zip_archive);
                    return;
                }

                QString filename = extractionDir.absolutePath() + '/' + QString(file_stat.m_filename);
                //qDebug() << "File: " << i << " " << filename << "\n";
                QDir dir;

                if (file_stat.m_is_directory)
                {
                    //qDebug() << file_stat.m_filename << " is a directory\n";
                }

                if (filename.endsWith('/')) // checking if it is a directory
                {
                    dir.mkpath(filename); // creating directory if
                }
                else
                {
                    dir.mkpath(QFileInfo(filename).path());

                    //qDebug() << "Extracting " << filename << "...\n";
                    mz_zip_reader_extract_to_file(&zip_archive, i, filename.toStdString().c_str(), 0);
                }

                int index = filename.lastIndexOf('/');

            }
            mz_zip_reader_end(&zip_archive);

            // Passing xml files location to parser by emitting a signal
            QString importfilepath = tempFolder + "/" + extractedFolderName;
            qDebug() << importfilepath << "IMPORT FILE PATH";
            emit Mediator::getInstance().parseXml(importfilepath, isAsset);
        }
        else if (importname.endsWith(".obj", Qt::CaseInsensitive))
        {
            // User can also import .obj file, but can be removed easily if client doesn't want it
            QString importfilename = "file:///" + importname;

            // Another signal that parses and imports .obj files
            emit Mediator::getInstance().addMeshToScene(importfilename, isAsset);
            
            //qDebug() << importfilename << "imported as an obj file";
        }
    }
}