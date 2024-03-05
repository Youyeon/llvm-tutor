//==============================================================================
// FILE:
//    PathTrace.cpp
//
// DESCRIPTION:
//    This pass performs ?
//
// USAGE:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libPathTrace.so `\`
//        -passes=-"path-trace" <bitcode-file>
//      The command line option is not available for the new PM

// License: MIT
//==============================================================================
#include "PathTrace.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/CFG.h"

#include <random>
#include <algorithm>

using namespace llvm;

#define DEBUG_TYPE "path-trace"

STATISTIC(SubstCount, "The # of removed specials");

//-----------------------------------------------------------------------------
// PathTrace Implementation
//-----------------------------------------------------------------------------

void PathTrace::traceBB(llvm::BasicBlock *BB) {
    for (BasicBlock *Pred : predecessors(BB)) {

    }

}

PreservedAnalyses PathTrace::run(llvm::Function &Func,
                                      llvm::FunctionAnalysisManager &) {
    for (auto &BB : Func) {
        if (!(BB->isEntryBlock()))
            traceBB(&BB);
    }

    return llvm::PreservedAnalyses::all();
}


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getPathTracePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "path-trace", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "path-trace") {
                    FPM.addPass(PathTrace(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getPathTracePluginInfo();
}
