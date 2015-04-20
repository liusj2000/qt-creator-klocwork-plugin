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

#ifndef ISSUEOUTPUTWINDOW_H
#define ISSUEOUTPUTWINDOW_H

#include "klocworkreportparser.h"

#include <coreplugin/id.h>
#include <coreplugin/ioutputpane.h>

#include <QAction>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QFile>
#include <QHash>
#include <QSet>
#include <QToolButton>
#include <QMenu>

namespace ProjectExplorer {
class Project;
}

namespace Klocwork {
namespace Internal {

class KlocworkIssueTreeItem;
class KlocworkRunner;
class KlocworkProjectTreeItem;


/*
 * A wiget that displays the Klocwork results.
 * Provides filtering functionality, and also the ability to change
 * the status of Klocwork issues.
 * Receives signals from KlocworkRunner when analysis results have updated or current
 * file has changed as to update any filters accordingly
 */
class IssueOutputPane : public Core::IOutputPane {

    Q_OBJECT

  public:
    explicit IssueOutputPane(KlocworkRunner *runner, QObject *parent = 0);
    ~IssueOutputPane();

    QString displayName() const;

    bool canFocus() const;
    bool canNavigate() const;
    bool canNext() const;
    bool canPrevious() const;
    void clearContents();
    void goToNext();
    void goToPrev();
    bool hasFocus() const;
    QWidget *outputWidget(QWidget *parent);
    int priorityInStatusBar() const;
    void setFocus();
    QList<QWidget *> toolBarWidgets() const;
    void visibilityChanged(bool visible);

    void filterIssue(QTreeWidgetItem *);
    void updateView(QTreeWidgetItem *);

  public slots:
    // updates the current filter on each Klocwork issue
    void updateView();

  signals:
    //sent when the status of a particular issue should be changed
    void doChangeIssueStatus(const QString &, const QString &, const QString &);

  private slots:
    void updateAnalysisResults(ProjectExplorer::Project *, QString,
                               QStringList);
    void updateIssueStatus(ProjectExplorer::Project *, QString, QString);
    void showIssueInEditor(QTreeWidgetItem *, int);
    void showChangeStatusListMenu();
    void updateChangeStatusListMenu();
    void showFilterStatusListMenu();
    void changeIssueStatus();
    void changedIssueStatusFilter();
    void filterIssueStatus();
    void klocworkProcessStarted();
    void klocworkProcessFinished();
    void setSelected();

  private:
    void initQToolButtons();
    void initStatusButtons(QMenu *, QMenu *);
    KlocworkProjectTreeItem *getKlocworkProject(QTreeWidgetItem *);
    KlocworkProjectTreeItem *getKlocworkProject(ProjectExplorer::Project *);

    KlocworkRunner *m_runner;
    QTreeWidget *m_treeWidget;
    QToolButton *m_filterOrigin;
    QToolButton *m_filterCurrentFile;
    QToolButton *m_changeStatusDropDown;
    QToolButton *m_filterStatusDropDown;
    QMenu *m_changeStatusListMenu;
    QMenu *m_filterStatusListMenu;

    KlocworkReportParser m_klocworkReportParser;

    QStringList m_issueFilterStatuses;

    QHash<Core::Id, KlocworkProjectTreeItem *> m_klocworkProjects;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // ISSUEOUTPUTWINDOW_H
