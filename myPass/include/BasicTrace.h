//==============================================================================
// FILE:
//    BasicTrace.h
//
// DESCRIPTION:
//    Declares the BasicTrace Passes:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_BASICTRACE_H
#define LLVM_TUTOR_BASICTRACE_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <string>

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------

struct BasicTrace : public llvm::PassInfoMixin<BasicTrace> {
    public:
        explicit BasicTrace(llvm::raw_ostream &OutS) : OS(OutS) {}

        std::string getLabel(llvm::BasicBlock* BB);

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);

        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;
};

#endif