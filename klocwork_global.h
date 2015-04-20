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

#ifndef KLOCWORK_GLOBAL_H
#define KLOCWORK_GLOBAL_H

#include <QtGlobal>

#if defined(KLOCWORK_LIBRARY)
#  define KLOCWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KLOCWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KLOCWORK_GLOBAL_H

