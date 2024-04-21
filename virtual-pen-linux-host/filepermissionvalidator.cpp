#include "filepermissionvalidator.h"
#include <QProcess>
#include <QString>
#include <string>
using namespace std;
FilePermissionValidator::FilePermissionValidator() {}

bool FilePermissionValidator::anyFileWriteableRecursive(std::string path){
    bool anyWriteableFile = false;
    for (const auto & entry : fs::directory_iterator(path)){
        if(entry.is_directory()){
            anyWriteableFile = anyFileWriteableRecursive(entry.path());
        }
        else if(canWriteToFile(QString::fromStdString(entry.path()))){
            return true;
        }
    }
    return anyWriteableFile;
}


bool FilePermissionValidator::canWriteToFile(QString path){
    QStringList arguments;
    arguments << "-c" << path;
    QProcess process;
    process.setReadChannel(QProcess::StandardError);
    process.start("touch", arguments);
    process.waitForFinished();
    QByteArray output = process.readAllStandardError();
    return output.size() == 0;
}
