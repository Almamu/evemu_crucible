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
bool TestTuple (PyTuple* rep, TrackedPythonArena& in, int length) {
    TrackedPythonArena out;

    Buffer marshaled;

    if (!MarshalDeflate (rep, marshaled)) {
        ::puts ("Failed to marshal tuple");
        return false;
    }

    if (marshaled.size() != length) {
        ::printf("Marshal length check failed, expected %d. got %zu\n", length, marshaled.size());
        return false;
    }

    PyDataType* unmarshaled = InflateUnmarshal(marshaled, &out);

    if (unmarshaled == nullptr) {
        ::puts ("Failed to unmarshal tuple");
        return false;
    }

    if (in.usedBytes() != out.usedBytes()) {
        ::puts ("Used memory doesn't match between marshaled and unmarshaled values");
        return false;
    }

    if (!rep->equals (unmarshaled)) {
        ::puts ("Marshaled and unmarshaled tuple do not match");
        return false;
    }

    ::printf ("Passed marshal test for Tuple of size %zu\n", rep->size());

    return true;
}

bool TestTupleEmpty() {
    TrackedPythonArena in;

    return TestTuple(in.Tuple(), in, 6);
}

bool TestTupleOne() {
    TrackedPythonArena in;

    return TestTuple(in.Tuple({in.None()}), in, 7);
}

bool TestTupleTwo() {
    TrackedPythonArena in;

    return TestTuple(in.Tuple({in.None(), in.None()}), in, 8);
}

bool TestTupleThree() {
    TrackedPythonArena in;

    return TestTuple(
        in.Tuple({in.None(), in.None(), in.None()}), in, 10
    );
}

bool TestTupleSizeEx() {
    TrackedPythonArena in;
    PyTuple::storage_type data;

    for (int i = 0; i < 255; i ++) {
        data.push_back (in.None());
    }

    return TestTuple(in.Tuple(data), in, 5 + 1 + 5 + 255);
}

bool TestTupleEqualsSameSize() {
    TrackedPythonArena in;
    PyTuple* first = in.Tuple({
        in.Int(100)
    });
    PyTuple* second = in.Tuple({
        in.Int(200)
    });

    return !first->equals(second);
}

bool TestTupleEqualsDifferentSize() {
    TrackedPythonArena in;
    PyTuple* first = in.Tuple({
        in.Int(100)
    });
    PyTuple* second = in.Tuple({
        in.Int(100),
        in.Int(100)
    });

    return !first->equals(second);
}

int marshal_TupleTest (int argc, char* argv[]) {
    if (TestTupleEmpty() == false)
        return EXIT_FAILURE;
    if (TestTupleOne() == false)
        return EXIT_FAILURE;
    if (TestTupleTwo() == false)
        return EXIT_FAILURE;
    if (TestTupleThree() == false)
        return EXIT_FAILURE;
    if (TestTupleSizeEx() == false)
        return EXIT_FAILURE;
    if (TestTupleEqualsSameSize() == false)
        return EXIT_FAILURE;
    if (TestTupleEqualsDifferentSize() == false)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}