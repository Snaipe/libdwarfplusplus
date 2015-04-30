#ifndef LIBDWARFPP_TAG_HH
# define LIBDWARFPP_TAG_HH

# include "dwarf.hh"

namespace Dwarf {

    class Tag {
    public:
        Tag(unsigned int tag);
        const char* get_name() const;
    private:
        unsigned int tag_;
    };
}

#endif /* !LIBDWARFPP_TAG_HH */
