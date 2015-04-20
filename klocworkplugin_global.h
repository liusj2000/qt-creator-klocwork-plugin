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


#ifndef KLOCWORKPLUGIN_GLOBAL_H
#define KLOCWORKPLUGIN_GLOBAL_H

#include <QtGlobal>

#if defined(KLOCWORKPLUGIN_LIBRARY)
#  define KLOCWORKPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KLOCWORKPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KLOCWORKPLUGIN_GLOBAL_H

