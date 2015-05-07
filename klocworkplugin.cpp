/****************************************************************************
**
** Copyright (C) 2015 Emenda Nordic AB
** Author: Jacob Lärfors (jacob.larfors@emenda.eu)
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "klocworkplugin.h"

#include "issueoutputpane.h"
#include "klocworkrunner.h"
#include "klocworkpluginconstants.h"
#include "klocworkprojectsettings.h"
#include "optionspage.h"
#include "klocworkoptions.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/idocument.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/statusbarmanager.h>
#include <coreplugin/statusbarwidget.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/iprojectmanager.h>
#include <projectexplorer/miniprojecttargetselector.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/projectpanelfactory.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>

#include <utils/fileutils.h>
#include <utils/qtcassert.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QProcess>
#include <QStatusBar>
#include <QtPlugin>

using namespace Klocwork::Internal;

using namespace ProjectExplorer;

KlocworkPlugin::KlocworkPlugin() {
    m_options = (new KlocworkOptions());
    m_runner = new KlocworkRunner(m_options);
    m_issueOutputPane = new IssueOutputPane(m_runner);
}

KlocworkPlugin::~KlocworkPlugin() {
    // members m_settings, m_runner and m_issueOutputPane are registered with
    // addAutoReleasedObject, and hence are automatically removed and destroyed
}

bool KlocworkPlugin::initialize(const QStringList &arguments,
                                QString *errorString) {

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    QAction *actionKwAnalyse = new QAction(tr("Analyse Current File"), this);
    Core::Command *cmdKwAnalyse = Core::ActionManager::registerAction(
        actionKwAnalyse, Constants::ACTION_ID_ANALYSESELECTION,
        Core::Context(Core::Constants::C_GLOBAL));
    cmdKwAnalyse->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+K")));
    connect(actionKwAnalyse, SIGNAL(triggered()), this,
            SLOT(analyseSelection()));

    QAction *actionKwAnalyseOpen = new QAction(tr("Analyse Open Files"), this);
    Core::Command *cmdKwAnalyseOpen = Core::ActionManager::registerAction(
        actionKwAnalyseOpen, Constants::ACTION_ID_ANALYSEOPEN,
        Core::Context(Core::Constants::C_GLOBAL));
    cmdKwAnalyseOpen->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+O")));
    connect(actionKwAnalyseOpen, SIGNAL(triggered()), this,
            SLOT(analyseOpen()));

    QAction *actionKwClean = new QAction(tr("Clean Klocwork Project"), this);
    Core::Command *cmdKwClean = Core::ActionManager::registerAction(
        actionKwClean, Constants::ACTION_ID_CLEANPROJECT,
        Core::Context(Core::Constants::C_GLOBAL));
    connect(actionKwClean, SIGNAL(triggered()), this,
            SLOT(cleanKlocworkProject()));

    Core::ActionContainer *menu =
        Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("Klocwork"));
    menu->addAction(cmdKwAnalyse);
    menu->addAction(cmdKwAnalyseOpen);
    menu->addAction(cmdKwClean);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)
        ->addMenu(menu);

    addAutoReleasedObject(m_issueOutputPane);

    KlocworkOptionsPage *optionsPage = new KlocworkOptionsPage(m_options);
    connect(optionsPage, SIGNAL(settingsChanged()), SLOT(updateSettings()));
    addAutoReleasedObject(optionsPage);

    connect(Core::DocumentModel::model(),
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &,
                               const QVector<int> &)),
            m_runner,
            SLOT(handleDocumentsChange(const QModelIndex &, const QModelIndex &,
                                       const QVector<int> &)));

    Core::EditorManager *editorManager = Core::EditorManager::instance();
    connect(editorManager, SIGNAL(currentEditorChanged(Core::IEditor *)),
            m_runner, SLOT(handleCurrentEditorChanged(Core::IEditor *)));
    connect(editorManager, SIGNAL(editorOpened(Core::IEditor *)), m_runner,
            SLOT(handleEditorOpened(Core::IEditor *)));

    m_klocworkSettingsPanelFactory = new ProjectExplorer::ProjectPanelFactory;
    m_klocworkSettingsPanelFactory->setPriority(100);
    QString displayName =
        QCoreApplication::translate("KlocworkSettingsPanelFactory", "Klocwork");
    m_klocworkSettingsPanelFactory->setDisplayName(displayName);
    QIcon icon = QIcon(QLatin1String(":/KlocworkPlugin/images/kwLogo.png"));
    m_klocworkSettingsPanelFactory
        ->setSimpleCreateWidgetFunction<KlocworkProjectSettings>(icon);
    ProjectExplorer::ProjectPanelFactory::registerFactory(
        m_klocworkSettingsPanelFactory);

    SessionManager *session = SessionManager::instance();
    connect(session, SIGNAL(projectAdded(ProjectExplorer::Project *)), this,
            SLOT(projectOpenened(ProjectExplorer::Project *)));
    connect(session, SIGNAL(projectRemoved(ProjectExplorer::Project *)), this,
            SLOT(projectClosed(ProjectExplorer::Project *)));

    return true;
}

void KlocworkPlugin::projectOpenened(ProjectExplorer::Project *project) {
    if (project) {
        m_runner->addProject(project);
    }
}

void KlocworkPlugin::projectClosed(ProjectExplorer::Project *project) {
    if (project) {
        m_runner->removeProject(project);
    }
}

void KlocworkPlugin::updateSettings() {}

void KlocworkPlugin::extensionsInitialized() {
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag KlocworkPlugin::aboutToShutdown() {
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void KlocworkPlugin::analyseSelection() {

    Core::IDocument *doc = Core::EditorManager::currentDocument();
    if (doc) {
        Project *project = ProjectTree::currentProject();
        if (project != 0) {
            m_runner->runAnalysis(project->activeTarget()->project(),
                                  QStringList() << doc->filePath().toString());
        }
    }
}

void KlocworkPlugin::analyseOpen() {

    QList<Core::IDocument *> documents = Core::DocumentModel::openedDocuments();

    QStringList files;

    for (Core::IDocument *doc : documents) {
        files.append(doc->filePath().toString());
    }

    if (!files.isEmpty()) {
        Project *project = ProjectTree::currentProject();
        if (project != 0) {
            m_runner->runAnalysis(project->activeTarget()->project(),
                                  files);
        }
    }
}

void KlocworkPlugin::cleanKlocworkProject() {
    Project *project = ProjectTree::currentProject();
    QTC_ASSERT(project != NULL, return );
    m_runner->cleanKlocworkProject(project->activeTarget()->project());
}
