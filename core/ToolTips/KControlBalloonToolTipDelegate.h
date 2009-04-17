/*****************************************************************************
 *   Copyright (C) 2008 by Simon St James <kdedevel@etotheipiplusone.com>    *
 *   Copyright (C) 2009 by Mathias Soeken <msoeken@informatik.uni-bremen.de> *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA              *
 *****************************************************************************/

#ifndef KCONTROLBALLOONTOOLTIPDELEGATE
#define KCONTROLBALLOONTOOLTIPDELEGATE

#include "KFormattedBalloonTipDelegate.h"

#include "KToolTipItem.h"

#include <QSize>
#include <QPainter>

const int PREVIEW_WIDTH = 32;
const int PREVIEW_HEIGHT = 32;

const int SUB_PREVIEW_WIDTH = 24;
const int SUB_PREVIEW_HEIGHT = 24;

class KControlBalloonToolTipDelegate : public KFormattedBalloonTipDelegate
{
    Q_OBJECT
public:
    KControlBalloonToolTipDelegate();
    virtual ~KControlBalloonToolTipDelegate();

    virtual QSize sizeHint( const KStyleOptionToolTip* option, const KToolTipItem* item ) const;
    virtual void paint( QPainter* painter, const KStyleOptionToolTip* option, const KToolTipItem* item ) const;
};

#endif

