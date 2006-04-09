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
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/menu.h>

#include "config/Config.h"
#include "ContextMenuMetadataItemVisitor.h"
#include "myTreeCtrl.h"
//-----------------------------------------------------------------------------
ContextMenuMetadataItemVisitor::ContextMenuMetadataItemVisitor(wxMenu* menu)
    : MetadataItemVisitor(), menuM(menu)
{
}
//-----------------------------------------------------------------------------
ContextMenuMetadataItemVisitor::~ContextMenuMetadataItemVisitor()
{
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Column& column)
{
    if (column.getTable()) // only for table columns
        addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Database&)
{
    menuM->Append(myTreeCtrl::Menu_Connect, _("&Connect"));
    menuM->Append(myTreeCtrl::Menu_ConnectAs, _("Connect &as..."));
    menuM->Append(myTreeCtrl::Menu_Disconnect, _("&Disconnect"));
    menuM->Append(myTreeCtrl::Menu_Reconnect, _("Reconnec&t"));
    menuM->Append(myTreeCtrl::Menu_Query, _("&Run a query..."));
    menuM->AppendSeparator();

    wxMenu* actions = new wxMenu();
    menuM->Append(0, _("Acti&ons"), actions);

    wxMenu* advanced = new wxMenu();
    menuM->Append(0, _("Ad&vanced"), advanced);

    menuM->Append(myTreeCtrl::Menu_DatabaseRegistrationInfo, _("Database registration &info..."));
    menuM->Append(myTreeCtrl::Menu_UnRegisterDatabase, _("&Unregister database"));

    // the actions submenu
    actions->Append(myTreeCtrl::Menu_DropDatabase, _("Drop database"));
    actions->Append(myTreeCtrl::Menu_Backup, _("&Backup database..."));
    actions->Append(myTreeCtrl::Menu_Restore, _("Rest&ore database..."));

    // the advanced submenu
    advanced->Append(myTreeCtrl::Menu_ShowConnectedUsers, _("&Show connected users"));
    advanced->Append(myTreeCtrl::Menu_MonitorEvents, _("&Monitor events"));
    advanced->Append(myTreeCtrl::Menu_DatabasePreferences, _("Database &preferences..."));

    menuM->AppendSeparator();
    menuM->Append(myTreeCtrl::Menu_DatabaseProperties, _("Data&base Properties"));
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Domain&)
{
    addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Exception&)
{
    addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Function&)
{
    addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Generator&)
{
    menuM->Append(myTreeCtrl::Menu_ShowGeneratorValue, _("Show &value"));
    menuM->Append(myTreeCtrl::Menu_SetGeneratorValue, _("&Set value"));
    addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(MetadataItem& metadataItem)
{
    NodeType type = metadataItem.getType();
    if (type == ntFunctions)
    {
        menuM->Append(myTreeCtrl::Menu_CreateObject, _("&Declare new..."));
        return;
    }

    if (type == ntGenerators)
        menuM->Append(myTreeCtrl::Menu_ShowAllGeneratorValues, _("Show &all values"));

    if (type == ntGenerators || type == ntTables || type == ntViews || type == ntProcedures ||
        type == ntTriggers || type == ntDomains || type == ntRoles || type == ntExceptions)
            menuM->Append(myTreeCtrl::Menu_CreateObject, _("Create &new..."));
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Procedure&)
{
    menuM->Append(myTreeCtrl::Menu_ExecuteProcedure, _("&Execute..."));
    addSelectMenu(false);       // false = not a table
    addRegularObjectMenu(true); // true = add Alter menu
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Role&)
{
    addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Root&)
{
    menuM->Append(myTreeCtrl::Menu_RegisterServer, _("&Register server..."));
    menuM->AppendSeparator();
    menuM->Append(wxID_ABOUT, _("&About FlameRobin..."));
    menuM->Append(wxID_PREFERENCES, _("&Preferences..."));
    menuM->AppendSeparator();
    menuM->Append(wxID_EXIT, _("&Quit"));
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Server&)
{
    menuM->Append(myTreeCtrl::Menu_RegisterDatabase, _("&Register existing database..."));
    menuM->Append(myTreeCtrl::Menu_CreateDatabase, _("Create &new database..."));
    menuM->Append(myTreeCtrl::Menu_RestoreIntoNew, _("Restore bac&kup into new database..."));
    menuM->AppendSeparator();
    menuM->Append(myTreeCtrl::Menu_GetServerVersion, _("Retrieve server &version"));
    menuM->Append(myTreeCtrl::Menu_ManageUsers, _("&Manage users..."));
    menuM->AppendSeparator();
    menuM->Append(myTreeCtrl::Menu_UnRegisterServer, _("&Unregister server"));
    menuM->Append(myTreeCtrl::Menu_ServerProperties, _("Server registration &info..."));
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Table&)
{
    menuM->Append(myTreeCtrl::Menu_Insert, _("&Insert into ..."));
    addSelectMenu(true);
    menuM->Append(myTreeCtrl::Menu_CreateTriggerForTable, _("Create new &trigger..."));
    addRegularObjectMenu();
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(Trigger&)
{
    addRegularObjectMenu(true); // true = add Alter menu
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::visit(View&)
{
    addSelectMenu(false);       // false = not a table
    addRegularObjectMenu(true); // true = add Alter menu
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::addSelectMenu(bool isTable)
{
    menuM->Append(myTreeCtrl::Menu_Browse, _("&Select * from ..."));
    menuM->Append(myTreeCtrl::Menu_BrowseColumns, _("Select &col1, col2, ... from ..."));
    menuM->AppendSeparator();
    if (config().get(wxT("ShowColumnsInTree"), true))
    {
        menuM->Append(myTreeCtrl::Menu_LoadColumnsInfo, _("Show columns in&fo"));
        if (isTable)
            menuM->Append(myTreeCtrl::Menu_AddColumn, _("&Add column..."));
        menuM->AppendSeparator();
    }
}
//-----------------------------------------------------------------------------
void ContextMenuMetadataItemVisitor::addRegularObjectMenu(bool alter)
{
    if (alter)
        menuM->Append(myTreeCtrl::Menu_AlterObject, _("&Alter..."));
    menuM->Append(myTreeCtrl::Menu_DropObject, _("Dr&op..."));
    menuM->Append(myTreeCtrl::Menu_ObjectProperties, _("Show P&roperties"));
}
//-----------------------------------------------------------------------------
