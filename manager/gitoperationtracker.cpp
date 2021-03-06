/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gitoperationtracker.h"

#include "pulldialog.h"
#include "checkoutdialog.h"

#include <QString>
#include <QDebug>

GitOperationTracker::GitOperationTracker() : is_pull( false ), is_checkout( false )
{

}

int GitOperationTracker::updateFetchProgress( double progress )
{
    if ( is_pull ) {
        PullDialog *pdialog = static_cast< PullDialog * > ( pulldialog_ );
        if ( !pdialog ) {
            return 1;
        }
        pdialog->updateProgress( progress );
    }

    return 0;
}

int GitOperationTracker::checkoutNotify(git_checkout_notify_t why, const char *path)
{
    if ( is_pull ) {
        PullDialog *pdialog = static_cast< PullDialog * > ( pulldialog_ );
        if ( !pdialog ) {
            return 1;
        }

        pdialog->addNotification( why, path );
    } else if ( is_checkout ) {
        CheckoutDialog *cdialog = static_cast< CheckoutDialog * > ( checkoutdialog_ );
        if ( !cdialog ) {
            return 1;
        }

        cdialog->addNotification( why, path );
    }

    return 0;
}
