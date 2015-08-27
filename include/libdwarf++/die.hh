#ifndef LIBDWARFPP_DIE_HH
# define LIBDWARFPP_DIE_HH

# include <functional>
# include <memory>
# include "dwarf.hh"
# include "tag.hh"
# include "exprloc.hh"

# include "xvector.hh"
# include <boost/variant.hpp>

namespace Dwarf {

    template <unsigned int TagId>
    class TaggedDie;
    class Die;
    class EmptyDie;

    typedef boost::make_variant_over<
            typelist::xvector<
                TaggedDie<DW_TAG_array_type>,
                TaggedDie<DW_TAG_class_type>,
                TaggedDie<DW_TAG_entry_point>,
                TaggedDie<DW_TAG_enumeration_type>,
                TaggedDie<DW_TAG_formal_parameter>,
                TaggedDie<DW_TAG_imported_declaration>,
                TaggedDie<DW_TAG_label>,
                TaggedDie<DW_TAG_lexical_block>,
                TaggedDie<DW_TAG_member>,
                TaggedDie<DW_TAG_pointer_type>,
                TaggedDie<DW_TAG_reference_type>,
                TaggedDie<DW_TAG_compile_unit>,
                TaggedDie<DW_TAG_string_type>,
                TaggedDie<DW_TAG_structure_type>,
                TaggedDie<DW_TAG_subroutine_type>,
                TaggedDie<DW_TAG_typedef>,
                TaggedDie<DW_TAG_union_type>,
                TaggedDie<DW_TAG_unspecified_parameters>,
                TaggedDie<DW_TAG_variant>,
                TaggedDie<DW_TAG_common_block>,
                TaggedDie<DW_TAG_common_inclusion>,
                TaggedDie<DW_TAG_inheritance>,
                TaggedDie<DW_TAG_inlined_subroutine>,
                TaggedDie<DW_TAG_module>,
                TaggedDie<DW_TAG_ptr_to_member_type>,
                TaggedDie<DW_TAG_set_type>,
                TaggedDie<DW_TAG_subrange_type>,
                TaggedDie<DW_TAG_with_stmt>,
                TaggedDie<DW_TAG_access_declaration>,
                TaggedDie<DW_TAG_base_type>,
                TaggedDie<DW_TAG_catch_block>,
                TaggedDie<DW_TAG_const_type>,
                TaggedDie<DW_TAG_constant>,
                TaggedDie<DW_TAG_enumerator>,
                TaggedDie<DW_TAG_file_type>,
                TaggedDie<DW_TAG_friend>,
                TaggedDie<DW_TAG_namelist>,
                TaggedDie<DW_TAG_namelist_item>,
                TaggedDie<DW_TAG_packed_type>,
                TaggedDie<DW_TAG_subprogram>,
                TaggedDie<DW_TAG_template_type_parameter>,
                TaggedDie<DW_TAG_template_value_parameter>,
                TaggedDie<DW_TAG_thrown_type>,
                TaggedDie<DW_TAG_try_block>,
                TaggedDie<DW_TAG_variant_part>,
                TaggedDie<DW_TAG_variable>,
                TaggedDie<DW_TAG_volatile_type>,
                TaggedDie<DW_TAG_dwarf_procedure>,
                TaggedDie<DW_TAG_restrict_type>,
                TaggedDie<DW_TAG_interface_type>,
                TaggedDie<DW_TAG_namespace>,
                TaggedDie<DW_TAG_imported_module>,
                TaggedDie<DW_TAG_unspecified_type>,
                TaggedDie<DW_TAG_partial_unit>,
                TaggedDie<DW_TAG_imported_unit>,
                TaggedDie<DW_TAG_mutable_type>,
                TaggedDie<DW_TAG_condition>,
                TaggedDie<DW_TAG_shared_type>,
                TaggedDie<DW_TAG_type_unit>,
                TaggedDie<DW_TAG_rvalue_reference_type>,
                TaggedDie<DW_TAG_template_alias>,
                TaggedDie<DW_TAG_coarray_type>,
                TaggedDie<DW_TAG_generic_subrange>,
                TaggedDie<DW_TAG_dynamic_type>,
                TaggedDie<DW_TAG_atomic_type>,
                TaggedDie<DW_TAG_call_site>,
                TaggedDie<DW_TAG_call_site_parameter>,
                Die,
                EmptyDie
            >
        >::type AnyDie;

