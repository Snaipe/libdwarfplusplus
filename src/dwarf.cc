/*
 *  This file is part of libdwarf++.
 *
 *  Copyright © 2015 Frankin "Snaipe" Mathieu <http://snaipe.me>
 *
 *  libdwarf++ is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libdwarf++ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libdwarf++.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <libdwarf++/dwarf.hh>
#include <libdwarf++/cu.hh>

extern "C" {
#include <fcntl.h>
};

namespace Dwarf {

    Debug::Debug(int fd, Dwarf::Unsigned access, Dwarf::Handler handler, Dwarf::Ptr errarg)
            throw (InitException, NoDebugInformationException)
        : fd_(fd)
    {

        dwarf::Dwarf_Error err;
        switch (dwarf::dwarf_init(fd, access, handler, errarg, &handle_, &err)) {
            default:
            case DW_DLV_ERROR:
                throw new InitException(err);
            case DW_DLV_NO_ENTRY:
                throw new NoDebugInformationException();
            case DW_DLV_OK:
                break;
        }

        begin_ = CUIterator::next(this);
        end_   = CUIterator::end(this);
    }

    Debug::~Debug() {
        try {
            close();
        } catch (Exception& ex) { }
    }

    void Debug::close() throw (Exception) {
        Error err;
        if (dwarf::dwarf_finish(handle_, &err) != DW_DLV_OK)
            throw new Exception(this, err);
    }

    CUIterator& Debug::begin() const {
        return *begin_;
    }

    CUIterator& Debug::end() const {
        return *end_;
    }

    int Debug::open_self() {
        return open("/proc/self/exe", O_RDONLY);
    }
};
