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
#ifndef LIBDWARFPP_CU_H
# define LIBDWARFPP_CU_H

# include <iterator>
# include "dwarf.hh"
# include "die.hh"

namespace Dwarf {

    class CompilationUnit {
    public:
        CompilationUnit(std::weak_ptr<const Debug> dbg,
                        std::shared_ptr<AnyDie> die,
                        Unsigned header_len = 0,
                        Half version_stamp = 0,
                        Unsigned abbrev_offset = 0,
                        Half address_size = 0,
                        Unsigned header = 0);

        bool operator==(const CompilationUnit &other) const;
        bool operator!=(const CompilationUnit &other) const;
        operator bool() const;
        Die& get_die() const;

        template <typename T>
        void visit(T& visitor) const {
            Die::visit_die(visitor, *die_);
        }

        template <typename T>
        void visit_headless(T& visitor) const {
            get_die().visit_headless(visitor);
        }

    private:
        std::weak_ptr<const Debug> dbg_;
        std::shared_ptr<Dwarf::AnyDie> die_;
        Unsigned header_len_;
        Half version_stamp_;
        Unsigned abbrev_offset_;
        Half address_size_;
        Unsigned header_;
    };

    class Debug;

    class CUIterator : public std::iterator<std::forward_iterator_tag, const CompilationUnit> {
    public:
        CUIterator(std::shared_ptr<const Debug>& dbg, std::shared_ptr<CompilationUnit>& value) throw (Exception);
        CUIterator(const CUIterator& other);

        const CompilationUnit& operator*() const;
        bool operator==(const CUIterator &other) const;
        bool operator!=(const CUIterator &other) const;
        CUIterator& operator=(const CUIterator& cu);
        CUIterator& operator++();

        static std::unique_ptr<CUIterator> next(std::shared_ptr<const Debug> dbg);
        static std::unique_ptr<CUIterator> end(std::shared_ptr<const Debug> dbg);

    private:
        static std::shared_ptr<CompilationUnit> next_cu(std::shared_ptr<const Debug> dbg);

        std::weak_ptr<const Debug> dbg_;
        bool end_;
        std::shared_ptr<std::unique_ptr<CUIterator>> next_;
        std::shared_ptr<CompilationUnit> value_;
    };
};

#endif /* !LIBDWARFPP_CU_H */
