/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2021 The EVEmu Team
    For the latest information visit https://evemu.dev
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:        Zhur
    Rewrite:    Allan
*/


#include "eve-xmlpktgen.h"

#include "DecodeGenerator.h"

ClassDecodeGenerator::ClassDecodeGenerator(FILE* outputFile)
: Generator(outputFile),
  mName(nullptr),
  mItemNumber(0)
{
    RegisterProcessors();
}

bool ClassDecodeGenerator::ProcessElementDef(const TiXmlElement* field)
{
    mName = field->Attribute("name");
    if (!mName)  {
        _log(COMMON__ERROR, "<element> at line %d is missing the name attribute, skipping.", field->Row());
        return false;
    }

    const TiXmlElement* main = field->FirstChildElement();

    fprintf(mOutputFile,
        "bool %s::Decode(PyDataType* packet) {\n",
        mName
   );

    mItemNumber = 0;

    push("packet");
    if (!ParseElement(main))
        return false;

    fprintf(mOutputFile,
        "    return true;\n"
        "}\n\n"
        "bool %s::Decode(PyDataType** packet) {\n"
        "    bool res = Decode(*packet);\n"
        "    PyDecRef(*packet);\n"
        "    *packet = nullptr;\n"
        "    return res;\n"
        "}\n\n"
        "bool %s::Decode(%s** packet) {\n"
        "    //quick forwarder to avoid making the user cast it if they have a properly typed object\n"
        "    return Decode((PyDataType**)packet);\n"
        "}\n\n",
        mName,
        mName, GetEncodeType(main)
   );

    return true;
}