    class Attribute final {
    public:
        Attribute(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Attribute attr);
        ~Attribute();

        Dwarf::Half form() const;

        template<typename T>
        T as() const {
            Dwarf::Error err = nullptr;
            T result = 0;

            auto dbg = dbg_.lock();

            int callres = DW_DLV_OK;
            switch (form()) {
                case DW_FORM_data1:
                case DW_FORM_data2:
                case DW_FORM_data4:
                case DW_FORM_data8:
                case DW_FORM_udata:     callres = dwarf::dwarf_formudata(attr_,      reinterpret_cast<Dwarf::Unsigned*>(&result), &err); break;
                case DW_FORM_sdata:     callres = dwarf::dwarf_formsdata(attr_,      reinterpret_cast<Dwarf::Signed*>(&result),   &err); break;
                case DW_FORM_addrx:
                case DW_FORM_addr:      callres = dwarf::dwarf_formaddr(attr_,       reinterpret_cast<Dwarf::Addr*>(&result),     &err); break;
                case DW_FORM_ref1:
                case DW_FORM_ref2:
                case DW_FORM_ref4:
                case DW_FORM_ref8:
                case DW_FORM_ref_sig8:
                case DW_FORM_ref_udata:
                case DW_FORM_ref_addr:  callres = dwarf::dwarf_global_formref(attr_, reinterpret_cast<Dwarf::Off*>(&result),      &err); break;
                case DW_FORM_string:    callres = dwarf::dwarf_formstring(attr_,     reinterpret_cast<char **>(&result),          &err); break;
                case DW_FORM_flag:      callres = dwarf::dwarf_formflag(attr_,       reinterpret_cast<Dwarf::Bool*>(&result),     &err); break;
                case DW_FORM_block1:
                case DW_FORM_block2:
                case DW_FORM_block4:
                case DW_FORM_block:     callres = dwarf::dwarf_formblock(attr_,      reinterpret_cast<Dwarf::Block**>(&result),   &err); break;
                case DW_FORM_exprloc:   callres = exprloc_eval(*dbg, attr_,          reinterpret_cast<uint64_t*>(&result),        &err); break;
                default: break;
            }
            if (callres == DW_DLV_ERROR) {
                throw Exception(dbg_, err);
            }
            return result;
        }

        inline std::shared_ptr<Die> as_die() const {
            Dwarf::Error err = nullptr;
            Dwarf::Off result;

            auto dbg = dbg_.lock();
            int callres;
            switch (form()) {
                case DW_FORM_ref1:
                case DW_FORM_ref2:
                case DW_FORM_ref4:
                case DW_FORM_ref8:
                case DW_FORM_ref_sig8:
                case DW_FORM_ref_udata:
                case DW_FORM_ref_addr:  callres = dwarf::dwarf_global_formref(attr_, &result, &err); break;
                default:
                    throw std::runtime_error("Unexpected non-reference attribute");
            }

            if (callres == DW_DLV_ERROR)
                throw Exception(dbg_, err);
            return dbg->offdie(result);
        }
    private:
        std::weak_ptr<const Debug> dbg_;
        dwarf::Dwarf_Attribute attr_;
    };

    std::shared_ptr<AnyDie> make_die(unsigned int tag, std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die &die);

    class Die {
    public:
        enum TraversalResult {
            TRAVERSE = 0,
            SKIP,
            BREAK,
        };

        using TraversalFunction = std::function<TraversalResult(Die&, void*)>;

