/*
  The contents of this file are subject to the Initial Developer's Public
  License Version 1.0 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License here:
  http://www.flamerobin.org/license.html.

  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.

  The Original Code is FlameRobin (TM).

  The Initial Developer of the Original Code is Milan Babuskov.

  Portions created by the original developer
  are Copyright (C) 2004 Milan Babuskov.

  All Rights Reserved.

  $Id$

  Contributor(s): Nando Dessena, Michael Hieke
*/

//-----------------------------------------------------------------------------
#ifndef FR_METADATAITEM_H
#define FR_METADATAITEM_H

#include <wx/string.h>

#include <algorithm>
#include <vector>

#include "core/Element.h"
#include "core/Subject.h"

class Root;
class Database;
class Dependency;
class MetadataItemVisitor;
//-----------------------------------------------------------------------------
typedef enum { ntUnknown, ntRoot, ntServer, ntDatabase,
    // each item type and (if applicable) its parent type
    ntTable, ntTables, ntView, ntViews, ntProcedure, ntProcedures,
    ntTrigger, ntTriggers, ntGenerator, ntGenerators, ntFunction, ntFunctions,
    ntSysTable, ntSysTables, ntException, ntExceptions, ntDomain, ntDomains,
    ntRole, ntRoles, ntColumn, ntParameter, ntIndex,
    // these are used only for images
    ntPrimaryKey, ntComputed,
    ntLastType
} NodeType;
//-----------------------------------------------------------------------------
NodeType getTypeByName(wxString name);
//-----------------------------------------------------------------------------
class MetadataItem: public Element, public Subject
{
private:
    wxString nameM;
    MetadataItem* parentM;
    wxString descriptionM;
    bool descriptionLoadedM;
    void setDescriptionM(wxString description);

protected:
    NodeType typeM;
    MetadataItem* getParentObjectOfType(NodeType type) const;

    virtual void loadDescription();
    void loadDescription(wxString loadStatement);
    virtual void saveDescription(wxString description);
    void saveDescription(wxString saveStatement, wxString description);

    virtual void lockChildren();
    virtual void unlockChildren();
public:
    MetadataItem();
    virtual ~MetadataItem();

    virtual void lockSubject();
    virtual void unlockSubject();

    bool getDependencies(std::vector<Dependency>& list, bool ofObject);  // load from db

    Database *getDatabase() const;
    Root* getRoot() const;

    virtual bool getChildren(std::vector<MetadataItem *>& temp);
    virtual size_t getChildrenCount() const { return 0; };
    void drop();    // removes its children (by calling drop() for each) and notifies it's parent
    virtual bool orderedChildren() const { return false; };

    // returns CREATE SQL statement template
    virtual wxString getCreateSqlTemplate() const { return wxT(""); };

    // returns complete DROP SQL statement
    virtual wxString getDropSqlStatement() const;

    // getters/setters
    virtual MetadataItem *getParent() const;
    void setParent(MetadataItem *parent);
    virtual const wxString& getName() const;
    virtual wxString getPrintableName();
    void setName(wxString name);
    virtual NodeType getType() const;
    void setType(NodeType type);

    // returns the name of the data type (f. ex. TABLE)
    virtual const wxString getTypeName() const;

    // returns the item path, currently only used to store settings in config().
    // It could also be used to locate items in the DBH tree.
    virtual const wxString getItemPath() const;

    // returns the id wxString of the metadata item that contributes to the path. The
    // predefined implementation just returns getId().
    virtual const wxString getPathId() const;
    // returns the id of the item (to be saved in config files, etc.).
    // The predefined implementation just returns getName().
    virtual const wxString getId() const;

    // items description (in database)
    wxString getDescription();
    void setDescription(wxString description);

    // returns true if the metadata item is a system (as opposed to user-defined) item.
    virtual bool isSystem() const;

    static const wxString pathSeparator;
    virtual void acceptVisitor(MetadataItemVisitor* visitor);
};
//-----------------------------------------------------------------------------
//! masks the object it points to so others see it transparently
class Dependency: public MetadataItem
{
private:
    MetadataItem *objectM;
    std::vector<wxString> fieldsM;

public:
    virtual MetadataItem *getParent() const;
    virtual const wxString& getName() const;
    virtual NodeType getType() const;
    virtual const wxString getTypeName() const;
    MetadataItem *getDependentObject() const;

    Dependency(MetadataItem *object);
    wxString getFields() const;
    void addField(const wxString& name);
    void setFields(const std::vector<wxString>& fields);
    bool operator== (const Dependency& other) const;
    bool operator!= (const Dependency& other) const;
};
//-----------------------------------------------------------------------------
#endif //FR_METADATAITEM_H
