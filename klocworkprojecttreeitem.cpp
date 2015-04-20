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

#include "klocworkprojecttreeitem.h"

#include <projectexplorer/project.h>

using namespace ProjectExplorer;

using namespace Klocwork::Internal;

KlocworkProjectTreeItem::KlocworkProjectTreeItem(Project *project, int type)
    : QTreeWidgetItem(type), m_project(project) {}

Project *KlocworkProjectTreeItem::getProject() { return m_project; }
