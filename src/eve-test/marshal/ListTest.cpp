/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2025 The EVEmu Team
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
    Author:     Almamu
*/

#include "eve-test.h"

/**
 * @param value The value to use for the test
 * @param length The expected marshal length as a result of the marshal process
 *
 * @return
 */
bool TestList (PyList* rep, TrackedPythonArena& in, int length) {
    TrackedPythonArena out;

    Buffer marshaled;

    if (!MarshalDeflate (rep, marshaled)) {
        ::puts ("Failed to marshal list");
        return false;
    }

    if (marshaled.size() != length) {
        ::printf("Marshal length check failed, expected %d. got %zu\n", length, marshaled.size());
        return false;
    }

    PyDataType* unmarshaled = InflateUnmarshal(marshaled, &out);

    if (unmarshaled == nullptr) {
        ::puts ("Failed to unmarshal list");
        return false;
    }

    if (in.usedBytes() != out.usedBytes()) {
        ::puts ("Used memory doesn't match between marshaled and unmarshaled values");
        return false;
    }

    if (!rep->equals (unmarshaled)) {
        ::puts ("Marshaled and unmarshaled list do not match");
        return false;
    }

    ::printf ("Passed marshal test for List of size %zu\n", rep->size());

    return true;
}

bool TestListEmpty() {
    TrackedPythonArena in;

    return TestList(in.List(), in, 6);
}

bool TestListOne() {
    TrackedPythonArena in;

    return TestList(in.List({in.None()}), in, 7);
}

bool TestListTwo() {
    TrackedPythonArena in;

    return TestList(in.List({in.None(), in.None()}), in, 9);
}

bool TestListSizeEx() {
    TrackedPythonArena in;
    PyTuple::storage_type data;

    for (int i = 0; i < 255; i ++) {
        data.push_back (in.None());
    }

    return TestList(in.List(data), in, 5 + 1 + 5 + 255);
}

bool TestListEqualsSameSize() {
    TrackedPythonArena in;
    PyList* first = in.List({
        in.Int(100)
    });
    PyList* second = in.List({
        in.Int(200)
    });

    return !first->equals(second);
}

bool TestListEqualsDifferentSize() {
    TrackedPythonArena in;
    PyList* first = in.List({
        in.Int(100)
    });
    PyList* second = in.List({
        in.Int(100),
        in.Int(100)
    });

    return !first->equals(second);
}

int marshal_ListTest (int argc, char* argv[]) {
    if (TestListEmpty() == false)
        return EXIT_FAILURE;
    if (TestListOne() == false)
        return EXIT_FAILURE;
    if (TestListTwo() == false)
        return EXIT_FAILURE;
    if (TestListSizeEx() == false)
        return EXIT_FAILURE;
    if (TestListEqualsSameSize() == false)
        return EXIT_FAILURE;
    if (TestListEqualsDifferentSize() == false)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}