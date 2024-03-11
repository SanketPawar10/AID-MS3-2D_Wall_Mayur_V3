#include <QTemporaryDir>
#include <QVector>

class TempDirManager {
public:
    static TempDirManager& instance() {
        static TempDirManager _instance;
        return _instance;
    }

    QString createTempDir() {
        QTemporaryDir* tempDir = new QTemporaryDir();
        if (tempDir->isValid()) {
            QString path = tempDir->path();
            tempDirs.append(tempDir); // Takes ownership of the directory
            qDebug() << "Temporary directory created at:" << path;
            return path;
        }
        else {
            qDebug() << "Failed to create temporary directory.";
            delete tempDir;
            return QString();
        }
    }

    ~TempDirManager() {
        for (QTemporaryDir* dir : tempDirs) {
            if (dir->isValid()) {
                qDebug() << "Removing temporary directory:" << dir->path();
            }
            delete dir;
        }
        tempDirs.clear();
    }

private:
    TempDirManager() {} // Constructor is private for singleton
    QVector<QTemporaryDir*> tempDirs; // Container for temporary directories

    // Prevent copying
    TempDirManager(const TempDirManager&) = delete;
    TempDirManager& operator=(const TempDirManager&) = delete;
};
