#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <string>
#include <sstream>  // Add this include
#include <fstream>
#include <map>
#include <set>
#include <queue>

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "ConstantPropagation"

namespace {

struct ConstantPropagation : public FunctionPass {
    static char ID;
    ConstantPropagation() : FunctionPass(ID) {}

std::string getLhsVar(const llvm::Instruction &ins) {
    std::string instrStr;
    llvm::raw_string_ostream rso(instrStr);
    ins.print(rso);
    rso.flush();
    std::string lhsVar;
    std::istringstream stream(instrStr);
    stream >> lhsVar;
    return lhsVar;
}

std::string getLhsVarFromValue(Value *ptrOperand){
    std::string ptrName;
    llvm::raw_string_ostream rso(ptrName);
    ptrOperand->printAsOperand(rso, false);  // `false` omits the type information
    rso.flush();
    return ptrName;
}

bool compareMaps(const map<string, int> &map1, const map<string, int> &map2) {
    errs()<<"Coming here&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<"\n";
    bool mapsEqual = true;

    // Check if each key-value pair in map1 exists in map2 and matches
    for (const auto &pair : map1) {
        auto it = map2.find(pair.first);
        if (it == map2.end() || it->second != pair.second) {
            mapsEqual = false;
            errs() << "Difference found: Key = " << pair.first
                   << ", map1 Value = " << pair.second
                   << ", map2 Value = " << (it != map2.end() ? std::to_string(it->second) : "Not found") << "\n";
        }
    }

    // Check if map2 has any additional keys not in map1
    for (const auto &pair : map2) {
        if (map1.find(pair.first) == map1.end()) {
            mapsEqual = false;
            errs() << "Extra key in map2: Key = " << pair.first
                   << ", Value = " << pair.second << "\n";
        }
    }

    // Return the overall comparison result
    return mapsEqual;
}
    bool runOnFunction(Function &F) override {

        // Map to hold constant propagation information with nullptr as "no value" indicator
        std::map<BasicBlock*, std::map<string, int>> OUT;

    for (auto &BB : F) {
        std::map<string,int>outM;
        for (auto &ins : BB) {
            errs() << ins << "\n";
            std::string lhsVar;
            lhsVar = getLhsVar(ins);
            // Perform all type checks in a single if condition
            if (auto *loadInst = dyn_cast<LoadInst>(&ins)) {
                outM[lhsVar] = INT_MIN;
            } else if (isa<AllocaInst>(ins)) {
                outM[lhsVar] = INT_MIN;
            } else if (isa<BinaryOperator>(ins)) {
                outM[lhsVar] = INT_MIN;
            }else if(auto *cmpInst = dyn_cast<ICmpInst>(&ins)){
                outM[lhsVar] = INT_MIN;
            }
        }
        OUT[&BB] = outM;
        errs() << "=====END OF BASIC BLOCK=====" << "\n";
    }
        int cnt = 0;
        BasicBlock &startBlock = F.getEntryBlock();
        queue<BasicBlock*> q;
        q.push(&startBlock);
        while (!q.empty()) {
            ++cnt;
            BasicBlock* block = q.front();
            q.pop();
            // use these maps for maintaining the temp maps for in and out
            // map<Value*, int> inMap,outMapTemp;
            map<string,int>inMapTemp,outMapTemp;

            //add in the logic for meet operator 
            for(auto predBB:predecessors(block)){
                for(auto mp:OUT[predBB]){
                    if(inMapTemp.find(mp.first) == inMapTemp.end()){
                        // the variable is only coming from one of the blocks at this point
                        inMapTemp[mp.first] = mp.second;
                    }else{
                        if(inMapTemp[mp.first] != mp.second){
                        inMapTemp[mp.first] = INT_MIN;
                        }
                    }
                }
            }
    

            outMapTemp = inMapTemp;
            for (auto &ins : *block) {
                errs() << "Instruction: " << ins << "\n";
                string lhsVar = getLhsVar(ins);
                // Handle Store Instruction
                if (ins.getOpcode() == Instruction::Store) {
                    StoreInst *storeInst = cast<StoreInst>(&ins);
                    Value *value = storeInst->getValueOperand();
                    auto *constValue = dyn_cast<ConstantInt>(value);
                    Value *ptr = storeInst->getPointerOperand();
                    if (constValue) {
                        Value *ptr = storeInst->getPointerOperand();
                        // mp[ptr] = constValue->getZExtValue();
                        // OUT[block][ptr] = constValue->getZExtValue();
                        // outMapTemp[ptr] = constValue->getZExtValueTemp();

                        // Convert the pointer operand to a string representation
                        // llvm::Value *ptrOperand = storeInst->getPointerOperand();
                        std::string ptrName;
                        // llvm::raw_string_ostream rso(ptrName);
                        // ptrOperand->printAsOperand(rso, false);  // `false` omits the type information
                        // rso.flush();
                        ptrName  = getLhsVarFromValue(ptr);
                        outMapTemp[ptrName] = constValue->getZExtValue();
                        // globalMap[ptrName] = constValue->getZExtValue();
                    }else{
                        string ptrVarName  = getLhsVarFromValue(ptr);
                        string valueVarName = getLhsVarFromValue(value);
                        outMapTemp[ptrVarName] = outMapTemp[valueVarName];
                        // globalMap[ptrVarName] = globalMap[valueVarName];
                        // errs()<<"In the store function"<<ptrVarName<<" "<<globalMap[ptrVarName]<<" "<<valueVarName<<" "<<globalMap[valueVarName]<<"\n";
                        errs()<<"In the store function"<<outMapTemp[ptrVarName]<<" "<<outMapTemp[valueVarName]<<"\n";

                    }
                } 

                // Handle Load Instruction
                else if (ins.getOpcode() == Instruction::Load) {
                    LoadInst *loadInst = cast<LoadInst>(&ins);
                    Value *ptr = loadInst->getPointerOperand();
                    string lhsVar = getLhsVar(ins);
                    string rhsVar = getLhsVarFromValue(ptr);
                    outMapTemp[lhsVar] = outMapTemp[rhsVar];
                    // globalMap[lhsVar] = globalMap[rhsVar];
                    errs()<<"rhs var is"<<rhsVar<<"-"<<outMapTemp[rhsVar]<<"\n";
                    errs()<<"lhs var is"<<lhsVar<<"-"<<outMapTemp[lhsVar]<<"\n";
                    // errs()<<"check check"<<globalMap[lhsVar]<<" "<<outMapTemp[rhsVar]<<" "<<lhsVar<<" "<<rhsVar<<"\n";

                    // if (outMapTemp.count(ptr)) {
                    //     // errTemps() << mp[ptr] << "\n";
                    //     // ConstantInt *constVal = ConstantInt::get(loadInst->getContext(), APInt(32, mp[ptr]));
                    //     // loadInst->replaceAllUsesWith(constVal);
                    //     // loadInst->replaceAllUsesWith(ptr);
                    //     outMapTemp[lhsVar] = outMapTemp[rhsVar];
                    //     globalMap[lhsVar] = outMapTemp[rhsVar];
                    //     errs()<<"check check"<<globalMap[lhsVar]<<" "<<outMapTemp[lhsVar]<<" "<<lhsVar<<" "<<rhsVar;
                    //     // errs() << "Replaced load instruction with pointer: " << *ptr << "\n";
                    // }
                }

                // Handle Binary Operations
                if (ins.isBinaryOp()) {
                    Value *lhs = ins.getOperand(0); // Left operand
                    Value *rhs = ins.getOperand(1); // Right operand
                    string operand1 = getLhsVarFromValue(lhs);
                    string operand2 = getLhsVarFromValue(rhs);
                    string binaryVar = getLhsVar(ins);

                    errs() << "The lhs operand in binary operation is " << operand1 << "\n";
                    errs() << "The rhs operand in binary operation is " << operand2 << "\n";
                    errs() << "The variable in binary operation is " << binaryVar << "\n";

                    auto *binaryInst = cast<BinaryOperator>(&ins);
                    int lhsval, rhsval;

                    // Retrieve operand values
                    if (auto *lhsConst = dyn_cast<ConstantInt>(lhs)) {
                        lhsval = lhsConst->getZExtValue();
                    } else {
                        lhsval = outMapTemp[operand1];
                    }

                    if (auto *rhsConst = dyn_cast<ConstantInt>(rhs)) {
                        rhsval = rhsConst->getZExtValue();
                    } else {
                        rhsval = outMapTemp[operand2];
                    }

                    // Perform the binary operation based on opcode
                    errs() << "Values are " << lhsval << " " << rhsval << "\n";

                    int computedVal = 0;
                    switch (binaryInst->getOpcode()) {
                        case Instruction::Add:
                            computedVal = lhsval + rhsval;
                            break;
                        case Instruction::Sub:
                            computedVal = lhsval - rhsval;
                            break;
                        case Instruction::Mul:
                            computedVal = lhsval * rhsval;
                            break;
                        case Instruction::SDiv:  // Signed division
                            if (rhsval != 0) {
                                computedVal = lhsval / rhsval;
                            } else {
                                // Handle division by zero case
                                errs() << "Error: Division by zero\n";
                                computedVal = 0;  // Or handle differently (e.g., error handling or setting to NaN)
                            }
                            break;
                        default:
                            errs() << "Unsupported binary operation (not +, -, *, /)\n";
                            computedVal = 0; // Default case if the operation is unknown
                    }
                    if(lhsval == INT_MIN || rhsval == INT_MIN){
                        errs()<<"One of the operand is not a constant";
                        computedVal = INT_MIN;
                    }
                    // Store the computed value into globalMap and temporary output map
                    // globalMap[binaryVar] = computedVal;
                    outMapTemp[binaryVar] = computedVal;
                    errs() << "Computed value: " << computedVal << "\n";
                }

                if (auto *cmpInst = dyn_cast<ICmpInst>(&ins)) {
                // Operands of the comparison
                Value *lhs = cmpInst->getOperand(0);  // Left-hand side operand
                Value *rhs = cmpInst->getOperand(1);  // Right-hand side operand
                string operand1 = getLhsVarFromValue(lhs);
                string operand2 = getLhsVarFromValue(rhs);

                int lhsVal,rhsVal;
                    
                    if (auto *lhsConst = dyn_cast<ConstantInt>(lhs)) {
                        lhsVal = lhsConst->getZExtValue();
                    } else {
                        string operand1 = getLhsVarFromValue(lhs);
                        lhsVal = outMapTemp[operand1];
                    }

                    if (auto *rhsConst = dyn_cast<ConstantInt>(rhs)) {
                        rhsVal = rhsConst->getZExtValue();
                    } else {
                        string operand2 = getLhsVarFromValue(rhs);    
                        rhsVal = outMapTemp[operand2];
                    }


                ICmpInst::Predicate predicate = cmpInst->getPredicate();
               if(rhsVal == INT_MIN || lhsVal == INT_MIN){
                    string lhsVarName = getLhsVar(ins);
                    outMapTemp[lhsVarName] = 2;

               }
                // Now check if it's followed by a branch instruction (conditional)
                if (auto *brInst = dyn_cast<BranchInst>(cmpInst->getNextNode())) {
                    if (brInst->isConditional()) {
                    string lhsVarName = getLhsVar(ins);

                        // This is a conditional branch
                        // // Get the successor blocks of the branch instruction
                        // BasicBlock *trueBlock = brInst->getSuccessor(0); // The block executed when the predicate is true
                        // BasicBlock *falseBlock = brInst->getSuccessor(1); // The block executed when the predicate is false
                        // BasicBlock *nextBlock = brInst->getParent()->getNextNode(); // Block after the conditional branch
                if(rhsVal == INT_MIN || lhsVal == INT_MIN){
                        string lhsVarName = getLhsVar(ins);
                        outMapTemp[lhsVarName] = 2;

                } else{                
                    bool condition = false;
                    switch (predicate) {
                        case ICmpInst::ICMP_EQ:
                            condition = (lhsVal == rhsVal);
                            break;
                        case ICmpInst::ICMP_NE:
                            condition = (lhsVal != rhsVal);
                            break;
                        case ICmpInst::ICMP_SGT:
                            condition = (lhsVal > rhsVal);
                            break;
                        case ICmpInst::ICMP_SLT:
                            condition = (lhsVal < rhsVal);
                            break;
                        case ICmpInst::ICMP_SGE:
                            condition = (lhsVal >= rhsVal);
                            break;
                        case ICmpInst::ICMP_SLE:
                            condition = (lhsVal <= rhsVal);
                            break;
                        default:
                            errs() << "Unhandled comparison predicate\n";
                            break;
                    }
                    outMapTemp[lhsVarName] = condition;
                    errs()<<"operand1 is "<<lhsVal<<" operand2"<<rhsVal<<"condition"<<condition<<"\n";
                    errs()<<"The value in cmp us =-=-=-=-=-=-"<<outMapTemp[lhsVarName]<<" "<<lhsVarName<<"\n";
                    }
                    // OUT[block] = outMap;

                    // if(condition){
                    //     q.push(trueBlock);
                    // }else{
                    //     q.push(falseBlock);
                    // }

                    }
                }
            }
            else if (auto *brInst = dyn_cast<BranchInst>(&ins)) {
                    if (brInst->isConditional()) {
                        // Conditional branch: It has two successors
                        BasicBlock *trueBlock = brInst->getSuccessor(0);  // Block to jump to if condition is true
                        BasicBlock *falseBlock = brInst->getSuccessor(1); // Block to jump to if condition is false
                        Value *cond = brInst->getCondition();
                        string condlhsVar = getLhsVarFromValue(cond);
                        errs()<<"The value of the branch instruction is"<<outMapTemp[condlhsVar]<<"\n";

                        if(outMapTemp[condlhsVar] == 1){
                            errs()<<"Adding true block to the queue"<<"\n";
                            q.push(trueBlock);
                        }else if(outMapTemp[condlhsVar] == 0){
                            errs()<<"Adding false block to the queue"<<"\n";
                            q.push(falseBlock);
                        }else{
                            errs()<<"Could not evaluate the condition. Adding both true and false blocks."<<"\n";
                            q.push(trueBlock);
                            q.push(falseBlock);
                        }
                        outMapTemp.erase(condlhsVar);
                        bool mapsEqual = compareMaps(OUT[block],outMapTemp);
                        errs() << "Comparing the OUT and the actual map: " << (mapsEqual ? "equal" : "not equal") << "\n";

                    } else {
                        // Unconditional branch: Only one successor
                        BasicBlock *nextBlock = brInst->getSuccessor(0); // The only successor for an unconditional branch
                        bool mapsEqual = compareMaps(OUT[block],outMapTemp);
                        errs() << "Comparing the OUT and the actual map: " << (mapsEqual ? "equal" : "not equal") << "\n";
                        // for(auto mp:OUT[block]){
                        //     errs()<<mp.first<<"----"<<mp.second<<block<<"\n";
                        // }
                        q.push(nextBlock); // Add the next block to the queue
                    }
                errs()<<"->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<"\n";
                for(auto mp:OUT[block]){
                    errs()<<mp.first<<"->"<<mp.second<<"\n";
                }

                    OUT[block] = outMapTemp;

                errs()<<"->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----------------------------long"<<"\n";
                for(auto mp:OUT[block]){
                    errs()<<mp.first<<"->"<<mp.second<<"\n";
                }
                errs()<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<coming here after every block"<<cnt<<"\n";
                if(cnt == 12){
                    while(!q.empty()){
                        BasicBlock* b = q.front();
                        q.pop();
                        errs()<<"Instructions in the block are mmmmmmmmmmmmmmmmmmmmmmmmmmmm-s\n";
                        for(auto &ins:*b){
                            errs()<<ins<<"\n";
                        }

                        for (BasicBlock *succ : successors(b)) {
                                errs() << succ->getName() << "\n";
                            }
                        errs()<<"Instructions in the block are mmmmmmmmmmmmmmmmmmmmmmmmmmmm-e\n";

                    }
                    errs()<<"Size of the queue is"<<q.size()<<"\n";   
                    return false;
                }
            }


            }
        }

        // Print Basic Block Info
        for (auto &block : F) {
            errs() << "==== Basic Block: " << block.getName() << " ====\n";

            // Print predecessors
            errs() << "Predecessors: ";
            for (auto *Pred : predecessors(&block)) {
                errs() << Pred->getName() << " ";
            }
            errs() << "\n";

            // Print successors
            errs() << "Successors: ";
            for (auto *Succ : successors(&block)) {
                errs() << Succ->getName() << " ";
            }
            errs() << "\n";
            errs() << "==== End of Basic Block: " << block.getName() << " ====\n";
        }
        return true;
    }
};

} // end of anonymous namespace

char ConstantPropagation::ID = 0;
static RegisterPass<ConstantPropagation> X("ConstantPropagation", "Constant Propagation Pass",
                                           false /* Only looks at CFG */,
                                           false /* Analysis Pass */);

