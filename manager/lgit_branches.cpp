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

#include "lgit_branches.h"

#include "singleton.h"
#include "messages.h"

#include <QString>
#include <QDebug>

#include "git2/branch.h"

#define MessagesI Singleton<Messages>::instance()

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

static int fetchhead_foreach_cb( const char *ref_name, const char *remote_url, const git_oid *oid,
                                 unsigned int is_merge, void *payload );

lgit_branches::lgit_branches()
{
    dummy_.invalid = INVALID_DUMMY;
    dummy_.name = std::string( "-" );
    dummy_.tip_sha = std::string( "-" );
}

int lgit_branches::list( git_repository *repo, int type )
{
    int error, retval = 0;

    if ( !repo ) {
        retval += 191;
        return retval;
    }

    cur_repo_ = repo;

    git_branch_t list_flags = GIT_BRANCH_ALL;
    switch ( type ) {

    case BRANCH_LOCAL:
        list_flags = GIT_BRANCH_LOCAL;
        break;

    case BRANCH_REMOTE:
        list_flags = GIT_BRANCH_REMOTE;
        break;

    default:
        break;

    }

    git_branch_iterator *iter;
    if ( ( error = git_branch_iterator_new( &iter, repo, list_flags ) ) < 0 ) {
        retval += 193 + ( 10000 * error * -1 );
        return retval;
    }

    raw_branches_.clear();

    git_reference *ref;
    git_branch_t out_type;

    while ( ( error = git_branch_next( &ref, &out_type, iter) ) != GIT_ITEROVER ) {
        if( error < 0 ) {
            retval += 197 + ( 10000 * error * -1 );
            return retval;
        }

        mybranch newbranch;

        // Fill .type
        newbranch.type = (out_type == GIT_BRANCH_LOCAL) ? BRANCH_LOCAL : BRANCH_REMOTE;

        // When obtained a reference, this will be set
        git_oid oid;
        bool have_oid = false;

        // Helper switch() vars
        const git_oid *oidp;
        const char* sym;

        newbranch.invalid = INVALID_CORRECT;

        switch( git_reference_type( ref ) ) {

        /* GIT_REF_INVALID */
        case GIT_REF_INVALID:
            newbranch.invalid = INVALID_TYPE;
            break;

        /* GIT_REF_OID */
        case GIT_REF_OID:
            newbranch.symbolic = false;

            // Prepare data for .tip_sha
            oidp = git_reference_target( ref );
            if( !oidp ) {
                newbranch.invalid = INVALID_OID;
                break;
            }
            oid = *oidp;
            have_oid = true;

            break;

        /* GIT_REF_SYMBOLIC */
        case GIT_REF_SYMBOLIC:
            newbranch.symbolic = true;

            // Set .symbolic_ref
            sym = git_reference_symbolic_target( ref );
            if( !sym ) {
                newbranch.invalid = INVALID_SYM;
                break;
            }
            newbranch.symbolic_ref = std::string( sym );

            // Prepare data for .tip_sha
            if ( ( error = git_reference_name_to_id( &oid, repo, newbranch.symbolic_ref.c_str() ) ) < 0 ) {
                newbranch.invalid = INVALID_OID2;
                break;
            }
            have_oid = true;

            break;

        /* GIT_REF_LISTALL */
        case GIT_REF_LISTALL:
            newbranch.invalid = INVALID_TYPE2;
            break;

        /* DEFAULT */
        default:
            newbranch.invalid = INVALID_TYPE3;
            break;

        }

        /*
         * AFTER SWITCH()
         */

        if( have_oid && newbranch.invalid == INVALID_CORRECT ) {
            char sha[ GIT_OID_HEXSZ + 1 ];
            git_oid_fmt( sha, &oid );
            sha[ GIT_OID_HEXSZ ] = '\0';
            newbranch.tip_sha = std::string( sha );

            // Fill branch::parents_shas
            retval += fill_oid_parents( newbranch, &oid );

            // Fill name
            const char *name;
            if ( ( error = git_branch_name( &name, ref ) ) < 0 ) {
                newbranch.invalid |= INVALID_BRANCH;
            } else {
                QStringList parts = QString::fromUtf8( name ).split( "/", QString::SkipEmptyParts );
                if ( parts.count() > 1 ) {
                    if ( newbranch.type == BRANCH_REMOTE ) {
                        newbranch.remote_name = parts.first().toStdString();
                        parts.removeFirst();
                        newbranch.name = parts.join("/").toStdString();
                    } else {
                        newbranch.name = std::string( name );
                    }
                } else {
                    newbranch.name = std::string( name );
                }

            }
        }

        git_reference_free(ref);

        raw_branches_.push_back( newbranch );
    }

    git_branch_iterator_free( iter );


    // Fill FETCH_HEAD data: is_in_fetch_head, is_pure_fetch_head, fetch_head_is_merge, fetch_head_remote_url
    git_repository_fetchhead_foreach( repo, fetchhead_foreach_cb, (void *) this );

    return retval;
}

