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
#ifndef FR_EVENT_FRAME_H
#define FR_EVENT_FRAME_H

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/panel.h>

#include "ibpp.h"

#include "core/Observer.h"
#include "controls/LogTextControl.h"
#include "gui/BaseFrame.h"

class Database;
class EventLogControl;
//-----------------------------------------------------------------------------
class EventWatcherFrame : public BaseFrame, public IBPP::EventInterface, public Observer
{
private:
    Database* databaseM;
    wxTimer timerM;
    bool monitoringM;
    bool skipEventsM;

    wxPanel* panel_controls;
    wxStaticText* static_text_monitored;
    wxStaticText* static_text_received;
    wxListBox* listbox_monitored;
    EventLogControl* eventlog_received;
    wxButton *button_add;
    wxButton *button_remove;
    wxButton *button_load;
    wxButton *button_save;
    wxButton *button_monitor;
    void createControls();
    void layoutControls();
    void updateControls();

    static wxString getFrameId(Database* db);

    void addEvents(wxString& s);    // multiline allowed
    void defineMonitoredEvents();
    virtual void ibppEventHandler(IBPP::IDatabase*, const std::string& name,
        int count);
protected:
    virtual const wxString getName() const;
public:
    EventWatcherFrame(wxWindow* parent, Database* db);

    void removeSubject(Subject* subject);
    void update();

    static EventWatcherFrame* findFrameFor(Database* db);
private:
    // event handling
    enum
    {
        ID_listbox_monitored = 101,
        ID_log_received,
        ID_button_add,
        ID_button_remove,
        ID_button_load,
        ID_button_save,
        ID_button_monitor,
        ID_timer
    };

    void OnButtonAddClick(wxCommandEvent& event);
    void OnButtonRemoveClick(wxCommandEvent& event);
    void OnButtonLoadClick(wxCommandEvent& event);
    void OnButtonSaveClick(wxCommandEvent& event);
    void OnButtonStartStopClick(wxCommandEvent& event);
    void OnListBoxSelected(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

    DECLARE_EVENT_TABLE()
};
//-----------------------------------------------------------------------------
#endif
