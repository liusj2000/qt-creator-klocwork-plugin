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

#include "klocworkreportparser.h"

#include "issueoutputpane.h"
#include "klocworkissuetreeitem.h"

#include "klocworkpluginconstants.h"

#include <QStringList>
#include <QTextStream>

using namespace Klocwork::Internal;

using namespace Klocwork::Constants;

namespace {
namespace REPORT {
enum {
    ID = 0,
    FILE,
    LINE_NO,
    COL_NO,
    SEVERITY,
    SEVERITY_NO,
    TAXONOMY,
    CODE,
    DISPLAY_AS,
    FUNCTION,
    UNKNOWN,
    MESSAGE,
    ORIGIN,
    STATUS
};
}
}

KlocworkReportParser::KlocworkReportParser() {}

void KlocworkReportParser::parseReport(IssueOutputPane *outputPane,
                                       QFile &fReport,
                                       QTreeWidgetItem *parent) {

    QTextStream in(&fReport);
    while (!in.atEnd()) {
        QStringList issue = in.readLine().split(QObject::tr(";"));
        KlocworkIssueTreeItem *treeItem = new KlocworkIssueTreeItem(
            issue[REPORT::FILE], issue[REPORT::LINE_NO].toInt(),
            KlocworkNodeType::IssueNode);

        // set the icon according to the severity
        if ((issue[REPORT::SEVERITY_NO]).toInt() > 2) {
            treeItem->setIcon(
                KlocworkIssueColumn::Icon,
                QIcon(QString::fromUtf8(
                    ":/KlocworkPlugin/images/yellowChevron.png")));
        } else {
            treeItem->setIcon(KlocworkIssueColumn::Icon,
                              QIcon(QString::fromUtf8(
                                  ":/KlocworkPlugin/images/redChevron.png")));
        }
        treeItem->getKlocworkTextMark()->setIcon(
            treeItem->icon(KlocworkIssueColumn::Icon));

        treeItem->setText(KlocworkIssueColumn::Message, issue[REPORT::MESSAGE]);
        treeItem->setToolTip(KlocworkIssueColumn::Message,
                             issue[REPORT::MESSAGE]);

        treeItem->setText(KlocworkIssueColumn::File, issue[REPORT::FILE]);
        treeItem->setToolTip(KlocworkIssueColumn::File, issue[REPORT::FILE]);

        treeItem->setText(KlocworkIssueColumn::LineNo, issue[REPORT::LINE_NO]);

        treeItem->setText(KlocworkIssueColumn::Code, issue[REPORT::CODE]);

        treeItem->setToolTip(KlocworkIssueColumn::Code, issue[REPORT::CODE]);

        treeItem->setText(KlocworkIssueColumn::Origin, issue[REPORT::ORIGIN]);

        treeItem->setText(KlocworkIssueColumn::Status, issue[REPORT::STATUS]);

        treeItem->setText(KlocworkIssueColumn::Severity,
                          issue[REPORT::SEVERITY]);

        treeItem->setText(KlocworkIssueColumn::Id, issue[REPORT::ID]);
        treeItem->setToolTip(KlocworkIssueColumn::Id, issue[REPORT::ID]);

        treeItem->setTextAlignment(KlocworkIssueColumn::File, Qt::AlignRight);

        // automatically expand project node if issue exists
        if (parent->childCount() == 0) {
            parent->setExpanded(true);
        }

        parent->addChild(treeItem);

        treeItem->setTextAlignment(KlocworkIssueColumn::File, Qt::AlignRight);

        outputPane->filterIssue(treeItem);

        // connect so that when the mark is pressed in the text editor, we set
        // that as the selected issue
        // in the tree view
        QObject::connect(treeItem, SIGNAL(selected()), outputPane,
                         SLOT(setSelected()));
    }
}
