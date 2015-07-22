#include <QCoreApplication>

#include <QDir>
#include <QFileInfo>
#include <qDebug>

bool RemoveDirectory(QString dirName)
{
    QDir dir(dirName);
    QString tmpdir = "";
    if(!dir.exists()){
        return false;
    }

    QFileInfoList fileInfoList = dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){
            tmpdir = dirName + ("/") + fileInfo.fileName();
            RemoveDirectory(tmpdir);
            dir.rmdir(fileInfo.fileName()); /**< 移除子目录 */
        }
        else if(fileInfo.isFile()){
            QFile tmpFile(fileInfo.fileName());
            dir.remove(tmpFile.fileName()); /**< 删除临时文件 */
        }
        else{
            ;
        }
    }

    /**< 返回上级目录，因为只有返回上级目录，才可以删除这个目录 */
    QString strDirName = dir.dirName();
    dir.cdUp();
    dir.rmdir(strDirName);
    return true;
}



void TraverseFiles(int level, const QString &strPath)
{
    QString strDepth(level, '-');
    QDir dir(strPath);
    QStringList listDir;
    QStringList::Iterator iter;

    // 遍历目录
    listDir = dir.entryList(QDir::Dirs, QDir::Name);
    for(iter = listDir.begin(); iter != listDir.end(); ++iter)
    {
        //! 0层只遍历com开头的目录
        if(level == 0 && !iter->startsWith("com.")) continue;
        if( "." == *iter ||".." == *iter ) continue;
        if("debug" == *iter ||
            "release" == *iter ||
            "GeneratedFiles" == *iter ||
            "Win32" == *iter)
        {
            qDebug() << strDepth + "<DELETE>" + *iter + "(folder)";;
            QString strDirName = strPath + "/" + *iter;
            RemoveDirectory(strDirName);
        }
        else
        {
            qDebug() << strDepth + *iter + "(folder)";
        }

        TraverseFiles(level+1, strPath + "/" + *iter);
    }

    //! 0 层不遍历文件
    if(level == 0) return;

    // 遍历文件
    listDir = dir.entryList(QDir::Files, QDir::Name);
    for(iter = listDir.begin(); iter != listDir.end(); ++iter)
    {
        if(iter->endsWith(".Release") ||
            iter->endsWith(".Debug") ||
            iter->endsWith(".pdb") ||
            iter->endsWith(".vcxproj") ||
            iter->endsWith(".vcxproj.filters") ||
            iter->endsWith(".vcxproj.user") ||
            iter->endsWith(".bak") ||
            iter->endsWith(".pro.user") ||
            iter->endsWith(".sdf"))
        {
            QString strKey = "com.";
            qint32 nStart = strPath.indexOf(strKey);
            if(nStart < 0) continue;
            nStart += strKey.length();
            qint32 nEnd = strPath.indexOf("/", nStart);
            if(nEnd < 0) continue;
            QString strProjectName = strPath.mid(nStart, nEnd - nStart);;
            strProjectName.replace(".", "_");

            qDebug() << strDepth + "<DELETE>" + *iter + "(file)" + "[" + strProjectName + "]";
            QString strFileName = strPath + "/" + *iter;

            QFile::remove(strFileName);
        }
        else
        {
            qDebug() << strDepth + *iter + "(file)" ;
        }
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TraverseFiles(0, ".");

    return 0;
}
