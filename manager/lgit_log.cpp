#include "lgit_log.h"

#include "singleton.h"
#include "messages.h"

#include <string>

#include <QDebug>

#include "git2.h"
#include "git2/oid.h"
#include "git2/revwalk.h"
#include "git2/commit.h"
#include "git2/tree.h"
#include "git2/diff.h"

#define MessagesI Singleton<Messages>::instance()

static int diff_file_callback( const git_diff_delta *delta, float progress, void *payload );
static int diff_binary_callback( const git_diff_delta *delta, const git_diff_binary *binary, void *payload );
static int diff_hunk_callback( const git_diff_delta *delta, const git_diff_hunk *hunk, void *payload );
static int diff_line_callback( const git_diff_delta *, const git_diff_hunk *, const git_diff_line *, void* );

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

lgit_log::lgit_log()
{

}

int lgit_log::peel_revision( git_repository *repo, git_tree **out, const std::string & revision, git_otype target_type )
{
    int error;

    git_object *obj;

    if ( ( error = git_revparse_single( &obj, repo, revision.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( "Incorrect revision occured (when peeling)" );
        return 367 + ( error * 10000 * -1 );
    }

    if( ( error = git_object_peel( (git_object **)out, obj, target_type ) ) < 0 ) {
        MessagesI.AppendMessageT( "Incorrect object occured (when peeling)" );
        return 373 + ( error * 10000 * -1 );
    }

    git_object_free(obj);

    return 0;
}

int lgit_log::log_of_tip( git_repository *repo, const std::string & sha, const std::string & hide )
{
    int error;
    git_revwalk *walk;
    git_oid oid, outid;
    git_object *obj;

    if ( ( error = git_oid_fromstr( &oid, sha.c_str() ) ) < 0 ) {
        MessagesI.AppendMessageT( "Invalid SHA given" );
        return 211 + ( 10000 * error * -1 );
    }

    if( hide != "" ) {
        if ( ( error = git_revparse_single(&obj, repo, hide.c_str() ) ) < 0 ) {
            MessagesI.AppendMessageT( "Invalid revision specification given (for hide)" );
            return 223 + ( 10000 * error * -1 );
        }
    }

    if ( ( error = git_revwalk_new( &walk, repo ) ) < 0 ) {
        MessagesI.AppendMessageT( "Unable to walk revisions" );
        return 227 + ( 10000 * error * -1 );
    }

    git_revwalk_push( walk, &oid );

    if( hide != "" ) {
        git_revwalk_hide( walk, git_object_id( obj ) );
    }

    int error2 = 0;
    while ( ( error2 = git_revwalk_next( &outid, walk ) ) == 0 ) {
        git_commit *commit;
        if ( ( error = git_commit_lookup( &commit, repo, &outid ) ) < 0 ) {
            MessagesI.AppendMessageT( "Unable to lookup commit" );
            continue;
        }

        log_entry newentry;

        // SHA
        char buf[ GIT_OID_HEXSZ + 1 ];
        const git_oid *commit_oid = git_commit_id( commit );
        if( commit_oid ) {
            git_oid_tostr( buf, sizeof(buf), commit_oid );
            newentry.sha = std::string( buf );
        }

        // AUTHOR
        const git_signature *sig;
        if( ( sig = git_commit_author( commit ) ) == 0 ) {
            MessagesI.AppendMessageT( "Unable to get commit's author" );
        } else {
            std::string name, email;
            if( sig->name ) {
                name = std::string( sig->name );
            }
            if( sig->email ) {
                email = std::string( " <" ) + std::string( sig->email ) + std::string( ">" );
            }
            newentry.author = name + email;
        }

        // MESSAGE
        const char *msg;
        if ( ( msg = git_commit_message( commit ) ) == 0 ) {
            MessagesI.AppendMessageT( "Unable to get commit's message" );
        } else {
            newentry.message = msg;
        }

        entries_.push_back( newentry );

        // qDebug() << newentry.sha << newentry.author << newentry.message;
    }

    if( error2 < 0 && error2 != GIT_ITEROVER ) {
        MessagesI.AppendMessageT( QString( "Problems during revision walk (%1)" ).arg( error2 ) );
    }

    git_revwalk_free( walk );

    return 0;
}

int lgit_log::equip_with_diff_data_single( git_repository *repo, log_entry & first, const log_entry & second )
{
    int error;
    git_tree *tree1, *tree2;

    if ( ( error = peel_revision( repo, &tree1, first.sha, GIT_OBJ_TREE ) ) > 0 ) {
        return error + 11 * 1000000;
    }

    if ( ( error = peel_revision( repo, &tree2, second.sha, GIT_OBJ_TREE ) ) < 0 ) {
        return error + 13 * 1000000;
    }

    git_diff_options diffopts;
    git_diff_find_options findopts;

    if ( ( error = git_diff_init_options( &diffopts, GIT_DIFF_OPTIONS_VERSION ) ) < 0 ) {
        MessagesI.AppendMessageT( "Could not initialize diff" );
        return 379 + ( 10000 * error * -1 );
    }

    if ( ( error = git_diff_find_init_options( &findopts, GIT_DIFF_FIND_OPTIONS_VERSION ) ) < 0 ) {
        MessagesI.AppendMessageT( "Could not initialize diff (find)" );
        return 383 + ( 10000 * error * -1 );
    }

    findopts.flags |= GIT_DIFF_FIND_RENAMES;
    findopts.flags |= GIT_DIFF_FIND_REWRITES;

    git_diff *diff;

    if ( ( error = git_diff_tree_to_tree( &diff, repo, tree2, tree1, &diffopts ) ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Diff didn't run successfully (exit code: %1)" ).arg( error * -1 ) );
        git_tree_free( tree1 );
        git_tree_free( tree2 );
        return 389 + ( 10000 * error * -1 );
    }


    if ( ( error = git_diff_find_similar( diff, & findopts ) ) ) {
        MessagesI.AppendMessageT( QString( "Diff (find) didn't run successfully (exit code: %1)" ).arg( error * -1 ) );
        git_tree_free( tree1 );
        git_tree_free( tree2 );
        git_diff_free( diff );
        return 397 + ( 10000 * error * -1 );
    }

    diff_run_state state;
    state.entry = &first;

    if ( ( error = git_diff_foreach( diff, diff_file_callback, diff_binary_callback, diff_hunk_callback, diff_line_callback, &state ) ) < 0 ) {
        MessagesI.AppendMessageT( QString( "Cannot read diff (exit code: %1)" ).arg( error * -1 ) );
        git_tree_free( tree1 );
        git_tree_free( tree2 );
        git_diff_free( diff );
        return 409 + ( 10000 * error * -1 );
    }

    git_tree_free( tree1 );
    git_tree_free( tree2 );
    git_diff_free( diff );
    return 0;
}

int lgit_log::equip_with_diff_data_first(git_repository *repo, log_entry &first)
{

}

int lgit_log::equip_with_diff_data( git_repository *repo )
{
    unsigned int size = count();

    for ( unsigned int i = 0; i < size; ++ i ) {
        if( i != size - 1 ) {
            equip_with_diff_data_single( repo, entries_[i], entries_[i+1] );
        } else {
            equip_with_diff_data_first( repo, entries_[i] );
        }
    }

    return 0;
}

int diff_file_callback( const git_diff_delta *delta, float progress, void *payload ) {
    diff_run_state *state = static_cast< diff_run_state * > ( payload );
    log_entry *entry = state->entry;

    state->clear();

    switch ( delta->status ) {
    case GIT_DELTA_ADDED:
        state->is_addition = true;
        entry->diff_files.push_back( my_diff_file( delta->old_file.path, delta->new_file.path ) );
        entry->diff_files.back().is_addition = true;
        entry->has_diff_data = true;
        break;
    case GIT_DELTA_DELETED:
        state->is_deletion = true;
        entry->diff_files.push_back( my_diff_file( delta->old_file.path, delta->new_file.path ) );
        entry->diff_files.back().is_deletion = true;
        entry->has_diff_data = true;
        break;
    case GIT_DELTA_MODIFIED:
        state->is_modification = true;
        entry->diff_files.push_back( my_diff_file( delta->old_file.path, delta->new_file.path ) );
        entry->diff_files.back().is_modification = true;
        entry->has_diff_data = true;
        break;
    case GIT_DELTA_RENAMED:
        state->is_rename = true;
        entry->diff_files.push_back( my_diff_file( delta->old_file.path, delta->new_file.path ) );
        entry->diff_files.back().is_rename = true;
        entry->has_diff_data = true;
        break;
    case GIT_DELTA_COPIED:
        state->is_copy = true;
        entry->diff_files.push_back( my_diff_file( delta->old_file.path, delta->new_file.path ) );
        entry->diff_files.back().is_copy = true;
        entry->has_diff_data = true;
        break;
    case GIT_DELTA_CONFLICTED:
        state->is_conflicted = true;
        entry->diff_files.push_back( my_diff_file( delta->old_file.path, delta->new_file.path ) );
        entry->diff_files.back().is_conflicted = true;
        entry->has_diff_data = true;
        break;
    case GIT_DELTA_UNMODIFIED:
    case GIT_DELTA_IGNORED:
    case GIT_DELTA_UNTRACKED:
    case GIT_DELTA_TYPECHANGE:
    case GIT_DELTA_UNREADABLE:
        state->is_other = true;
        break;
    default:
        break;
    }

    return 0;
}

int diff_binary_callback( const git_diff_delta *, const git_diff_binary *, void *payload ) {
    diff_run_state *state = static_cast< diff_run_state * > ( payload );
    log_entry *entry = state->entry;

    return 0;
}

int diff_hunk_callback( const git_diff_delta *, const git_diff_hunk *hunk, void *payload ) {
    diff_run_state *state = static_cast< diff_run_state * > ( payload );
    log_entry *entry = state->entry;

    if( state->is_modification && entry->diff_files.size() > 0 ) {
        entry->diff_files.back().hunks.push_back( myhunk( hunk->header ) );
    }

    return 0;
}

int diff_line_callback( const git_diff_delta *, const git_diff_hunk *, const git_diff_line *line, void *payload ) {
    diff_run_state *state = static_cast< diff_run_state * > ( payload );
    log_entry *entry = state->entry;

    if( state->is_modification && entry->diff_files.size() > 0 ) {
        entry->diff_files.back().hunks.back().ulines.push_back( std::string( 1, line->origin ) + std::string( line->content, line->content_len ) );
    }

    return 0;
}
