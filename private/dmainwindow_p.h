/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DSplitedWindow_P_H
#define DSplitedWindow_P_H

#include "dmainwindow.h"

#include <dtkwidget_global.h>
#include <dshadowline.h>

#include <DObjectPrivate>

class QShortcut;

DWIDGET_BEGIN_NAMESPACE

class DPlatformWindowHandle;
class DTitlebar;
class DSplitedWindowPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
public:
    DSplitedWindowPrivate(DSplitedWindow *qq);

    void init();
    void updateTitleShadowGeometry();

    DPlatformWindowHandle   *handle     = Q_NULLPTR;
    DTitlebar               *titlebar   = Q_NULLPTR;
    DShadowLine             *titleShadow = nullptr;
    QShortcut               *help       = Q_NULLPTR;

private:
    D_DECLARE_PUBLIC(DSplitedWindow)
};

DWIDGET_END_NAMESPACE

#endif // DSplitedWindow_P_H
