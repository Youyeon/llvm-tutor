//========================================================================
// FILE:
//    RemoveSpecials.h
//
// DESCRIPTION:
//    Declares the RemoveSpecials Pass
//
// License: MIT
//========================================================================
#ifndef LLVM_TUTOR_REMOVESPECIALS_H
#define LLVM_TUTOR_REMOVESPECIALS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using ResultRemoveSpecials = llvm::StringMap<unsigned>;

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
struct RemoveSpecials : public llvm::PassInfoMixin<RemoveSpecials> {
    public:
        explicit RemoveSpecials(llvm::raw_ostream &OutS) : OS(OutS) {}

        using Result = ResultRemoveSpecials;
        llvm::PreservedAnalyses run(llvm::Function &F,
                                    llvm::FunctionAnalysisManager &);

        // Checks whether the input instruction Insts are candidate of INF/NaN branch
        // which is consecutive instructions: `icmp - br`
        // This branch will be always False in our program, thus we will remove it.
        bool isTargetRemoveInst(const llvm::Instruction *Inst);

        // Replace the destination of incoming edges of BBToErase by BBToRetain
        unsigned updateBranchTargets(llvm::BasicBlock *BBToErase,
                                    llvm::BasicBlock *BBToRetain);

        // If BBs in flow are determined as useless, add BBs to DeleteList
        bool removeUselessBlock(llvm::BasicBlock *BB,
                                llvm::SmallPtrSet<llvm::BasicBlock *, 8> &DeleteList);

        // Without isRequired returning true, this pass will be skipped for functions
        // decorated with the optnone LLVM attribute.
        static bool isRequired() { return true; }

    private:
        llvm::raw_ostream &OS;

};
#endif