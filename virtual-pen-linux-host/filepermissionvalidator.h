#ifndef FILEPERMISSIONVALIDATOR_H
#define FILEPERMISSIONVALIDATOR_H
#include <string>
#include <QString>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;
class FilePermissionValidator
{
public:
    FilePermissionValidator();
    bool canWriteToFile(QString path);
    bool anyFileWriteableRecursive(string path);
private:
    bool traverseFilePath(std::string path);
};



#endif // FILEPERMISSIONVALIDATOR_H
