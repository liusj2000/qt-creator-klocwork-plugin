#include "klocworkissuetreeitem.h"

#include "klocworktextmark.h"

#include <texteditor/textmark.h>

#include <QIcon>
#include <QTreeWidget>

using namespace Klocwork::Internal;

using namespace TextEditor;

KlocworkIssueTreeItem::KlocworkIssueTreeItem(const QString &filename,
                                             int lineNumber, int type)
    : QTreeWidgetItem(type), m_mark(this, filename, lineNumber) {

    m_mark.setPriority(TextMark::NormalPriority);
}

void KlocworkIssueTreeItem::showIssue() {
    m_mark.setVisible(true);
    this->setHidden(false);
}

void KlocworkIssueTreeItem::hideIssue() {
    m_mark.setVisible(false);
    this->setHidden(true);
}

KlocworkTextMark *KlocworkIssueTreeItem::getKlocworkTextMark() {
    return &m_mark;
}
