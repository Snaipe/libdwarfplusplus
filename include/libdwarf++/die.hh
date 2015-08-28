#ifndef LIBDWARFPP_DIE_HH
# define LIBDWARFPP_DIE_HH

# include <functional>
# include <memory>
# include "dwarf.hh"
# include "tag.hh"
# include "exprloc.hh"

# include "anydie.hh"

namespace Dwarf {

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

        inline std::shared_ptr<AnyDie> as_die() const {
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

    struct DieData {

        DieData(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die& die);
        ~DieData();

        std::weak_ptr<const Debug> dbg_;
        dwarf::Dwarf_Die die;
        std::shared_ptr<AnyDie> sibling, child;
        char *name;
        Dwarf::Off offset;
    };

    class Die {
    public:
        enum TraversalResult {
            TRAVERSE = 0,
            SKIP,
            BREAK,
        };

        using TraversalFunction = std::function<TraversalResult(Die&, void*)>;

        Die(std::weak_ptr<const Debug> dbg, dwarf::Dwarf_Die& die);
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
            return data_->die;
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

        Die(const Die& other) = default;
        Die& operator=(const Die& other) = default;

        Die(Die&& other) = default;
        Die& operator=(Die&& other) = default;

    protected:
        Die();

        void init_sibling();
        void init_child();

        std::weak_ptr<const Debug> dbg_;
        std::shared_ptr<DieData> data_;
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
                visit_die(visitor, *handle.data_->child);
        }
        handle.init_sibling();
        visit_die(visitor, *handle.data_->sibling);
    }

    template <typename T>
    void Die::visit_headless(T& visitor) {
        if (typeid(*this) == typeid(EmptyDie))
            return;

        init_child();
        visit_die(visitor, *data_->child);
    }

    template <unsigned int Tag>
    std::shared_ptr<AnyDie> Die::make_die(std::weak_ptr<const Debug>& dbg, dwarf::Dwarf_Die die) {
        return std::make_shared<AnyDie>(TaggedDie<Tag>(dbg, die));
    }
}

#endif /* !LIBDWARFPP_DIE_HH */
