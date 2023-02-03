#include "aethervisor_test.h"

/*	log out-of-module function calls and jmps		*/

void ExecuteHook(GuestRegisters* registers, void* return_address, void* o_guest_rip)
{
	AddressInfo retaddr_info = { return_address };
	AddressInfo rip_info = { o_guest_rip };

	Logger::Get()->Print(COLOR_ID::green, "[EXECUTE]\n");  
	Logger::Get()->Print(COLOR_ID::none, "return address = ");

	retaddr_info.Print();

	Logger::Get()->Print(COLOR_ID::none, "RIP = ", o_guest_rip);

	rip_info.Print();

	Logger::Get()->Print(COLOR_ID::none, "\n\n");
}


/*	log specific reads and writes		*/

void ReadWriteHook(GuestRegisters* registers, void* o_guest_rip)
{
	AddressInfo rip_info = { o_guest_rip };

	ZydisDecodedOperand operands[5] = { 0 };

	auto instruction = Disasm::Disassemble((uint8_t*)o_guest_rip, operands);

	Logger::Get()->Print(COLOR_ID::magenta, "[READ/WRITE]\n");
	Logger::Get()->Print(COLOR_ID::none, "RIP = ");

	rip_info.Print();

	ZydisRegisterContext context;

	Disasm::MyRegContextToZydisRegContext(registers, &context, o_guest_rip);

	for (int i = 0; i < instruction.operand_count_visible; ++i)
	{
		auto mem_target = Disasm::GetMemoryAccessTarget(
			instruction, &operands[i], (ZyanU64)o_guest_rip, &context);

		if (operands[i].actions & ZYDIS_OPERAND_ACTION_MASK_WRITE)
		{
			Logger::Get()->Print(COLOR_ID::none, "[write => 0x%02x]\n", mem_target);
		}
		else if (operands[i].actions & ZYDIS_OPERAND_ACTION_MASK_READ)
		{
			Logger::Get()->Print(COLOR_ID::none, "[read => 0x%02x]\n", mem_target);
		}
	}

	Logger::Get()->Print(COLOR_ID::none, "\n\n");
}

void SandboxTest()
{
    AetherVisor::InstrumentationHook(AetherVisor::sandbox_readwrite, ReadWriteHook);
	AetherVisor::InstrumentationHook(AetherVisor::sandbox_execute, ExecuteHook);

	AetherVisor::SandboxRegion(beclient, PeHeader(beclient)->OptionalHeader.SizeOfImage);

	auto kernel32 = (uint8_t*)GetModuleHandleA("kernel32.dll");

	AetherVisor::DenySandboxMemAccess(kernel32 + 0x1005);
}