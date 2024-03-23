class DumpStack(gdb.Command):
    def __init__ (self):
        super (DumpStack, self).__init__ ("dump-stack", gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        frame = gdb.selected_frame()
        sp = int(frame.read_register("sp"))
        i = gdb.selected_inferior()
        print("current stack @", hex(sp))
        for ptr in range(0x80000 - 8, sp, -8):
            v = struct.unpack("<Q", i.read_memory(ptr, 8))[0]
            s = gdb.find_pc_line(v)
            if s.symtab:
                print("{:08x} {:016x} {}".format(ptr, v, s))
            else:
                print("{:08x} {:016x}".format(ptr, v))
        print("old stack")
        for ptr in range(sp, sp - 0x400, -8):
            v = struct.unpack("<Q", i.read_memory(ptr, 8))[0]
            s = gdb.find_pc_line(v)
            if s.symtab:
                print("{:08x} {:016x} {}".format(ptr, v, s))

DumpStack()

M64 = {
    0b0000: "EL0t",
    0b0100: "EL1t",
    0b0101: "EL1h"
}

class Armv8AException(gdb.Command):
    def __init__ (self):
        super (Armv8AException, self).__init__ ("armv8a-exception", gdb.COMMAND_USER)
    
    def print_data_abort(self, frame, iss):
        isv = (iss >> 23) & 0x1
        sas = (iss >> 21) & 0x3
        sse = (iss >> 20) & 0x1
        srt = (iss >> 15) & 0x1f
        sf = (iss >> 14) & 0x1
        ar = (iss >> 13) & 0x1
        vncr = (iss >> 12) & 0x1
        _set = (iss >> 10) & 0x3
        fnv = (iss >> 9) & 0x1
        ea = (iss >> 8) & 0x1
        cm = (iss >> 7) & 0x1
        s1ptw = (iss >> 6) & 0x1
        wnr = (iss >> 5) & 0x1
        dfsc = iss & 0x1f
        try:
            value = int(frame.read_register("FAR_EL1"))
            print("FAR", hex(value))
        except ValueError:
            print("FAR unreadable")
        if isv:
            # print("isv valid", sas, sse, srt, sf, ar)
            access_sizes = ("Byte", "Halfword", "Word", "Doubleword")
            print("Access size:", access_sizes[sas])
            print("Sign extended:", "Yes" if sse else "No")
            print("Register:", hex(srt), "64-bit" if sf else "32-bit")
            print("Acquire/Release:", "Yes" if ar else "No")
        if dfsc == 0b010000:
            print("Not on translation table walk")
            if not fnv:
                value = int(frame.read_register("FAR_EL1"))
                print("FAR", hex(value))
        elif dfsc == 0b000101:
            print("translation fault level 1")
        elif dfsc == 0b000000:
            print("address size fault level 0")
        elif dfsc == 0b000001:
            print("address size fault level 1")
        elif dfsc == 0b010001:
            print("tag check fault")
        elif dfsc == 0b100001:
            print("alignment fault")
        else:
            print(bin(dfsc))
        print(vncr, _set, fnv, ea, cm, s1ptw, wnr, dfsc)

    def print_instruction_abort(self, frame, iss):
        _set = (iss >> 10) & 0x3
        fnv = (iss >> 9) & 0x1
        ea = (iss >> 8) & 0x1
        s1ptw = (iss >> 6) & 0x1
        ifsc = iss & 0x1f
        if ifsc == 0b010000:
            print("Not on translation table walk")
            if not fnv:
                value = int(frame.read_register("FAR_EL1"))
                print("FAR", hex(value))
        elif ifsc == 0b00101:
            print("translation fault level 1")
        elif ifsc == 0b01001:
            print("access flag fault level 1")
        # elif dfsc == 0b100001:
        #     print("alignment fault")
        else:
            print(bin(ifsc))

    def invoke (self, arg, from_tty):
        frame = gdb.selected_frame()
        value = int(frame.read_register("ESR_EL1"))
        pstate = int(frame.read_register("SPSR_EL1"))
        if value == 0:
            return None
        print("Program state:")
        if (pstate & (1 << 4)) != 0:
            print("aarch32")
        else:
            print("aarch64")
            print(M64.get(pstate & 0xf, "undefined"))
            print("Flags: ", end="")
            for bit, name in zip(range(6, 10), ("F", "I", "A", "D")):
                if (pstate & (1 << bit)) != 0:
                    print(name, end=" ")
            print()
        print(hex(pstate))
        iss2 = (value >> 32) & 0x1ff
        ec = (value >> 26) & 0x3ff
        il = (value >> 25) & 0x1
        iss = value & 0xffffff
        if ec == 0b000000:
            print("Unknown fault")
        elif ec == 0b000001:
            print("Trapped WF*")
        elif ec == 0b000011:
            print("Trapped MCR or MRC")
        elif ec == 0b000100:
            print("Trapped MCRR or MRRC")
        elif ec == 0b000101:
            print("Trapped MCR or MRC")
        elif ec == 0b000110:
            print("Trapped LDC or STC")
        elif ec == 0b000111:
            print("Trapped SIMD")
        elif ec == 0b001000:
            print("Trapped VMRS")
        elif ec == 0b001001:
            print("Trapped pointer authentication")
        elif ec == 0b001010:
            print("Trapped LD64B or ST64B*")
        elif ec == 0b001100:
            print("Trapped MRRC")
        elif ec == 0b001101:
            print("Branch target exception")
        elif ec == 0b001110:
            print("Illegal execution state")
        elif ec == 0b010001:
            print("SVC instruction")
        elif ec == 0b010010:
            print("HVC instruction")
        elif ec == 0b010011:
            print("SMC instruction")
        elif ec == 0b010101:
            print("SVC instruction")
        elif ec == 0b010110:
            print("HVC instruction")
        elif ec == 0b010111:
            print("SMC instruction")
        elif ec == 0b011000:
            print("Trapped MRS, MRS or system instruction")
        elif ec == 0b011001:
            print("Trapped SVE")
        elif ec == 0b011010:
            print("Trapped ERET")
        elif ec == 0b011100:
            print("Failed pointer authentication")
        elif ec == 0b100000:
            print("Instruction abort from lower level")
        elif ec == 0b100001:
            print("Instruction abort from same level")
            self.print_instruction_abort(frame, iss)
        elif ec == 0b100010:
            print("PC alignment failure")
        elif ec == 0b100100:
            print("Data abort from lower level")
        elif ec == 0b100101:
            print("Data abort from same level")
            self.print_data_abort(frame, iss)
        elif ec == 0b100110:
            print("SP alignment fault")
        elif ec == 0b101000:
            print("32-bit floating point exception")
        elif ec == 0b101100:
            print("64-bit floating point exception")
        elif ec == 0b101111:
            print("SError interrupt")
        elif ec == 0b110000:
            print("Breakpoint from lower level")
        elif ec == 0b110001:
            print("Breakpoint from same level")
        elif ec == 0b110010:
            print("Software step from lower level")
        elif ec == 0b110011:
            print("Software step from same level")
        elif ec == 0b110100:
            print ("Watch point from same level")
        elif ec == 0b110101:
            print("Watch point from lower level")
        elif ec == 0b111000:
            print("Breakpoint in aarch32 mode")
        elif ec == 0b111010:
            print("Vector catch in aarch32")
        elif ec == 0b111100:
            print("Brk instruction in aarch64")
        print(hex(int(value)), iss2, bin(ec), il, iss)

Armv8AException()

from gdb.unwinder import Unwinder

class FrameId(object):
    def __init__(self, sp, pc):
        self.sp = sp
        self.pc = pc

import struct

class ExceptionUnwinder(Unwinder):
    def __init__(self):
        super(ExceptionUnwinder, self).__init__("ExceptionUnwinder")

    def __call__(self, pending_frame):
        pc = pending_frame.read_register("pc")
        if "hard_fault" not in str(pc) and "irq" not in str(pc):
            return None

        # Create UnwindInfo.  Usually the frame is identified by the stack 
        # pointer and the program counter.
        sp = pending_frame.read_register("sp")
        unwind_info = pending_frame.create_unwind_info(FrameId(sp, pc))

        regs = ["x" + str(i) for i in range(28, -1, -1)] + ["pc", "spsr_el1", "x29", "x30"]

        i = gdb.selected_inferior()
        for j, reg in enumerate(regs):
            ptr = int(sp) + (j + 1) * 8
            v = struct.unpack("<Q", i.read_memory(ptr, 8))[0]
            # print("{:08x} {} - {:016x}".format(ptr, reg, v))

        # Find the values of the registers in the caller's frame and 
        # save them in the result:
            if not reg.endswith("el1"):
                unwind_info.add_saved_register(reg, gdb.Value(v))
        #....
        sp = int(sp) + (len(regs) + 1) * 8
        unwind_info.add_saved_register("sp", gdb.Value(sp))

        # Return the result:
        return unwind_info
        #return None

gdb.unwinder.register_unwinder(None, ExceptionUnwinder(), replace=True)
