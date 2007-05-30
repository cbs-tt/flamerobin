/*
  Copyright (c) 2004-2007 The FlameRobin Development Team

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

#ifndef DATAGRIDROWS_H
#define DATAGRIDROWS_H
//-----------------------------------------------------------------------------
#include <vector>

#include <ibpp.h>
//----------------------------------------------------------------------
class DataGridRowBuffer;
class wxMBConv;
//----------------------------------------------------------------------
class ResultsetColumnDef
{
private:
    wxString nameM;
public:
    ResultsetColumnDef(const wxString& name);
    virtual ~ResultsetColumnDef();

    virtual wxString getAsString(DataGridRowBuffer* buffer) = 0;
    virtual unsigned getBufferSize() = 0;
    wxString getName();
    virtual bool isNumeric();
    virtual void setValue(DataGridRowBuffer* buffer, unsigned col,
        const IBPP::Statement& statement, wxMBConv* converter) = 0;
};
//----------------------------------------------------------------------
class DataGridRows
{
private:
    std::vector<ResultsetColumnDef*> columnDefsM;
    std::vector<DataGridRowBuffer*> buffersM;
    unsigned bufferSizeM;
public:
    DataGridRows();
    ~DataGridRows();

    bool addRow(const IBPP::Statement& statement, wxMBConv* converter);
    void clear();
    unsigned getRowCount();
    unsigned getRowFieldCount();
    wxString getRowFieldName(unsigned col);
    bool initialize(const IBPP::Statement& statement);
    bool isRowFieldNumeric(unsigned col);

    wxString getFieldValue(unsigned row, unsigned col);
    bool isFieldNull(unsigned row, unsigned col);
    void setFieldValue(unsigned row, unsigned col,
        const wxString& value);
};
//----------------------------------------------------------------------
#endif