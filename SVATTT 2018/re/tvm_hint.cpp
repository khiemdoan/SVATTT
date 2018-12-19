#define VM_STACK_SIZE      16
#define VM_MEMORY_SIZE     40

enum VmOpcode
{
    VmNop = 0,
    VmPushI4,
    VmLoad,
    VmLoadTopIdx,
    VmStore,
    VmAdd,
    VmSub,
    VmMul,
    VmAnd,
    VmOr,
    VmXor,
    VmNot,
    VmShl,
    VmShr,
    VmIsZero,
    VmPutChar,
    VmExit,

    VmOpcodeEnd,
};



define TEA_CONST_MASK          0x12344321 // Mask the TEA constants
#define TEA_CONST               0x9e3779b9

#define ENCRYPT_ITERATION()     VM_LOAD(MEM_POS_SUM), \
                                VM_PUSH_I4(TEA_CONST_MASK ^ TEA_CONST), \
                                VM_PUSH_I4(TEA_CONST_MASK), \
                                VmXor, \
                                VmAdd, \
                                VM_STORE(MEM_POS_SUM), \
                                VM_LOAD(MEM_POS_V1), \
                                VM_PUSH_I4(4), \
                                VmShl, \
                                VM_LOAD(MEM_POS_KEY), \
                                VM_PUSH_I4(KM0), \
                                VmXor, \
                                VmAdd, \
                                VM_LOAD(MEM_POS_SUM), \
                                VM_LOAD(MEM_POS_V1), \
                                VmAdd, \
                                VmXor, \
                                VM_LOAD(MEM_POS_V1), \
                                VM_PUSH_I4(5), \
                                VmShr, \
                                VM_LOAD(MEM_POS_KEY + 1), \
                                VM_PUSH_I4(KM1), \
                                VmXor, \
                                VmAdd, \
                                VmXor, \
                                VM_LOAD(MEM_POS_V0), \
                                VmAdd, \
                                VM_STORE(MEM_POS_V0), \
                                VM_LOAD(MEM_POS_V0), \
                                VM_PUSH_I4(4), \
                                VmShl, \
                                VM_LOAD(MEM_POS_KEY + 2), \
                                VM_PUSH_I4(KM2), \
                                VmXor, \
                                VmAdd, \
                                VM_LOAD(MEM_POS_V0), \
                                VM_LOAD(MEM_POS_SUM), \
                                VmAdd, \
                                VmXor, \
                                VM_LOAD(MEM_POS_V0), \
                                VM_PUSH_I4(5), \
                                VmShr, \
                                VM_LOAD(MEM_POS_KEY + 3), \
                                VM_PUSH_I4(KM3), \
                                VmXor, \
                                VmAdd, \
                                VmXor, \
                                VM_LOAD(MEM_POS_V1), \
                                VmAdd, \
                                VM_STORE(MEM_POS_V1)

// Loop 32 times
#define ENCRYPT_LOOP()          ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION(), \
                                ENCRYPT_ITERATION()

#define ENCRYPT_INPUT(idx)      VM_PUSH_I4(0), \
                                VM_STORE(MEM_POS_SUM), \
                                VM_LOAD(MEM_POS_INPUT + idx), \
                                VM_STORE(MEM_POS_V0), \
                                VM_LOAD(MEM_POS_INPUT + idx + 1), \
                                VM_STORE(MEM_POS_V1), \
                                ENCRYPT_LOOP(), \
                                VM_LOAD(MEM_POS_V0), \
                                VM_LOAD(MEM_POS_TARGET_VALUE + idx), \
                                VmXor, \
                                VM_LOAD(MEM_POS_MAGIC), \
                                VmOr, \
                                VM_STORE(MEM_POS_MAGIC), \
                                VM_LOAD(MEM_POS_V1), \
                                VM_LOAD(MEM_POS_TARGET_VALUE + idx + 1), \
                                VmXor, \
                                VM_LOAD(MEM_POS_MAGIC), \
                                VmOr, \
                                VM_STORE(MEM_POS_MAGIC)

#define SCREEN_KEY              0xDC


#define DISPLAY_CHAR_AT(idx)    VM_LOAD(MEM_POS_MAGIC), \
                                VmLoadTopIdx, \
                                VM_PUSH_I4(idx << 3), \
                                VmShr, \
                                VM_PUSH_I4(0xFF), \
                                VmAnd, \
                                VM_PUSH_I4(SCREEN_KEY), \
                                VM_LOAD(MEM_POS_MAGIC), \
                                VM_LOAD(MEM_POS_SUM), \
                                VmSub, \
                                VM_PUSH_I4(2), \
                                VmShl, \
                                VmAdd, \
                                VM_PUSH_I4(idx), \
                                VmAdd, \
                                VmXor, \
                                VmPutChar

