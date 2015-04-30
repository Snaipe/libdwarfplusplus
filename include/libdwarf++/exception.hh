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
#ifndef LIBDWARFPP_EXCEPTION_H
# define LIBDWARFPP_EXCEPTION_H

# include <exception>
# include <memory>
# include "cdwarf"

namespace Dwarf {

    using Error = dwarf::Dwarf_Error;

    class Debug;

    class Exception : public std::exception {
    public:
        Exception(std::weak_ptr<Debug> dbg, Error& err) : err_(err), dbg_(dbg) {};
        virtual ~Exception() throw();

        virtual const char *what() const throw() override;
        virtual const Unsigned get_errno() const throw();

    protected:
        Exception(Error& err) : err_(err) {};
        Exception() {};

        Error err_;
        std::weak_ptr<Debug> dbg_;
    };

    class InitException : public Exception {
    public:
        InitException(Error& err) : Exception(err) {};
        ~InitException();
    };

    class NoDebugInformationException : public Exception {
    public:
        NoDebugInformationException() : Exception() { }

        virtual const char *what() const throw() override {
            return "The object does not contain DWARF debugging information";
        }

        virtual const Unsigned get_errno() const throw() override {
            return 0;
        }
    };

    class DebugClosedException : public Exception {
    public:
        DebugClosedException() : Exception() {}
        ~DebugClosedException();

        virtual const char *what() const throw() override;
        virtual const Unsigned get_errno() const throw() override;
    };

}

#endif /* !LIBDWARFPP_EXCEPTION_H */
