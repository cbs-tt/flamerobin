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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/timer.h>

#include "config/Config.h"
#include "gui/BackupRestoreBaseFrame.h"
#include "gui/controls/LogTextControl.h"
#include "metadata/database.h"
#include "metadata/server.h"
#include "ugly.h"
//-----------------------------------------------------------------------------
BackupRestoreBaseFrame::BackupRestoreBaseFrame(wxWindow* parent, Database* db)
    : BaseFrame(parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    databaseM = db;
    serverM = db->getServer();

    threadM = 0;
    threadMsgTimeMillisM = 0;
    verboseMsgsM = true;
    storageNameM = wxT("unassigned");

    // create controls in constructor of descendant class (correct tab order)
    panel_controls = 0;
    label_filename = 0;
    text_ctrl_filename = 0;
    button_browse = 0;
    checkbox_showlog = 0;
    button_start = 0;
    text_ctrl_log = 0;

#include "backup.xpm"
    wxBitmap bmp(backup_xpm);
    wxIcon icon;
    icon.CopyFromBitmap(bmp);
    SetIcon(icon);
}
//-----------------------------------------------------------------------------
//! implementation details
void BackupRestoreBaseFrame::addThreadMsg(const wxString msg,
    bool& notificationNeeded)
{
    notificationNeeded = false;
    wxLongLong millisNow = ::wxGetLocalTimeMillis();

    wxCriticalSectionLocker locker(critsectM);
    threadMsgsM.Add(msg);
    // we post no more than 10 events per second to prevent flooding of
    // the message queue, and to keep the frame responsive for user interaction
    if ((millisNow - threadMsgTimeMillisM).GetLo() > 100)
    {
        threadMsgTimeMillisM = millisNow;
        notificationNeeded = true;
    }
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::cancelBackupRestore()
{
    if (threadM != 0)
    {
        threadM->Delete();
        threadM = 0;
    }
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::clearLog()
{
    msgKindsM.Clear();
    msgsM.Clear();
    text_ctrl_log->ClearAll();
}
//-----------------------------------------------------------------------------
bool BackupRestoreBaseFrame::Destroy()
{
    cancelBackupRestore();
    return BaseFrame::Destroy();
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::doReadConfigSettings(const wxString& prefix)
{
    BaseFrame::doReadConfigSettings(prefix);

    bool verbose = true;
    config().getValue(prefix + Config::pathSeparator + wxT("verboselog"),
        verbose);
    checkbox_showlog->SetValue(verbose);

    wxString bkfile;
    config().getValue(prefix + Config::pathSeparator + wxT("backupfilename"),
        bkfile);
    if (!bkfile.empty())
        text_ctrl_filename->SetValue(bkfile);
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::doWriteConfigSettings(const wxString& prefix) const
{
    BaseFrame::doWriteConfigSettings(prefix);
    config().setValue(prefix + Config::pathSeparator + wxT("verboselog"),
        checkbox_showlog->GetValue());
    config().setValue(prefix + Config::pathSeparator + wxT("backupfilename"),
        text_ctrl_filename->GetValue());
}
//-----------------------------------------------------------------------------
const wxString BackupRestoreBaseFrame::getStorageName() const
{
    if (storageNameM == wxT("unassigned"))
    {
        StorageGranularity g = sgObject;
        config().getValue(getName() + wxT("StorageGranularity"), g);

        switch (g)
        {
        case sgFrame:
            storageNameM = getName();
            break;
        case sgObject:
            storageNameM = getName() + Config::pathSeparator + databaseM->getItemPath();
            break;
        default:
            storageNameM = wxT("");
            break;
        }
    }
    return storageNameM;
}
//-----------------------------------------------------------------------------
bool BackupRestoreBaseFrame::startThread(wxThread* thread)
{
    wxASSERT(threadM == 0);
    if (wxTHREAD_NO_ERROR != thread->Create())
    {
        ::wxMessageBox(_("Error creating thread!"), _("Error"), wxOK|wxICON_ERROR);
        delete thread;
        return false;
    }
    if (wxTHREAD_NO_ERROR != thread->Run())
    {
        ::wxMessageBox(_("Error starting thread!"), _("Error"), wxOK|wxICON_ERROR);
        delete thread;
        return false;
    }
    threadM = thread;
    return true;
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::threadOutputMsg(const wxString msg, MsgKind kind)
{
    wxString s(msg);
    switch (kind)
    {
    case error_message:
        s.Prepend(wxT("e"));
        break;
    case important_message:
        s.Prepend(wxT("i"));
        break;
    case progress_message:
        s.Prepend(wxT("p"));
        break;
    default:
        wxASSERT(false);
        return;
    }
    bool doPostMsg = false;
    addThreadMsg(s, doPostMsg);
    if (doPostMsg)
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_thread_output);
        wxPostEvent(this, event);
    }
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::updateMessages(size_t firstmsg, size_t lastmsg)
{
    if (lastmsg > msgsM.GetCount())
        lastmsg = msgsM.GetCount();
    for (size_t i = firstmsg; i < lastmsg; i++)
    {
        switch ((MsgKind)msgKindsM[i])
        {
            case progress_message:
                if (verboseMsgsM)
                    text_ctrl_log->logMsg(msgsM[i]);
                break;
            case important_message:
                text_ctrl_log->logImportantMsg(msgsM[i]);
                break;
            case error_message:
                text_ctrl_log->logErrorMsg(msgsM[i]);
                break;
        }
    }
}
//-----------------------------------------------------------------------------
//! event handlers
BEGIN_EVENT_TABLE(BackupRestoreBaseFrame, BaseFrame)
    EVT_CHECKBOX(BackupRestoreBaseFrame::ID_checkbox_showlog, BackupRestoreBaseFrame::OnVerboseLogChange)
    EVT_MENU(BackupRestoreBaseFrame::ID_thread_finished, BackupRestoreBaseFrame::OnThreadFinished)
    EVT_MENU(BackupRestoreBaseFrame::ID_thread_output, BackupRestoreBaseFrame::OnThreadOutput)
    EVT_TEXT(BackupRestoreBaseFrame::ID_text_ctrl_filename, BackupRestoreBaseFrame::OnSettingsChange)
END_EVENT_TABLE()
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::OnSettingsChange(wxCommandEvent& WXUNUSED(event))
{
    if (IsShown())
        updateControls();
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::OnThreadFinished(wxCommandEvent& event)
{
    threadM = 0;
    OnThreadOutput(event);
    updateControls();
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::OnThreadOutput(wxCommandEvent& WXUNUSED(event))
{
    wxCriticalSectionLocker locker(critsectM);
    threadMsgTimeMillisM = ::wxGetLocalTimeMillis();

    size_t first = msgsM.GetCount();
    for (size_t i = 0; i < threadMsgsM.GetCount(); i++)
    {
        wxString s(threadMsgsM[i]);
        if (s.Length() == 0)
            continue;
        switch (s.GetChar(0))
        {
        case 'e':
            msgKindsM.Add((int)error_message);
            break;
        case 'i':
            msgKindsM.Add((int)important_message);
            break;
        case 'p':
            msgKindsM.Add((int)progress_message);
            break;
        }
        // this depends on server type, so just in case...
        if (s.Last() != '\n')
            s.Append('\n');
        msgsM.Add(s.Mid(1));
    }
    threadMsgsM.Clear();

    updateMessages(first, msgsM.GetCount());
}
//-----------------------------------------------------------------------------
void BackupRestoreBaseFrame::OnVerboseLogChange(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor wait;
    verboseMsgsM = checkbox_showlog->IsChecked();

    text_ctrl_log->Freeze();
    text_ctrl_log->ClearAll();
    updateMessages(0, msgsM.GetCount());
    text_ctrl_log->Thaw();
}
//-----------------------------------------------------------------------------