        Die(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die& die);
        Die(Die const& other) = delete;
        Die& operator=(Die const& other) = delete;
        ~Die();

        virtual void traverse(TraversalFunction func, void* data);
        virtual void traverse_headless(TraversalFunction func, void* data);
        virtual Die& sibling();
        virtual Die& child();

        struct visitor_to_die : public boost::static_visitor<Die&> {
            template <typename T>
            Die& operator()(T& ptr) {
                return ptr;
            }
        };

        template <typename T>
        static void visit_die(T& visitor, AnyDie& die);

        template <typename T>
        void visit_headless(T& visitor);

        const Tag get_tag() const throw(Exception);
        const char* get_name() const throw(Exception);

        const dwarf::Dwarf_Die& get_handle() const {
            return die_;
        }

        Dwarf::Off get_offset() const throw(Exception);

        std::unique_ptr<const Attribute> get_attribute(Dwarf::Half attr) const;

        std::shared_ptr<const Debug> get_debug() const {
            return dbg_.lock();
        }

        template <unsigned int Tag>
        static std::shared_ptr<AnyDie> make_die(std::weak_ptr<const Debug>& dbg, dwarf::Dwarf_Die die);

        static unsigned int get_tag_id(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die raw_die) {
            Error err;
            Half tag;
            switch (dwarf::dwarf_tag(raw_die, &tag, &err)) {
                case DW_DLV_ERROR:
                    throw Exception(dbg, err);
                default: break;
            }
            return tag;
        }

        Die(Die && other) noexcept = default;
        Die& operator=(Die && other) noexcept = default;

    protected:
        Die();

        void init_sibling();
        void init_child();

        std::weak_ptr<const Debug> dbg_;
        dwarf::Dwarf_Die die_;
        std::shared_ptr<AnyDie> sibling_, child_;
        std::unique_ptr<Dwarf::string> name_;
        std::unique_ptr<Dwarf::Off> offset_;
    };

    class EmptyDie : public Die {
    public:
        EmptyDie() : Die() {};
        inline virtual void traverse([[gnu::unused]] TraversalFunction func, [[gnu::unused]] void* data) override {}
        inline virtual void traverse_headless([[gnu::unused]] TraversalFunction func, [[gnu::unused]] void* data) override {}
        inline virtual Die& sibling() { Die* d = nullptr; return *d; }
        inline virtual Die& child()   { Die* d = nullptr; return *d; }
    };

    template <unsigned int TagId>
    class TaggedDie : public Die {
    public:
        TaggedDie(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die &die)
                : Die(dbg, die)
        {}
    };

    class DefaultDieVisitor : public boost::static_visitor<Die::TraversalResult> {
        template <typename T>
        Die::TraversalResult operator()(T& die) {
            return Die::TraversalResult::TRAVERSE;
        }
    };

    template <typename T>
    void Die::visit_die(T& visitor, AnyDie& die) {
        if (die.type() == typeid(EmptyDie))
            return;

        visitor_to_die vtd;

        Die& handle = die.apply_visitor(vtd);

        switch (die.apply_visitor(visitor)) {
            case Die::TraversalResult::SKIP:  break;
            case Die::TraversalResult::BREAK: return;

            default:
            case Die::TraversalResult::TRAVERSE:
                handle.init_child();
                visit_die(visitor, *handle.child_);
        }
        handle.init_sibling();
        visit_die(visitor, *handle.sibling_);
    }

    template <typename T>
    void Die::visit_headless(T& visitor) {
        if (typeid(*this) == typeid(EmptyDie))
            return;

        init_child();
        visit_die(visitor, *child_);
    }

    template <unsigned int Tag>
    std::shared_ptr<AnyDie> Die::make_die(std::weak_ptr<const Debug>& dbg, dwarf::Dwarf_Die die) {
        return std::make_shared<AnyDie>(TaggedDie<Tag>(dbg, die));
    }
}

#endif /* !LIBDWARFPP_DIE_HH */
