#include "libdwarf++/die.hh"
#include <unordered_map>

namespace Dwarf {

    DieData::DieData(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die& die)
        : dbg_(dbg)
        , die(die)
        , sibling()
        , child()
        , name(nullptr)
        , offset(0)
    {}

    DieData::~DieData() {
        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (dbg) {
            if (die) dbg->dealloc(die);
            if (name) dbg->dealloc(name);
        }
    }

    Die::Die(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die& die)
        : dbg_(dbg)
        , data_(new DieData(dbg, die))
    {}

    Die::Die() {}

    Die::~Die() {}

    void Die::traverse(std::function<TraversalResult(Die&, void*)> func, void* data) {
        switch (func(*this, data)) {
            case Die::TraversalResult::SKIP:  break;
            case Die::TraversalResult::BREAK: return;

            default:
            case Die::TraversalResult::TRAVERSE:
                child().traverse(func, data);
        }
        sibling().traverse(func, data);
    }

    void Die::traverse_headless(std::function<TraversalResult(Die&, void*)> func, void* data) {
        child().traverse(func, data);
    }

    Die& Die::sibling() {
        init_sibling();
        Die::visitor_to_die visitor;
        return data_->sibling->apply_visitor(visitor);
    }

    Die& Die::child() {
        init_child();
        Die::visitor_to_die visitor;
        return data_->child->apply_visitor(visitor);
    }

    void Die::init_sibling() {
        if (data_->sibling)
            return;

        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (!dbg)
            throw DebugClosedException();
        Error err;
        dwarf::Dwarf_Die sibling = nullptr;
        switch (dwarf::dwarf_siblingof(dbg->get_handle(), data_->die, &sibling, &err)) {
            case DW_DLV_NO_ENTRY:
                data_->sibling = std::make_shared<AnyDie>(EmptyDie());
                return;
            case DW_DLV_ERROR:
                throw Exception(dbg, err);
            default: break;
        }
        data_->sibling = Dwarf::make_die(get_tag_id(dbg_, sibling), dbg_, sibling);
    }

    void Die::init_child() {
        if (data_->child)
            return;

        std::shared_ptr<const Debug> dbg = dbg_.lock();
        if (!dbg)
            throw DebugClosedException();
        Error err;
        dwarf::Dwarf_Die child;
        switch (dwarf::dwarf_child(data_->die, &child, &err)) {
            case DW_DLV_NO_ENTRY:
                data_->child = std::make_shared<AnyDie>(EmptyDie());
                return;
            case DW_DLV_ERROR:
                throw Exception(dbg, err);
            default: break;
        }
        data_->child = Dwarf::make_die(get_tag_id(dbg_, child), dbg_, child);
    }

    const Tag Die::get_tag() const throw(Exception) {
        Error err;
        Half tag;
        switch (dwarf::dwarf_tag(data_->die, &tag, &err)) {
            case DW_DLV_ERROR:
                throw Exception(dbg_, err);
            default: break;
        }
        return Tag(tag);
    }

    const char* Die::get_name() const throw(Exception) {
        if (data_->name)
            return data_->name;

        char* name;
        Error err;
        switch (dwarf::dwarf_diename(data_->die, &name, &err)) {
            case DW_DLV_ERROR:
                throw Exception(dbg_, err);
            case DW_DLV_NO_ENTRY:
                return nullptr;
            default: break;
        }
        data_->name = name;
        return name;
    }

    Dwarf::Off Die::get_offset() const throw(Exception) {
        if (data_->offset > 0)
            return data_->offset;

        Error err;
        switch (dwarf::dwarf_dieoffset(data_->die, &data_->offset, &err)) {
            case DW_DLV_ERROR:
                throw Exception(dbg_, err);
            default: break;
        }
        return data_->offset;
    }

    std::unique_ptr<const Attribute> Die::get_attribute(Dwarf::Half attr) const {
        Dwarf::Error err;
        dwarf::Dwarf_Attribute result;
        switch (dwarf::dwarf_attr(data_->die, attr, &result, &err)) {
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

    using tag_to_die_map = typename std::unordered_map<unsigned int, std::shared_ptr<AnyDie>(*)(std::weak_ptr<const Debug>&, dwarf::Dwarf_Die)>;

    template <unsigned int... Tags>
    struct populate_map;

    template <unsigned int FirstTag, unsigned int... Tags>
    struct populate_map<FirstTag, Tags...> {
        static void run(tag_to_die_map & map) {
            populate_map<FirstTag>::run(map);
            populate_map<Tags...>::run(map);
        }
    };

    template <unsigned int Tag>
    struct populate_map<Tag> {
        static void run(tag_to_die_map & map) {
            map.insert(std::make_pair(Tag, Die::make_die<Tag>));
        }
    };

    template <unsigned int... Tags>
    static tag_to_die_map create_map() {
        tag_to_die_map map;
        populate_map<Tags...>::run(map);
        return map;
    }

    tag_to_die_map tag_to_die = create_map<
            DW_TAG_array_type,
            DW_TAG_class_type,
            DW_TAG_entry_point,
            DW_TAG_enumeration_type,
            DW_TAG_formal_parameter,
            DW_TAG_imported_declaration,
            DW_TAG_label,
            DW_TAG_lexical_block,
            DW_TAG_member,
            DW_TAG_pointer_type,
            DW_TAG_reference_type,
            DW_TAG_compile_unit,
            DW_TAG_string_type,
            DW_TAG_structure_type,
            DW_TAG_subroutine_type,
            DW_TAG_typedef,
            DW_TAG_union_type,
            DW_TAG_unspecified_parameters,
            DW_TAG_variant,
            DW_TAG_common_block,
            DW_TAG_common_inclusion,
            DW_TAG_inheritance,
            DW_TAG_inlined_subroutine,
            DW_TAG_module,
            DW_TAG_ptr_to_member_type,
            DW_TAG_set_type,
            DW_TAG_subrange_type,
            DW_TAG_with_stmt,
            DW_TAG_access_declaration,
            DW_TAG_base_type,
            DW_TAG_catch_block,
            DW_TAG_const_type,
            DW_TAG_constant,
            DW_TAG_enumerator,
            DW_TAG_file_type,
            DW_TAG_friend,
            DW_TAG_namelist,
            DW_TAG_namelist_item,
            DW_TAG_packed_type,
            DW_TAG_subprogram,
            DW_TAG_template_type_parameter,
            DW_TAG_template_value_parameter,
            DW_TAG_thrown_type,
            DW_TAG_try_block,
            DW_TAG_variant_part,
            DW_TAG_variable,
            DW_TAG_volatile_type,
            DW_TAG_dwarf_procedure,
            DW_TAG_restrict_type,
            DW_TAG_interface_type,
            DW_TAG_namespace,
            DW_TAG_imported_module,
            DW_TAG_unspecified_type,
            DW_TAG_partial_unit,
            DW_TAG_imported_unit,
            DW_TAG_mutable_type,
            DW_TAG_condition,
            DW_TAG_shared_type,
            DW_TAG_type_unit,
            DW_TAG_rvalue_reference_type,
            DW_TAG_template_alias,
            DW_TAG_coarray_type,
            DW_TAG_generic_subrange,
            DW_TAG_dynamic_type,
            DW_TAG_atomic_type,
            DW_TAG_call_site,
            DW_TAG_call_site_parameter
        >();

    std::shared_ptr<AnyDie> make_die(unsigned int tag, std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die &die) {
        auto it = tag_to_die.find(tag);
        if (it == tag_to_die.end())
            return std::make_shared<AnyDie>(Die(dbg, die));
        return (it->second)(dbg, die);
    }
}
