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

# include <vector>
# include <memory>
# include "cdwarf"
# include "exception.hh"

namespace Dwarf {

    template <typename T> struct TypeKind {};
    template <> struct TypeKind<Error> { enum {Kind = DW_DLA_ERROR}; };

    class CUIterator;
    class CompilationUnit;

    class Debug final {
    public:
        Debug(int fd = open_self(), Unsigned access = DW_DLC_READ, Handler = nullptr, Ptr errarg = nullptr)
                throw(InitException, NoDebugInformationException);

        ~Debug();

        template <typename T>
        void dealloc(T& val);

        CUIterator& begin() const;
        CUIterator& end() const;

        bool operator==(const Debug &other) const {
            return fd_ == other.fd_;
        }

        bool operator!=(const Debug &other) const {
            return !(*this == other);
        }

        void close() throw (Exception);

        const dwarf::Dwarf_Debug& get_handle() const {
            return handle_;
        }

    private:
        static int open_self();

        int fd_;
        dwarf::Dwarf_Debug handle_;
        std::vector<CompilationUnit> cus_;

        std::unique_ptr<CUIterator> begin_;
        std::unique_ptr<CUIterator> end_;
    };
};

# include "dwarf.hxx"

#endif /* !LIBDWARFPP_DWARF_HH */
