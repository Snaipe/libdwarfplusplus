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
#include <libdwarf++/exception.hh>
#include <libdwarf++/dwarf.hh>
#include <cstdlib>

namespace Dwarf {
    const char *Exception::what() const throw() {
        return dwarf::dwarf_errmsg(err_);
    }

    const Unsigned Dwarf::Exception::errno() const throw() {
        return dwarf::dwarf_errno(err_);
    }

    Exception::~Exception() throw() {
        dbg_->dealloc(err_);
    }

    InitException::~InitException() throw() {
        std::free(err_);
    }
};