#define DISPLAY_ITERATION()     DISPLAY_CHAR_AT(0), \
                                DISPLAY_CHAR_AT(1), \
                                DISPLAY_CHAR_AT(2), \
                                DISPLAY_CHAR_AT(3), \
                                VM_LOAD(MEM_POS_MAGIC), \
                                VM_PUSH_I4(1), \
                                VmAdd, \
                                VM_STORE(MEM_POS_MAGIC)


#define DISPLAY_MESSAGE()       VM_LOAD(MEM_POS_MAGIC), \
                                VM_STORE(MEM_POS_SUM), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION(), \
                                DISPLAY_ITERATION()
								
								
static constexpr uint8_t bytecode[] =
    {
        // Store the key
        VM_PUSH_I4(key.data[0] ^ KM0),
        VM_STORE(MEM_POS_KEY),
        VM_PUSH_I4(key.data[1] ^ KM1),
        VM_STORE(MEM_POS_KEY + 1),
        VM_PUSH_I4(key.data[2] ^ KM2),
        VM_STORE(MEM_POS_KEY + 2),
        VM_PUSH_I4(key.data[3] ^ KM3),
        VM_STORE(MEM_POS_KEY + 3),

        // Encrypt the input
        ENCRYPT_INPUT(0),
        ENCRYPT_INPUT(2),
        ENCRYPT_INPUT(4),
        ENCRYPT_INPUT(6),

        // Check the encryption result
        VM_LOAD(MEM_POS_MAGIC),
        VmIsZero,

        // Determine to display the good-boy or the bad-boy message
        VM_PUSH_I4(8),
        VmMul,
        VM_PUSH_I4(MEM_POS_SCREEN),
        VmAdd,
        VM_STORE(MEM_POS_MAGIC),
        DISPLAY_MESSAGE(),

        VmExit,
    };
	
typedef struct
{
    uint32_t sp; // stack pointer
    uint32_t mp; // memory pointer
    uint32_t pc;
    uint32_t stack[VM_STACK_SIZE];
    uint32_t mem[VM_MEMORY_SIZE];

    bool finished;
    const uint8_t *bytecode;
    uint32_t byteCodeSize;
    PutCharProto fnPutChar;
} STACK_VM;

static const Handler VmHandlers[] =
{
    HandleNop,
    HandlePushI4,
    HandleLoad,
    HandleLoadTopIdx,
    HandleStore,
    HandleAdd,
    HandleSub,
    HandleMul,
    HandleAnd,
    HandleOr,
    HandleXor,
    HandleNot,
    HandleShl,
    HandleShr,
    HandleIsZero,
    HandlePutChar,
    HandleExit,
};

bool VmExec(
    STACK_VM&      vm,
    const uint8_t  *bytecode,
    uint32_t       bytecodeSize)
{
    vm.bytecode = bytecode;
    vm.byteCodeSize = bytecodeSize;
    while (!vm.finished)
    {
        if (vm.pc >= bytecodeSize)
            return false;

        if (!VmHandlers[bytecode[vm.pc]](vm))
            return false;
    }
    return true;
}

bool HandleNop(STACK_VM &vm)
{
    vm.pc++;
    return true;
}

#define MEM_POS_SUM             0
#define MEM_POS_V0              (MEM_POS_SUM + 2)
#define MEM_POS_V1              (MEM_POS_SUM + 1)
#define MEM_POS_MAGIC           3


#define MEM_POS_KEY             4   // Where to store the key in VM's memory
#define MEM_POS_INPUT           8   // Where to store the user input in VM's memory
#define MEM_POS_TARGET_VALUE    16  // Where to store the target buffer we want to reach

#define MEM_POS_SCREEN          24  // Where to store the screen buffer


int main()
{
    printf("Please enter password: ");
    char password[33];
    if (fgets(password, sizeof(password), stdin) == nullptr)
        return 1;
    size_t len = STRLEN(password);
    if (len > 0 && password[len - 1] == '\n')
        password[len - 1] = 0;

    VM::STACK_VM vm = {};
    MEMCPY(&vm.mem[MEM_POS_INPUT], password, 32);
    MEMCPY(&vm.mem[MEM_POS_TARGET_VALUE], &encrypted, sizeof(encrypted));
    MEMCPY(&vm.mem[MEM_POS_SCREEN], &badBoy, sizeof(badBoy));
    MEMCPY(&vm.mem[MEM_POS_SCREEN + 8], &goodBoy, sizeof(goodBoy));
    vm.fnPutChar = &WriteChar;
    
    if (!VM::VmExec(vm, bytecode, sizeof(bytecode)))
    {
        return 1;
    }
    return 0;
}