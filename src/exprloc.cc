#include <libdwarf++/dwarf.hh>
#include <libdwarf++/exprloc.hh>
#include <libdwarf++/cdwarf>
#include <vector>

#define BINOP(Op) \
    do {                                \
        uint64_t a = stack.back();      \
        stack.pop_back();               \
        uint64_t b = stack.back();      \
        stack.pop_back();               \
        stack.push_back(b Op a);        \
    } while (0)

#define UNOP(Op) \
    do {                                \
        int64_t a = stack.back();       \
        stack.pop_back();               \
        stack.push_back(Op(a));         \
    } while (0)

namespace Dwarf {
    int exprloc_eval(const Dwarf::Debug& dbg, dwarf::Dwarf_Attribute attr, uint64_t* result, Dwarf::Error* err) {

        Dwarf::Locdesc **locdescs;
        Dwarf::Signed len;
        if (dwarf::dwarf_loclist_n(attr, &locdescs, &len, err) == DW_DLV_ERROR) {
            return DW_DLV_ERROR;
        }

        std::vector<uint64_t> stack;

        bool has_result = false;

        for (int i = 0; i < len; ++i) {
            Dwarf::Locdesc *l = locdescs[i];
            for (int j = 0; j < l->ld_cents; ++j) {
                Dwarf::Loc& op = l->ld_s[j];
                switch (op.lr_atom) {
                    case DW_OP_lit0 ... DW_OP_lit31:
                        stack.push_back(static_cast<uint64_t>(DW_OP_lit31 - op.lr_atom));
                        break;
                    case DW_OP_addr:
                    case DW_OP_const1s:
                    case DW_OP_const1u:
                    case DW_OP_const2s:
                    case DW_OP_const2u:
                    case DW_OP_const4s:
                    case DW_OP_const4u:
                    case DW_OP_const8s:
                    case DW_OP_const8u:
                    case DW_OP_consts:
                    case DW_OP_constu:
                        stack.push_back(op.lr_number);
                        break;

                    case DW_OP_breg0 ... DW_OP_breg31:

                    case DW_OP_drop:
                        stack.pop_back();
                        break;
                    case DW_OP_dup:
                        stack.push_back(stack.back());
                        break;
                    case DW_OP_pick:
                        stack.push_back(stack[stack.size() - op.lr_number - 1]);
                        break;
                    case DW_OP_over:
                        stack.push_back(stack[stack.size() - 2]);
                        break;
                    case DW_OP_swap:
                        std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
                        break;
                    case DW_OP_rot:
                        std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
                        std::swap(stack[stack.size() - 2], stack[stack.size() - 3]);
                        break;

                    case DW_OP_deref: {
                        uint64_t *data = (uint64_t *) stack.back();
                        stack.pop_back();
                        stack.push_back(*data);
                    } break;

                    case DW_OP_deref_size: {
                        char *data = (char *) stack.back();
                        stack.pop_back();

                        uint64_t result; std::copy(data, data + op.lr_number, (char*) &result);
                        stack.push_back(result);
                    } break;

                    case DW_OP_abs:     UNOP(std::abs); break;
                    case DW_OP_neg:     UNOP(-); break;
                    case DW_OP_and:     BINOP(&); break;
                    case DW_OP_or:      BINOP(|); break;
                    case DW_OP_xor:     BINOP(^); break;
                    case DW_OP_shl:     BINOP(<<); break;
                    case DW_OP_shr:     BINOP(>>); break;
                    case DW_OP_plus:    BINOP(+); break;
                    case DW_OP_minus:   BINOP(-); break;
                    case DW_OP_div:     BINOP(/); break;
                    case DW_OP_mul:     BINOP(*); break;

                    case DW_OP_lt:      BINOP(<); break;
                    case DW_OP_le:      BINOP(<=); break;
                    case DW_OP_gt:      BINOP(>); break;
                    case DW_OP_ge:      BINOP(>=); break;
                    case DW_OP_eq:      BINOP(==); break;
                    case DW_OP_ne:      BINOP(!=); break;

                    case DW_OP_plus_uconst: {
                        uint64_t a = stack.back();
                        stack.pop_back();
                        stack.push_back(a + op.lr_number);
                    } break;

                    case DW_OP_stack_value: {
                        *result = stack.back();
                        has_result = true;
                    } break;

                    case DW_OP_nop: break;
                    default: throw std::runtime_error("Opcode not implemented");
                }
            }

            dbg.dealloc(l->ld_s);
        }
        dbg.dealloc(locdescs);

        if (!has_result)
            *result = stack.back();
        return DW_DLV_OK;
    }
}


