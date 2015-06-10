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

    class Debug;

    struct string {
        string(std::weak_ptr<Debug> dbg);
        string(std::weak_ptr<Debug> dbg, char* str);
        ~string();

    private:
        std::weak_ptr<Debug> dbg_;
    public:
        char* str;
    };

    template <typename T> struct TypeKind {};
    template <> struct TypeKind<dwarf::Dwarf_Error>     { enum {Kind = DW_DLA_ERROR}; };
    template <> struct TypeKind<dwarf::Dwarf_Die >      { enum {Kind = DW_DLA_DIE}; };
    template <> struct TypeKind<dwarf::Dwarf_Attribute> { enum {Kind = DW_DLA_ATTR}; };
    template <> struct TypeKind<char *>                 { enum {Kind = DW_DLA_STRING}; };

    class CUIterator;
    class CompilationUnit;

    class Debug final : public std::enable_shared_from_this<Debug> {
    public:

        virtual ~Debug();

        template <typename T>
        void dealloc(T& val);

        CUIterator& begin() const;
        CUIterator& end() const;

        operator std::shared_ptr<Debug>();

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

        std::shared_ptr<Die> offdie(Dwarf::Off offset) const;

        static std::shared_ptr<Debug> open(const char *path);
        static std::shared_ptr<Debug> self();

    private:
        Debug(int fd, Unsigned access = DW_DLC_READ, Handler = nullptr, Ptr errarg = nullptr)
            throw(InitException, NoDebugInformationException);

        int fd_;
        dwarf::Dwarf_Debug handle_;
        std::vector<CompilationUnit> cus_;

        std::unique_ptr<CUIterator> begin_;
        std::unique_ptr<CUIterator> end_;
    };
};

# include "dwarf.hxx"

#endif /* !LIBDWARFPP_DWARF_HH */
