#include <libdwarf++/tag.hh>

namespace Dwarf {

    Tag::Tag(unsigned int tag) : tag_(tag) {}

    const char* Tag::get_name() const {
        const char *name;
        dwarf::dwarf_get_TAG_name(tag_, &name);
        return name;
    }

    unsigned int Tag::get_id() const {
        return tag_;
    }

}
