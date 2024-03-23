coprocessor_registers = {
    "vbar": "0 12 0 0",
    "dfsr": "0 5 0 0",
    "ifsr": "0 5 0 1",
    "far": "0 6 0 0",
    "ifar": "0 6 0 2"
}

modes = {
    0b10000 : "User",
    0b10001 : "FIQ",
    0b10010 : "IRQ",
    0b10011 : "Supervisor",
    0b10111 : "Abort",
    0b11011 : "Undefined",
    0b11111 : "System",
    0b10110 : "Secure Monitor"
}

MODE_ABORT = 0b10111

faults = {
    0 : {
        0b0001: "Alignment fault",
        0b0010: "Instruction debug event fault",
        0b0011: "Access bit fault on section",
        0b0100: "Instruction cache maintenance operation fault",
        0b0101: "Translation section fault",
        0b0110: "Access bit fault on page",
        0b0111: "Translation page fault",
        0b1000: "Precise external abort",
        0b1001: "Domain section fault",
        0b1011: "Domain page fault",
        0b1100: "External abort on translation, first level",
        0b1101: "Permission section fault",
        0b1110: "External abort on translation, second level",
        0b1111: "Permission page fault"
    },
    1 : {
        0b0110: "Imprecise external abort"
    }
}

def read_coprocessor(name):
    return int(gdb.execute("mon arm mrc 15 " + coprocessor_registers[name], False, True))

class Arm11Exception(gdb.Command):
    def __init__ (self):
        super (Arm11Exception, self).__init__ ("arm11-exception", gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        frame = gdb.selected_frame()
        cpsr = int(frame.read_register("cpsr"))
        mode = cpsr & 0x1f
        print("Current CPU mode:", modes[mode])
        vbar = read_coprocessor("vbar")
        print("Vector base address: 0x{:08x}".format(vbar))
        if mode == MODE_ABORT:
            dfsr = read_coprocessor("dfsr")
            ifsr = read_coprocessor("ifsr")
            far = read_coprocessor("far")
            ifar = read_coprocessor("ifar")
            if dfsr != 0:
                print("Data fault: 0x{:08x}".format(dfsr))
                print("Fault address: 0x{:08x}".format(far))
                if (dfsr & (1 << 11)) == 0:
                    print("  Read access")
                else:
                    print("  Write access")
                print("  Domain:", (dfsr >> 4) & 0xf)
                fsr = dfsr
            if ifsr != 0:
                print("Instruction fault: 0x{:08x}".format(ifsr))
                print("  Fault address: 0x{:08x}".format(ifar))
                fsr = ifsr
            if (fsr & (1 << 12)) == 0:
                print("  AXI Decode error")
            else:
                print("  AXI Decode error")
            bit10 = (fsr >> 10) & 0x1
            reason = faults[bit10][fsr & 0xf]
            print("  Fault reason:", reason)

        # pstate = int(frame.read_register("SPSR_EL1"))
        # if value == 0:
        #     return None


Arm11Exception()

from gdb.unwinder import Unwinder

class FrameId(object):
    def __init__(self, sp, pc):
        self.sp = sp
        self.pc = pc

import struct

mode_suffixes = {
    0b10000 : "usr",
    0b10001 : "fiq",
    0b10010 : "irq",
    0b10011 : "svc",
    0b10111 : "abt",
    0b11011 : "und",
    0b11111 : "usr",
    0b10110 : "mon"
}

class ExceptionUnwinder(Unwinder):
    def __init__(self):
        super(ExceptionUnwinder, self).__init__("ExceptionUnwinder")

    def __call__(self, pending_frame):
        pc = pending_frame.read_register("pc")
        psr = pending_frame.read_register("cpsr")
        mode = int(psr) & 0x1f
        if "hard_fault" not in str(pc) and "irq" not in str(pc) and "err_hang" not in str(pc):
            return None
        print(str(pc))
        mode = mode_suffixes[mode]
        print("spsr_" + mode)
        spsr = pending_frame.read_register("spsr_" + mode)
        previous_mode = mode_suffixes[int(spsr) & 0x1f]
        print(previous_mode)

        # Create UnwindInfo.  Usually the frame is identified by the stack 
        # pointer and the program counter.
        sp = pending_frame.read_register("sp")
        unwind_info = pending_frame.create_unwind_info(FrameId(sp, pc))

        regs = ["r" + str(i) for i in range(4)] + ["r12", "pc"]
        uint32_t = gdb.lookup_type("uint32_t")

        # Copy all registers to the new frame. Ones we saved will be restored after this.
        for r in pending_frame.architecture().registers():
            unwind_info.add_saved_register(r.name, pending_frame.read_register(r.name))

        i = gdb.selected_inferior()
        for j, reg in enumerate(regs):
            ptr = int(sp) + (j + 1) * 4
            mem = i.read_memory(ptr, 4)
            v = struct.unpack("<I", mem)[0]
            print("{:08x} {} - {:08x}".format(ptr, reg, v))

        # Find the values of the registers in the caller's frame and 
        # save them in the result:
            unwind_info.add_saved_register(reg, gdb.Value(mem, uint32_t))
        #....
        sp = pending_frame.read_register("sp_" + previous_mode)
        pc = pending_frame.read_register("lr_" + mode)
        pc -= 4
        if mode == "abt":
            pc -= 4
        print("pc {:08x} sp {:08x}".format(int(pc), int(sp)))
        unwind_info.add_saved_register("sp", gdb.Value(sp))
        unwind_info.add_saved_register("pc", gdb.Value(pc))
        unwind_info.add_saved_register("cpsr", spsr)

        print("done")
        # Return the result:
        return unwind_info
        #return None

gdb.unwinder.register_unwinder(None, ExceptionUnwinder(), replace=True)

class DumpStack(gdb.Command):
    def __init__ (self):
        super (DumpStack, self).__init__ ("dump-stack", gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        frame = gdb.selected_frame()
        sp = int(frame.read_register("sp"))
        i = gdb.selected_inferior()
        print("current stack @", hex(sp))
        for ptr in range(0x202000 - 4, sp, -4):
            v = struct.unpack("<I", i.read_memory(ptr, 4))[0]
            s = gdb.find_pc_line(v)
            if s.symtab:
                print("{:04x} {:016x} {}".format(ptr, v, s))
            else:
                print("{:04x} {:016x}".format(ptr, v))
        print("old stack")
        for ptr in range(sp, sp - 0x400, -4):
            v = struct.unpack("<I", i.read_memory(ptr, 4))[0]
            s = gdb.find_pc_line(v)
            if s.symtab:
                print("{:04x} {:016x} {}".format(ptr, v, s))

DumpStack()