int lgit_branches::fill_oid_parents(mybranch & newbranch, const git_oid *oid)
{
    int error, retval = 0;
    const git_oid *oidp;

    git_repository *repo = cur_repo_;

    git_object *obj;
    if( ( error = git_object_lookup( &obj, repo, oid, GIT_OBJ_ANY ) ) < 0 ) {
        newbranch.invalid = INVALID_SHATWO;
    } else {
        git_otype otype = git_object_type( obj );
        if ( otype == GIT_OBJ_COMMIT ) {
            git_commit *commit = ( git_commit * ) obj;
            int count = git_commit_parentcount( commit );
            for ( int i = 0; i < count; i ++ ) {
                oidp = git_commit_parent_id( commit, i );
                if( !oidp ) {
                    newbranch.invalid = INVALID_SHATWO2;
                    continue;
                }
                char sha[ GIT_OID_HEXSZ + 1 ];
                git_oid_fmt( sha, oidp );
                sha[ GIT_OID_HEXSZ ] = '\0';
                newbranch.parents_shas.push_back( std::string(sha) );
            }
        } else {
            if( otype == GIT_OBJ_BAD ) {
                newbranch.invalid = INVALID_SHATWO3;
            }
        }

        git_object_free( obj );
    }

    return retval;
}

mybranch & lgit_branches::findSha( const char *_sha )
{
    std::string sha = std::string( _sha );

    for( std::vector< mybranch >::iterator it = raw_branches_.begin(); it != raw_branches_.end(); it ++ ) {
        if( it->tip_sha == sha ) {
            return *it;
        }
    }

    return dummy_;
}

const mybranch & lgit_branches::findFetchHeadSha( const char *_sha ) const
{
    std::string sha = std::string( _sha );

    for( std::vector< mybranch >::const_iterator it = raw_branches_.begin(); it != raw_branches_.end(); it ++ ) {
        if( it->tip_sha == sha && it->is_in_fetch_head ) {
            return *it;
        }
    }

    return dummy_;
}

const mybranch &lgit_branches::findNameWithType( const char *_name, int type ) const
{
    std::string name = std::string( _name );

    for( std::vector< mybranch >::const_iterator it = raw_branches_.begin(); it != raw_branches_.end(); it ++ ) {
        if( type == FIND_BRANCH_FETCH_HEAD && !it->is_in_fetch_head ) {
            continue;
        }
        if( type == FIND_BRANCH_LOCAL && it->type != BRANCH_LOCAL ) {
            continue;
        }
        if( type == FIND_BRANCH_REMOTE && ( it->type != BRANCH_REMOTE || it->is_in_fetch_head ) ) {
            continue;
        }
        if( it->name == name ) {
            return *it;
        }
    }

    return dummy_;
}

