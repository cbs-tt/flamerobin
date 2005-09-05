//-----------------------------------------------------------------------------
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

  The Initial Developer of the Original Code is Gregory Sapunkov.

  Portions created by the original developer
  are Copyright (C) 2004 Gregory Sapunkov.

  All Rights Reserved.

  $Id$

  Contributor(s): Milan Babuskov, Nando Dessena
*/
//-----------------------------------------------------------------------------

#ifndef FR_VISITOR_H
#define FR_VISITOR_H

// [GoF] Visitor pattern. Abstract generic Visitor.

class Element;
//-----------------------------------------------------------------------------
class Visitor
{
protected:
    Visitor() {};
    virtual ~Visitor() {};
    virtual void defaultAction();
public:
    virtual void visit(Element& element);
public:
};
//-----------------------------------------------------------------------------
#endif //FR_VISITOR_H