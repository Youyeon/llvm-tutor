//==============================================================================
// FILE:
//    RemoveSpecials.cpp
//
// DESCRIPTION:
//    This pass performs ?
//
// USAGE:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libRemoveSpecials.so `\`
//        -passes=-"remove-specials" <bitcode-file>
//      The command line option is not available for the new PM

// License: MIT
//==============================================================================
#include "RemoveSpecials.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <random>

using namespace llvm;

#define DEBUG_TYPE "remove-specials"

STATISTIC(SubstCount, "The # of removed specials");

//-----------------------------------------------------------------------------
// RemoveSpecials Implementation
//-----------------------------------------------------------------------------

bool RemoveSpecials::isTargetRemoveInst(const llvm::Instruction *Inst) {
  StringRef name = Inst->getOpcodeName(); //icmp
  if (name != "icmp")
    return false;
  
  Value* operand = Inst->getOperand(1);
  OS << "[RemoveSpecials] : " << name << ", " << *operand << "\n";
  return true;
}


bool RemoveSpecials::removeUselessBlock(llvm::BasicBlock *BB,
                          llvm::SmallPtrSet<llvm::BasicBlock *, 8> &DeleteList) {
  bool Changed = false;
  for (auto &Inst : *BB) {
    bool isTarget = isTargetRemoveInst(&Inst);
    Changed |= isTarget;
  }

  return Changed;
}

PreservedAnalyses RemoveSpecials::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
  bool Changed = false;
  SmallPtrSet<BasicBlock *, 8> DeleteList;
  for (auto &BB : Func) {
    Changed |= removeUselessBlock(&BB, DeleteList);
  }

  for (BasicBlock *BB : DeleteList) {
    DeleteDeadBlock(BB);
  }

  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
  // isTargetRemoveInst(Func);
  // return PreservedAnalyses::all();
}


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getRemoveSpecialsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "remove-specials", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "remove-specials") {
                    FPM.addPass(RemoveSpecials(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getRemoveSpecialsPluginInfo();
}
