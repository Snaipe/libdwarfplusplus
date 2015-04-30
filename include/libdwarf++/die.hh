#ifndef LIBDWARFPP_DIE_HH
# define LIBDWARFPP_DIE_HH

# include <functional>
# include <memory>
# include "dwarf.hh"
# include "tag.hh"

namespace Dwarf {

    class Die {
    public:
        enum TraversalResult {
            TRAVERSE = 0,
            SKIP,
            BREAK,
        };

        Die(std::weak_ptr<Debug> dbg, dwarf::Dwarf_Die& die);
        Die(const Die& other) = delete;
        ~Die();

        virtual void traverse(std::function<TraversalResult(Die&)> func);
        virtual std::shared_ptr<Die> sibling();
        virtual std::shared_ptr<Die> child();

        const Tag get_tag() const throw(Exception) {
            Error err;
            Half tag;
            switch (dwarf::dwarf_tag(die_, &tag, &err)) {
                case DW_DLV_ERROR:
                    throw Exception(dbg_, err);
                default: break;
            }
            return Tag(tag);
        }

        const dwarf::Dwarf_Die& get_handle() const {
            return die_;
        }

    protected:
        Die();

    private:
        void init_sibling();
        void init_child();

        std::weak_ptr<Debug> dbg_;
        dwarf::Dwarf_Die die_;
        std::shared_ptr<Die> sibling_, child_;
    };

    class EmptyDie : public Die {
    public:
        EmptyDie() : Die() {};
        inline virtual void traverse(std::function<TraversalResult(Die&)> func) override {}
        inline virtual std::shared_ptr<Die> sibling() { return nullptr; }
        inline virtual std::shared_ptr<Die> child()   { return nullptr; }
    };
}

#endif /* !LIBDWARFPP_DIE_HH */
