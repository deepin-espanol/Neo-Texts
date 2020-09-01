/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wp <wangpeng_cm@deepin.com>
 *
 * Maintainer: wp <wangpeng_cm@deepin.com>
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

#ifndef DSLIDER_P_H
#define DSLIDER_P_H

#include "dslider.h"

#include <DObjectPrivate>
#include <DIconButton>
#include <DFloatingWidget>
#include <DLabel>

#include <QLayout>

DWIDGET_BEGIN_NAMESPACE

class SliderStrip;
class DSliderPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
    Q_DECLARE_PUBLIC(DSlider)

public:
    DSliderPrivate(DSlider *q);

    void init();

public:
    SpecialSlider *slider;
    bool handleVisible;
    Qt::Orientation orientation = Qt::Horizontal;
    QSlider::TickPosition aleDirection = QSlider::NoTicks;

    DIconButton *leftIcon;
    DIconButton *rightIcon;

    SliderStrip *left;
    SliderStrip *right;

    QGridLayout *layout;
    DFloatingWidget *tipvalue;
    DLabel *label;

    QSize iconSize;
    bool mouseWheelEnabled;

    void updtateTool(int value);
};

DWIDGET_END_NAMESPACE

#endif // DSLIDER_P_H
