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

#include "issueoutputpane.h"

#include "klocworkissuetreeitem.h"
#include "klocworkpluginconstants.h"
#include "klocworkprojecttreeitem.h"
#include "klocworkrunner.h"

#include <coreplugin/idocument.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>

#include <projectexplorer/project.h>

#include <texteditor/textmark.h>

#include <utils/qtcassert.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QString>
#include <QTextStream>

using namespace Klocwork::Internal;
using namespace Klocwork::Constants;

namespace {
const char TOOLTIP_CHANGE_ISSUE_STATUS[] = "Change selected issue status.";
const char TOOLTIP_FILTER_ISSUE_STATUS[] = "Filter issues by status.";
const char TOOLTIP_FILTER_CURRENT_FILE[] = "Show issues in current file only.";
const char TOOLTIP_FILTER_LOCAL[] = "Show local issues only.";
const char TOOLTIP_NOT_CHANGE_ISSUE_STATUS[] =
    "Cannot change status whilst analysis is running.";
}

IssueOutputPane::IssueOutputPane(KlocworkRunner *runner, QObject *parent)
    : Core::IOutputPane(parent), m_runner(runner) {
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setColumnCount(KlocworkIssueColumn::NumFields);
    m_treeWidget->setHeaderLabels(
        QStringList() << tr("") << tr("Message") << tr("File") << tr("Line")
                      << tr("Code") << tr("Origin") << tr("Status")
                      << tr("Severity") << tr("ID"));

    // some column width settings
    m_treeWidget->setColumnWidth(KlocworkIssueColumn::Icon, 100);
    m_treeWidget->setColumnWidth(KlocworkIssueColumn::Message, 300);
    m_treeWidget->setColumnWidth(KlocworkIssueColumn::File, 150);
    m_treeWidget->setColumnWidth(KlocworkIssueColumn::LineNo, 50);

    initQToolButtons();

    m_changeStatusListMenu = new QMenu();
    m_filterStatusListMenu = new QMenu();

    initStatusButtons(m_changeStatusListMenu, m_filterStatusListMenu);

    m_changeStatusDropDown = new QToolButton();
    m_changeStatusDropDown->setIcon(
        QIcon(QString::fromUtf8(":/KlocworkPlugin/images/doTriage.png")));
    m_changeStatusDropDown->setMenu(m_changeStatusListMenu);
    m_changeStatusDropDown->setToolTip(tr(TOOLTIP_CHANGE_ISSUE_STATUS));

    m_filterStatusDropDown = new QToolButton();
    m_filterStatusDropDown->setIcon(
        QIcon(QString::fromUtf8(":/KlocworkPlugin/images/filtericon.png")));
    m_filterStatusDropDown->setMenu(m_filterStatusListMenu);
    m_filterStatusDropDown->setToolTip(tr(TOOLTIP_FILTER_ISSUE_STATUS));

    connect(m_runner, SIGNAL(updateIssueFilter()), this, SLOT(updateView()));

    connect(m_runner, SIGNAL(updateAnalysisResults(ProjectExplorer::Project *,
                                                   QString, QStringList)),
            this, SLOT(updateAnalysisResults(ProjectExplorer::Project *,
                                             QString, QStringList)));
    connect(
        m_runner,
        SIGNAL(updateIssueStatus(ProjectExplorer::Project *, QString, QString)),
        this,
        SLOT(updateIssueStatus(ProjectExplorer::Project *, QString, QString)));
    connect(m_runner, SIGNAL(klocworkProcessStarted()), this,
            SLOT(klocworkProcessStarted()));
    connect(m_runner, SIGNAL(klocworkProcessFinished()), this,
            SLOT(klocworkProcessFinished()));

    connect(m_changeStatusDropDown, SIGNAL(clicked()), this,
            SLOT(showChangeStatusListMenu()));
    connect(m_changeStatusListMenu, SIGNAL(aboutToShow()), this,
            SLOT(updateChangeStatusListMenu()));
    connect(m_filterStatusDropDown, SIGNAL(clicked()), this,
            SLOT(showFilterStatusListMenu()));
    connect(m_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(showIssueInEditor(QTreeWidgetItem *, int)));
}

IssueOutputPane::~IssueOutputPane() {
    m_treeWidget->clear();
    m_klocworkProjects.clear();
}

void IssueOutputPane::initQToolButtons() {
    m_filterOrigin = new QToolButton();
    m_filterOrigin->setIcon(
        QIcon(QString::fromUtf8(":/KlocworkPlugin/images/showLocal.png")));
    m_filterOrigin->setCheckable(true);
    m_filterOrigin->setToolTip(tr(TOOLTIP_FILTER_LOCAL));
    connect(m_filterOrigin, SIGNAL(clicked()), this, SLOT(updateView()));

    m_filterCurrentFile = new QToolButton();
    m_filterCurrentFile->setIcon(QIcon(
        QString::fromUtf8(":/KlocworkPlugin/images/showCurrentFile.png")));
    m_filterCurrentFile->setCheckable(true);
    m_filterCurrentFile->setToolTip(tr(TOOLTIP_FILTER_CURRENT_FILE));
    connect(m_filterCurrentFile, SIGNAL(clicked()), this, SLOT(updateView()));
}

