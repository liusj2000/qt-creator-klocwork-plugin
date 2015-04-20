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

#ifndef KlocworkRunner_H
#define KlocworkRunner_H

#include <coreplugin/id.h>

#include <QFuture>
#include <QHash>
#include <QMutex>
#include <QProcess>
#include <QProgressBar>
#include <QQueue>
#include <QTimer>

namespace Core {
class IEditor;
}

namespace ProjectExplorer {
class Project;
}

namespace Klocwork {
namespace Internal {

class KlocworkOptions;
class AnalysisOutputPane;
class IssueOutputPane;
class KlocworkProject;

enum KlocworkCommandType {
    KwcheckCreateProject,
    KwcheckRun,
    KwcheckSet,
    KwcheckSetStatus,
};

typedef struct kwCmd {
    kwCmd(KlocworkProject *project, KlocworkCommandType type,
          const QString program)
        : cmdType(type), kwProject(project), program(program) {}
    KlocworkCommandType cmdType;
    KlocworkProject *kwProject;
    QString program;
    QStringList args;
    QStringList files;
} KlocworkCommand;

/*
 *  Klocwork Analysis Runner
 *  Creates local Klocwork project, updates project settings,
 *  launches the Klocwork analysis, generates report file
 */
class KlocworkRunner : public QObject {
    Q_OBJECT
  public:
    explicit KlocworkRunner(KlocworkOptions *settings, QObject *parent = 0);
    ~KlocworkRunner();
    // creates KlocworkCommand to create local Klocwork project
    void createProject(KlocworkProject *);
    // appends files to analyse to m_analysisQueue and starts timer.
    // When timer finishes, a KlocworkCommand to run an analysis is created
    void runAnalysis(ProjectExplorer::Project *, const QStringList &);
    // removes local Klocwork project
    void cleanKlocworkProject(ProjectExplorer::Project *);
    void cleanKlocworkProject(KlocworkProject *);
    // creates KlocworkCommand to change the status of a Klocwork issue
    void changeIssueStatus(ProjectExplorer::Project *, const QString &,
                           const QString &, const QString &);
    // adds new project to list of Klocwork projects
    void addProject(ProjectExplorer::Project *);
    void removeProject(ProjectExplorer::Project *);

signals:
    void updateAnalysisResults(ProjectExplorer::Project *, const QString &,
                               const QStringList);
    void updateIssueStatus(ProjectExplorer::Project *, const QString &,
                           const QString &);
    void updateIssueFilter();
    void klocworkProcessStarted();
    void klocworkProcessFinished();

  public slots:
    void updateProject();

  private slots:
    // signalled if editor changes, but only acts upon file save
    void handleDocumentsChange(const QModelIndex &topLeft,
                               const QModelIndex &bottomRight,
                               const QVector<int> &roles);
    // signals to the Klocwork issue list to be updated in case file-specific
    // filters
    // are applied
    void handleCurrentEditorChanged(Core::IEditor *);
    void handleEditorOpened(Core::IEditor *);

    void runQueuedAnalysis();
    void runQueuedCmd();

    // QProcess handling.
    void readOutput();
    void readError();
    void started();
    void error(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void analysisForceCancelled();

  private:
    // checks that there exists a Klocwork project and creates if not
    bool checkExistingProject(KlocworkProject *);
    void startProcess(KlocworkCommand *);
    // adds Klocwork licencing options to argument list
    void addLicenseOptions(QStringList &args);
    // method to get (or create) KlocworkProject from m_klocworkProjects
    KlocworkProject *getKlocworkProject(ProjectExplorer::Project *);
    // get the currently active project. This is taken from the active target
    ProjectExplorer::Project *getActiveProject();

    // Binary runner.
    QProcess m_process;
    // timer is used in case of file save (which triggers multiple signals with
    // very short concession). Timer ensures we only run the analysis once
    QTimer m_timer;
    // used for the progress bar
    QFutureInterface<void> *m_futureInterface;

    // Plugin's settings
    KlocworkOptions *m_options;

    // store the currently running command
    KlocworkCommand *m_currentCmd;
    // store the currently (or queued) running Klocwork project
    KlocworkProject *m_analysisKwProject;
    // list of source files to be analysed by Klocwork
    QStringList m_analysisQueue;

    // in case multiple commands are called at the same time, like create
    // Klocwork project and update existing project.
    QQueue<KlocworkCommand *> m_cmdQueue;
    // mutex is used to make m_cmdQueue thread safe
    QMutex m_cmdMutex;

    // stores instances of KlocworkProjects, one for each
    // ProjectExplorer::Project
    // to be analysed
    QHash<Core::Id, KlocworkProject *> m_klocworkProjects;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // KlocworkRunner_H
