
#include "klocworktextmark.h"

#include "klocworkissuetreeitem.h"

using namespace Klocwork::Internal;

KlocworkTextMark::KlocworkTextMark(KlocworkIssueTreeItem *kwItem,
                                   const QString &filename, int lineNumber)
    : TextMark(filename, lineNumber), m_kwItem(kwItem) {}

void KlocworkTextMark::updateLineNumber(int lineNumber) {
    TextMark::updateLineNumber(lineNumber);
    m_kwItem->setText(3, QString::number(lineNumber));
}

void KlocworkTextMark::clicked() { emit m_kwItem->selected(); }

bool KlocworkTextMark::isDraggable() const { return false; }

bool KlocworkTextMark::isClickable() const { return true; }