int lgit_branches::create( const std::string & name, const std::string & tip_sha )
{
    int error;

    git_oid oid;
    git_commit *tip_commit;

    if ( tip_sha != "" ) {
        if ( ( error = git_oid_fromstr( &oid, tip_sha.c_str() ) ) < 0 ) {
            MessagesI.AppendMessageT( "Provided SHA is invalid, cannot create branch" );
            return 491 + ( 10000 * error * -1 );
        }
    } else {
        if ( ( error = git_reference_name_to_id( &oid, cur_repo_, "HEAD" ) ) < 0 ) {
            MessagesI.AppendMessageT( "Could not read HEAD, is repository consistent? (2)");
            return 521 + ( 10000 * error * -1 );
        }
    }

    // Find new tip commit, to be used as tree pointer in checkout
    if ( ( error = git_commit_lookup( &tip_commit, cur_repo_, &oid ) ) < 0 ) {
        MessagesI.AppendMessageT( "Provided commit doesn't exist, cannot create branch" );
        return 499 + ( 10000 * error * -1 );
    }

    git_reference *newbranch;
    if ( ( error = git_branch_create( &newbranch, cur_repo_, name.c_str(), tip_commit, 0 ) ) < 0 ) {
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( QString( "Could not create branch: %1 (%2)" ).arg( spec_error ).arg( error * -1 ) );
        return 503 + ( error * 10000 * -1 );
    }

    return 0;
}

int lgit_branches::removeLocal( const std::string & name )
{
    int error;
    git_reference *ref;
    std::string ref_name = "refs/heads/" + name;

    if ( ( error = git_reference_lookup( &ref, cur_repo_, ref_name.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Could not resolve branch name, cannot delete branch <b>" ) + QString::fromStdString( name ) + "</b>" );
        return 541 + ( 10000 * error * -1 );
    }

    if ( ( error = git_branch_delete( ref ) ) < 0 ) {
        const char *spec_error = giterr_last()->message;
        MessagesI.AppendMessageT( QString( "Cannot delete branch <b>" ) + QString::fromStdString( name ) + "</b>: " +
                                  QString::fromUtf8( spec_error ) + QString( " (%1)" ).arg( error ) );
        return 547 + ( 10000 * error * -1 );
    }

    return 0;
}

static int fetchhead_foreach_cb( const char *ref_name, const char *remote_url, const git_oid *oidp,
                                 unsigned int is_merge, void *payload )
{
    int error;

    lgit_branches *_branches = static_cast< lgit_branches * > ( payload );
    if( !_branches ) {
        MessagesI.AppendMessageT( "FETCH_HEAD-related internal error (1)" );
        return 1;
    }

    lgit_branches & branches = *_branches;

    char sha[ GIT_OID_HEXSZ + 1 ];
    git_oid_fmt( sha, oidp );
    sha[ GIT_OID_HEXSZ ] = '\0';

    mybranch & branch = branches.findSha( sha );
    mybranch newbranch;
    QStringList parts = QString::fromUtf8( ref_name ).split( "/", QString::SkipEmptyParts );
    if( parts.count() < 1 ) {
        MessagesI.AppendMessageT( "FETCH_HEAD-related internal error (2)" );
        return 2;
    }

    newbranch.name = parts.last().toStdString();
    newbranch.is_in_fetch_head = true;
    newbranch.is_pure_fetch_head = ( branch.invalid == INVALID_DUMMY ? true : false );
    newbranch.fetch_head_is_merge = (is_merge != 0);
    newbranch.fetch_head_remote_url = std::string( remote_url );
    newbranch.tip_sha = std::string( sha );
    if( ( error = branches.fill_oid_parents( newbranch, oidp ) ) > 0 ) {
        MessagesI.AppendMessageT( QString( "FETCH_HEAD-related internal error (3/%1)" ).arg( error ) );
    }
    // A FETCH_HEAD-only branch is considered remote, with tip
    // being stored not in refs/remotes/<name>, but in FETCH_HEAD
    newbranch.type = BRANCH_REMOTE;

    branches.add( newbranch );

    // qDebug() << "CB" << QString::fromStdString( branch.name ) << ref_name << remote_url << sha << is_merge;

    return 0;
}
