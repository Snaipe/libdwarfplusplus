#ifndef LIBDWARFPP_DIE_HH
# define LIBDWARFPP_DIE_HH

# include <functional>
# include <memory>
# include "dwarf.hh"
# include "tag.hh"
# include "exprloc.hh"

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
            Dwarf::Half attrform = form();
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
                case DW_FORM_ref_udata: callres = dwarf::dwarf_formref(attr_,        reinterpret_cast<Dwarf::Off*>(&result),      &err); break;
                case DW_FORM_ref_addr:  callres = dwarf::dwarf_global_formref(attr_, reinterpret_cast<Dwarf::Off*>(&result),      &err); break;
                case DW_FORM_string:    callres = dwarf::dwarf_formstring(attr_,     reinterpret_cast<char **>(&result),          &err); break;
                case DW_FORM_flag:      callres = dwarf::dwarf_formflag(attr_,       reinterpret_cast<Dwarf::Bool*>(&result),     &err); break;
                case DW_FORM_block:     callres = dwarf::dwarf_formblock(attr_,      reinterpret_cast<Dwarf::Block**>(&result),   &err); break;
                case DW_FORM_exprloc:   callres = exprloc_eval(*dbg, attr_,          reinterpret_cast<uint64_t*>(&result),        &err); break;
                default: break;
            }
            if (callres == DW_DLV_ERROR) {
                throw Exception(dbg_, err);
            }
            return result;
        }
    private:
        std::weak_ptr<const Debug> dbg_;
        dwarf::Dwarf_Attribute attr_;
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
        Die(const Die& other) = delete;
        ~Die();

        virtual void traverse(TraversalFunction func, void* data);
        virtual void traverse_headless(TraversalFunction func, void* data);
        virtual std::shared_ptr<Die> sibling();
        virtual std::shared_ptr<Die> child();

        const Tag get_tag() const throw(Exception);
        const char* get_name() const throw(Exception);

        const dwarf::Dwarf_Die& get_handle() const {
            return die_;
        }

        Dwarf::Off get_offset() const throw(Exception);

        std::unique_ptr<const Attribute> get_attribute(Dwarf::Half attr) const;

    protected:
        Die();

    private:
        void init_sibling();
        void init_child();

        std::weak_ptr<const Debug> dbg_;
        dwarf::Dwarf_Die die_;
        std::shared_ptr<Die> sibling_, child_;
        std::unique_ptr<Dwarf::string> name_;
        std::unique_ptr<Dwarf::Off> offset_;
    };

    class EmptyDie : public Die {
    public:
        EmptyDie() : Die() {};
        inline virtual void traverse([[gnu::unused]] TraversalFunction func, [[gnu::unused]] void* data) override {}
        inline virtual void traverse_headless([[gnu::unused]] TraversalFunction func, [[gnu::unused]] void* data) override {}
        inline virtual std::shared_ptr<Die> sibling() { return nullptr; }
        inline virtual std::shared_ptr<Die> child()   { return nullptr; }
    };
}

#endif /* !LIBDWARFPP_DIE_HH */
