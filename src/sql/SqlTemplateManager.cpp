/*
  Copyright (c) 2004-2010 The FlameRobin Development Team

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

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/regex.h>

#include "config/Config.h"
#include "core/FRError.h"
#include "metadata/metadataitem.h"
#include "sql/SqlTemplateManager.h"

//-----------------------------------------------------------------------------
TemplateDescriptor::TemplateDescriptor(wxFileName templateFileName)
    : templateFileNameM(templateFileName), menuPositionM(0)
{
    loadDescriptionFromConfigFile();
}
//-----------------------------------------------------------------------------
void TemplateDescriptor::loadDescriptionFromConfigFile()
{
    wxFileName confFileName(templateFileNameM);
    confFileName.SetExt(wxT("conf"));
    if (confFileName.FileExists())
    {
        configM.setConfigFileName(confFileName);
        menuCaptionM = configM.get(wxT("templateInfo/menuCaption"), templateFileNameM.GetName());
        menuPositionM = configM.get(wxT("templateInfo/menuPosition"), 0);
        if (!configM.getValue(wxT("templateInfo/matchesType"), matchesTypeM))
            matchesTypeM = wxT(".*");
        if (!configM.getValue(wxT("templateInfo/matchesName"), matchesNameM))
            matchesNameM = wxT(".*");
    }
    else
    {
        menuCaptionM = templateFileNameM.GetName();
        menuPositionM = 0;
        matchesTypeM = wxT(".*");
        matchesNameM = wxT(".*");
    }
}
//-----------------------------------------------------------------------------
bool TemplateDescriptor::operator<(const TemplateDescriptor& right) const
{
    int p = getMenuPosition();
    int rp = right.getMenuPosition();
    if (p == rp)
        return (getMenuCaption().Cmp(right.getMenuCaption()) < 0);
    else
        return (p < rp);
}
//-----------------------------------------------------------------------------
wxString TemplateDescriptor::getMenuCaption() const
{
    if (!menuCaptionM.IsEmpty())
        return menuCaptionM;
    return templateFileNameM.GetName();
}
//-----------------------------------------------------------------------------
 bool TemplateDescriptor::matches(const MetadataItem& metadataItem) const
{
    wxRegEx typeRegEx(matchesTypeM, wxRE_ADVANCED);
    if (!typeRegEx.IsValid())
        throw FRError(_("Invalid regex"));
    if (typeRegEx.Matches(metadataItem.getTypeName()))
    {
        wxRegEx nameRegEx(matchesNameM, wxRE_ADVANCED);
        if (!nameRegEx.IsValid())
            throw FRError(_("Invalid regex"));
        if (nameRegEx.Matches(metadataItem.getName_()))
            return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
//! needed in checkDescriptorsSorted() to sort on objects instead of pointers
bool templateDescriptorPointerLT(const TemplateDescriptor* left, const TemplateDescriptor* right)
{
    return *left < *right;
}
//-----------------------------------------------------------------------------
SqlTemplateManager::SqlTemplateManager(const MetadataItem& metadataItem)
    : metadataItemM(metadataItem)
{
    collectDescriptors();
}
//-----------------------------------------------------------------------------
void SqlTemplateManager::collectDescriptors()
{
    wxArrayString fileNames;
    wxString path = config().getSqlTemplatesPath();
    wxDir::GetAllFiles(path, &fileNames, wxT("*.sql"), wxDIR_FILES);
    
    descriptorsM.clear();
    for (wxString::size_type i = 0; i < fileNames.Count(); i++)
    {
        TemplateDescriptor *td = new TemplateDescriptor(fileNames[i]);
        try
        {
            if (td->matches(metadataItemM))
                descriptorsM.push_back(td);
            else
                delete td;
        }
        catch (...)
        {
            if (td)
                delete td;
        }
    }
    descriptorsM.sort(templateDescriptorPointerLT);
}
//-----------------------------------------------------------------------------
TemplateDescriptorList::const_iterator SqlTemplateManager::descriptorsBegin() const
{
    return descriptorsM.begin();
}
//-----------------------------------------------------------------------------
TemplateDescriptorList::const_iterator SqlTemplateManager::descriptorsEnd() const
{
    return descriptorsM.end();
}
//-----------------------------------------------------------------------------