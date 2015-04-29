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
#include <libdwarf++/cu.hh>

namespace Dwarf {

    // CompilationUnit

    CompilationUnit::CompilationUnit(std::weak_ptr<Debug> dbg,
            std::shared_ptr<Dwarf::Die> die,
            Unsigned header_len,
            Half version_stamp,
            Unsigned abbrev_offset,
            Half address_size,
            Unsigned header)
        : dbg_(dbg)
        , die_(die)
        , header_len_(header_len)
        , version_stamp_(version_stamp)
        , abbrev_offset_(abbrev_offset)
        , address_size_(address_size)
        , header_(header)
    {}

    bool CompilationUnit::operator==(const CompilationUnit &other) const {
        std::shared_ptr<Debug> dbg = dbg_.lock();
        std::shared_ptr<Debug> odbg = other.dbg_.lock();
        if (!dbg || !odbg)
            throw new DebugClosedException();
        return *dbg == *odbg && header_ == other.header_;
    }

    bool CompilationUnit::operator!=(const CompilationUnit &other) const {
        return !(*this == other);
    }

    CompilationUnit::operator bool() const {
        return header_ != 0;
    }

    std::shared_ptr<Die> CompilationUnit::get_die() const {
        return die_;
    }

    // CUIterator

    CUIterator::CUIterator(std::shared_ptr<Debug>& dbg, std::shared_ptr<CompilationUnit>& value) throw (Exception)
            : dbg_(dbg)
            , next_(std::make_shared<std::unique_ptr<CUIterator>>(nullptr))
            , value_(value)
    {
        end_ = !value;
    }

    CUIterator::CUIterator(const CUIterator& other)
            : dbg_(other.dbg_)
            , end_(other.end_)
            , next_(other.next_)
            , value_(other.value_)
    {}

    const CompilationUnit& CUIterator::operator*() const {
        return *value_;
    }

    bool CUIterator::operator==(const CUIterator &other) const {
        return (end_ && other.end_) || value_ == other.value_;
    }

    bool CUIterator::operator!=(const CUIterator &other) const {
        return !(*this == other);
    }

    CUIterator& CUIterator::operator=(const CUIterator& cu) {
        next_  = std::move(cu.next_);
        value_ = std::move(cu.value_);
        end_   = cu.end_;
        dbg_   = cu.dbg_;
        return *this;
    }

    CUIterator& CUIterator::operator++() {
        if (!end_) {
            if (!*next_) {
                std::shared_ptr<Debug> dbg = dbg_.lock();
                if (!dbg)
                    throw new DebugClosedException();
                *next_ = next(dbg);
            }
            *this = **next_;
        }
        return *this;
    }

    std::unique_ptr<CUIterator> CUIterator::next(std::shared_ptr<Debug> dbg) {
        std::shared_ptr<CompilationUnit> cu = next_cu(dbg);
        return std::make_unique<CUIterator>(dbg, cu);
    }

    std::unique_ptr<CUIterator> CUIterator::end(std::shared_ptr<Debug> dbg) {
        std::shared_ptr<CompilationUnit> cu = nullptr;
        return std::make_unique<CUIterator>(dbg, cu);
    }

    std::shared_ptr<CompilationUnit> CUIterator::next_cu(std::shared_ptr<Debug> dbg) {
        Error err;
        Unsigned header_len, abbrev_offset, header;
        Half version_stamp, address_size;

        switch (dwarf::dwarf_next_cu_header(dbg->get_handle(),
                &header_len,
                &version_stamp,
                &abbrev_offset,
                &address_size,
                &header,
                &err))
        {
            case DW_DLV_NO_ENTRY:
                return nullptr;
            case DW_DLV_ERROR:
                throw new Exception(dbg, err);
            default: break;
        }

        dwarf::Dwarf_Die die;
        switch (dwarf::dwarf_siblingof(dbg->get_handle(), NULL, &die, &err)) {
            case DW_DLV_NO_ENTRY:
                return nullptr;
            case DW_DLV_ERROR:
                throw new Exception(dbg, err);
            default: break;
        }
        std::weak_ptr<Debug> wk_dbg = dbg;
        std::shared_ptr<Die> d = std::make_shared<Die>(wk_dbg, die);

        return std::make_shared<CompilationUnit>(dbg, d, header_len, version_stamp, abbrev_offset, address_size, header);
    }
}