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

#ifndef KLOCWORKPROJECTTREEITEM_H
#define KLOCWORKPROJECTTREEITEM_H

#include <QObject>
#include <QTreeWidgetItem>
#include <QWidget>

namespace ProjectExplorer {
class Project;
}

namespace Klocwork {
namespace Internal {

/*
 * Represents a project in the list of Klocwork issues.
 * Klocwork issues are grouped by project for synchronisation purposes
 */
class KlocworkProjectTreeItem : public QTreeWidgetItem {
  public:
    KlocworkProjectTreeItem(ProjectExplorer::Project *, int type = Type);

    ProjectExplorer::Project *getProject();

  private:
    ProjectExplorer::Project *m_project;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // KLOCWORKPROJECTTREEITEM_H
