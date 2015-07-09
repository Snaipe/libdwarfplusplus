#ifndef LIBDWARFPP_EXPRLOC_HH
# define LIBDWARFPP_EXPRLOC_HH

# include "dwarf.hh"

namespace Dwarf {
    int exprloc_eval(const Dwarf::Debug& dbg, dwarf::Dwarf_Attribute attr, uint64_t* result, Dwarf::Error* err);
}

#endif /* !LIBDWARFPP_EXPRLOC_HH */
