#include <libdwarf++/die.hh>

namespace Dwarf {

    Die::Die(std::weak_ptr<Debug> dbg, dwarf::Dwarf_Die& die)
        : dbg_(dbg)
        , die_(die)
        , sibling_()
        , child_()
    {}

    Die::Die() {}

    Die::~Die() {
        std::shared_ptr<Debug> dbg = dbg_.lock();
        if (dbg) {
            dbg->dealloc(die_);
        }
    }

    void Die::traverse(std::function<TraversalResult(Die &)> func) {
        switch (func(*this)) {
            case Die::TraversalResult::SKIP:  break;
            case Die::TraversalResult::BREAK: return;

            default:
            case Die::TraversalResult::TRAVERSE:
                child()->traverse(func);
        }
        sibling()->traverse(func);
    }

    std::shared_ptr<Die> Die::sibling() {
        init_sibling();
        return sibling_;
    }

    std::shared_ptr<Die> Die::child() {
        init_child();
        return child_;
    }

    void Die::init_sibling() {
        if (sibling_)
            return;

        std::shared_ptr<Debug> dbg = dbg_.lock();
        if (!dbg)
            throw new DebugClosedException();
        Error err;
        dwarf::Dwarf_Die sibling;
        switch (dwarf::dwarf_siblingof(dbg->get_handle(), die_, &sibling, &err)) {
            case DW_DLV_NO_ENTRY:
                sibling_ = std::make_shared<EmptyDie>();
                return;
            case DW_DLV_ERROR:
                throw Exception(dbg, err);
            default: break;
        }
        sibling_ = std::make_shared<Die>(dbg_, sibling);
    }

    void Die::init_child() {
        if (child_)
            return;

        std::shared_ptr<Debug> dbg = dbg_.lock();
        if (!dbg)
            throw new DebugClosedException();
        Error err;
        dwarf::Dwarf_Die child;
        switch (dwarf::dwarf_child(die_, &child, &err)) {
            case DW_DLV_NO_ENTRY:
                child_ = std::make_shared<EmptyDie>();
                return;
            case DW_DLV_ERROR:
                throw Exception(dbg, err);
            default: break;
        }
        child_ = std::make_shared<Die>(dbg_, child);
    }

    const Tag Die::get_tag() const throw(Exception) {
        Error err;
        Half tag;
        switch (dwarf::dwarf_tag(die_, &tag, &err)) {
            case DW_DLV_ERROR:
                throw Exception(dbg_, err);
            default: break;
        }
        return Tag(tag);
    }

}
