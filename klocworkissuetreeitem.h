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

#ifndef KLOCWORKISSUETREEITEM_H
#define KLOCWORKISSUETREEITEM_H

#include "klocworktextmark.h"

#include <QTreeWidgetItem>
#include <QWidget>

namespace TextEditor {
class TextMark;
}

namespace Klocwork {
namespace Internal {

/*
 * Represents a KlocworkIssue in the QTreeWidget of Klocwork results in class
 * IssueOutputPane.
 */
class KlocworkIssueTreeItem : public QObject, public QTreeWidgetItem {

    Q_OBJECT

  public:
    KlocworkIssueTreeItem(const QString &filename, int lineNumber, int type = Type);

    void showIssue();
    void hideIssue();

    KlocworkTextMark *getKlocworkTextMark();

  signals:
    void selected();

private:
    QTreeWidget *m_treeWidget;
    KlocworkTextMark m_mark;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // KLOCWORKISSUETREEITEM_H
