#include "BasicTrace.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "basic-trace"

std::string BasicTrace::getLabel(BasicBlock* BB) {
  std::string str;
  raw_string_ostream myOS(str);
  BB->printAsOperand(myOS, false);
  return myOS.str();
}

PreservedAnalyses BasicTrace::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
    errs() << "- Start of function [" << Func.getName() << "]\n";
    for (auto &BB : Func) {
      // errs() << "- Start of BasicBlock [" << BB.getName() << "], num of instructions = " << BB.size() << "\n";
      errs() << "- Start of BasicBlock [" << getLabel(&BB) << "], num of instructions = " << BB.size() << "\n";
      for (Instruction &I : BB) {
        errs() << "    - Instruction : " << I << "\n";
      }
    }

    return llvm::PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getBasicTracePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "basic-trace", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "basic-trace") {
                    FPM.addPass(BasicTrace(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getBasicTracePluginInfo();
}