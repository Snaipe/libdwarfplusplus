#ifndef LIBDWARFPP_DIE_HH
# define LIBDWARFPP_DIE_HH

# include <functional>
# include <memory>
# include "dwarf.hh"

namespace Dwarf {

    class Die {
    public:
        Die(std::weak_ptr<Debug> dbg, dwarf::Dwarf_Die& die);
        Die(const Die& other) = delete;
        ~Die();

        virtual void traverse(std::function<void(Die&)> func);
        virtual std::shared_ptr<Die> sibling();
        virtual std::shared_ptr<Die> child();

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
        inline virtual void traverse(std::function<void(Die&)> func) override {}
        inline virtual std::shared_ptr<Die> sibling() { return nullptr; }
        inline virtual std::shared_ptr<Die> child()   { return nullptr; }
    };
}

#endif /* !LIBDWARFPP_DIE_HH */
