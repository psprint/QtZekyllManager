#ifndef GITOPERATIONTRACKER_H
#define GITOPERATIONTRACKER_H

#include "git2.h"
#include "git2/checkout.h"

class GitOperationTracker
{
public:
    GitOperationTracker();

    int updateFetchProgress( double progress );
    int checkoutNotify( git_checkout_notify_t why, const char *path );

    void setPullDialog( void * pullDialog ) { pulldialog_ = pullDialog; }
private:
    void *pulldialog_;
};

#endif // GITOPERATIONTRACKER_H
