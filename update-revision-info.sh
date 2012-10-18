#/bin/sh

FRDIR=`dirname $0`
REVISIONINFOFILE=src/revisioninfo.h


SVNVERSION=""
if [ -x "$(command -v svn 2>/dev/null)" ]; then
    SVNVERSION=`svn info $FRDIR 2>/dev/null | awk '/Revision/{print $2}'`
fi
#if [ "$SVNVERSION" != "" ]; then
#   echo "svn revision $SVNVERSION"
#else
#    echo "no svn revision found"
#fi


# this will already contain the quotes needed in the header file
GITHASH="";
if [ -x "$(command -v git 2>/dev/null)" ]; then
    GITHASH="\""`git log -1 --pretty=format:%h $FRDIR 2>/dev/null`"\""
fi
#if [ "$GITHASH" != "" ]; then
#    echo "git hash $GITHASH"
#else
#    echo "no git hash found"
#fi


if test -f $REVISIONINFOFILE ; then
    HEADER_SVNVERSION=`awk '/FR_VERSION_SVN/{print $3}' $REVISIONINFOFILE`
    HEADER_GITHASH=`awk '/FR_GIT_HASH/{print $3}' $REVISIONINFOFILE`
    if [ "$HEADER_SVNVERSION" != "$SVNVERSION" ]; then
        echo "svn revision has changed from \"$HEADER_SVNVERSION\" to \"$SVNVERSION\""
        WRITE_HEADER="y"
    fi
    if [ "$HEADER_GITHASH" != "$GITHASH" ]; then
        echo "git hash has changed from $HEADER_GITHASH to $GITHASH"
        WRITE_HEADER="y"
    fi
else
    echo "$REVISIONINFOFILE not found"
    WRITE_HEADER="y"
fi

if [ "$WRITE_HEADER" == "y" ]; then
    echo "// auto-generated by update-revision-info script" > $REVISIONINFOFILE

    if [ "$SVNVERSION" != "" ]; then
        echo "Writing svn revision $SVNVERSION to $REVISIONINFOFILE"
        echo "#define FR_VERSION_SVN $SVNVERSION" >> $REVISIONINFOFILE
    else
        echo "Writing no svn revision to $REVISIONINFOFILE"
        echo "#undefine FR_VERSION_SVN" >> $REVISIONINFOFILE
    fi

    if [ "$GITHASH" != "" ]; then
        echo "Writing git hash $GITHASH to $REVISIONINFOFILE"
        echo "#define FR_GIT_HASH $GITHASH" >> $REVISIONINFOFILE
    else
        echo "Writing no git hash to $REVISIONINFOFILE"
        echo "#undefine FR_GIT_HASH" >> $REVISIONINFOFILE
    fi
fi
