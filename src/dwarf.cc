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
#include "libdwarf++/dwarf.hh"
#include "libdwarf++/cu.hh"

namespace posix {
extern "C" {
#include <fcntl.h>
};
}

namespace Dwarf {

    // string

    string::string(std::weak_ptr<const Debug> dbg) : dbg_(dbg), str(nullptr) {};
    string::string(std::weak_ptr<const Debug> dbg, char* str) : dbg_(dbg), str(str) {};

    string::~string() {
        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (dbg && str)
            dbg->dealloc(str);
    }

    // debug

    Debug::Debug(int fd, Dwarf::Unsigned access, Dwarf::Handler handler, Dwarf::Ptr errarg)
            throw (InitException, NoDebugInformationException)
        : fd_(fd)
    {

        dwarf::Dwarf_Error err;
        switch (dwarf::dwarf_init(fd, access, handler, errarg, &handle_, &err)) {
            default:
            case DW_DLV_ERROR:
                throw InitException(err);
            case DW_DLV_NO_ENTRY:
                throw NoDebugInformationException();
            case DW_DLV_OK:
                break;
        }

    }

    Debug::~Debug() {
        Error err;
        dwarf::dwarf_finish(handle_, &err);
    }

    void Debug::close() const throw (Exception) {
        Error err;
        if (dwarf::dwarf_finish(handle_, &err) != DW_DLV_OK)
            throw Exception(shared_from_this(), err);
    }

    CUIterator& Debug::begin() const {
        return *begin_;
    }

    CUIterator& Debug::end() const {
        return *end_;
    }

    CUIterator& Debug::cbegin() const {
        return *begin_;
    }

    CUIterator& Debug::cend() const {
        return *end_;
    }

    Debug::operator std::shared_ptr<const Debug>() const {
        return shared_from_this();
    }

    std::shared_ptr<const Debug> Debug::open(const char *path) {
        int fd = posix::open(path, O_RDONLY);
        if (fd == -1)
            return nullptr;
        std::shared_ptr<Debug> ref(new Debug(fd));
        ref->begin_ = CUIterator::next(ref);
        ref->end_   = CUIterator::end(ref);
        return ref;
    }

    std::shared_ptr<const Debug> Debug::self() {
        return open("/proc/self/exe");
    }

    std::shared_ptr<Die> Debug::offdie(Dwarf::Off offset) const {
        dwarf::Dwarf_Die die;
        Dwarf::Error err;
        switch (dwarf::dwarf_offdie(handle_, offset, &die, &err)) {
            case DW_DLV_NO_ENTRY: return nullptr;
            case DW_DLV_ERROR: throw Exception(shared_from_this(), err);
            default: break;
        }
        return std::make_shared<Die>(shared_from_this(), die);
    }
};
