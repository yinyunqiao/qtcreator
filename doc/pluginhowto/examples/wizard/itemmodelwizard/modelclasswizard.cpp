#include "modelclasswizard.h"
#include "modelnamepage.h"

#include <QFile>
#include <QFileInfo>
#include <cppeditor/cppeditor.h>
#include <cppeditor/cppeditorconstants.h>
#include <coreplugin/basefilewizard.h>

ModelClassWizard::ModelClassWizard(const Core::BaseFileWizardParameters &parameters,QObject *parent)
: Core::BaseFileWizard(parameters, parent)
{
}

ModelClassWizard::~ModelClassWizard()
{
}

QWizard * ModelClassWizard::createWizardDialog(
        QWidget *parent,
        const QString &defaultPath,
        const WizardPageList &extensionPages) const
{
    // Create a wizard
    QWizard* wizard = new QWizard(parent);
    wizard->setWindowTitle("Model Class Wizard");

    // Make our page as first page
    ModelNamePage* page = new ModelNamePage(wizard);
    int pageId = wizard->addPage(page);
    wizard->setProperty("_PageId_", pageId);
    page->setPath(defaultPath);

    // Now add the remaining pages
    foreach (QWizardPage *p, extensionPages)
        wizard->addPage(p);
    return wizard;
}

QString ModelClassWizard::readFile(const QString& fileName, const QMap<QString,QString>&
                                   replacementMap) const
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QString retStr = file.readAll();
    QMap<QString,QString>::const_iterator it = replacementMap.begin();
    QMap<QString,QString>::const_iterator end = replacementMap.end();

    while(it != end)
    {
        retStr.replace(it.key(), it.value());
        ++it;
    }
    return retStr;
}

Core::GeneratedFiles ModelClassWizard::generateFiles(
        const QWizard *w,QString *errorMessage) const
{
    Q_UNUSED(errorMessage);
    Core::GeneratedFiles ret;
    int pageId = w->property("_PageId_").toInt();
    ModelNamePage* page = qobject_cast<ModelNamePage*>(w->page(pageId));

    if(!page)
        return ret;
    ModelClassParameters params = page->parameters();
    QMap<QString,QString> replacementMap;

    replacementMap["{{UPPER_CLASS_NAME}}"] = params.className.toUpper();
    replacementMap["{{BASE_CLASS_NAME}}"] = params.baseClass;
    replacementMap["{{CLASS_NAME}}"] = params.className;
    replacementMap["{{CLASS_HEADER}}"] = QFileInfo(params.headerFile).fileName();

    Core::GeneratedFile headerFile(params.path + "/" + params.headerFile);
    headerFile.setEditorKind(CppEditor::Constants::CPPEDITOR_KIND);

    Core::GeneratedFile sourceFile(params.path + "/" + params.sourceFile);
    sourceFile.setEditorKind(CppEditor::Constants::CPPEDITOR_KIND);

    if(params.baseClass == "QAbstractItemModel")
    {
        headerFile.setContents(readFile(":/CustomProject/ItemModelHeader", replacementMap) );
        sourceFile.setContents(readFile(":/CustomProject/ItemModelSource", replacementMap) );
    }

    else if(params.baseClass == "QAbstractTableModel")
    {
        headerFile.setContents(readFile(":/CustomProject/TableModelHeader", replacementMap) );
        sourceFile.setContents(readFile(":/CustomProject/TableModelSource", replacementMap) );
    }

    else if(params.baseClass == "QAbstractListModel")
    {
        headerFile.setContents(readFile(":/CustomProject/ListModelHeader", replacementMap) );
        sourceFile.setContents(readFile(":/CustomProject/ListModelSource", replacementMap) );
    }

    ret << headerFile << sourceFile;
    return ret;
}
