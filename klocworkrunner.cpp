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

#include "klocworkrunner.h"

#include "klocworkoptions.h"
#include "klocworkpluginconstants.h"
#include "klocworkproject.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/idocument.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/progressmanager/futureprogress.h>
#include <coreplugin/progressmanager/progressmanager.h>

#include <projectexplorer/project.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/target.h>

#include <utils/qtcassert.h>

#include <QDir>
#include <QMessageBox>
#include <QRegularExpression>
#include <QThread>

using namespace Klocwork::Internal;
using namespace ProjectExplorer;

namespace {

const char KW_REPORT_FORMAT[] = "scriptable";
const char KW_LICENCE_HOST[] = "--license-host";
const char KW_LICENCE_PORT[] = "--license-port";
const char KW_SHOW_SYSTEM_ISSUES[] = "--system";

const char KW_CMD_RUN[] = "run";
const char KW_CMD_SET_STATUS[] = "set-status";
const char KW_CMD_CREATE[] = "create";
const char KW_CMD_SET[] = "set";

const char KW_OPTION_STATUS[] = "--status";

const int CHECK_DELAY_IN_MS = 200;

enum ErrorField {
    ErrorFieldFile = 0,
    ErrorFieldLine,
    ErrorFieldSeverity,
    ErrorFieldId,
    ErrorFieldMessage
};

enum KlocworkCommand { KwRun = 0, KwSetStatus, KwSetProperty, KwCreate };
}

KlocworkRunner::KlocworkRunner(KlocworkOptions *options, QObject *parent)
    : QObject(parent), m_futureInterface(0), m_options(options) {

    Q_ASSERT(m_options != 0);

    m_timer.setSingleShot(true);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(runQueuedAnalysis()));
    connect(&m_process, SIGNAL(readyReadStandardOutput()), SLOT(readOutput()));
    connect(&m_process, SIGNAL(readyReadStandardError()), SLOT(readError()));
    connect(&m_process, SIGNAL(started()), SLOT(started()));
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)),
            SLOT(error(QProcess::ProcessError)));
    connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            SLOT(finished(int, QProcess::ExitStatus)));
}

KlocworkRunner::~KlocworkRunner() {
    if (m_process.isOpen()) {
        m_process.terminate();
    }
}

void KlocworkRunner::handleDocumentsChange(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight,
                                           const QVector<int> &roles) {
    Q_UNUSED(roles);

    if (m_options->checkOnSave()) {
        int beginRow = topLeft.row();
        int endRow = bottomRight.row();

        QStringList filesToAnalyse;
        for (int row = beginRow; row <= endRow; ++row) {
            Core::DocumentModel::Entry *entry =
                Core::DocumentModel::entryAtRow(row);
            if (entry == 0) {
                continue;
            }
            Core::IDocument *document = entry->document;
            if (document == 0) {
                continue;
            }

            if (document->isModified() == false) {
                filesToAnalyse.append(document->filePath().toString());
            }
        }
        if (!filesToAnalyse.isEmpty()) {
            runAnalysis(getActiveProject(), filesToAnalyse);
        }
    }
}

void KlocworkRunner::handleCurrentEditorChanged(Core::IEditor *editor) {
    if (editor != 0) {
        // if editor changed, update filters, if filter current file issues is
        // applied
        emit updateIssueFilter();
    }
}

void KlocworkRunner::handleEditorOpened(Core::IEditor *editor) {
    if (editor != 0 && m_options->checkOnSave()) {
        runAnalysis(getActiveProject(),
                    QStringList() << editor->document()->filePath().toString());
    }
}

void KlocworkRunner::addProject(Project *project) {
    getKlocworkProject(project);
    connect(project, SIGNAL(aboutToSaveSettings()), this,
            SLOT(updateProject()));
}

void KlocworkRunner::removeProject(Project *project) {

    m_klocworkProjects.remove(project->id());

    disconnect(project, SIGNAL(aboutToSaveSettings()), this,
               SLOT(updateProject()));
}

KlocworkProject *
KlocworkRunner::getKlocworkProject(ProjectExplorer::Project *project) {
    Project *activeProject = project->activeTarget()->project();
    QTC_ASSERT(activeProject != 0, return 0);

    // only handle currently active project
    if (project->id() != activeProject->id()) {
        return 0;
    }

    KlocworkProject *kwProject = m_klocworkProjects[project->id()];

    if (kwProject == 0) {
        kwProject = new KlocworkProject(project);
        m_klocworkProjects[project->id()] = kwProject;
    }
    return kwProject;
}