void IssueOutputPane::initStatusButtons(QMenu *statusChangeMenu,
                                        QMenu *statusFilterMenu) {
    QStringList statusList = tr(Constants::KW_STATUS_LIST).split(tr(","));

    // for each status, create a filter and change status button
    for (QStringList::const_iterator it = statusList.constBegin();
         it != statusList.constEnd(); ++it) {

        QAction *statusChange = new QAction(0);
        QAction *statusFilter = new QAction(0);

        statusChange->setText(*it);
        statusFilter->setText(*it);

        statusChangeMenu->addAction(statusChange);

        statusFilter->setCheckable(true);

        statusFilterMenu->addAction(statusFilter);

        connect(statusChange, SIGNAL(triggered()), this,
                SLOT(changeIssueStatus()));

        connect(statusFilter, SIGNAL(changed()), this,
                SLOT(changedIssueStatusFilter()));
        connect(statusFilter, SIGNAL(triggered()), this,
                SLOT(filterIssueStatus()));

        if ((*it) == tr("Analyze") || (*it) == tr("Fix")) {
            statusFilter->setChecked(true);
        }
    }
}

QString IssueOutputPane::displayName() const { return tr("Klocwork Issues"); }

bool IssueOutputPane::canFocus() const { return true; }
bool IssueOutputPane::canNavigate() const { return false; }
bool IssueOutputPane::canNext() const { return false; }
bool IssueOutputPane::canPrevious() const { return false; }
void IssueOutputPane::goToNext() {}
void IssueOutputPane::goToPrev() {}
bool IssueOutputPane::hasFocus() const { return true; }
void IssueOutputPane::setFocus() {}
void IssueOutputPane::visibilityChanged(bool visible) { Q_UNUSED(visible) }
int IssueOutputPane::priorityInStatusBar() const { return 1; }

void IssueOutputPane::clearContents() {
    m_klocworkProjects.clear();
    m_treeWidget->clear();
}

QWidget *IssueOutputPane::outputWidget(QWidget *parent) {
    Q_UNUSED(parent)
    return m_treeWidget;
}

QList<QWidget *> IssueOutputPane::toolBarWidgets() const {

    return QList<QWidget *>() << m_filterOrigin << m_filterCurrentFile
                              << m_changeStatusDropDown
                              << m_filterStatusDropDown;
}

void IssueOutputPane::showIssueInEditor(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    Core::EditorManager::openEditorAt(
        item->text(KlocworkIssueColumn::File),
        item->text(KlocworkIssueColumn::LineNo).toInt(), 0);
}

void IssueOutputPane::showChangeStatusListMenu() {
    m_changeStatusDropDown->showMenu();
}

void IssueOutputPane::updateChangeStatusListMenu() {
    QTreeWidgetItem *item = m_treeWidget->currentItem();
    for (QList<QAction *>::const_iterator it =
             m_changeStatusListMenu->actions().constBegin();
         it != m_changeStatusListMenu->actions().constEnd(); ++it) {
        if ((item == 0) || (item->type() == KlocworkNodeType::ProjectNode) ||
            (*it)->text() == item->text(KlocworkIssueColumn::Status)) {
            (*it)->setEnabled(false);
        } else {
            (*it)->setEnabled(true);
        }
    }
}

void IssueOutputPane::showFilterStatusListMenu() {
    m_filterStatusDropDown->showMenu();
}

void IssueOutputPane::changeIssueStatus() {
    QDialog dialog(0);
    QFormLayout form(&dialog);

    form.addRow(new QLabel(tr("Comment")));
    QPlainTextEdit *textEdit = new QPlainTextEdit(&dialog);
    form.addRow(textEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        QAction *action = static_cast<QAction *>(QObject::sender());
        QTC_ASSERT(action != 0, return );
        QTreeWidgetItem *item = m_treeWidget->currentItem();
        KlocworkProjectTreeItem *projectItem = getKlocworkProject(item);
        if (projectItem != 0) {
            m_runner->changeIssueStatus(projectItem->getProject(),
                                        action->text(), textEdit->toPlainText(),
                                        item->text(KlocworkIssueColumn::Id));
        }
    }
}

void IssueOutputPane::changedIssueStatusFilter() {
    QAction *action = static_cast<QAction *>(QObject::sender());
    QTC_ASSERT(action != 0, return );
    if (action->isChecked()) {
        m_issueFilterStatuses.append(action->text());
    } else {
        m_issueFilterStatuses.removeAll(action->text());
    }
}

