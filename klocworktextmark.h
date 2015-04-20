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

#include <texteditor/textmark.h>

#ifndef KLOCWORKTEXTMARK_H
#define KLOCWORKTEXTMARK_H

namespace Klocwork {
namespace Internal {

class KlocworkIssueTreeItem;

/*
 * Simple class to display a mark in the editor where a Klocwork issue has
 * been detected
 */
class KlocworkTextMark : public QObject, public TextEditor::TextMark
{
    Q_OBJECT

public:
    KlocworkTextMark(KlocworkIssueTreeItem *, const QString &, int);

    void updateLineNumber(int lineNumber);

    void clicked();

    bool isDraggable() const;
    bool isClickable() const;

signals:
    void selected();

private:
    KlocworkIssueTreeItem *m_kwItem;

};



} // namespace Internal
} // namespace KlocworkPlugin

#endif // KLOCWORKTEXTMARK_H