Project *KlocworkRunner::getActiveProject() {
    Project *project = ProjectTree::currentProject();
    if (project != 0) {
        return project->activeTarget()->project();
    }
    return 0;
}

void KlocworkRunner::createProject(KlocworkProject *kwProject) {

    QDir dir = QDir(kwProject->getKlocworkProjectDir());

    if (!dir.cdUp()) {
        if (!dir.exists()) {
            if (!QDir().mkpath(dir.absolutePath())) {
                // if cannot create parent directory, return
                Core::MessageManager::write(
                    tr("Error: cannot create directory ") + dir.absolutePath(),
                    Core::MessageManager::Flash);
                return;
            } else {
                // simply delete .kwlp directory or Klocwork wil complain that
                // it already
                // exists
                dir.cdUp();
                dir.rmdir(
                    QFileInfo(kwProject->getKlocworkProjectDir()).fileName());
            }
        }
    }

    KlocworkCommand *cmd = new KlocworkCommand(kwProject, KwcheckCreateProject,
                                               m_options->binaryFile());
    cmd->args << tr(KW_CMD_CREATE);
    cmd->args << tr("--project-dir");
    cmd->args << kwProject->getKlocworkProjectDir();
    cmd->args << tr("--settings-dir");
    cmd->args << kwProject->getKlocworkSettingsDir();
    if (!kwProject->getKlocworkServerProject().isEmpty()) {
        cmd->args << tr("--project");
        cmd->args << kwProject->getKlocworkServerProject();
    }

    startProcess(cmd);
}

void KlocworkRunner::updateProject() {
    Project *project = static_cast<Project *>(QObject::sender());

    if (project != 0) {

        KlocworkProject *kwProject = getKlocworkProject(project);

        if (kwProject && kwProject->updateSettings()) {
            if (kwProject != 0 &&
                !kwProject->getKlocworkServerProject().isEmpty()) {
                KlocworkCommand *cmd = new KlocworkCommand(
                    kwProject, KwcheckSet, m_options->binaryFile());
                cmd->args << tr(KW_CMD_SET);
                cmd->args << tr("--project-dir");
                cmd->args << kwProject->getKlocworkProjectDir();
                if (!kwProject->getKlocworkServerProject().isEmpty()) {
                    cmd->args << tr("klocwork.project=") +
                                     kwProject->getKlocworkServerProject();
                }
                if (!kwProject->getKlocworkServerHost().isEmpty()) {
                    cmd->args << tr("klocwork.host=") +
                                     kwProject->getKlocworkServerHost();
                }
                if (!kwProject->getKlocworkServerPort().isEmpty()) {
                    cmd->args << tr("klocwork.port=") +
                                     kwProject->getKlocworkServerPort();
                }
                startProcess(cmd);
            }
        }
    }
}

void KlocworkRunner::cleanKlocworkProject(Project *project) {
    KlocworkProject *kwProject = getKlocworkProject(project);
    if (kwProject) {
        cleanKlocworkProject(getKlocworkProject(project));
    }
}

void KlocworkRunner::cleanKlocworkProject(KlocworkProject *kwProject) {
    QDir(kwProject->getKlocworkProjectDir()).removeRecursively();
    QDir(kwProject->getKlocworkSettingsDir()).removeRecursively();
}

bool KlocworkRunner::checkExistingProject(KlocworkProject *kwProject) {
    if (!QDir(kwProject->getKlocworkProjectDir()).exists() ||
        !QDir(kwProject->getKlocworkSettingsDir()).exists()) {
        Core::MessageManager::write(
            tr("Cleaning up Klocwork project and re-creating..."),
            Core::MessageManager::Silent);
        // run cleanup operation and re-create Klocwork project
        cleanKlocworkProject(kwProject);
        createProject(kwProject);
        return true;
    }
    return false;
}

void KlocworkRunner::addLicenseOptions(QStringList &args) {
    if (!m_options->kwLicenseHost().isEmpty()) {
        args << tr(KW_LICENCE_HOST);
        args << m_options->kwLicenseHost();
    }
    if (!m_options->kwLicensePort().isEmpty()) {
        args << tr(KW_LICENCE_PORT);
        args << m_options->kwLicensePort();
    }
}

