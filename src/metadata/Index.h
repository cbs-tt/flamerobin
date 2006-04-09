/*
Copyright (c) 2004, 2005, 2006 The FlameRobin Development Team

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  $Id$

*/

//-----------------------------------------------------------------------------
#ifndef FR_INDEX_H
#define FR_INDEX_H

#include <vector>

#include "metadata/metadataitem.h"
//-----------------------------------------------------------------------------
class Index: public MetadataItem
{
public:
    // needs to be declared here as type is used in private section
    enum IndexType { itAscending, itDescending };

private:
    bool isSystemM;
    bool uniqueFlagM;
    bool activeM;
    IndexType indexTypeM;
    double statisticsM;
    std::vector<wxString> segmentsM;
protected:
    virtual void loadDescription();
    virtual void saveDescription(wxString description);
public:
    Index(bool unique, bool active, bool ascending, double statistics,
        bool system);

    virtual bool isSystem() const;
    bool isActive();
    bool isUnique();
    double getStatistics();
    IndexType getIndexType();
    wxString getFieldsAsString();
    std::vector<wxString> *getSegments();
    virtual void acceptVisitor(MetadataItemVisitor* v);
};
//-----------------------------------------------------------------------------
#endif
