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
bool TestFloat (double value, int length) {
    TrackedPythonArena in;
    TrackedPythonArena out;

    PyFloat* rep = in.Float(value);
    Buffer marshaled;

    if (!MarshalDeflate (rep, marshaled)) {
        ::puts ("Failed to marshal float");
        return false;
    }

    if (marshaled.size() != length) {
        ::printf("Marshal length check failed, expected %d. got %zu\n", length, marshaled.size());
        return false;
    }

    PyDataType* unmarshaled = InflateUnmarshal(marshaled, &out);

    if (unmarshaled == nullptr) {
        ::puts ("Failed to unmarshal float");
        return false;
    }

    if (in.usedBytes() != out.usedBytes()) {
        ::puts ("Used memory doesn't match between marshaled and unmarshaled values");
        return false;
    }


    if (!rep->equals (unmarshaled)) {
        ::puts ("Marshaled and unmarshaled float do not match");
        return false;
    }

    ::printf ("Passed marshal test for float with value %f\n", rep->value());

    return true;
}

int marshal_FloatTest (int argc, char* argv[]) {
    /** List of different values to test integers against */
    double values[] = {
        0.0, 150.0
    };
    /** Lenghts of the marshal output for some sort of validation */
    int64 lengths[] = {
        6, 14
    };

    for (int i = 0; i < 2; i ++) {
        if (TestFloat (values [i], lengths [i]) == false)
          return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}