void KlocworkRunner::changeIssueStatus(Project *project, const QString &status,
                                       const QString &comment,
                                       const QString &id) {

    KlocworkProject *kwProject = getKlocworkProject(project);

    KlocworkCommand *cmd = new KlocworkCommand(kwProject, KwcheckSetStatus,
                                               m_options->binaryFile());
    cmd->program = m_options->binaryFile();
    cmd->args << tr(KW_CMD_SET_STATUS);
    addLicenseOptions(cmd->args);
    cmd->args << tr("--project-dir");
    cmd->args << kwProject->getKlocworkProjectDir();
    cmd->args << tr(KW_OPTION_STATUS);
    cmd->args << status;

    if (!comment.isEmpty()) {
        cmd->args << tr("--comment");
        cmd->args << comment;
    }

    cmd->args << id;
    startProcess(cmd);
}

void KlocworkRunner::runAnalysis(ProjectExplorer::Project *project,
                                 const QStringList &filesToCheck) {

    QTC_ASSERT(project != 0, return );

    // file save triggers call more than once in quick succession. Timer removes
    // chance of duplicate analysis.
    if (m_timer.isActive()) {
        return;
    }

    // check that there is not an analysis running or pending for another
    // project
    if (!m_analysisQueue.isEmpty() &&
        m_analysisKwProject->getProjectId() != project->id()) {
        QMessageBox::warning(0, tr("Klocwork Warning"),
                             tr("Can only analyse one project at a time"));
        return;
    }

    m_analysisKwProject = getKlocworkProject(project);

    // filesToCheck contains the files that were last changed.
    // If analysis is already running/scheduled, then we want to know if
    // the file(s) under analysis are the same or not.
    //
    // If they are not the same, we should analyse the file(s)
    // under analysis again, as well as the newly updated file(s)
    if (m_analysisQueue != filesToCheck) {
        m_analysisQueue += filesToCheck;
        m_analysisQueue.removeDuplicates();
        m_analysisQueue.sort();
    }

    // check if current command is an analysis, and we want to restart it if it
    // is,
    // so kill the current analysis. If current process is running which is not
    // an
    // analysis, then analysis will be triggered once that command is finished
    if (m_process.isOpen()) {
        if (m_currentCmd != 0 && m_currentCmd->cmdType == KwcheckRun) {
            m_process.kill();
        }
    } else {
        // if no current process is running, start the timer
        m_timer.start(CHECK_DELAY_IN_MS);
    }
}

void KlocworkRunner::runQueuedAnalysis() {

    // before running the analysis, check the Klocwork project.
    // If checkExistingProject returns true, then project needs to be created,
    // so
    // return and wait for that process to be finished, which will then trigger
    // the analysis
    if (checkExistingProject(m_analysisKwProject)) {
        return;
    }

    KlocworkCommand *cmd = new KlocworkCommand(m_analysisKwProject, KwcheckRun,
                                               m_options->binaryFile());
    cmd->args << tr(KW_CMD_RUN);

    cmd->args << tr("--project-dir");
    cmd->args << m_analysisKwProject->getKlocworkProjectDir();

    addLicenseOptions(cmd->args);

    cmd->args << tr("-b");
    cmd->args << m_analysisKwProject->getKlocworkBuildSpec();
    cmd->args << tr(KW_SHOW_SYSTEM_ISSUES);
    cmd->args << tr("-F");
    cmd->args << tr(KW_REPORT_FORMAT);
    cmd->args << tr("--report");
    cmd->args << m_analysisKwProject->getKlocworkReportsFile();
    cmd->args << tr(KW_OPTION_STATUS);
    cmd->args << tr(Constants::KW_STATUS_LIST);
    cmd->args += m_analysisQueue;

    startProcess(cmd);
}

void KlocworkRunner::runQueuedCmd() {
    if (m_process.isOpen()) {
        // return, as queued jobs will be run after current process is finished
        return;
    }

    m_cmdMutex.lock();
    KlocworkCommand *cmd = m_cmdQueue.dequeue();
    m_cmdMutex.unlock();
    startProcess(cmd);
}

