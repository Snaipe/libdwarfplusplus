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
#ifndef LIBDWARFPP_DWARF_HH
# define LIBDWARFPP_DWARF_HH

# include "cdwarf"
# include "exception.hh"

namespace Dwarf {

    template <typename T> struct TypeKind {};
    template <> struct TypeKind<Error> { enum {Kind = DW_DLA_ERROR}; };

    class Debug final {
    public:
        Debug(int fd, Unsigned access = DW_DLC_READ, Handler = nullptr, Ptr errarg = nullptr)
                throw(InitException, NoDebugInformationException);

        ~Debug();

        template <typename T>
        void dealloc(T& val);

        void close() throw (Exception);

    private:
        dwarf::Dwarf_Debug handle_;
    };
};

# include "dwarf.hxx"

#endif /* !LIBDWARFPP_DWARF_HH */
