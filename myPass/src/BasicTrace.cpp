#include "BasicTrace.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
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

std::vector<std::string> BasicTrace::getPredecessors(BasicBlock* BB) {
  std::vector<std::string> Preds;
  for (BasicBlock *Pred : predecessors(BB)) {
    Preds.push_back(getLabel(Pred));
  }
  return Preds;
}

void BasicTrace::printPredecessors(BasicBlock* BB) {
  std::vector<std::string> Preds = getPredecessors(BB);
  for (std::string str : Preds)
        errs() << str << ", ";
      errs() << "\n";
}

void BasicTrace::TracePathBB(BasicBlock* BB) {
  if (predecessors(BB).empty())
    errs() << " | "; // divider of different path

  for (BasicBlock *Pred : predecessors(BB)) {
    TracePathBB(Pred);
  }
  errs() << getLabel(BB) << " ";
}

PreservedAnalyses BasicTrace::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
    errs() << "- Start of Function [" << Func.getName() << "]\n";
    for (auto &BB : Func) {
      errs() << "    - BasicBlock [" << getLabel(&BB) << "], num of instructions = " << BB.size() << "\n";
      TracePathBB(&BB);
      errs() << "\n";
      // errs() << "    - Start of BasicBlock [" << getLabel(&BB) << "], num of instructions = " << BB.size() << "\n";
      // errs() << "    - Predecessors: "; printPredecessors(&BB);

      // for (Instruction &I : BB) {
      //   errs() << "        - Instruction : " << I << "\n";
      // }
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