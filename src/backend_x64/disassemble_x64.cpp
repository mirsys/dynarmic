/* This file is part of the dynarmic project.
 * Copyright (c) 2018 MerryMage
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include <string>

#include <fmt/format.h>

#ifdef DYNARMIC_USE_LLVM
#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>
#endif

#include "backend_x64/disassemble_x64.h"
#include "common/assert.h"
#include "common/common_types.h"

namespace Dynarmic::BackendX64 {

std::string DisassembleX64(const void* begin, const void* end) {
    std::string result;

#ifdef DYNARMIC_USE_LLVM
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86Disassembler();
    LLVMDisasmContextRef llvm_ctx = LLVMCreateDisasm("x86_64", nullptr, 0, nullptr, nullptr);
    LLVMSetDisasmOptions(llvm_ctx, LLVMDisassembler_Option_AsmPrinterVariant);

    const u8* pos = reinterpret_cast<const u8*>(begin);
    size_t remaining = reinterpret_cast<size_t>(end) - reinterpret_cast<size_t>(pos);
    while (pos < end) {
        char buffer[80];
        size_t inst_size = LLVMDisasmInstruction(llvm_ctx, const_cast<u8*>(pos), remaining, reinterpret_cast<u64>(pos), buffer, sizeof(buffer));
        ASSERT(inst_size);
        for (const u8* i = pos; i < pos + inst_size; i++)
            result += fmt::format("{:02x} ", *i);
        for (size_t i = inst_size; i < 10; i++)
            result += "   ";
        result += buffer;
        result += '\n';

        pos += inst_size;
        remaining -= inst_size;
    }

    LLVMDisasmDispose(llvm_ctx);
#else
    result += fmt::format("(recompile with DYNARMIC_USE_LLVM=ON to disassemble the generated x86_64 code)\n");
    result += fmt::format("start: {:016x}, end: {:016x}\n", begin, end);
#endif

    return result;
}

} // namespace Dynarmic::BackendX64