void IssueOutputPane::filterIssueStatus() { updateView(); }

void IssueOutputPane::klocworkProcessStarted() {
    if (m_changeStatusDropDown != 0) {
        m_changeStatusDropDown->setEnabled(false);
        m_changeStatusDropDown->setToolTip(tr(TOOLTIP_NOT_CHANGE_ISSUE_STATUS));
    }
}

void IssueOutputPane::klocworkProcessFinished() {
    if (m_changeStatusDropDown != 0) {
        m_changeStatusDropDown->setEnabled(true);
        m_changeStatusDropDown->setToolTip(tr(TOOLTIP_CHANGE_ISSUE_STATUS));
    }
}

void IssueOutputPane::setSelected() {
    KlocworkIssueTreeItem *item =
        dynamic_cast<KlocworkIssueTreeItem *>(QObject::sender());
    if (item != NULL) {
        m_treeWidget->setCurrentItem(item);
    }
}

void IssueOutputPane::updateAnalysisResults(ProjectExplorer::Project *project,
                                            QString report,
                                            QStringList analysedFiles) {

    QFile fReport(report);
    if (fReport.open(QIODevice::ReadOnly)) {

        KlocworkProjectTreeItem *projectItem = getKlocworkProject(project);

        // TODO: instead of deleting all issues in analysed files and re-reading
        // results file,
        // perhaps use class to store issues and only add/remove new/fixed
        // issues...
        for (QStringList::const_iterator it = analysedFiles.constBegin();
             it != analysedFiles.constEnd(); ++it) {
            qDeleteAll(m_treeWidget->findItems(*it, Qt::MatchRecursive,
                                               KlocworkIssueColumn::File));
        }

        // parse report and populate projectItem with any issues found
        m_klocworkReportParser.parseReport(this, fReport, projectItem);

        fReport.close();
    } else {
        QMessageBox::warning(
            0, tr("Klocwork Results"),
            tr("Warning: Unable to read Klocwork results file: ") + report);
    }
}

void IssueOutputPane::updateIssueStatus(ProjectExplorer::Project *project,
                                        QString id, QString status) {
    KlocworkProjectTreeItem *projectItem = getKlocworkProject(project);
    int size = projectItem->childCount();

    for (int i = 0; i < size; ++i) {
        QTreeWidgetItem *item = projectItem->child(i);
        if (item->text(KlocworkIssueColumn::Id) == id) {
            item->setText(KlocworkIssueColumn::Status, status);
            filterIssue(item);
        }
    }
}

void IssueOutputPane::filterIssue(QTreeWidgetItem *item) {
    if (item->type() == KlocworkNodeType::ProjectNode) {
        return;
    }

    KlocworkIssueTreeItem *kwItem = static_cast<KlocworkIssueTreeItem *>(item);

    Core::IDocument *doc = Core::EditorManager::currentDocument();

    bool hideSystemIssue =
        (m_filterOrigin->isChecked() &&
         kwItem->text(KlocworkIssueColumn::Origin) == tr("System"));

    bool hideCurrentFileIssue =
        (doc && m_filterCurrentFile->isChecked() &&
         kwItem->text(KlocworkIssueColumn::File) != doc->filePath().toString());

    bool hideStatusIssue = (!m_issueFilterStatuses.contains(
        kwItem->text(KlocworkIssueColumn::Status), Qt::CaseSensitive));

    if (hideSystemIssue || hideCurrentFileIssue || hideStatusIssue) {
        kwItem->hideIssue();
    } else {
        kwItem->showIssue();
    }
}

void IssueOutputPane::updateView() {
    updateView(m_treeWidget->invisibleRootItem());
}

void IssueOutputPane::updateView(QTreeWidgetItem *item) {
    for (int i = 0; i < item->childCount(); ++i) {
        filterIssue(item->child(i));

        updateView(item->child(i));
    }
}

KlocworkProjectTreeItem *
IssueOutputPane::getKlocworkProject(QTreeWidgetItem *item) {
    if (item != 0) {
        if (item->type() == KlocworkNodeType::ProjectNode) {
            return dynamic_cast<KlocworkProjectTreeItem *>(item);
        } else {
            return getKlocworkProject(item->parent());
        }
    } else {
        return 0;
    }
}

KlocworkProjectTreeItem *
IssueOutputPane::getKlocworkProject(ProjectExplorer::Project *project) {
    KlocworkProjectTreeItem *projectItem = m_klocworkProjects[project->id()];

    if (projectItem == 0) {
        projectItem =
            new KlocworkProjectTreeItem(project, KlocworkNodeType::ProjectNode);
        projectItem->setText(KlocworkIssueColumn::Icon, project->displayName());
        m_klocworkProjects[project->id()] = projectItem;
        m_treeWidget->addTopLevelItem(projectItem);
    }
    return projectItem;
}
