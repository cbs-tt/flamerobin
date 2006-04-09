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

#include <fstream>
#include <sstream>

#include "Identifier.h"
#include "SimpleParser.h"
//-----------------------------------------------------------------------------
// returns false if errors occur
bool SimpleParser::stripSql(wxString &sql)
{
    while (true)    // strip quotes and brackets
    {
        wxString::size_type p1, p2;
        p1 = sql.find(wxT("/*"));
        p2 = sql.find(wxT("'"));
        if (p1 == wxString::npos && p2 == wxString::npos)    // no more
            break;

        if (p1 != wxString::npos && (p1 < p2 || p2 == wxString::npos))
        {
            p2 = sql.find(wxT("*/"), p1);
            if (p2 == wxString::npos)
            {
                wxMessageBox(_("Cannot parse sql, please close the comments."), _("Error."), wxOK|wxICON_WARNING);
                return false;
            }
            sql.erase(p1, p2 - p1 + 1);
            continue;
        }

        if (p2 != wxString::npos && (p2 < p1 || p1 == wxString::npos))
        {
            p1 = p2;
            p2 = sql.find(wxT("'"), p1+1);
            if (p2 == wxString::npos)
            {
                wxMessageBox(_("Cannot parse sql, please close the quotes."), _("Error."), wxOK|wxICON_WARNING);
                return false;
            }
            sql.erase(p1, p2 - p1 + 1);
            continue;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
// get next token from sql wxString
// returns number of characters removed
wxString::size_type SimpleParser::nextToken(wxString& in, wxString& out)
{
    if (in.empty())
        return 0;

    wxString::size_type retval = 0;

    const wxString spaces(wxT("\n\r\t "));
    const wxString delims(wxT(";,"));
    const wxString all = spaces + delims;

    wxString::size_type pos, endp;
    if (in.find_first_of(spaces) == 0)        // strip starting spaces
    {
        pos = in.find_first_not_of(spaces);
        if (pos == wxString::npos)
            return 0;
        retval += pos;
        in.erase(0, pos);
    }

    pos = in.find_first_of(delims);            // starts with delims?
    if (pos == 0)
    {
        out = in.substr(0, 1);
        in.erase(0, 1);
        retval += 1;
        return retval;
    }

    endp = in.find_first_of(all);            // starts with other characters
    out = in.substr(0, endp);
    in.erase(0, endp);
    retval += endp;
    return retval;
}
//-----------------------------------------------------------------------------
// gets not-quoted table names from SELECT sql script
// input: sql statement without SELECT clause, i.e. should start with: "FROM"
//
// TODO: this needs to support Identifiers
wxString::size_type SimpleParser::getTableNames(std::vector<wxString>& list, wxString sql)
{
    sql += wxT(" ");     // parser needs blank space at end

    wxString::size_type retval = 0;
    wxString keywords[] = {wxT("LEFT"), wxT("RIGHT"), wxT("INNER"), wxT("OUTER"),
        wxT("FULL"), wxT("JOIN"), wxT("WHERE"), wxT("GROUP"), wxT("ORDER")};
    wxString ss(sql);
    wxString s;
    retval += nextToken(ss, s);            // remove FROM
    while (true)
    {
        wxString::size_type c = nextToken(ss, s);        // a table: push it to the list
        if (c == 0)
            return retval;
        retval += c;
        Identifier temp;
        temp.setFromSql(s);         // strip quotes if needed
        list.push_back(temp.get());

        while (true)
        {
            c = nextToken(ss, s);        // alias, comma, keyword (left, outer, inner, right, join, where, group, order), ;
            if (c == 0)
                return retval;
            if (s == wxT(";"))
                return retval;
            if (s == wxT(","))    // next is a table
            {
                retval += c;
                break;
            }

            bool is_join = false;
            for (int i = 0; i < sizeof(keywords) / sizeof(wxString); ++i)
            {
                if (s == keywords[i])
                {
                    if (i < 5)
                    {
                        int c2 = nextToken(ss, s);
                        if (c2 == 0)
                            break;
                        retval += c2;
                        i = -1;        // start from beginning
                        continue;
                    }
                    else if (s == wxT("JOIN"))    // 5
                    {
                        is_join = true;
                        break;
                    }
                    else
                        return retval;    // no more tables
                }
            }
            retval += c;
            if (is_join)
                break;
        }
    }

    // never gets here
    //return retval;
}
//-----------------------------------------------------------------------------
//! removes comments from sql statements, with taking care of single quotes
void SimpleParser::removeComments(wxString& sql, const wxString startComment, const wxString endComment)
{
    using namespace std;
    wxString::size_type oldpos = 0;
    while (true)
    {
        wxString::size_type pos = sql.find(startComment, oldpos);
        if (pos == wxString::npos)
            break;

        wxString::size_type quote = sql.find(wxT("'"), oldpos);
        if (quote != wxString::npos && quote < pos)    // move to the next quote
        {
            oldpos = 1 + sql.find(wxT("'"), quote+1);    // end quote
            continue;
        }

        oldpos = sql.find(endComment, pos + startComment.length());
        if (oldpos == wxString::npos)    // unclosed comment
            break;

        sql.erase(pos, oldpos - pos + endComment.length());
        oldpos = pos;
    }
}
//-----------------------------------------------------------------------------