bool ClassDecodeGenerator::ProcessElement(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessElement field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s.Decode(%s)) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: unable to decode element %s\");\n"
        "        return false;\n"
        "    }\n\n",
        name, v,
            mName, name
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessElementPtr(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessElementPtr field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* type = field->Attribute("type");
    if (!type) {
        _log(COMMON__ERROR, "field at line %d is missing the type attribute, skipping.", field->Row());
        return false;
    }

    const char* v = top();
    fprintf(mOutputFile,
        "    SafeDelete(%s);\n"
        "    %s = new %s;\n\n"
        "    if (!%s->Decode(%s)) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: unable to decode element %s\");\n"
        "        return false;\n"
        "    }\n\n",
        name,
        name, type,

        name, v,
            mName, name
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessRaw(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessRaw field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* unusual = field->Attribute("unusual");
    const char* v = top();
/*
    if (unusual != nullptr) {
        fprintf(mOutputFile,
                "    if (%s->IsFloat()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" unusual is enabled. %s was decoded as PyFloat\");\n"
                "        %s = %s->as<PyFloat>()->value();\n"
                "    } else if (%s->IsLong()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" unusual is enabled. %s was decoded as PyLong\");\n"
                "        %s = %s->AsLong()->value();\n"
                "    } else if (%s->IsBool()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" unusual is enabled. %s was decoded as PyBool\");\n"
                "        %s = %s->as<PyBool>()->value();\n"
                "    } else if (%s->IsString()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" unusual is enabled. %s was decoded as PyString\");\n"
                "        %s = %s->as<PyString>()->content();\n"
                "    } else if (%s->IsWString()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" unusual is enabled. %s was decoded as PyWString\");\n"
                "        %s = %s->AsWString()->content();\n"
                "    } else {\n",
                "        _log(XMLP__DECODE_WARNING, \" unusual is enabled. %s was decoded as PyDataType\");\n"
                "        PySafeDecRef(%s);\n"
                "        %s = %s;\n"
                "        PyIncRef(%s);\n"
                "    }\n\n",
                v,name,
                name, v,
                v,name,
                name, v,
                v,name,
                name, v,
                v,name,
                name, v,
                v,name,
                name, v,
                name,
                name,
                name, v,
                name
        );
    } else {
        */
        fprintf(mOutputFile,
                "    PySafeDecRef(%s);\n"
                "    %s = %s;\n"
                "    PyIncRef(%s);\n\n",
                name,
                name, v,
                name
        );
    //}

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessInt(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessInt field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }
    /** @note:  commented code is depreciated in favor of PyDataType::IntegerValue(), which tests and decodes as integers */

    //const char* safe = field->Attribute("safe");
    const char* none_marker = field->Attribute("none_marker");

    const char* v = top();
    if (none_marker != nullptr)
        fprintf(mOutputFile,
                "    if (%s->is<PyNone>())\n"
                "        %s = %s;\n"
                "    else\n",
                v,
                name, none_marker
        );

    fprintf(mOutputFile, "    %s = %s->i64();\n", name, v);

    /*
    fprintf(mOutputFile,
            "    if (%s->IsInt())\n"
            "        %s = %s->i64();\n"
            "    else\n",
            v,
            name, v
    );

    if (safe != nullptr)
        fprintf(mOutputFile,
                "    if (%s->IsFloat()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" Safe is enabled. %s was decoded as PyFloat\");\n"
                "        %s = %s->as<PyFloat>()->value();\n"
                "    } else if (%s->IsLong()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" Safe is enabled. %s was decoded as PyLong\");\n"
                "        %s = %s->AsLong()->value();\n"
                "    } else if (%s->IsBool()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" Safe is enabled. %s was decoded as PyBool\");\n"
                "        %s = %s->as<PyBool>()->value();\n"
                "    } else\n",
                v,name,
                name, v,
                v,name,
                name, v,
                v,name,
                name, v
        );

    fprintf(mOutputFile,
            "    {\n"
            "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not an Integer: %%s\", %s->TypeString());\n"
            "        return false;\n"
            "    }\n"
            "\n",
            mName, name, v
    );
*/

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessLong(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessLong field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* v = top();
    fprintf(mOutputFile, "    %s = %s->i64();\n", name, v);

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessReal(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessReal field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* safe = field->Attribute("safe");
    const char* none_marker = field->Attribute("none_marker");

    const char* v = top();
    if (none_marker != nullptr)
        fprintf(mOutputFile,
                "    if (%s->is<PyNone>())\n"
                "        %s = %s;\n"
                "    else ",
                v,
                name, none_marker
       );

    fprintf(mOutputFile,
            "    if (%s->is<PyFloat>())\n"
            "        %s = %s->as<PyFloat>()->value();\n"
            "    else ",
            v,
            name, v
    );

    if (safe != nullptr) {
        fprintf(mOutputFile, "\n    %s = %s->i64();\n", name, v);
    } else {
        fprintf(mOutputFile,
                "{\n"
                "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a Double: %%s\", %s->TypeString());\n"
                "        return false;\n"
                "    }\n\n",
                mName, name, v
        );
    }

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessBool(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessBool field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* v = top();
    fprintf(mOutputFile, "    %s = %s->i64();\n", name, v);

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessNone(const TiXmlElement* field)
{
    const char* v = top();
    fprintf(mOutputFile,
            "    if (!%s->is<PyNone>()) {\n"
            "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a None: %%s\", %s->TypeString());\n"
            "        return false;\n"
            "    }\n\n",
            v, mName, v, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessBuffer(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessBuffer field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* v = top();
    fprintf(mOutputFile,
            "    PySafeDecRef(%s);\n"
            "    if (%s->is<PyBuffer>()) {\n"
            "        %s = %s->as<PyBuffer>();\n"
            "        PyIncRef(%s);\n"
            "    } else if (%s->is<PyString>()) {\n"
            "        %s = new PyBuffer(*%s->as<PyString>());\n"
            "    } else {\n"
            "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a buffer: %%s\", %s->TypeString());\n"
            "        return false;\n"
            "    }\n\n",
            name,
            v,
            name, v,
            name,
            v,
            name, v,
            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessString(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessString field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* safe = field->Attribute("safe");
    const char* none_marker = field->Attribute("none_marker");
    const char* v = top();
    if (none_marker != nullptr)
        fprintf(mOutputFile,
                "    if (%s->is<PyNone>())\n"
                "        %s = \"%s\";\n"
                "    else ",
                v,
                name, none_marker
       );

    fprintf(mOutputFile,
            "    if (%s->is<PyString>())\n"
            "        %s = %s->as<PyString>()->content();\n"
            "    else ",
            v,
            name, v
    );

    fprintf(mOutputFile,
            "    {\n"
            "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a string: %%s\", %s->TypeString());\n"
            "        return false;\n"
            "    }\n"
            "\n",
            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessStringInline(const TiXmlElement* field)
{
    const char* value = field->Attribute("value");
    if (!value) {
        _log(COMMON__ERROR, "String element at line %d has no value attribute.", field->Row());
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "string_%u", ++mItemNumber);

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyString>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a string: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyString* %s(%s->as<PyString>());\n"
        "    if (\"%s\" != %s->content()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: expected %s to be '%s', but it's '%%s'\", %s->content().c_str());\n"
        "        return false;\n"
        "    }\n\n",
        v,
            mName, iname, v,
        iname, v,

        value, iname,
            mName, iname, value, iname
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessWString(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessWString field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* safe = field->Attribute("safe");
    const char* none_marker = field->Attribute("none_marker");
    const char* v = top();
    if (none_marker != nullptr)
        fprintf(mOutputFile,
                "    if (%s->is<PyNone>())\n"
                "        %s = \"%s\";\n"
                "    else ",
                v,
                name, none_marker
        );

    /** @todo update these to use  PyDataType::StringContent()  */
    fprintf(mOutputFile,
            "    if (%s->is<PyString>() && %s->as<PyString>()->utf8())\n"
            "        %s = %s->as<PyString>()->content();\n"
            "    else ",
            v, v,
            name, v
    );

    if (safe != nullptr)
        fprintf(mOutputFile,
                "    if (%s->is<PyString>()) {\n"
                "        _log(XMLP__DECODE_WARNING, \" Safe is enabled. %s was decoded as PyString\");\n"
                "        %s = %s->as<PyString>()->content();\n"
                "    } else ",
                v,name,
                name, v
        );

    fprintf(mOutputFile,
            "    {\n"
            "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a string: %%s\", %s->TypeString());\n"
            "        return false;\n"
            "    }\n\n",
            mName, name, v
    );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessWStringInline(const TiXmlElement* field)
{
    const char* value = field->Attribute("value");
    if (!value)   {
        _log(COMMON__ERROR, "WString element at line %d has no value attribute.", field->Row());
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "wstring_%u", ++mItemNumber);

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyString>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a wstring: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyString* %s(%s->as<PyString>());\n"
        "    if (\"%s\" != %s->content()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: expected %s to be '%s', but it's '%%s'\", %s->content().c_str());\n"
        "        return false;\n"
        "    }\n\n",
        v,
            mName, iname, v,
        iname, v,

        value, iname,
            mName, iname, value, iname
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessToken(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessToken field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    bool optional = false;
    const char* optional_str = field->Attribute("optional");
    if (optional_str != nullptr)
        optional = str2<bool>(optional_str);

    fprintf(mOutputFile,
        "    PySafeDecRef(%s);\n",
        name
   );

    const char* v = top();
    if (optional)
        fprintf(mOutputFile,
            "    if (%s->is<PyNone>())\n"
            "        %s = nullptr;\n"
            "    else ",
            v,
                name
       );

    fprintf(mOutputFile,
        "    if (%s->is<PyToken>()) {\n"
        "        %s = %s->as<PyToken>();\n"
        "        PyIncRef(%s);\n"
        "    } else {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a token: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n",
        v,
        name, v,
            name,

            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessTokenInline(const TiXmlElement* field)
{
    const char* value = field->Attribute("value");
    if (!value)  {
        _log(COMMON__ERROR, "Token element at line %d has no value attribute.", field->Row());
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "token_%u", ++mItemNumber);

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyToken>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a token: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyToken* %s(%s->as<PyToken>());\n"
        "    if (%s->content() != \"%s\") {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: expected %s to be '%s', but it's '%%s'\", %s->content().c_str());\n"
        "        return false;\n"
        "    }\n\n",
        v,
            mName, iname, v,
        iname, v,

        iname, value,
            mName, iname, value, iname
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessObject(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessObject field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    bool optional = false;
    const char* optional_str = field->Attribute("optional");
    if (nullptr != optional_str)
        optional = str2<bool>(optional_str);

    fprintf(mOutputFile,
        "    PySafeDecRef(%s);\n",
        name
   );

    const char* v = top();
    if (optional) {
        fprintf(mOutputFile,
            "    if (%s->is<PyNone>())\n"
            "        %s = nullptr;\n"
            "    else ",
            v,
                name
       );
    }

    //make sure its an object
    fprintf(mOutputFile,
        "    if (%s->is<PyObject>()) {\n"
        "        %s = %s->as<PyObject>();\n"
        "        PyIncRef(%s);\n"
        "    } else {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong type: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n",
        v,
        name, v,
            name,

            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessObjectInline(const TiXmlElement* field)
{
    char iname[16];
    snprintf(iname, sizeof(iname), "obj_%u", ++mItemNumber);

    //make sure its an object
    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyObject>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong type: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyObject* %s(%s->as<PyObject>());\n\n",
        v,
            mName, iname, v,
        iname, v
   );

    char aname[32];
    snprintf(aname, sizeof(aname), "%s->arguments()", iname);
    push(aname);

    char tname[32];
    snprintf(tname, sizeof(tname), "%s->type()", iname);
    push(tname);

    if (!ParseElementChildren(field, 2))
        return false;

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessObjectEx(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessObjectEx field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }
    const char* type = field->Attribute("type");
    if (type == nullptr)  {
        std::cout << std::endl <<  "DecodeGen::ProcessObjectEx field at line " << field->Row() << " is missing the type attribute, skipping.";
        return false;
    }

    bool optional = false;
    const char* optional_str = field->Attribute("optional");
    if (optional_str != nullptr)
        optional = str2<bool>(optional_str);

    fprintf(mOutputFile,
        "    PySafeDecRef(%s);\n",
        name
   );

    const char* v = top();
    if (optional) {
        fprintf(mOutputFile,
            "    if (%s->is<PyNone>())\n"
            "        %s = nullptr;\n"
            "    else",
            v,
                name
       );
    }

    fprintf(mOutputFile,
        "    if (%s->is<PyObjectEx>()) {\n"
        "        %s = (%s*)%s->as<PyObjectEx>();\n"
        "        PyIncRef(%s);\n"
        "    } else {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong type: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n",
        v,
        name, type, v,
            name,

            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessTuple(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessTuple field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    bool optional = false;
    const char* optional_str = field->Attribute("optional");
    if (optional_str != nullptr)
        optional = str2<bool>(optional_str);

    fprintf(mOutputFile,
        "    PySafeDecRef(%s);\n",
        name
   );

    const char* v = top();
    if (optional)
        fprintf(mOutputFile,
            "    if (%s->is<PyNone>())\n"
            "        %s = nullptr;\n"
            "    else",
            v,
                name
       );

    fprintf(mOutputFile,
        "    if (%s->is<PyTuple>()) {\n"
        "        %s = %s->as<PyTuple>();\n"
        "        PyIncRef(%s);\n"
        "    } else {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a tuple: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n",
        v,
        name, v,
            name,

            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessTupleInline(const TiXmlElement* field)
{
    //first, we need to know how many elements this tuple has:
    const TiXmlNode* i(nullptr);

    uint8 count = 0;
    while ((i = field->IterateChildren(i))) {
        if (i->Type() == TiXmlNode::TINYXML_ELEMENT)
            ++count;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "tuple%u", ++mItemNumber);

    const char* v = top();
    //now we can generate the tuple decl
    fprintf(mOutputFile,
        "    if (!%s->is<PyTuple>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong type: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyTuple* %s(%s->as<PyTuple>());\n"
        "    if (%s->size() != %u) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong size: expected %d, but got %%lu\", %s->size());\n"
        "        return false;\n"
        "    }\n\n",
        v,
            mName, iname, v,

        iname, v,
        iname, count,
            mName, iname, count, iname
   );

    //now we need to queue up all the storage locations for the fields
    //need to be backward
    char varname[64];
    while(count-- > 0) {
        snprintf(varname, sizeof(varname), "%s->at(%u)", iname, count);
        push(varname);
    }

    if (!ParseElementChildren(field))
        return false;

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessList(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessList field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    //this should be done better:
    bool optional = false;
    const char* optional_str = field->Attribute("optional");
    if (optional_str != nullptr)
        optional = str2<bool>(optional_str);

    fprintf(mOutputFile,
        "    PySafeDecRef(%s);\n",
        name
   );

    const char* v = top();
    if (optional)
        fprintf(mOutputFile,
            "    if (%s->is<PyNone>())\n"
            "        %s = nullptr;\n"
            "    else",
            v,
            name
       );

    fprintf(mOutputFile,
        "    if (%s->is<PyList>()) {\n"
        "        %s = %s->as<PyList>();\n"
        "        PyIncRef(%s);\n"
        "    } else {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a list: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n",
        v,
        name, v,
            name,

            mName, name, v
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessListInline(const TiXmlElement* field)
{
    //first, we need to know how many elements this tuple has:
    const TiXmlNode* i(nullptr);

    uint8 count = 0;
    while ((i = field->IterateChildren(i))) {
        if (i->Type() == TiXmlNode::TINYXML_ELEMENT)
            ++count;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "list%u", ++mItemNumber);

    const char* v = top();
    //now we can generate the tuple decl
    fprintf(mOutputFile,
        "    if (!%s->is<PyList>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a list: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyList* %s(%s->as<PyList>());\n"
        "    if (%s->size() != %u) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong size: expected %d, but got %%lu\", %s->size());\n"
        "        return false;\n"
        "    }\n\n",
        v,
            mName, iname, v,
        iname, v,
        iname, count,
            mName, iname, count, iname
   );

    //now we need to queue up all the storage locations for the fields
    //need to be backward
    char varname[64];
    while(count-- > 0) {
        snprintf(varname, sizeof(varname), "%s->at(%u)", iname, count);
        push(varname);
    }

    if (!ParseElementChildren(field))
        return false;

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessListInt(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessListInt field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "list_%u", ++mItemNumber);

    const char* v = top();
    //make sure its a list
    fprintf(mOutputFile,
        "    if (!%s->is<PyList>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a list: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyList* %s(%s->as<PyList>());\n"
        "    %s.clear();\n\n"
        "    PyList::const_iterator %s_cur = %s->begin();\n"
        "    for (size_t %s_index(0); %s_cur != %s->end(); ++%s_cur, ++%s_index) {\n"
        "        if (!(*%s_cur)->is<PyInt>()) {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Element %%u in list %s is not an integer: %%s\", %s_index, (*%s_cur)->TypeString());\n"
        "            return false;\n"
        "        }\n\n"
        "        const PyInt* t = (*%s_cur)->as<PyInt>();\n"
        "        %s.push_back(t->value());\n"
        "    }\n\n",
        v,
            mName, name, v,

        iname, v,
        name,

        iname, iname,
        iname, iname, iname, iname, iname,
            iname,
                mName, iname, iname, iname,

            iname,
            name
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessListLong(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessListLong field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "list_%u", ++mItemNumber);

    const char* v = top();
    //make sure its a list
    fprintf(mOutputFile,
        "    if (!%s->is<PyList>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a list: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyList* %s(%s->as<PyList>());\n"
        "    %s.clear();\n\n"
        "    PyList::const_iterator %s_cur = %s->begin();\n"
        "    for (size_t %s_index(0); %s_cur != %s->end(); ++%s_cur, ++%s_index) {\n"
        "        if ((*%s_cur)->is<PyInt>())  {\n"
        "            PyInt* t = (*%s_cur)->as<PyInt>();\n"
        "            %s.push_back(t->value());\n"
        "        } else {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Element %%u in list %s is not a long integer: %%s\", %s_index, (*%s_cur)->TypeString());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n",
        v,
            mName, name, v,

        iname, v,
        name,

        iname, iname,
        iname, iname, iname, iname, iname,
        iname,
            iname,
            name,

            mName, iname, iname, iname
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessListStr(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessListStr field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "list_%u", ++mItemNumber);

    const char* v = top();
    //make sure its a list
    fprintf(mOutputFile,
        "    if (!%s->is<PyList>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a list: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyList* %s(%s->as<PyList>());\n"
        "    %s.clear();\n\n"
        "    PyList::const_iterator %s_cur = %s->begin();\n"
        "    for (uint32 %s_index(0); %s_cur != %s->end(); ++%s_cur, ++%s_index) {\n"
        "        if (!(*%s_cur)->is<PyString>()) {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Element %%u in list %s is not a string: %%s\", %s_index, (*%s_cur)->TypeString());\n"
        "            return false;\n"
        "        }\n\n"
        "        const PyString* t = (*%s_cur)->as<PyString>();\n"
        "        %s.push_back(t->content());\n"
        "    }\n\n",
        v,
            mName, name, v,

        iname, v,
        name,

        iname, iname,
        iname, iname, iname, iname, iname,
            iname,
                mName, iname, iname, iname,

            iname,
            name
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessDict(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr)
    {
        std::cout << std::endl <<  "DecodeGen::ProcessDict field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    //this should be done better:
    bool optional = false;
    const char* optional_str = field->Attribute("optional");
    if (optional_str != nullptr)
        optional = str2<bool>(optional_str);

    fprintf(mOutputFile,
        "    PySafeDecRef(%s);\n",
        name
   );

    const char* v = top();
    if (optional)
        fprintf(mOutputFile,
            "    if (%s->is<PyNone>())\n"
            "        %s = nullptr;\n"
            "    else",
            v,
                name
       );

    fprintf(mOutputFile,
        "    if (%s->is<PyDict>()) {\n"
        "        %s = %s->as<PyDict>();\n"
        "        PyIncRef(%s);\n"
        "    } else {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a dict: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n",
        v,
        name, v,
            name,

            mName, name, v
   );

    pop();
    return true;
}

/*
 * this function could be improved quite a bit by not parsing and checking the
 * dictInlineEntry elements over and over again
 */
bool ClassDecodeGenerator::ProcessDictInline(const TiXmlElement* field)
{
    char iname[16];
    snprintf(iname, sizeof(iname), "dict%u", ++mItemNumber);

    bool soft = false;
    const char* soft_str = field->Attribute("soft");
    if (soft_str)
        soft = str2<bool>(soft_str);

    const char* v = top();
    //make sure its a dict
    fprintf(mOutputFile,
        "    if (!%s->is<PyDict>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is the wrong type: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyDict* %s(%s->as<PyDict>());\n\n",
        v,
            mName, iname, v,
        iname, v
   );

    //now generate the "found" flags for each expected element.
    const TiXmlNode* i(nullptr);

    bool empty = true;
    uint8 count = 0;
    while ((i = field->IterateChildren(i))) {
        if (i->Type() == TiXmlNode::TINYXML_ELEMENT) {
            const TiXmlElement* ele = i->ToElement();

            //we only handle dictInlineEntry elements
            if (strcmp(ele->Value(), "dictInlineEntry") != 0) {
                std::cout << std::endl <<  "DecodeGen::ProcessDictInline non-dictInlineEntry in <dictInline> at line " << ele->Row() << ", ignoring.";
                continue;
            }

            fprintf(mOutputFile,
                "    bool %s_%u = false;\n",
                iname, count
           );
            ++count;

            empty = false;
        }
    }

    fprintf(mOutputFile,
        "\n"
   );

    if (empty)
        fprintf(mOutputFile,
            "    // %s is empty from our perspective, not enforcing though.\n",
            iname
       );
    else {
        //setup the loop...
        fprintf(mOutputFile,
            "    PyDict::const_iterator %s_cur = %s->begin();\n"
            "    for (; %s_cur != %s->end(); ++%s_cur) {\n"
            "        if (!%s_cur->first->is<PyString>()) {\n"
            "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: a key in %s is the wrong type: %%s\", %s_cur->first->TypeString());\n"
            "            return false;\n"
            "        }\n\n"
            "        const PyString* key_string__ = %s_cur->first->as<PyString>();\n\n",
            iname, iname,
            iname, iname, iname,
                iname,
                    mName, iname, iname,

                iname
       );

        count = 0;
        while ((i = field->IterateChildren(i))) {
            if (i->Type() == TiXmlNode::TINYXML_ELEMENT) {
                const TiXmlElement* ele = i->ToElement();

                //we only handle dictInlineEntry elements
                if (strcmp(ele->Value(), "dictInlineEntry") != 0) {
                    std::cout << std::endl <<  "DecodeGen::ProcessDictInline non-dictInlineEntry in <dictInline> at line " << ele->Row() << ", ignoring.";
                    continue;
                }
                const char* key = ele->Attribute("key");
                if (key == nullptr) {
                    std::cout << std::endl <<  "DecodeGen::ProcessDictInline <dictInlineEntry> at line " << ele->Row() << " is missing the key attribute, skipping.";
                    return false;
                }

                //conditional prefix...
                fprintf(mOutputFile,
                    "        if (key_string__->content() == \"%s\") {\n"
                    "            %s_%u = true;\n",
                    key,
                        iname, count
               );
                ++count;

                //now process the data part, putting the value into `varname`
                //TODO: figure out a reasonable way to fix the indention here...
                char vname[64];
                snprintf(vname, sizeof(vname), "%s_cur->second", iname);
                push(vname);

                if (!ParseElementChildren(ele, 1))
                    return false;

                //fixed suffix...
                fprintf(mOutputFile,
                    "        } else\n"
               );
            }
        }

        if (soft)
            fprintf(mOutputFile,
                "        {\n"
                "            /* do nothing, soft dict */\n"
                "        }\n"
                "    }\n\n"
           );
        else
            fprintf(mOutputFile,
                "        {\n"
                "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Unknown key string '%%s' in %s\", key_string__->content().c_str());\n"
                "            return false;\n"
                "        }\n"
                "    }\n\n",
                mName, iname
           );

        //finally, check the "found" flags for each expected element.
        count = 0;
        while ((i = field->IterateChildren(i))) {
            if(i->Type() == TiXmlNode::TINYXML_ELEMENT)  {
                const TiXmlElement* ele = i->ToElement();

                //we only handle dictInlineEntry elements
                if (strcmp(ele->Value(), "dictInlineEntry") != 0)  {
                    std::cout << std::endl <<  "DecodeGen::ProcessDictInline non-dictInlineEntry in <dictInline> at line " << ele->Row() << ", ignoring.";
                    continue;
                }
                const char* key = ele->Attribute("key");
                if (key == nullptr) {
                    std::cout << std::endl <<  "DecodeGen::ProcessDictInline <dictInlineEntry> at line " << ele->Row() << " is missing the key attribute, skipping.";
                    return false;
                }

                fprintf(mOutputFile,
                    "    if (!%s_%u) {\n"
                    "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: Missing dict entry '%s' in %s\");\n"
                    "        return false;\n"
                    "    }\n\n",
                    iname, count,
                        mName, key, iname
               );
                ++count;
            }
        }
    }

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessDictRaw(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictRaw field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    const char* key = field->Attribute("key");
    if (key == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictRaw field at line " << field->Row() << " is missing the key attribute, skipping.";
        return false;
    }
    const char* value = field->Attribute("value");
    if (value == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictRaw field at line " << field->Row() << " is missing the value attribute, skipping.";
        return false;
    }
    const char* pykey = field->Attribute("pykey");
    if (pykey == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictRaw field at line " << field->Row() << " is missing the pykey attribute, skipping.";
        return false;
    }
    const char* pyvalue = field->Attribute("pyvalue");
    if (pyvalue == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictRaw field at line " << field->Row() << " is missing the pyvalue attribute, skipping.";
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "dict_%u", ++mItemNumber);

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyDict>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a dict: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyDict* %s(%s->as<PyDict>());\n"
        "    %s.clear();\n\n"
        "    PyDict::const_iterator %s_cur = %s->begin();\n"
        "    for (size_t %s_index(0); %s_cur != %s->end(); ++%s_cur, ++%s_index) {\n"
        "        if (!%s_cur->first->is<Py%s>()) {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Key %%u in dict %s is not %s: %%s\", %s_index, %s_cur->first->TypeString());\n"
        "            return false;\n"
        "        }\n\n"
        "        const Py%s* k = %s_cur->first->as<Py%s>();\n"
        "        if (!%s_cur->second->is<Py%s>()) {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Value %%d in dict %s is not %s: %%s\", %s_index, %s_cur->second->TypeString());\n"
        "            return false;\n"
        "        }\n\n"
        "        const Py%s *v = %s_cur->second->as<Py%s>();\n"
        "        %s[ k->value() ] = v->value();\n"
        "    }\n\n",
        v,
            mName, name, v,

        iname, v,
        name,

        iname, iname,
        iname, iname, iname, iname, iname,
            iname, pykey,
                mName, iname, pykey, iname, iname,

            pykey, iname, pykey,
            iname, pyvalue,
                mName, iname, pyvalue, iname, iname,

            pyvalue, iname, pyvalue,
            name
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessDictInt(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictInt field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "dict_%u", ++mItemNumber);

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyDict>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a dict: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyDict* %s(%s->as<PyDict>());\n"
        "    %s.clear();\n\n"
        "    PyDict::const_iterator %s_cur = %s->begin();\n"
        "    for (size_t %s_index(0); %s_cur != %s->end(); ++%s_cur, ++%s_index) {\n"
        "        if (!%s_cur->first->is<PyInt>()) {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Key %%u in dict %s is not an integer: %%s\", %s_index, %s_cur->first->TypeString());\n"
        "            return false;\n"
        "        }\n\n"
        "        const PyInt* k = %s_cur->first->as<PyInt>();\n"
        "        %s[ k->value() ] = %s_cur->second->clone();\n"
        "    }\n\n",
        v,
            mName, name, v,

        iname, v,
        name,

        iname, iname,
        iname, iname, iname, iname, iname,
            iname,
                mName, iname, iname, iname,
            iname,
            name, iname
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessDictStr(const TiXmlElement* field)
{
    const char* name = field->Attribute("name");
    if (name == nullptr) {
        std::cout << std::endl <<  "DecodeGen::ProcessDictStr field at line " << field->Row() << " is missing the name attribute, skipping.";
        return false;
    }

    char iname[16];
    snprintf(iname, sizeof(iname), "dict_%u", ++mItemNumber);

    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PyDict>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a dict: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PyDict* %s(%s->as<PyDict>());\n"
        "    %s.clear();\n\n"
        "    PyDict::const_iterator %s_cur = %s->begin();\n"
        "    for (size_t %s_index(0); %s_cur != %s->end(); ++%s_cur, ++%s_index) {\n"
        "        if (!%s_cur->first->is<PyString>()) {\n"
        "            _log(XMLP__DECODE_ERROR, \"Decode %s failed: Key %%u in dict %s is not a string: %%s\", %s_index, %s_cur->first->TypeString());\n"
        "            return false;\n"
        "        }\n\n"
        "        const PyString* k = %s_cur->first->as<PyString>();\n"
        "        %s[ k->content() ] = %s_cur->second->clone();\n"
        "    }\n\n",
        v,
            mName, name, v,

        iname, v,
        name,

        iname, iname,
        iname, iname, iname, iname, iname,
            iname,
                mName, iname, iname, iname,
            iname,
            name, iname
   );

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessSubStreamInline(const TiXmlElement* field)
{
    char iname[16];
    snprintf(iname, sizeof(iname), "ss_%u", ++mItemNumber);

    //make sure its a substream
    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PySubStream>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a substream: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PySubStream* %s(%s->as<PySubStream>());\n"
        "    //make sure its decoded\n"
        "    if (%s->decoded() == nullptr) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: Unable to decode %s\");\n"
        "        return false;\n"
        "    }\n\n",
        v,
            mName, iname, v,
        iname, v,
        iname,
            mName, iname
   );

    char ssname[32];
    snprintf(ssname, sizeof(ssname), "%s->decoded()", iname);
    push(ssname);

    //Decode the sub-element
    if (!ParseElementChildren(field, 1))
        return false;

    pop();
    return true;
}

bool ClassDecodeGenerator::ProcessSubStructInline(const TiXmlElement* field)
{
    char iname[16];
    snprintf(iname, sizeof(iname), "ss_%u", ++mItemNumber);

    //make sure its a substruct
    const char* v = top();
    fprintf(mOutputFile,
        "    if (!%s->is<PySubStruct>()) {\n"
        "        _log(XMLP__DECODE_ERROR, \"Decode %s failed: %s is not a substruct: %%s\", %s->TypeString());\n"
        "        return false;\n"
        "    }\n\n"
        "    PySubStruct* %s(%s->as<PySubStruct>());\n\n",
        v,
            mName, iname, v,
        iname, v
   );

    char ssname[32];
    snprintf(ssname, sizeof(ssname), "%s->sub()", iname);
    push(ssname);

    //Decode the sub-element
    if (!ParseElementChildren(field, 1))
        return false;

    pop();
    return true;
}
