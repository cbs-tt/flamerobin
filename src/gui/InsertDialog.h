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

#ifndef FR_INSERTDIALOG_H
#define FR_INSERTDIALOG_H
//-----------------------------------------------------------------------------
#include <wx/wx.h>
#include <wx/image.h>

#include "gui/BaseDialog.h"
class DataGridTable;
//-----------------------------------------------------------------------------
class InsertDialog: public BaseDialog
{
public:
    InsertDialog(wxWindow* parent, const wxString& tableName, DataGridTable *);
    void OnOkButtonClick(wxCommandEvent& event);

private:
    wxString tableNameM;
    void set_properties();
    void do_layout();

protected:
    wxStaticText* labelFieldName;
    wxStaticText* labelDataType;
    wxStaticText* labelModifiers;
    wxStaticText* labelValue;
    wxFlexGridSizer* flexSizerM;

    wxButton* button_ok;
    wxButton* button_cancel;
    virtual const wxString getName() const;
    virtual bool getConfigStoresWidth() const;
    virtual bool getConfigStoresHeight() const;

    DECLARE_EVENT_TABLE()
};
//-----------------------------------------------------------------------------
#endif