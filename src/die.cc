#include <libdwarf++/die.hh>

namespace Dwarf {

    Die::Die(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die& die)
        : dbg_(dbg)
        , die_(die)
        , sibling_()
        , child_()
        , name_(new Dwarf::string(dbg))
        , offset_(new unsigned long long int())
    {}

    Die::Die() {}

    Die::~Die() {
        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (dbg) {
            dbg->dealloc(die_);
        }
    }

    void Die::traverse(std::function<TraversalResult(Die&, void*)> func, void* data) {
        switch (func(*this, data)) {
            case Die::TraversalResult::SKIP:  break;
            case Die::TraversalResult::BREAK: return;

            default:
            case Die::TraversalResult::TRAVERSE:
                child()->traverse(func, data);
        }
        sibling()->traverse(func, data);
    }

    void Die::traverse_headless(std::function<TraversalResult(Die&, void*)> func, void* data) {
        child()->traverse(func, data);
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

        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (!dbg)
            throw DebugClosedException();
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

        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (!dbg)
            throw DebugClosedException();
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

    const char* Die::get_name() const throw(Exception) {
        if (name_->str)
            return name_->str;

        char* name;
        Error err;
        switch (dwarf::dwarf_diename(die_, &name, &err)) {
            case DW_DLV_ERROR:
                throw Exception(dbg_, err);
            case DW_DLV_NO_ENTRY:
                return nullptr;
            default: break;
        }
        name_->str = name;
        return name_->str;
    }

    Dwarf::Off Die::get_offset() const throw(Exception) {
        if (*offset_ > 0)
            return *offset_;

        Error err;
        switch (dwarf::dwarf_dieoffset(die_, &*offset_, &err)) {
            case DW_DLV_ERROR:
                throw Exception(dbg_, err);
            default: break;
        }
        return *offset_;
    }

    std::unique_ptr<const Attribute> Die::get_attribute(Dwarf::Half attr) const {
        Dwarf::Error err;
        dwarf::Dwarf_Attribute result;
        switch (dwarf::dwarf_attr(die_, attr, &result, &err)) {
            case DW_DLV_NO_ENTRY: return nullptr;
            case DW_DLV_ERROR: throw Exception(dbg_, err);
            default: break;
        }
        return std::make_unique<Attribute>(dbg_, result);
    }

    Attribute::Attribute(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Attribute attr)
        : dbg_(dbg)
        , attr_(attr)
    {}

    Attribute::~Attribute() {
        if (std::shared_ptr<const Debug> dbg = dbg_.lock()) {
            dbg->dealloc(attr_);
        }
    }

    Dwarf::Half Attribute::form() const {
        Dwarf::Half res;
        Dwarf::Error err;
        switch (dwarf::dwarf_whatform(attr_, &res, &err)) {
            case DW_DLV_ERROR: throw Exception(dbg_, err);
            default: return res;
        }
    }
}
