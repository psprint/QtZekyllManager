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

#include "lzcsde.h"

LZCSDE::LZCSDE()
{
    dummy_entry_.setId(-1);
}

bool LZCSDE::insertFromListing( int id, const QString & listing ) {
    QRegExp rx1("([a-z0-9][a-z0-9][a-z0-9])\\.([A-Z])--(.*) >>(.*)<<");
    QRegExp rx2("([a-z0-9][a-z0-9][a-z0-9]) >>(.*)<<");
    QRegExp rx3("([a-z0-9][a-z0-9][a-z0-9])\\.([A-Z])--(.*)");
    rx1.setCaseSensitivity(Qt::CaseSensitive);
    rx2.setCaseSensitivity(Qt::CaseSensitive);
    rx3.setCaseSensitivity(Qt::CaseSensitive);
    if (rx1.indexIn( listing ) != -1) {
        LZCSDE_Entry entry;
        entry.setId( id );
        entry.setListing( listing );
        entry.setZekyll( rx1.cap(1) );
        entry.setChecked( true );
        entry.setSection( rx1.cap(2) );
        entry.setDescription( rx1.cap(3) );
        entry.setError( rx1.cap(4) );

        entries_.append( entry );
        return true;
    } else if (rx2.indexIn( listing ) != -1) {
        LZCSDE_Entry entry;
        entry.setId( id );
        entry.setListing( listing );
        entry.setZekyll( rx2.cap(1) );
        entry.setChecked( true );
        entry.setSection( "" );
        entry.setDescription( "" );
        entry.setError( rx2.cap(2) );

        entries_.append( entry );
        return true;
    } else if (rx3.indexIn( listing ) != -1) {
        LZCSDE_Entry entry;
        entry.setId( id );
        entry.setListing( listing );
        entry.setZekyll( rx3.cap(1) );
        entry.setChecked( true );
        entry.setSection( rx3.cap(2) );
        entry.setDescription( rx3.cap(3) );
        entry.setError( "" );

        entries_.append( entry );
        return true;
    } else {
        return false;
    }
}

LZCSDE_Entry & LZCSDE::getId( int id ) {
    for( QVector<LZCSDE_Entry>::iterator it = entries_.begin(); it != entries().end(); it ++ ) {
        if( id == it->id() ) {
            return *it;
        }
    }
    return dummy_entry_;
}

LZCSDE_Entry & LZCSDE::getId( const QString & id ) {
    bool ok = false;
    int intid = id.toInt(&ok);
    if( ok ) {
        return getId( intid );
    } else {
        return dummy_entry_;
    }
}

bool LZCSDE::removeId( int id ) {
    LZCSDE_Entry *found = NULL;
    int size = entries_.count();
    int idx = -1;
    for( int i=0; i<size; i++ ) {
        if( id == entries_[i].id() ) {
            found = &entries_[i];
            idx = i;
            break;
        }
    }

    if( found ) {
        entries_.removeAt( idx );
        return true;
    } else {
        return false;
    }
}

bool LZCSDE::removeId( const QString & id ) {
    bool ok = false;
    int intid = id.toInt( &ok );
    if(!ok) {
        return false;
    } else {
        return removeId( intid );
    }
}

bool LZCSDE::move( int sourceId, int destId ) {
    int sourceIdx = findIdxOfId( sourceId );
    int destIdx = findIdxOfId( destId );
    if( sourceIdx == -1 || destIdx == -1 ) {
        return false;
    }

    LZCSDE_Entry source = entries_[ sourceIdx ];
    LZCSDE_Entry dest = entries_[ destIdx ];
    entries_[ destIdx ] = source;
    entries_[ sourceIdx ] = dest;
    return true;
}

int LZCSDE::findIdxOfId( int id ) {
    int size = entries_.count();
    int idx = -1;
    for( int i=0; i<size; i++ ) {
        if( id == entries_[i].id() ) {
            idx = i;
            break;
        }
    }
    return idx;
}

QStringList LZCSDE::getZekylls() {
    QStringList result;
    int size = entries_.count();
    for( int i=0; i<size; i++ ) {
        result << entries_[i].zekyll();
    }
    return result;
}

bool LZCSDE::updateSectionOfId( const QString & id, const QString & section ) {
    bool ok = false;
    int intid = id.toInt( &ok );
    if(!ok) {
        return false;
    }

    int idx = findIdxOfId( intid );
    if( idx == -1 ) {
        return false;
    }

    entries_[idx].setSection( section );
    return true;
}

bool LZCSDE::updateDescriptionOfId( const QString & id, const QString & description ) {
    bool ok = false;
    int intid = id.toInt( &ok );
    if(!ok) {
        return false;
    }

    int idx = findIdxOfId( intid );
    if( idx == -1 ) {
        return false;
    }

    entries_[idx].setDescription( description );
    return true;
}

bool LZCSDE::updateZekyllOfId( const QString & id, const QString & zekyll )
{
    bool ok = false;
    int intid = id.toInt( &ok );
    if(!ok) {
        return false;
    }

    int idx = findIdxOfId( intid );
    if( idx == -1 ) {
        return false;
    }

    entries_[idx].setZekyll( zekyll );
    return true;
}
