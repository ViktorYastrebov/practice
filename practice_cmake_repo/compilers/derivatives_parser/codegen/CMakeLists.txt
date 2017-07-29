cmake_minimum_required(VERSION 3.5)

project(derivatives_codegen)

file(GLOB CODEGEN_SOURCE "*.cpp" ".cc")
file(GLOB CODEGEN_HEADERS "*.h" "*.hpp")

MESSAGE(STATUS "LLVM_INCLUDE=${LLVM_INCLUDE}")

SET(LLVM_LINK_LIBS LLVMAnalysis.lib
LLVMCore.lib
LLVMExecutionEngine.lib
LLVMInstCombine.lib
LLVMObject.lib
LLVMRuntimeDyld.lib
LLVMScalarOpts.lib
LLVMSupport.lib
LLVMX86CodeGen.lib
LLVMX86AsmPrinter.lib
LLVMX86AsmParser.lib
LLVMX86Desc.lib
LLVMX86Info.lib
LLVMX86Disassembler.lib
LLVMAsmPrinter.lib
LLVMDebugInfoCodeView.lib
LLVMSelectionDAG.lib
LLVMCodeGen.lib
LLVMTarget.lib
LLVMBitWriter.lib
LLVMInstrumentation.lib
LLVMTransformUtils.lib
LLVMProfileData.lib
LLVMBitReader.lib
LLVMX86Utils.lib
LLVMMCParser.lib
LLVMMCDisassembler.lib
LLVMMC.lib)

#include_directories(${QUEX_DIR} ./lexer)
include_directories(${LOKI_INCLUDE} ${LLVM_INCLUDE} ${QUEX_DIR} ../lexer)
link_directories(${LOKI_LIB} ${LLVM_LIBS})

#add_library(derivatives_parser_objects STATIC ${CODEGEN_SOURCE} ${CODEGEN_HEADERS})
add_executable(deriv_codegen ${CODEGEN_SOURCE} ${CODEGEN_HEADERS})

if(WIN32)
   target_link_libraries(deriv_codegen ${LLVM_LINK_LIBS} derives_parser)
endif()