void KlocworkRunner::startProcess(KlocworkCommand *cmd) {
    if (m_process.isOpen()) {
        m_cmdMutex.lock();
        m_cmdQueue.enqueue(cmd);
        m_cmdMutex.unlock();
        return;
    }

    m_currentCmd = cmd;

    if (m_currentCmd->program.isEmpty() ||
        !QFile(m_currentCmd->program).exists() ||
        !QFileInfo(m_currentCmd->program).isExecutable()) {
        Core::MessageManager::write(
            tr("Error: no executable specified, "
               ""
               ""
               "or executable specified does not exist or is not executable."
               ""
               ""
               " Please check the Klocwork Plugin IDE options."),
            Core::MessageManager::Flash);
        return;
    }

    Core::MessageManager::write(tr("Calling: ") + m_currentCmd->program +
                                    tr(" ") + m_currentCmd->args.join(tr(" ")),
                                Core::MessageManager::Silent);
    m_process.start(cmd->program, cmd->args);
}

void KlocworkRunner::analysisForceCancelled() {
    if (m_process.isOpen()) {
        m_process.kill();
    }
    m_analysisQueue.clear();
}

void KlocworkRunner::readOutput() {
    m_process.setReadChannel(QProcess::StandardOutput);

    while (!m_process.atEnd()) {
        QByteArray rawLine = m_process.readLine();
        QString line = QString::fromUtf8(rawLine).trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QRegularExpression reProgress(tr("(\\d+)(%)"));
        QRegularExpressionMatch match = reProgress.match(line);
        if (match.hasMatch()) {
            int progress = match.captured(1).toInt();
            if (progress < 100) {
                m_futureInterface->setProgressValue(progress);
            }
        }

        Core::MessageManager::write(line, Core::MessageManager::Silent);
    }
}

void KlocworkRunner::readError() {
    m_process.setReadChannel(QProcess::StandardError);

    while (!m_process.atEnd()) {
        QByteArray rawLine = m_process.readLine();
        QString line = QString::fromUtf8(rawLine).trimmed();
        if (line.isEmpty()) {
            continue;
        }
        // Core::MessageManager::write (line, Core::MessageManager::Silent);
        Core::MessageManager::write(line, Core::MessageManager::Silent);
    }
}

void KlocworkRunner::started() {
    emit klocworkProcessStarted();
    Core::MessageManager::write(tr("Klocwork Process Started"),
                                Core::MessageManager::Silent);

    m_futureInterface = new QFutureInterface<void>;
    Core::FutureProgress *progress = Core::ProgressManager::addTask(
        m_futureInterface->future(), tr("Klocwork"), Core::Id("Klocwork_TASK"));

    connect(progress, SIGNAL(canceled()), this, SLOT(analysisForceCancelled()));
    m_futureInterface->setProgressRange(0, 100);
    m_futureInterface->reportStarted();
}

void KlocworkRunner::error(QProcess::ProcessError error) {
    Q_UNUSED(error);

    Core::MessageManager::write(tr("Klocwork error occured."),
                                Core::MessageManager::Flash);
}

void KlocworkRunner::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode);

    QTC_ASSERT(m_futureInterface != 0, return );

    m_futureInterface->reportFinished();
    m_process.close();

    // if analysis finished without being interrupted/killed
    if (exitStatus == QProcess::NormalExit) {

        switch (m_currentCmd->cmdType) {
        case KwcheckRun: {
            emit updateAnalysisResults(
                m_currentCmd->kwProject->getProject(),
                m_currentCmd->kwProject->getKlocworkReportsFile(),
                m_analysisQueue);
            m_analysisQueue.clear();
            break;
        }
        case KwcheckSetStatus: {
            // status arument will come after KW_OPTION_STATUS, so add one
            int indexOfStatus =
                m_process.arguments().indexOf(QRegExp(tr(KW_OPTION_STATUS))) +
                1;
            // last argument of set-status command will always be the issue ID
            emit updateIssueStatus(m_currentCmd->kwProject->getProject(),
                                   m_process.arguments().last(),
                                   m_process.arguments().at(indexOfStatus));
            break;
        }
        default:
            break;
        }
    }

    Core::MessageManager::write(tr("Klocwork Process Finished"),
                                Core::MessageManager::Silent);

    delete m_currentCmd;
    m_currentCmd = 0;
    if (!m_cmdQueue.isEmpty()) {
        Core::MessageManager::write(tr("Starting queued command."),
                                    Core::MessageManager::Silent);
        runQueuedCmd();
    } else if (!m_analysisQueue.isEmpty()) {
        Core::MessageManager::write(tr("Starting queued analysis."),
                                    Core::MessageManager::Silent);
        runQueuedAnalysis();

    } else {
        emit klocworkProcessFinished();
    }
}
