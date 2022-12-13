#pragma once
#include "disassembly.h"

namespace Disasm
{
	void MyRegContextToZydisRegContext(VcpuData* vcpu_data, GuestRegisters* guest_regs, ZydisRegisterContext* context)
	{
		context->values[ZYDIS_REGISTER_RAX] = vcpu_data->guest_vmcb.save_state_area.Rax;
		context->values[ZYDIS_REGISTER_RSP] = vcpu_data->guest_vmcb.save_state_area.Rsp;
		context->values[ZYDIS_REGISTER_RIP] = vcpu_data->guest_vmcb.save_state_area.Rip;
		context->values[ZYDIS_REGISTER_RFLAGS] = vcpu_data->guest_vmcb.save_state_area.Rflags.Flags;
		context->values[ZYDIS_REGISTER_RCX] = guest_regs->rcx;
		context->values[ZYDIS_REGISTER_RDX] = guest_regs->rdx;
		context->values[ZYDIS_REGISTER_RBX] = guest_regs->rbx;
		context->values[ZYDIS_REGISTER_RBP] = guest_regs->rbp;
		context->values[ZYDIS_REGISTER_RSI] = guest_regs->rsi;
		context->values[ZYDIS_REGISTER_RDI] = guest_regs->rdi;
		context->values[ZYDIS_REGISTER_R8] = guest_regs->r8;
		context->values[ZYDIS_REGISTER_R9] = guest_regs->r9;
		context->values[ZYDIS_REGISTER_R10] = guest_regs->r10;
		context->values[ZYDIS_REGISTER_R11] = guest_regs->r11;
		context->values[ZYDIS_REGISTER_R12] = guest_regs->r12;
		context->values[ZYDIS_REGISTER_R13] = guest_regs->r13;
		context->values[ZYDIS_REGISTER_R14] = guest_regs->r14;
		context->values[ZYDIS_REGISTER_R15] = guest_regs->r15;
	}

	ZydisDecodedInstruction Disassemble(uint8_t* instruction, ZydisDecodedOperand* operands)
	{
		ZydisDecodedInstruction zydis_insn;

		ZydisDecoderDecodeFull(
			&zydis_decoder,
			instruction, 16,
			&zydis_insn,
			operands,
			ZYDIS_MAX_OPERAND_COUNT_VISIBLE,
			ZYDIS_DFLAG_VISIBLE_OPERANDS_ONLY
		);

		return zydis_insn;
	}

	/*	Gets total instructions length closest to byte_length	*/
	int	LengthOfInstructions(void* address, int byte_length)
	{
		ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT_VISIBLE];

		int insns_len = 0;
		for (insns_len = 0; insns_len < byte_length;)
		{
			int cur_insn_len = Disassemble((uint8_t*)address + insns_len, operands).length;
			insns_len += cur_insn_len;
		}

		return insns_len;
	}

	ZyanU64 GetMemoryAccessTarget(
		ZydisDecodedInstruction& instruction, 
		ZydisDecodedOperand* operand, 
		ZyanU64 runtime_address, 
		ZydisRegisterContext* registers
	)
	{
		auto destination = 0ULL;
		
		ZydisCalcAbsoluteAddressEx(&instruction, operand, runtime_address, registers, &destination);

		return destination;
	}

	uint8_t* ForEachInstruction(uint8_t* start, uint8_t* end, bool(*Callback)(uint8_t* insn_addr, ZydisDecodedInstruction instruction))
	{
		size_t instruction_size = NULL;

		uint8_t* instruction = 0;

		for (instruction = start; instruction < end; instruction = instruction + instruction_size)
		{
			ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT_VISIBLE];

			auto insn = Disasm::Disassemble(instruction, operands);

			if (!Callback(instruction, insn))
			{
				break;
			}

			instruction_size = insn.length;
		}

		return instruction;
	}

	int Init()
	{
		return ZydisDecoderInit(&zydis_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
	}
};