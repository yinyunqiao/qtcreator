#include "donothingplugin.h"
#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>

#include <QKeySequence>
#include <QStringList>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QtPlugin>

DoNothingPlugin::DoNothingPlugin()
{
    // Do nothing
}

DoNothingPlugin::~DoNothingPlugin()
{
    // Do notning
}

void DoNothingPlugin::extensionsInitialized()
{
    // Do nothing
}

bool DoNothingPlugin::initialize(const QStringList& args, QString *errMsg)
{
    Q_UNUSED(args);
    Q_UNUSED(errMsg);

    // Fetch the action manager
    Core::ActionManager* am = Core::ICore::instance()->actionManager();

    // Create a DoNothing menu
    Core::ActionContainer* ac = am->createMenu("DoNothingPlugin.DoNothingMenu");
    ac->menu()->setTitle("DoNothing");

    // Create a command for "DoNothing".
    QAction *action = new QAction(tr("DoNothing"),this);
    Core::Command* cmd = am->registerAction(action,
        QLatin1String("DoNothingPlugin.DoNothing"),
        Core::Context(Core::Constants::C_GLOBAL));

    // Add DoNothing menu to the menubar
    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(ac, Core::Constants::G_DEFAULT_THREE);

    // Add the "DoNothing" action to the DoNothing menu
    ac->addAction(cmd);

    // Connect the action
    connect(action, SIGNAL(triggered(bool)), this, SLOT(performAction()));
    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag DoNothingPlugin::shutdown()
{
    return SynchronousShutdown;
}

void DoNothingPlugin::performAction()
{
    QMessageBox::information(0, tr("DoNothing Plugin"),
                             tr("Seriously dude, this plugin does nothing"));
}

Q_EXPORT_PLUGIN(DoNothingPlugin)
